# Что хотим

1. В библиотеке только самая база
    * прием фреймов, их разбор на данные и ID функций\
    * базовый класс объектов для наследования в коде и реализации конкретного поведения
2. Таймеры обрабатываются библиотекой, но вызывается функция в классе-наследнике. Наверное, виртуальная.
3. Менеджер нуженю Он делает:
    * регистрирует объекты, с которыми работаем
    * принимает фреймы в кольцевой буфер
    * отправляет фреймы из кольцевого буфера
4. Решили, что ID объектов в плате будут всегда идти подряд. Значит
    * в конструкторе менеджера задаем базовый ID и количество объектов
    * упрощается проверка на то, принимать фрейм в буфер или нет
5. В шаблонных классах (если они будут) - только самое необходимое. Всё остальное в базовых классах, чтобы не раздувать код
6. Старт платы возможен в рабочем режиме и в режиме инициализации. Для инициализации плата работает только с фиксированными объектами с самыми большими ID. Запуск такого режима по зажатой кнопке при подаче питания. Этот функционал никак не влияет на библиотеку
7. для can_frame_t есть путаница с длиной данных. Там поле raw_data_length, но "raw" - слово непонятное и что там должна быть длина данные + функция не всегда ясно. ~~Можно сделать класс и перегрузить операторы доступа к данным, чтобы класс сам считал длину, но это перебор.~~ Решили убрать union и не вычленять во фрейме func_id.



# Архитектура

## Кольцевой буфер

**Вопрос:** сделаем его таким, чтобы принимал на запись и читал из буфера фреймы? Или байты?
Вроде так логичнее фреймы - мы же их храним. Понимать ограничения на этапе работы с кодом будет проще. Отлаживать по идее тоже.
Но у нас фреймы имеют разную длину пейлоада. И тогда не понятно, как возвращать оставшееся в буфере место...
Можно зарезервировать под каждый фрейм максимальное место (1 байт function ID + 7 байт на пейлоад). Просто часть из этого места не будем использовать. Жалко терять. Но может и ладно.
~~Можно этот вопрос оставить на потом. Базовый интерфейс буфера будет примерно одинаков, переделывать потом не много.~~
Скорее всего надо работать с фреймами и резервировать место под максимальные фреймы. Смотри код ниже - функция getFrameForWrite().

Два подхода:
* уже есть данные для фрейма и их просто хочется записать. Тогда это метод `pop()`.
* будем данные формировать на ходу и сразу в буфере. Тогда используем метод `getFrameForWrite()`.

**Важно!** При размере буфера равном степени двойки буфер можно хорошо оптимизировать по скорости выполнения.


```cpp
template <uint8_t _buffer_size> // _buffer_size измеряем в байтах или в количестве фреймов?
class CanFrameRingBuffer
{
private:
    can_frame_t _buffer[_buffer_size]; // это если у нас буфер из фреймов (резервируем место под максимальный фрейм)

public:
    uint8_t capacity() const { return _buffer_size; } // сколько всего влезает данных
    uint8_t dataLength() const { ... } // сколько данных уже лежит в буфере
    bool isEmpty() const { ... } // пуст ли буфер
    bool isFull() const { ... } // полон ли буфер

    bool push(can_frame_t data) { ... } // пишем фрейм в буфер, возвращаем true если данные влезли
    bool push(can_object_id_t id, uint8_t *data, uint8_t length) { ... } // аналогично, только данные для записи иначе представляем 

    can_frame_t *getFrameForWrite() { ... } // возвращаем указатель на место в буфере для записи туда данных, nullptr если места нет
    // ВНИМАНИЕ! Тут потенциальная засада, если буфер работает с байтами, есть свободных 5 байт, а код по этому указателю запишет 8 байт

    bool pop(can_frame_t &data) { ... } // заполняет структуру данными из буфера, возвращает false если данных нет или что-то не получилось

    // Спорные методы:

    bool remove() { ... } // удаляет последний фрейм в буфере без выгрузки его куда-либо. Наверное, может понадобиться, если решим при переполнении очищать фреймы. Но можно и выкинуть эту функцию

    can_frame_t *peek() { ... } // возвращает указатель на самый старый фрейм в буфере или nullptr, если буфер пуст
    // не знаю, нужно ли. Может быть полезно, ели надо что-то в буфере поправить. Но скорее всего это лишняя функция и ее надо в мусор.
};
```


## Менеджер

**ВОПРОСЫ:**
* Что делаем с широковещательными фреймами? У нас есть функции `lock`, `request` и раньше была `system_request` (сейчас ее вычеркнули из таблицы). Они должны отрабатываться на уровне менеджера/объекта? Или спускаем на уровень кода платы?


