#include "CanFrameRingBuffer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

class CanFrameRingBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test data
        testFrame.object_id = 0x123;
        testFrame.function_id = CAN_FUNC_SET_IN;
        testFrame.data[0] = 0x42;
        testFrame.raw_data_length = 2;
        testFrame.initialized = true;
    }

    can_frame_t testFrame;
    static constexpr uint8_t BUFFER_SIZE = 4;
};

TEST_F(CanFrameRingBufferTest, DefaultConstructor)
{
    CanFrameRingBuffer<BUFFER_SIZE> buffer;
    EXPECT_EQ(buffer.capacity(), BUFFER_SIZE);
    EXPECT_EQ(buffer.dataLength(), 0);
    EXPECT_TRUE(buffer.isEmpty());
    EXPECT_FALSE(buffer.isFull());
}

TEST_F(CanFrameRingBufferTest, PushAndPopSingleFrame)
{
    CanFrameRingBuffer<BUFFER_SIZE> buffer;
    
    // Push a frame
    EXPECT_TRUE(buffer.push(testFrame));
    EXPECT_EQ(buffer.dataLength(), 1);
    EXPECT_FALSE(buffer.isEmpty());
    
    // Pop the frame
    can_frame_t receivedFrame;
    EXPECT_TRUE(buffer.pop(receivedFrame));
    
    // Verify frame contents
    EXPECT_EQ(receivedFrame.object_id, testFrame.object_id);
    EXPECT_EQ(receivedFrame.raw_data_length, testFrame.raw_data_length);
    EXPECT_EQ(receivedFrame.function_id, testFrame.function_id);
    EXPECT_EQ(receivedFrame.data[0], testFrame.data[0]);
    EXPECT_TRUE(receivedFrame.initialized);
    
    // Buffer should be empty now
    EXPECT_TRUE(buffer.isEmpty());
    EXPECT_EQ(buffer.dataLength(), 0);
}

TEST_F(CanFrameRingBufferTest, BufferWraparound)
{
    CanFrameRingBuffer<BUFFER_SIZE> buffer;
    
    // Fill buffer completely
    for(uint8_t i = 0; i < BUFFER_SIZE - 1; i++) {
        testFrame.data[0] = i;  // Modify data for each frame
        EXPECT_TRUE(buffer.push(testFrame));
    }
    
    // Remove two frames
    can_frame_t dummy;
    EXPECT_TRUE(buffer.pop(dummy));
    EXPECT_EQ(dummy.data[0], 0);  // Verify first popped frame
    EXPECT_TRUE(buffer.pop(dummy));
    EXPECT_EQ(dummy.data[0], 1);  // Verify second popped frame
    
    // Add two more frames (should wrap around)
    testFrame.raw_data[1] = 0xAA;
    EXPECT_TRUE(buffer.push(testFrame));
    testFrame.raw_data[1] = 0xBB;
    EXPECT_TRUE(buffer.push(testFrame));
    
    // Verify correct length
    EXPECT_EQ(buffer.dataLength(), BUFFER_SIZE - 1);
    EXPECT_TRUE(buffer.isFull());
}

TEST_F(CanFrameRingBufferTest, FullBufferBehavior)
{
    CanFrameRingBuffer<BUFFER_SIZE> buffer;
    
    // Fill buffer to one less than capacity (due to ring buffer implementation)
    for(uint8_t i = 0; i < BUFFER_SIZE - 1; i++) {
        EXPECT_TRUE(buffer.push(testFrame));
    }
    
    EXPECT_TRUE(buffer.isFull());
    EXPECT_FALSE(buffer.isEmpty());
    
    // Attempt to push when full should fail
    EXPECT_FALSE(buffer.push(testFrame));
    
    // Remove one frame
    can_frame_t dummy;
    EXPECT_TRUE(buffer.pop(dummy));
    
    // Should be able to push again
    EXPECT_TRUE(buffer.push(testFrame));
}

TEST_F(CanFrameRingBufferTest, PeekAndRemove)
{
    CanFrameRingBuffer<BUFFER_SIZE> buffer;
    
    // Push a frame
    EXPECT_TRUE(buffer.push(testFrame));
    
    // Peek at the frame
    const can_frame_t& peekedFrame = buffer.peek();
    EXPECT_EQ(peekedFrame.object_id, testFrame.object_id);
    EXPECT_EQ(peekedFrame.raw_data_length, testFrame.raw_data_length);
    EXPECT_EQ(buffer.dataLength(), 1);  // Peek shouldn't affect length
    
    // Remove the frame
    EXPECT_TRUE(buffer.remove());
    EXPECT_TRUE(buffer.isEmpty());
    
    // Remove on empty buffer should fail
    EXPECT_FALSE(buffer.remove());
}

TEST_F(CanFrameRingBufferTest, GetWriteFrame)
{
    CanFrameRingBuffer<BUFFER_SIZE> buffer;
    
    // Get write frame and modify it
    can_frame_t& writeFrame = buffer.getWriteFrame();
    writeFrame = testFrame;
    
    // Manually push it by advancing head (simulating actual usage pattern)
    EXPECT_TRUE(buffer.push(writeFrame));
    
    // Verify the frame was written correctly
    can_frame_t receivedFrame;
    EXPECT_TRUE(buffer.pop(receivedFrame));
    EXPECT_EQ(receivedFrame.object_id, testFrame.object_id);
    EXPECT_EQ(receivedFrame.raw_data_length, testFrame.raw_data_length);
}

TEST_F(CanFrameRingBufferTest, InvalidPushParameters)
{
    CanFrameRingBuffer<BUFFER_SIZE> buffer;
    uint8_t data[] = {0x01, 0x02};
    
    // Test null data pointer
    EXPECT_FALSE(buffer.push(0x123, nullptr, 2));
    
    // Test zero length
    EXPECT_FALSE(buffer.push(0x123, data, 0));
    
    // Test excessive length
    EXPECT_FALSE(buffer.push(0x123, data, CAN_FRAME_MAX_PAYLOAD + 2));
    
    // Valid push should still work
    EXPECT_TRUE(buffer.push(0x123, data, 2));
}
