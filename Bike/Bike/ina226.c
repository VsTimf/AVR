#include "ina226.h"
#include <avr/io.h>
#include <util/delay.h>

unsigned char READ_PCF  = 0x81;
unsigned char WRITE_PCF = 0x80;


const float _r_shunt = 0.001;         // Сопротивление шунта
const float _i_max = 80;           // Макс. ожидаемый ток
const uint8_t _iic_address = 0x00;  // Адрес на шине I2c

float _current_lsb = 0.0;  // LSB для тока
float _power_lsb = 0.0;    // LSB для мощности
uint16_t _cal_value = 0;   // Калибровочное значение


void start_cond (void)
{
  PORTC=_BV(SDA1)|_BV(SCL1);
  _delay_us(WDELAY1);
  PORTC&=~_BV(SDA1);
  _delay_us(WDELAY1);
  PORTC&=~_BV(SCL1);
}

void stop_cond (void)
{
  PORTC=_BV(SCL1);
  _delay_us(WDELAY1);
  PORTC&=~_BV(SDA1);
  _delay_us(WDELAY1);
  PORTC|=_BV(SDA1);
}

void send_byte (unsigned char data)
{ 
	unsigned char i;

	for (i=0;i<8;i++)
	{
		if ((data&0x80)==0x00) 
			PORTC&=~_BV(SDA1);	//set SDA
	    else 
			PORTC|=_BV(SDA1);
			
		_delay_us(WDELAY1);
		PORTC|=_BV(SCL1);				//SCL impulse
		_delay_us(WDELAY1);
		PORTC&=~_BV(SCL1);
		data=data<<1;
	}
   
	DDRC&=~_BV(SDA1);
	PORTC|=_BV(SDA1);

	_delay_us(WDELAY1);
	PORTC|=_BV(SCL1);
    _delay_us(WDELAY1);
	
    if ((PINC&_BV(SDA1))==_BV(SDA1)) 
		_delay_us(WDELAY1);				//Reading ACK
		
   PORTC&=~_BV(SCL1);
   PORTC&=~_BV(SDA1);
   DDRC|=_BV(SDA1);
}



unsigned char get_byte (void)
{ 
	unsigned char i;
	unsigned char res = 0;
	
	DDRC&=~_BV(SDA1);
	PORTC|=_BV(SDA1);
	
	for (i=0;i<8;i++)
	{
		res=res<<1;
		PORTC|=_BV(SCL1);				//SCL impulse
		_delay_us(WDELAY1);
		if ((PINC&_BV(SDA1))==_BV(SDA1)) res=res|0x01;	//Reading SDA
		PORTC&=~_BV(SCL1);
	   _delay_us(WDELAY1);   
	}

	PORTC&=~_BV(SDA1);
	DDRC|=_BV(SDA1); 
  
	_delay_us(WDELAY1);
	PORTC|=_BV(SCL1);
	_delay_us(WDELAY1);
	PORTC&=~_BV(SCL1);

	return res;
}



void writeRegister (unsigned char reg, uint16_t data)  // Запись 16-ти битного регистра INA219
{ 
	unsigned char b = data&0xFF;
	unsigned char a = (data>>8)&0xFF;
	
	start_cond();
	send_byte (WRITE_PCF);
	send_byte (reg);
	send_byte (a);
	send_byte (b);
	stop_cond();   
}

unsigned int readRegister (unsigned char reg)  // Чтение 16-ти битного регистра INA219
{ 
	unsigned int data;
	unsigned char datal;
	unsigned char datah;
	
	start_cond();
	send_byte (WRITE_PCF);
	send_byte (reg);
	start_cond();
	send_byte(READ_PCF);
	datah=get_byte();
	datal=get_byte();
	stop_cond();
	data=(datal|datah<<8);
	
	return data;
}



