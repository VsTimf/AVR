 #ifndef _INA226_H
 #define _INA226_H
 
 #include "stdint.h"
 
/*
�����������:
    INA226 ina226 (������������� �����, ������������ ��������� ���, ����� �� ���� I2c)

    INA226 ina226;						// ����������� �������� ��� ������ INA226 (0.1 ��, 0.8 �, ����� 0x40) - �������� ��� ������ ������
    INA226 ina226 (0x41);				// ���� � ����. ��� �� ���������, ����� 0x41 - �������� ��� ���������� �������
    INA226 ina226 (0.05f);				// ���� 0.05 ��, ����. ��� � ����� �� ��������� (0.8 �, 0x40) - ���������� ������ ��� ����� �/�
    INA226 ina226 (0.05f, 1.5f);		// ���� 0.05 ���, ����. ��������� ��� 1.5 �, ����� �� ��������� (0x40) - ���������� ������ ��� ����� �/�
    INA226 ina226 (0.05f, 1.5f, 0x41);	// ���� 0.05 ���, ����. ��������� ��� 1.5 �, ����� 0x41  - ���������� ������ ��� ����� �/�

������:
        bool begin();							    // ������������� ������ � �������� �����������, ������ false ���� INA226 �� �������
        bool begin(SCL, SDA);						// ������������� ������ ��� ESP8266/ESP32
    void sleep(true / false);				    // ��������� � ���������� ������ ������� �����������������, � ����������� �� ���������
    void setAveraging(avg);					    // ��������� ���������� ���������� ��������� (��. ������� ����)
    void setSampleTime(ch, time);			    // ��������� ������� ������� ���������� � ���� (INA226_VBUS / INA226_VSHUNT), �� ��������� INA226_CONV_1100US

    float getShuntVoltage(); 				    // ��������� ���������� �� �����
    float getVoltage();	 					    // ��������� ����������
    float getCurrent();  					    // ��������� ���
    float getPower(); 						    // ��������� ��������

    uint16_t getCalibration();	 			    // ��������� ������������� �������� (����� ������ �������������� �������������)
    void setCalibration(calibration value);	    // �������� ������������� �������� 	(����� ������� ��� � EEPROM)
    void adjCalibration(calibration offset);    // ���������� ������������� �������� �� ��������� �������� (����� ������ �� ����)

    ������ 1.0 �� 31.10.2021
*/


#define SDA1   4
#define SCL1   5

#define WDELAY1 100



/* Public-����������� (���������) */
#define INA226_VBUS true     // ����� ���, ���������� ���������� ���� (0-36�)
#define INA226_VSHUNT false  // ����� ���, ���������� ���������� �� �����

#define INA226_CONV_140US 0b000  // ����� ������� (���������� ������� ��� ���������)
#define INA226_CONV_204US 0b001
#define INA226_CONV_332US 0b010
#define INA226_CONV_588US 0b011
#define INA226_CONV_1100US 0b100
#define INA226_CONV_2116US 0b101
#define INA226_CONV_4156US 0b110
#define INA226_CONV_8244US 0b111

#define INA226_AVG_X1 0b000  // ���������� ���������� (��������������� ����������� ����� ���������)
#define INA226_AVG_X4 0b001
#define INA226_AVG_X16 0b010
#define INA226_AVG_X64 0b011
#define INA226_AVG_X128 0b100
#define INA226_AVG_X256 0b101
#define INA226_AVG_X512 0b110
#define INA226_AVG_X1024 0b111

/* Private-����������� (������) */
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

void writeRegister (unsigned char reg, uint16_t data);  // ������ 16-�� ������� �������� INA219
unsigned int readRegister (unsigned char reg);  // ������ 16-�� ������� �������� INA219



// ��������� �������������� ��������
void setCalibration(uint16_t cal); 
 
// ������ �������������� ��������
uint16_t getCalibration(void);
	 
// ���������� �������������� ��������
void adjCalibration(int16_t adj);
	 
// ��������� ����������� ���������� �������
void setAveraging(uint8_t avg); 

// ��������� ���������� ��� ���������� ������
// void setSampleTime(uint8_t ch, uint8_t mode); 
 
// ������ ���������� �� �����
// float getShuntVoltage(void); 
 
// ������ ����������
float getVoltage(void); 

// ������ ����
float getCurrent(void);
 
// ������ ��������
// float getPower(void); 
 
// ��������� ������� �������������� �������� � �������������
void calibrate(void); 


#endif //_INA226_H