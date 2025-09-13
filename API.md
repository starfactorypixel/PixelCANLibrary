# CANLibrary API

### CANManager
Базовый объект взаимодействия с сетью CAN
```cpp
// Конструктор
[-] CANManager<uint8_t _max_objects = 16, uint8_t _can_frame_buffer_size = 16, uint32_t tick_time = 10> can_manager();

// Добавить объект CAN в менеджер
[-] void can_manager.AddObj(CANObjectInterface &can_object);

// Обновление менеджера. Вызывать в loop()
[-] void can_manager.Tick(uint32_t time);

// Вставить полученный CAN пакет в менеджер. Происходит только проверка на необходимый CANObject и копирование паента в очеред. Обработка происходит через Tick();
[-] void can_manager.PutFrame(CANFrame_t &frame);
[-] void can_manager.PutFrame(can_object_id_t id, uint8_t *data, uint8_t length);

// Получить на отправку CAN пакет из менеджера
[-] bool can_manager.GetFrame(CANFrame_t &frame);
[-] bool can_manager.GetFrame(can_object_id_t &id, uint8_t *&data, uint8_t &length);

// [?] Если GetFrame будет дёргаться в прерывании когда будет свободный can-mailbox, как его дёрнуть в первый раз, когда до этого мы всё отправвили и прерывания остановились. Нужен метод в менеджере, который заставит дёргуть CAN контроллер который в свою очередь дёрнет GetFrame.

// Метод отправляет произвольный пакет в CAN
[-] void can_manager.SendRaw(CANFrame_t &frame);
[-] void can_manager.SendRaw(can_object_id_t id, uint8_t *data, uint8_t length);
```

### CANObject, CANObjectBase, CANObjectInterface
Базовые объекты конкретного CAN объекта
```cpp
// Конструктор
// typename T - Тип поля данных в CAN объекте
// uint8_t _item_count - Кол-во полей данных в CAN объекте
[-] CANObject<typename T, uint8_t _item_count = 1> can_object(can_object_id_t id);

// Регистрация функции CAN протокола
// Реактивные - обработка входящих пакетов
[-] CANFunctionReactiveInterface *can_object.RegisterReactiveFunction(CANFunctionReactiveInterface *reactive_function);

// Регистрация функции CAN протокола
// Проактивные - работа по времени и(или) по внешним событиям
[-] CANFunctionProactiveInterface *can_object.RegisterProactiveFunction(CANFunctionProactiveInterface *proactive_function);

// Получить CAN ID объекта
[-] can_object_id_t can_object.GetId();

// Кол-во полей данных в CAN объекте
[-] uint8_t GetDataFieldCount();

// Размер одного поля данных в CAN объекте (байт)
[-] uint8_t GetOneDataFieldSize();

// Set/Get полей данных CAN объекте
// index - Номер поля данных (отсчёт с нуля)
[-] void SetValue(uint8_t index, T value);
[-] void SetValue(uint8_t index, void *value, uint8_t value_size);
[-] T GetValue(uint8_t index);
[-] void *GetValuePtr(uint8_t index);
```

### CANFunctionInterface
Функции CAN объекта

```cpp

```



### Плата IO, Порт 1,2,3
```cpp

class MySetFunc : public CANFunctionSet
{
	protected:
		virtual void _FunctionCallBack(CANObjectInterface &parent_can_object, int32_t user_id) override
		{
			uint8_t val = parent_can_object.GetValue(0);
			//

			Power::obj.Set(user_id, val);
			bool result = Power::obj.On(user_id);
			if(result)
			{
				// Генерирует event ok
			}
			else
			{
				// Генерирует event error
			}

			return code;
		}
};

CANManager<16> can_manager();
CANObject<uint8_t, 1> can_object_1(0x0164);
CANObject<uint8_t, 1> can_object_2(0x0165);
CANObject<uint8_t, 1> can_object_3(0x0166);
// 0x0164	Out1	set | toggle | request | event	---	uint8_t	00 || FF	1 + 1	{ type[0] } or { type[0] data[1] }		Выход 1
//CANFunctionSet set_function();
//CANFunctionToggle toggle_function();
MySetFunc mysetfunc();

void setup()
{
	can_manager.AddObj(can_object_1);
	can_manager.AddObj(can_object_2);
	can_manager.AddObj(can_object_3);
	
	can_object_1.RegisterReactiveFunction(mysetfunc, 1);
	can_object_2.RegisterReactiveFunction(mysetfunc, 2);
	can_object_3.RegisterReactiveFunction(mysetfunc, 3);
}

void loop()
{
	can_manager.Tick(millis());
}
```












