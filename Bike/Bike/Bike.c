/*
 * Bike.c
 *
 * Program  for electro bike computer.
 * It have: small 128x64 screen lh155ba;
 *          1 user button;
 *          INA226 current&voltage sensor;
 *          timer0 connect to pulse sensor on the wheel
 *
 * 2 work modes:
 * --setup 
 *			sets the number of pulses at a distance of 10 meters
 *          sets the number of battery cells [10 .. 20]
 *
 * --ride
 *          shows current speed, battery, current
 *			shows odometer or battery capacity (change on button click)
 *          resets odometer or battery capacity (change on button long press)
 *
 * Created: 12.09.2024 12:37:29
 *  Author: Fly
 */ 
//#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>


#include "lcd.h"
#include "ina226.h"
#include "button.h"


#define LOW_CELL_V 3.2				// ����������� ���������� ������ �������
#define HIGH_CELL_V 4.0				// ������������ ���������� ������ �������

#define VOLT_COEFF 4.0				// ����������� ���������� ����������

static void setup_mode();			// ������� ����������� ��������


// ���������� � EEPROM
uint16_t param[2] = {10, 10};			    // ���������� ��������� �������� (0 - ���-�� ������� �� 10�, 1 - ���-�� ����� �������)

uint8_t first_start EEMEM;
uint16_t  eep_dist_cal EEMEM;
uint16_t  eep_bat_sel  EEMEM;
float  eep_capacity    EEMEM;
float  eep_odometr     EEMEM;



volatile uint8_t f_time = 0;		// ���� 10ms, ��������������� � ���������� TIM1

uint8_t  time = 0;					// ������� 1�
uint16_t time_min = 0;				// ������� 60�


uint8_t speed = 0;					// ������� ��������
float odometr = 0;					// ������� ������
uint16_t speed_pulse_cnt = 0;		// ���-�� ��������� � ������� (���������� ������������ �������� ������� ��������� * 256)


float current = 0;					// ������� ���
float voltage = 0;					// ������� ����������

float curr_min = 0;					// ��� �� ������
float volt_min = 0;					// ���������� �� ������

float capacity = 0;					// ������� �������
float battery = 0;					// ����������� �� ������ ���������� �������



uint8_t show_mode = 1;				// �������� ������� ��� �������
uint8_t fcharging;					// �������� ��������� �������



// ����������� �� ������������ �������� ��������� � �������
ISR(TIMER0_OVF_vect){
	speed_pulse_cnt += 256;
}


// ���������� 10ms
ISR(TIMER1_OVF_vect){
	TCNT1 = 63036;
	f_time = 1;
}


// ���������� �� ������
//ISR(INT1_vect) {}

// ���������� �� ���������� �������
ISR(INT0_vect) {
	eeprom_write_float (&eep_capacity, capacity);   // ��������� �������
	eeprom_write_float (&eep_odometr, odometr);    // ��������� ���������
}


// ������ EEPROM ��� ������
void eeprom_data_read()
{
	uint8_t first_start_val = 0xAA;
	if(eeprom_read_byte(&first_start) != first_start_val)
	{
		eeprom_write_byte(&first_start, first_start_val);
		
		eeprom_write_float (&eep_capacity, capacity);   // ��������� �������
		eeprom_write_float (&eep_odometr, odometr);		// ��������� ���������
		
		eeprom_write_word(&eep_dist_cal, param[0]);		// ��������� ���������� ��������
		eeprom_write_word(&eep_bat_sel, param[1]);		// ��������� ���-�� ����� �������
	}
	else
	{
		capacity = eeprom_read_float (&eep_capacity);	// ������ �������� �������
		odometr =  eeprom_read_float (&eep_odometr);	// ������ �������� ��������
		
		param[0] = eeprom_read_word(&eep_dist_cal);		// ������ �������� ���������� ��������
		param[1] = eeprom_read_word(&eep_bat_sel);		// ������ �������� ���������� ����� �������
	}
	
}




void init()
{
	DDRD=0b00000000;
	PORTD=0b00001000;

	DDRC= 0b00110000;
	PORTC=0b00000000;
	
	DDRB= 0b00011110;
	PORTB=0b00000000;
	
	TCCR1B|= (0<<CS12)|(1<<CS11)|(1<<CS10); //���� ������� �������� �������� �� 64
	TIMSK1 |= (1<<TOIE1);
	TCNT1 = 63036;
	
	
	TCCR0B|=(1<<CS02)|(1<<CS01)|(0<<CS00); // ���� �� ������� T0

	
	//EICRA|= ((1<<ISC11)|(0<<ISC10));				// ����������� ���������� �� ���  ������
	EICRA|= ((1<<ISC01)|(1<<ISC00));				// ����������� ���������� �� ���  ������� ����
	
	EIMSK|=(1<<INT0)/* |(1<<INT1)*/;				// ���������� ���������� �� int
}



