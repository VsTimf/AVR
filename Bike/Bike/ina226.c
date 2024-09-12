#include "ina226.h"
#include <avr/io.h>
#include <util/delay.h>

unsigned char READ_PCF  = 0x81;
unsigned char WRITE_PCF = 0x80;


const float _r_shunt = 0.001;         // ������������� �����
const float _i_max = 80;           // ����. ��������� ���
const uint8_t _iic_address = 0x00;  // ����� �� ���� I2c

float _current_lsb = 0.0;  // LSB ��� ����
float _power_lsb = 0.0;    // LSB ��� ��������
uint16_t _cal_value = 0;   // ������������� ��������


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



void writeRegister (unsigned char reg, uint16_t data)  // ������ 16-�� ������� �������� INA219
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

unsigned int readRegister (unsigned char reg)  // ������ 16-�� ������� �������� INA219
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



// ��������� �������������� ��������
void setCalibration(uint16_t cal) 
{
	writeRegister(INA226_CAL_REG_ADDR, cal);  // ����� �������� � ������� ����������
	_cal_value = cal;                         // ��������� ���������� ����������
}  
	
	
// ������ �������������� ��������
uint16_t getCalibration(void) 
{
	_cal_value = readRegister(INA226_CAL_REG_ADDR);  // ��������� ���������� ���������
	return _cal_value;                               // ���������� ��������
}
	 
// ���������� �������������� ��������
void adjCalibration(int16_t adj) 
{
	setCalibration(getCalibration() + adj);  // ������ � ������������ ��������
	_cal_value = _cal_value + adj;           // ��������� ���������� ����������
}
	 
// ��������� ����������� ���������� �������
void setAveraging(uint8_t avg) 
{
	uint16_t cfg_register = readRegister(INA226_CFG_REG_ADDR) & ~(0b111 << 9);  // ������ ����. �������, ������� ���� AVG2-0
	writeRegister(INA226_CFG_REG_ADDR, cfg_register | avg << 9);                // ����� ����� �������� ����. �������
}


//     // ��������� ���������� ��� ���������� ������
//     void setSampleTime(uint8_t ch, uint8_t mode) {
//         uint16_t cfg_register = readRegister(INA226_CFG_REG_ADDR);  // ������ ����. �������
//         cfg_register &= ~((0b111) << (ch ? 6 : 3));                 // ���������� ������ ����� ���, � ����������� �� ������
//         cfg_register |= mode << (ch ? 6 : 3);                       // ����� ������ ����� ���, � ����������� �� ������
//         writeRegister(INA226_CFG_REG_ADDR, cfg_register);           // ����� ����� �������� ����. ��������
//     }
// 
//     // ������ ���������� �� �����
//     float getShuntVoltage(void) {
//         setCalibration(_cal_value);                           // �������������� ���������� ���������� (�� ������ ���������� ������ INA219)
//         int16_t value = readRegister(INA226_SHUNT_REG_ADDR);  // ������ �������� ���������� �����
//         return value * 0.0000025f;                            // LSB = 2.5uV = 0.0000025V, �������� � ����������
//     }
// 


// ������ ����������
float getVoltage(void) 
{
    uint16_t value = readRegister(INA226_VBUS_REG_ADDR);  // ������ �������� ����������
    return value * 0.00125f;                              // LSB = 1.25mV = 0.00125V, �������� �������� �� 12 ��� � ��������
}

// ������ ����
float getCurrent(void) 
{
    setCalibration(_cal_value);                         // �������������� ���������� ���������� (�� ������ ���������� ������ INA219)
    int16_t value = readRegister(INA226_CUR_REG_ADDR);  // ������ �������� ����
    return value * _current_lsb;                        // LSB �������������� �� ������ ����. ���������� ����, �������� � ����������
}
// 
//     // ������ ��������
//     float getPower(void) {
//         setCalibration(_cal_value);                            // �������������� ���������� ���������� (�� ������ ���������� ������ INA219)
//         uint16_t value = readRegister(INA226_POWER_REG_ADDR);  // ������ �������� ��������
//         return value * _power_lsb;                             // LSB � 25 ��� ������ LSB ��� ����, �������� ����������
//     }
 
// ��������� ������� �������������� �������� � �������������
void calibrate(void) 
{
    writeRegister(INA226_CFG_REG_ADDR, 0x8000);  // �������������� �����
 
    _current_lsb = _i_max / 32768.0f;                          // ������ LSB ��� ���� (��. ���� INA219)
    _power_lsb = _current_lsb * 25.0f;                         // ������ LSB ��� �������� (��. ���� INA219)
    _cal_value = trunc(0.00512f / (_current_lsb * _r_shunt));  // ������ �������������� �������� (��. ���� INA219)
 
    setCalibration(_cal_value);  // ���������� ����������� ������������� ��������
}