### Мысли №5475
```cpp

class myInOut8: public CANObject<uint8_t, 1>
{
  private:
    uint8_t _port = 1;
  public:
    myInOut8(can_object_id_t id, uint8_t port) : _port(port), CANObject(id){};
    uint8_t GetPort() { return _port; };
    void SetPort(uint8_t port) { _port = port; };
}

class myInOut16: public CANObject<uint16_t, 1>
{
  private:
    uint8_t _port = 1;
  public:
    myInOut16(can_object_id_t id, uint8_t port) : _port(port), CANObject(id){};
    uint8_t GetPort() { return _port; };
    void SetPort(uint8_t port) { _port = port; };
}

CANManager<16> can_manager();
myInOut8 can_object_out1(0x0164, 1);
myInOut8 can_object_out2(0x0165, 2);
myInOut8 can_object_out3(0x0166, 3);
// 0x0164	Out1	set | toggle | request | event	---	uint8_t	00 || FF	1 + 1	{ type[0] } or { type[0] data[1] }		Выход 1
myInOut16 can_object_in1(0x016C, 100, 1);	// 100 - Установка таймеров. Здесь?


// obj.SetValue(); Не использовать
void MyCallbackReactive(const CANObjectInterface &obj/*, const CANFrame_t &rx, CANFrame_t &tx*/)
{
	myInOut8 *myobj = (myInOut8 *)&obj;
	uint8_t val = myobj.GetValue(0);
	uint8_t port = myobj.GetPort();
	
	bool result;
	switch(myobj.Getfuncid())
	{
		case 0x01:
			Power::obj.Set(port, val);
			result = Power::obj.On(port);
			break;
		
		case 0x02:
			Power::obj.Toggle(port, val);
			result = Power::obj.On(port);
			break;
	}

	myobj.Setfuncid() = (result) ? 0x65 : 0xE6;
	myobj.SetValue(....);
	//myobj.Setinitialized() = true;
}


// obj.SetValue(); Не использовать
void MyCallbackProactive(const CANObjectInterface &obj, const uint32_t time/*, CANFrame_t &tx*/)
{
	myInOut16 *myobj = (myInOut16 *)&obj;
	uint8_t port = myobj->GetPort();

	uint16_t val = Analog::obj.Read(port);
	myobj.Setfuncid() = (val < 50) ? 0x61 : 0x63;
	myobj.SetValue(val);
	//myobj.Setinitialized() = true;


	// Как делать event от железа ????
	// по маленькому значению таймера
}


void setup()
{
	can_manager.AddObj(can_object_out1);
	can_manager.AddObj(can_object_out2);
	can_manager.AddObj(can_object_out3);
	
	can_object_out1.RegisterReactiveCallback(MyCallbackReactive);
	can_object_out2.RegisterReactiveCallback(MyCallbackReactive);
	can_object_out3.RegisterReactiveCallback(MyCallbackReactive);

	can_object_in1.RegisterProactiveCallback(MyCallbackProactive);

/*
	// Прикольно но нужно ли?
	can_object_out3.RegisterCallback(MyCallback);
	can_object_out3.RegisterCallback(MyCallback, FuncID);
	can_object_out3.RegisterCallbackEx(MyCallback, FuncID_MASK);	// Битовая маска
	can_object_out3.RegisterCallbackEx(MyCallback, FuncID_FROM, FuncID_TO);
*/
}

void loop()
{
	can_manager.Tick(millis());
}
```