int main()
{
	init();

	_lcd_init();
	
	calibrate();  // ���������� INA226
	
	
	LCD_clear_display();
	eeprom_data_read();
	sei();
	
	
	
	if((PIND&_BV(PD3))==0x00)			// ��������� ���� ��� ��������� ������ ������
	setup_mode();
	
	
	current = getCurrent();
	battery = getVoltage()*VOLT_COEFF;
	
	_lcd_show_speed(0);
	_lcd_show_odometr(odometr);
	_lcd_show_battery(LOW_CELL_V * param[1], HIGH_CELL_V * param[1], battery);
	_lcd_show_curr(current, voltage);
	_lcd_show_time();
	
	
	while(1)
	{
		while(!f_time);
		f_time = 0;
		
		button_process();							// ����� ������
		
		if(button_is_pressed(B_CLICK_RELEASE))		// ����� ������ ���������
		{
			if(show_mode)
			_lcd_show_cap(capacity);
			else
			_lcd_show_odometr(odometr);
			
			show_mode = !show_mode;
		}
		
		if(button_is_pressed(B_LONG))				// ����� ��������
		{
			if(show_mode)
			{
				odometr = 0.0;
				_lcd_show_odometr(odometr);
				eeprom_write_float (&eep_odometr, odometr);    // ��������� ���������			!! ������ ������ ��� ����� ������� !!
			}
			else
			{
				capacity = 0.0;
				_lcd_show_cap(capacity);
				eeprom_write_float (&eep_odometr, odometr);    // ��������� ���������			!! ������ ������ ��� ����� ������� !!
			}
		}
		
		if(++time == 50)
		{
			_lcd_hide_time_pointers();				// ������� ��� ����� � �����
			voltage = getVoltage()*VOLT_COEFF;
			volt_min += voltage;
		}

		 
		if(time >= 100)
		{
			time = 0;
			_lcd_show_time();						// �������� �����
			
			
			current = getCurrent(); 
			
			
			// ����������� �������
			if(!fcharging && current < -0.1)
			{
				fcharging = 1;
				_lcd_show_charging();
			}
			
			if(fcharging && current >= 0.0)
			{
				fcharging = 0;
				_lcd_hide_charging();
			}
			
			if(current < 0)
				current = -current;
			 
			curr_min += current;
			
			
			float s = (speed_pulse_cnt + TCNT0)/param[0];	// ���������� = ���-�� �������� / [���-�� ��������� / ����] !!!!!!!!!!!
			
			
			TCNT0 = 0;
			speed_pulse_cnt = 0;
			
			odometr += s/1000;								// ��������� � ����������
			speed = s*3.6;									// ��������� �� �/� � ��/�
			
			
			
			_lcd_show_curr(current, voltage);
			_lcd_show_speed(speed);
			
			if(show_mode)
				_lcd_show_odometr(odometr);
			
			
			
			
		}
		
		
		if(++time_min > 6000)					//  �������� �������� ������ �������. �� ������ - 60 ��������.
		{
			capacity += (curr_min / 60) / 60;
			battery = volt_min / 60;
			
			if(!show_mode)
				_lcd_show_cap(capacity);
			
			_lcd_show_battery(LOW_CELL_V * param[1], HIGH_CELL_V * param[1], battery);
			
			time_min = 0;
			curr_min = 0;
			volt_min = 0;
		}
		
	}
}



// Setup: ��������� ���-�� ��������� �� 10�
void setup_speed_calibration(void)
{
	uint8_t upd_counter = 0;
	
	TCNT0 = 0;
	speed_pulse_cnt = 0;
	
	_lcd_show_speed_setup();
	
	while(1)
	{
		while(!f_time);
		f_time = 0;
		
		button_process();
		
		if(++upd_counter >= 10)								// ������ 100�� �������� ������ �� ������
		{
			upd_counter = 0;
			_lcd_show_pulse_counter(speed_pulse_cnt + TCNT0);
		}
		
		if(button_is_pressed(B_LONG))
		{
			param[0] = (speed_pulse_cnt + TCNT0) / 10;
			eeprom_write_word(&eep_dist_cal, param[0]);		// ��������� ���������� ���������
			speed_pulse_cnt = 0;
			TCNT0 = 0;
			return;
		}
	}
}


// Setup: ��������� ���������� ����� �������
void setup_battery_cell(void)
{
	uint8_t new_cells_qnt = param[1];
	
	_lcd_show_batcell_setup(new_cells_qnt);
	
	while(1)
	{
		while(!f_time);
		f_time = 0;
		
		button_process();
		
		if(button_is_pressed(B_CLICK_RELEASE))
		{
			new_cells_qnt = (new_cells_qnt >= 20) ? 10 : new_cells_qnt + 1;
			_lcd_show_batcell_setup(new_cells_qnt);
		}
		
		if(button_is_pressed(B_LONG))
		{
			param[1] = new_cells_qnt;
			eeprom_write_word(&eep_bat_sel, param[1]);   // ��������� ���-�� ����� �������
			return;
		}
	}
}



// Setup
void setup_mode(void)
{
	static uint8_t curr_setting = 0;
	
	_lcd_show_settings(curr_setting, param[curr_setting]);
	
	while((PIND&_BV(PD3))==0x00);
	
	while(1)
	{
		while(!f_time);
		f_time = 0;
		
		button_process();
		
		if(button_is_pressed(B_CLICK_RELEASE))
		{
			curr_setting = (curr_setting >= 2) ? 0 : curr_setting+1;
			_lcd_show_settings(curr_setting, param[curr_setting]);
		}
		
		if(button_is_pressed(B_LONG))
		{
			LCD_clear_display();
			
			switch(curr_setting)
			{
				case 0:
				setup_speed_calibration();
				break;
				
				case 1:
				setup_battery_cell();
				break;
				
				case 2:
				return;
			}
			
			LCD_clear_display();
			_lcd_show_settings(curr_setting, param[curr_setting]);
		}
	}
}