```cpp
template <
    can_object_id_t _base_obj_id,
    uint8_t _max_objects,
    uint8_t _can_frame_rx_buffer_size = 16,
    uint8_t _can_frame_tx_buffer_size = 16,
    uint8_t _tick_time = 10
>
class CANManager: public CanManagerInterface
{
private:
    CanFrameRingBuffer<_can_frame_rx_buffer_size> _rx_buffer;
    CanFrameRingBuffer<_can_frame_tx_buffer_size> _tx_buffer;

    CANObjectInterface *_objects[_max_objects] = {nullptr};

public:
    CANManager() {};
    virtual ~CANManager() = default;

    bool addObject(CANObjectInterface &can_object) { ... } // добавляем объект, возвращаем false если ID объекта не влезает в диапазон от _base_obj_id до _base_obj_id + _max_objects

    bool hasCanObject(can_object_id_t id) { ... } // есть ли такой объект. Можно просто проверять попадание ID в диапазон, но лучше еще проверять, что такой объект добавлен (указатель не nullptr)

    CANObjectInterface *getCanObject(can_object_id_t id) { ... } // возвращаем объект или nullptr, если объекта нет

    void tick(uint32_t time) { ... } // тикаем таймерами и что там ещё должно отрабатывать само

    bool hasFrameToSend() { ... } // есть ли в TX буфере что отправлять
    bool getFrameToSend(can_frame_t &can_frame) { ... } // забираем фрейм на отправку (фрейм удаляется из буфера в момент забора, а не когда уже отправлен)

    bool hasSpaceInTX() { ... } // есть ли в очереди на отправку место для фрейма
    bool pushFrameToTX(can_frame_t &can_frame) { ... } // кидаем фрейм буфер исходящих, возвращаем false, если нет места или что-то ещё пошло не так
    bool pushFrameToTX(can_object_id_t id, can_function_id_t function_id, uint8_t *data = nullptr, uint8_t length = 0) { ... } // то же самое, но данные по другому представили

    bool hasSpaceInRX() { ... } // есть ли в очереди на обработку место для фрейма
    bool pushFrameToRX(can_frame_t &can_frame) { ... } // кидаем фрейм во входящий буфер, возвращаем false, если нет места или что-то ещё пошло не так
    bool pushFrameToRX(can_object_id_t id, can_function_id_t function_id, uint8_t *data = nullptr, uint8_t length = 0) { ... } // то же самое, но данные по другому представили
    
    // теоретически от методов hasSpaceInRX(), hasSpaceInTX() и hasFrameToSend() можно отказаться. Просто проверять, что методы pushFrameToRX(), pushFrameToTX() и getFrameToSend() не вернули false

    // Надо ли иметь методы, возвращающие размер очереди на отправку или сколько на прием лежит? Наверное, это не будет использоваться нигде
};
```


## Объект

Объект наследуем от базового объекта. Сам базовый объект наследуем от интерфейса. Возможно, интерфейс - это лишнее. Надо будет подумать. Но теоретически интерфейс поможет отвязать менеджера от объекта и упростит возню с зависимостями при компиляции.

В базовый класс выносим весь основной функционал объекта - базовые проверки, ID, что там еще.

Хранением данных в объекте либа не занимается. Хранит пусть основной код.

**Вопросы:**
* Надо ли в методах референс на объект, чтобы можно было делать цепочки вызовов? Такая возможность давно в либе, но по-моему ни разу не использовалась. Может быть полезна в сеттерах типа `obj.setId(0x123).setTimerPeriod(5000).setBlaBla(42)`. Но читаемость от такого может страдать. Один вызов на строчку конятнее по идее. Для компилятора вообще пофиг.


```cpp
class CANObjectBase: public CANObjectInterface // наследуем от интерфейса или нет?
{
private:
    can_object_id_t _id = CAN_SYSTEM_ID_BROADCAST;
    CanManagerInterface *_parent = nullptr; // нужен нам, чтобы запихивать фреймы на отправку, когда нам нужно

protected:
    // набор виртуальных пустых функций (но не полностью виртуальные)
    // наследники их переопределят, если хотят что-то особенное делать
    // если не переопределят, то ничего вызываться не будет, так как функции пустые в базовом классе
    virtual loop(uint32_t time) {} // в потомках можем использовать, если что-то надо сделать или проверить иногда
    virtual frameProcessor(can_frame_t &can_frame) {} // переопределяем в потомке для обработки фреймов
    virtual craftTimerFrame(can_frame_t &can_frame) {} // переопределяем в потомке для отработки таймера

public:
    CANObjectBase() = delete;
    CANObjectBase(can_object_id_t object_id) : _id(object_id) {};
    virtual ~CANObjectBase() = default;

    void setParent(CanManagerInterface &parent) : _parent(&parent) {}; // менеджер при добавлении объекта сам вызовет этот метод и установит себя как родителя
    CanManagerInterface *getParent() { return _parent; }; 

    can_object_id_t getId() { return _id; }

    //uint8_t getDataFieldCount() { ... }
    //uint8_t getOneDataFieldSize() { ... }
    //void setValue(uint8_t index, void *value, uint8_t value_size) { ... }
    //void *getValuePtr(uint8_t index) { ... }

    void tick(uint32_t time) { // обрабатываем всякие таймеры и периодические события
        ...;
        if(настала_пора_таймера) {
            can_frame_t timer_frame;
            this->craftTimerFrame(timer_frame); // вызываем метод потомка
            sendFrame(timer_frame);
        }
        this->loop(); // вызываем метод потомка
        ...
    }

    void processFrame(can_frame_t &can_frame) { // обрабатываем фрейм
        ...; // тут можем проверять 
        this->frameProcessor(can_frame); // вызываем метод потомка
        ...
    }

    bool sendFrame(can_frame_t &can_frame) { ... } // кидаем в менеджера фрейм на отправку
    bool sendFrame(can_function_id_t function_id, uint8_t *data, uint8_t data_length) { ... } // кидаем в менеджера фрейм на отправку
    // по идее берем _parent и просто вызываем у него метод pushFrameToTX() подставляя свой id
};
```





