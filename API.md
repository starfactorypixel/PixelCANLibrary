# CANLibrary API

### CANManager
Базовый объект взаимодействия с сетью CAN
```cpp
// Конструктор
CANManager<uint8_t _max_objects = 16, uint8_t _can_frame_buffer_size = 16, uint32_t tick_time = 10> can_manager();

// Добавить объект CAN в менеджер
void can_manager.AddObj(CANObjectInterface &can_object);

// Обновление менеджера. Вызывать в loop()
void can_manager.Tick(uint32_t time);

// Вставить полученный CAN пакет в менеджер. Происходит только проверка на необходимый CANObject и копирование паента в очеред. Обработка происходит через Tick();
void can_manager.PutFrame(CANFrame_t &frame);
void can_manager.PutFrame(can_object_id_t id, uint8_t *data, uint8_t length);

// Получить на отправку CAN пакет из менеджера
bool can_manager.GetFrame(CANFrame_t &frame);
bool can_manager.GetFrame(can_object_id_t &id, uint8_t *&data, uint8_t &length);

// [?] Если GetFrame будет дёргаться в прерывании когда будет свободный can-mailbox, как его дёрнуть в первый раз, когда до этого мы всё отправвили и прерывания остановились. Нужен метод в менеджере, который заставит дёргуть CAN контроллер который в свою очередь дёрнет GetFrame.

// Метод отправляет произвольный пакет в CAN
void can_manager.SendRaw(CANFrame_t &frame);
void can_manager.SendRaw(can_object_id_t id, uint8_t *data, uint8_t length);
```

### CANObjectInterface
Объект конкретного CAN объекта
```cpp

```