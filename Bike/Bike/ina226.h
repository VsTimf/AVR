 #ifndef _INA226_H
 #define _INA226_H
 
 #include "stdint.h"
 
/*
Конструктор:
    INA226 ina226 (Сопротивление шунта, Максимальный ожидаемый ток, Адрес на шине I2c)

    INA226 ina226;						// Стандартные значения для модуля INA226 (0.1 Ом, 0.8 А, адрес 0x40) - подойдет для одного модуля
    INA226 ina226 (0x41);				// Шунт и макс. ток по умолчанию, адрес 0x41 - подойдет для нескольких модулей
    INA226 ina226 (0.05f);				// Шунт 0.05 Ом, макс. ток и адрес по умолчанию (0.8 А, 0x40) - Допиленный модуль или голая м/с
    INA226 ina226 (0.05f, 1.5f);		// Шунт 0.05 Ома, макс. ожидаемый ток 1.5 А, адрес по умолчанию (0x40) - Допиленный модуль или голая м/с
    INA226 ina226 (0.05f, 1.5f, 0x41);	// Шунт 0.05 Ома, макс. ожидаемый ток 1.5 А, адрес 0x41  - Допиленные модули или голые м/с

Методы:
        bool begin();							    // Инициализация модуля и проверка присутствия, вернет false если INA226 не найдена
        bool begin(SCL, SDA);						// Инициализация модуля для ESP8266/ESP32
    void sleep(true / false);				    // Включение и выключение режима низкого энергопотребления, в зависимости от аргумента
    void setAveraging(avg);					    // Установка количества усреднений измерений (см. таблицу ниже)
    void setSampleTime(ch, time);			    // Установка времени выборки напряжения и тока (INA226_VBUS / INA226_VSHUNT), по умолчанию INA226_CONV_1100US

    float getShuntVoltage(); 				    // Прочитать напряжение на шунте
    float getVoltage();	 					    // Прочитать напряжение
    float getCurrent();  					    // Прочитать ток
    float getPower(); 						    // Прочитать мощность

    uint16_t getCalibration();	 			    // Прочитать калибровочное значение (после старта рассчитывается автоматически)
    void setCalibration(calibration value);	    // Записать калибровочное значение 	(можно хранить его в EEPROM)
    void adjCalibration(calibration offset);    // Подкрутить калибровочное значение на указанное значение (можно менять на ходу)

    Версия 1.0 от 31.10.2021
*/


#define SDA1   4
#define SCL1   5

#define WDELAY1 100



/* Public-определения (константы) */
#define INA226_VBUS true     // Канал АЦП, измеряющий напряжение шины (0-36в)
#define INA226_VSHUNT false  // Канал АЦП, измеряющий напряжение на шунте

#define INA226_CONV_140US 0b000  // Время выборки (накопления сигнала для оцифровки)
#define INA226_CONV_204US 0b001
#define INA226_CONV_332US 0b010
#define INA226_CONV_588US 0b011
#define INA226_CONV_1100US 0b100
#define INA226_CONV_2116US 0b101
#define INA226_CONV_4156US 0b110
#define INA226_CONV_8244US 0b111

#define INA226_AVG_X1 0b000  // Встроенное усреднение (пропорционально увеличивает время оцифровки)
#define INA226_AVG_X4 0b001
#define INA226_AVG_X16 0b010
#define INA226_AVG_X64 0b011
#define INA226_AVG_X128 0b100
#define INA226_AVG_X256 0b101
#define INA226_AVG_X512 0b110
#define INA226_AVG_X1024 0b111

/* Private-определения (адреса) */
#define INA226_CFG_REG_ADDR 0x00
#define INA226_SHUNT_REG_ADDR 0x01
#define INA226_VBUS_REG_ADDR 0x02
#define INA226_POWER_REG_ADDR 0x03
#define INA226_CUR_REG_ADDR 0x04
#define INA226_CAL_REG_ADDR 0x05




void start_cond (void);
void stop_cond (void);

void send_byte (unsigned char data);
unsigned char get_byte (void);

void writeRegister (unsigned char reg, uint16_t data);  // Запись 16-ти битного регистра INA219
unsigned int readRegister (unsigned char reg);  // Чтение 16-ти битного регистра INA219



// Установка калибровочного значения
void setCalibration(uint16_t cal); 
 
// Чтение калибровочного значения
uint16_t getCalibration(void);
	 
// Подстройка калибровочного значения
void adjCalibration(int16_t adj);
	 
// Установка встроенного усреднения выборок
void setAveraging(uint8_t avg); 

// Установка разрешения для выбранного канала
// void setSampleTime(uint8_t ch, uint8_t mode); 
 
// Чтение напряжения на шунте
// float getShuntVoltage(void); 
 
// Чтение напряжения
float getVoltage(void); 

// Чтение тока
float getCurrent(void);
 
// Чтение мощности
// float getPower(void); 
 
// Процедура расчёта калибровочного значения и инициализации
void calibrate(void); 


#endif //_INA226_H