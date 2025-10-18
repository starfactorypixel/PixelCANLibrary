#include "CanManager.h"
#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

// Mock CAN object for testing
class MockCanObject : public CANObjectInterface {
public:
    MockCanObject(can_object_id_t id) : CANObjectInterface(id) {}

    uint8_t getDataFieldCount() override { return 1; }
    uint8_t getOneDataFieldSize() override { return 1; }
    void setValue(uint8_t index, void* value, uint8_t value_size) override {}
    void* getValuePtr(uint8_t index) override { return nullptr; }
    void process(uint32_t time) override {
        processCallCount++;
        lastProcessTime = time;
        
        // If we have an RX frame and it matches our test condition, prepare a response
        can_frame_t* rxFrame = nullptr;
        setRxFrame(*rxFrame);
        
        can_frame_t* txFrame = nullptr;
        setTxFrame(*txFrame);
    }

    // Test helper methods
    int processCallCount = 0;
    uint32_t lastProcessTime = 0;
};

class CanManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test frame
        testFrame.object_id = TEST_OBJECT_ID;
        testFrame.function_id = CAN_FUNC_SET_IN;
        testFrame.data[0] = 0x42;             // test data
        testFrame.raw_data_length = 2;
        testFrame.initialized = true;

        // Create test objects
        testObject1 = new MockCanObject(TEST_OBJECT_ID);
        testObject2 = new MockCanObject(TEST_OBJECT_ID + 1);
    }

    void TearDown() override {
        delete testObject1;
        delete testObject2;
    }

    static constexpr uint8_t MAX_OBJECTS = 4;
    static constexpr uint8_t BUFFER_SIZE = 16;
    static constexpr uint8_t TICK_TIME = 10;
    static constexpr can_object_id_t TEST_OBJECT_ID = 0x123;

    can_frame_t testFrame;
    MockCanObject* testObject1;
    MockCanObject* testObject2;
};

TEST_F(CanManagerTest, DefaultConstructor)
{
    CANManager<MAX_OBJECTS, BUFFER_SIZE, TICK_TIME> manager;
    EXPECT_FALSE(manager.hasFrameToSend());
}

TEST_F(CanManagerTest, ObjectRegistration)
{
    CANManager<MAX_OBJECTS, BUFFER_SIZE, TICK_TIME> manager;
    
    // Register first object
    EXPECT_TRUE(manager.registerObject(*testObject1));
    EXPECT_TRUE(manager.hasCanObject(TEST_OBJECT_ID));
    
    // Register second object
    EXPECT_TRUE(manager.registerObject(*testObject2));
    EXPECT_TRUE(manager.hasCanObject(TEST_OBJECT_ID + 1));
    
    // Verify object retrieval
    auto* retrieved = manager.getCanObject(TEST_OBJECT_ID);
    EXPECT_EQ(retrieved, testObject1);
    
    // Try to find non-existent object
    EXPECT_FALSE(manager.hasCanObject(0xFFFF));
    EXPECT_EQ(manager.getCanObject(0xFFFF), nullptr);
}

TEST_F(CanManagerTest, MaxObjectLimit)
{
    CANManager<2, BUFFER_SIZE, TICK_TIME> manager;  // Only allow 2 objects
    
    EXPECT_TRUE(manager.registerObject(*testObject1));
    EXPECT_TRUE(manager.registerObject(*testObject2));
    
    // Try to register a third object (should fail)
    MockCanObject extraObject(0x456);
    EXPECT_FALSE(manager.registerObject(extraObject));
}

TEST_F(CanManagerTest, IncomingFrameValidation)
{
    CANManager<MAX_OBJECTS, BUFFER_SIZE, TICK_TIME> manager;
    manager.registerObject(*testObject1);
    
    uint8_t data[] = {CAN_FUNC_SET_IN, 0x42};
    
    // Valid frame
    EXPECT_TRUE(manager.incomingCANFrame(TEST_OBJECT_ID, data, 2));
    
    // Invalid cases
    EXPECT_FALSE(manager.incomingCANFrame(TEST_OBJECT_ID, nullptr, 2));  // null data
    EXPECT_FALSE(manager.incomingCANFrame(TEST_OBJECT_ID, data, 0));     // zero length
    EXPECT_FALSE(manager.incomingCANFrame(0xFFFF, data, 2));             // unknown object
}

TEST_F(CanManagerTest, BroadcastFrameHandling)
{
    CANManager<MAX_OBJECTS, BUFFER_SIZE, TICK_TIME> manager;
    manager.registerObject(*testObject1);
    manager.registerObject(*testObject2);
    
    uint8_t data[] = {CAN_FUNC_REQUEST_IN, 0x42};  // Allowed broadcast function
    
    // Valid broadcast
    EXPECT_TRUE(manager.incomingCANFrame(CAN_SYSTEM_ID_BROADCAST, data, 2));
    
    // Invalid broadcast function
    data[0] = CAN_FUNC_SET_IN;  // Not allowed for broadcast
    EXPECT_FALSE(manager.incomingCANFrame(CAN_SYSTEM_ID_BROADCAST, data, 2));
}

TEST_F(CanManagerTest, TickTiming)
{
    CANManager<MAX_OBJECTS, BUFFER_SIZE, TICK_TIME> manager;
    manager.registerObject(*testObject1);
    
    // First tick should process
    EXPECT_EQ(manager.getRxFrameBufferLength(), 0);
    manager.incomingCANFrame(TEST_OBJECT_ID, testFrame.raw_data, testFrame.raw_data_length);
    EXPECT_EQ(manager.getRxFrameBufferLength(), 1);
    manager.tick(100);
    
    // Immediate tick shouldn't process (< TICK_TIME elapsed)
    EXPECT_EQ(manager.getRxFrameBufferLength(), 0);
    manager.incomingCANFrame(TEST_OBJECT_ID, testFrame.raw_data, testFrame.raw_data_length);
    EXPECT_EQ(manager.getRxFrameBufferLength(), 1);
    manager.tick(105);
    
    // Tick after TICK_TIME should process
    EXPECT_EQ(manager.getRxFrameBufferLength(), 1);
    manager.tick(111);  // 11ms after first tick
    
    // Verify object was processed correct number of times
    EXPECT_EQ(testObject1->processCallCount, 2);  // Only first and third ticks
    EXPECT_EQ(testObject1->lastProcessTime, 111);
}

TEST_F(CanManagerTest, FrameBufferBehavior)
{
    CANManager<MAX_OBJECTS, BUFFER_SIZE, TICK_TIME> manager;
    manager.registerObject(*testObject1);
    
    uint8_t data[] = {CAN_FUNC_SET_IN, 0x42};
    
    // Fill receive buffer
    for(uint8_t i = 0; i < BUFFER_SIZE - 1; i++) {
        EXPECT_TRUE(manager.incomingCANFrame(TEST_OBJECT_ID, data, 2));
    }
    
    // Process frames
    manager.tick(100);
    
    // Verify no frames are pending to send (our mock doesn't generate responses)
    EXPECT_FALSE(manager.hasFrameToSend());
    
    can_frame_t dummyFrame;
    EXPECT_FALSE(manager.getFrameToSend(dummyFrame));
}