// Установка калибровочного значения
void setCalibration(uint16_t cal) 
{
	writeRegister(INA226_CAL_REG_ADDR, cal);  // Пишем значение в регистр калибровки
	_cal_value = cal;                         // Обновляем внутреннюю переменную
}  
	
	
// Чтение калибровочного значения
uint16_t getCalibration(void) 
{
	_cal_value = readRegister(INA226_CAL_REG_ADDR);  // Обновляем внутреннюю переменну
	return _cal_value;                               // Возвращаем значение
}
	 
// Подстройка калибровочного значения
void adjCalibration(int16_t adj) 
{
	setCalibration(getCalibration() + adj);  // Читаем и модифицируем значение
	_cal_value = _cal_value + adj;           // Обновляем внутреннюю переменную
}
	 
// Установка встроенного усреднения выборок
void setAveraging(uint8_t avg) 
{
	uint16_t cfg_register = readRegister(INA226_CFG_REG_ADDR) & ~(0b111 << 9);  // Читаем конф. регистр, сбросив биты AVG2-0
	writeRegister(INA226_CFG_REG_ADDR, cfg_register | avg << 9);                // Пишем новое значение конф. регистр
}


//     // Установка разрешения для выбранного канала
//     void setSampleTime(uint8_t ch, uint8_t mode) {
//         uint16_t cfg_register = readRegister(INA226_CFG_REG_ADDR);  // Читаем конф. регистр
//         cfg_register &= ~((0b111) << (ch ? 6 : 3));                 // Сбрасываем нужную пачку бит, в зависимости от канала
//         cfg_register |= mode << (ch ? 6 : 3);                       // Пишем нужную пачку бит, в зависимости от канала
//         writeRegister(INA226_CFG_REG_ADDR, cfg_register);           // Пишем новое значение конф. регистра
//     }
// 
//     // Чтение напряжения на шунте
//     float getShuntVoltage(void) {
//         setCalibration(_cal_value);                           // Принудительное обновление калибровки (на случай внезапного ребута INA219)
//         int16_t value = readRegister(INA226_SHUNT_REG_ADDR);  // Чтение регистра напряжения шунта
//         return value * 0.0000025f;                            // LSB = 2.5uV = 0.0000025V, умножаем и возвращаем
//     }
// 


// Чтение напряжения
float getVoltage(void) 
{
    uint16_t value = readRegister(INA226_VBUS_REG_ADDR);  // Чтение регистра напряжения
    return value * 0.00125f;                              // LSB = 1.25mV = 0.00125V, Сдвигаем значение до 12 бит и умножаем
}

// Чтение тока
float getCurrent(void) 
{
    setCalibration(_cal_value);                         // Принудительное обновление калибровки (на случай внезапного ребута INA219)
    int16_t value = readRegister(INA226_CUR_REG_ADDR);  // Чтение регистра тока
    return value * _current_lsb;                        // LSB рассчитывается на основе макс. ожидаемого тока, умножаем и возвращаем
}
// 
//     // Чтение мощности
//     float getPower(void) {
//         setCalibration(_cal_value);                            // Принудительное обновление калибровки (на случай внезапного ребута INA219)
//         uint16_t value = readRegister(INA226_POWER_REG_ADDR);  // Чтение регистра мощности
//         return value * _power_lsb;                             // LSB в 25 раз больше LSB для тока, умножаем возвращаем
//     }
 
// Процедура расчёта калибровочного значения и инициализации
void calibrate(void) 
{
    writeRegister(INA226_CFG_REG_ADDR, 0x8000);  // Принудительный сброс
 
    _current_lsb = _i_max / 32768.0f;                          // расчёт LSB для тока (см. доку INA219)
    _power_lsb = _current_lsb * 25.0f;                         // расчёт LSB для мощности (см. доку INA219)
    _cal_value = trunc(0.00512f / (_current_lsb * _r_shunt));  // расчёт калибровочного значения (см. доку INA219)
 
    setCalibration(_cal_value);  // Записываем стандартное калибровочное значение
}