# Как это будет работать

В коде платы подключаем либу и настраиваем объекты.
Все конкретные объекты наследуем от базового класса из либы.

```cpp
class myOutPort: public CANObjectBase
{
  private:
    uint8_t _port = 1;
    uint8_t _data = 0;

  protected:
    void handlerSetFunction(can_frame_t &can_frame) {
        // делаем что тут надо по функции SET делать, устанавливаем выход или что там еще
        // типа такого
        _data = can_frame.data[0];
        Power::obj.Set(_port, _data);
        bool result = Power::obj.On(_port);
        this->sendFrame((result) ? CAN_FUNC_EVENT_OK /*0x65*/ : CAN_FUNC_EVENT_ERROR /* 0xE6 */, // function_id
                        &_data, sizeof(_data)); // это нарушает протокол для CAN_FUNC_EVENT_ERROR, но лень писать два разных обработчика
    }

    void handlerToggleFunction(can_frame_t &can_frame) {
        // тут по сути и входящий фрейм не нужен, просто переключаем
        // типа так
        Power::obj.Toggle(_port, val);
        bool result = Power::obj.On(_port);
        _data = Power::obj.Get(_port);
        this->sendFrame((result) ? CAN_FUNC_EVENT_OK /*0x65*/ : CAN_FUNC_EVENT_ERROR /* 0xE6 */, // function_id
                        &_data, sizeof(_data)); // это нарушает протокол для CAN_FUNC_EVENT_ERROR, но лень писать два разных обработчика
    }

    // обработка входящих фреймов тут
    virtual frameProcessor(can_frame_t &can_frame) override {
        switch(can_frame.function_id)
        {
            case CAN_FUNC_SET_IN /* 0x01 */:
                handlerSetFunction(can_frame);
                break;
            
            case CAN_FUNC_TOGGLE_IN /* 0x02 */:
                handlerToggleFunction(can_frame);
                break;
        }
    }

  public:
    myIOPort(can_object_id_t id, uint8_t port) : CANObjectBase(id), _port(port) {};
    uint8_t GetPort() { return _port; };
    void SetPort(uint8_t port) { _port = port; };
}

class myInPort: public CANObjectBase
{
  private:
    uint8_t _port = 1;
    uint16_t _data = 0;

  protected:
    // будет вызываться каждый раз из базового класса из метода tick()
    virtual loop(uint32_t time) override {
        uint16_t new_data = Analog::obj.Read(_port);
        if (_data != new_data) {
            _data = new_data;
            this->sendFrame(CAN_FUNC_EVENT_OK /*0x65*/, &_data, sizeof(_data));
        }
    }

    // будет вызван кодом базового класса когда надо будет отрабатывать таймер
    // актуальное значение в _data уже будет после срабатывания прошлого loop()
    virtual craftTimerFrame(can_frame_t &can_frame) override {
        can_frame.function_id = (_data < 50) ? CAN_FUNC_TIMER_NORMAL /* 0x61 */ : CAN_FUNC_TIMER_CRITICAL /* 0x63 */;
        memcpy(can_frame.data, &_data, sizeof(_data));
        can_frame.raw_data_length = sizeof(_data) + 1;
    }

  public:
    myInPort(can_object_id_t id, uint8_t port) : CANObjectBase(id), _port(port) {};
    uint8_t GetPort() { return _port; };
    void SetPort(uint8_t port) { _port = port; };
}

CANManager<0x0160, 22> can_manager();
    // _base_obj_id = 0x0160
    // _max_objects = 22 (objectIDs = 0x0160 .. 0x0175) 
    // _can_frame_rx_buffer_size = 16 (default)
    // _can_frame_tx_buffer_size = 16 (default)
    // _tick_time = 10 (default)

myOutPort can_object_out1(0x0164, 1); // id = 0x0164, port = 1
myOutPort can_object_out2(0x0165, 2);
myOutPort can_object_out3(0x0166, 3);
myInPort can_object_in1(0x016C, 4); // id = 0x016C, port = 4

void setup()
{
    can_manager.AddObj(can_object_out1);
    can_manager.AddObj(can_object_out2);
    can_manager.AddObj(can_object_out3);
    can_manager.AddObj(can_object_in1);
    
    can_object_in1.SetTimer(100);    // Интервал таймера
}

void loop()
{
    can_manager.Tick(millis());
}
```