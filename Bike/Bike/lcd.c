#include "lcd.h"
#include <stdlib.h>
#include "stdio.h"

char buf[15];


void _lcd_init(void){ LCD_init(); }
	
// Отображение скорости
void _lcd_show_speed(uint8_t speed)
{
	LCD_xy_pos(0,8);
	
	LCD_put_digit(speed / 10);
	LCD_put_digit(speed % 10);
	
	LCD_xy_pos(6,32);
	LCD_printf("km/h");
}
	
	
// Отображение тока и напряжения	
void _lcd_show_curr(float curr, float volt)
{
	dtostrf(curr, 4, 1, buf);
	sprintf(buf+4, "A");
	
	LCD_xy_pos(9, 0);
	LCD_printf(buf);
	
	dtostrf(volt, 4, 1, buf);
	sprintf(buf+4, "V");
	
	LCD_xy_pos(9, 16);
	LCD_printf(buf);
}


// Отображение емкости
void _lcd_show_cap(float cap)
{
	if(cap >= 100)
		cap = 99.9;
	
	dtostrf(cap, 4, 1, buf);
	sprintf(buf+4, "Ah");
	
	LCD_xy_pos(0, 56);
	LCD_printf(buf);
}


// Отображение одометра
void _lcd_show_odometr(float odo)
{
	if(odo >= 100)
		odo = 99.9;
	
	dtostrf(odo, 4, 1, buf);
	sprintf(buf+4, "km");
	
	LCD_xy_pos(0, 56);
	LCD_printf(buf);
}
	
	
// Отображение времени
void _lcd_show_time()
{
	static uint8_t t1 = 255, t2=0, t3=0, t4=0;

	if(t1==9)
	{
		t1=0;
		if(t2==5)
		{
			t2=0;
			if(t3==9)
			{
				t3=0;
				t4++;
			}
			else t3++;
		}
		else t2++;
	}
	else t1++;
	
	
	sprintf(buf, "%d%d:%d%d", t4,t3,t2,t1);
	LCD_xy_pos(9, 56);
	LCD_printf(buf);
}


// Скрыть точки между минутами и секундами
void _lcd_hide_time_pointers(void)
{
	LCD_xy_pos(11, 56);
	LCD_putc(' ');
}


// Отображение индикатора зарядки
void _lcd_show_charging()
{
	LCD_xy_pos(0, 0);
	LCD_printf("Charging");
}


// Скрытие индикатора зарядки
void _lcd_hide_charging()
{
	LCD_xy_pos(0, 0);
	LCD_printf("        ");
}


// Отображение заряда батареи
void _lcd_show_battery(float low, float high, float curr)
{
	float battery = 0;
	
	// Calc ---
	if(curr < low)
	curr = low;
	
	
	battery = (curr - low) / ((high - low) / 6);
	
	
	// Display ---
	LCD_xy_pos(15, 0);
	
	LCD_printf("F");
	LCD_x_pos(15);
	
	
	
	for (uint8_t i = 6; i; i--)
	{
		LCD_write_byte(0x80, 0);
		
		if(battery >= i)
		{
			LCD_write_byte(0xBE, 0);
			LCD_write_byte(0xBE, 0);
			LCD_write_byte(0xBE, 0);
			LCD_write_byte(0xBE, 0);
			LCD_write_byte(0xBE, 0);
			LCD_write_byte(0xBE, 0);
		}
		else
		{
			LCD_write_byte(0x80, 0);
			LCD_write_byte(0x80, 0);
			LCD_write_byte(0x80, 0);
			LCD_write_byte(0x80, 0);
			LCD_write_byte(0x80, 0);
			LCD_write_byte(0x80, 0);
		}
		
		LCD_write_byte(0x80, 0);
	}
	
	
	LCD_printf("L");
}


	
// Отображение экрана настроек
void _lcd_show_settings(uint8_t setting, uint16_t param)
{
	LCD_xy_pos(6, 0);
	LCD_printf("SETUP");
	
	LCD_xy_pos(0, 16);
	LCD_printf("                ");
	
	LCD_xy_pos(0, 24);
	if(setting != 2)	//exit
	{
		sprintf(buf, "curr: %d ", param);
		LCD_printf(buf);	
	}
	else
		LCD_printf("                ");
	
	
	
	switch(setting)
	{
		case 0:
			
			LCD_xy_pos(0, 16);
			LCD_printf("Speed calibr-on");
		break;
		
		case 1:
			LCD_xy_pos(0, 16);
			LCD_printf("Bat. cell setup");
		break;
		
		case 2:
		LCD_xy_pos(3, 16);
		LCD_printf("Exit Setup");
		break;
	}
	
	LCD_xy_pos(0, 48);
	LCD_printf("long press ->Yes");
	
	LCD_xy_pos(0, 56);
	LCD_printf("short press->No ");
}


// Отображение настройки коэф. скорости/дистанции
void _lcd_show_speed_setup(void)
{
	LCD_xy_pos(2, 0);
	LCD_printf("SPEED  SETUP");
	
	LCD_xy_pos(0, 16);
	LCD_printf("Move 10 meters..");
	LCD_xy_pos(0, 32);
	LCD_printf("after long press");
}


// Отображение количества импульсов
void _lcd_show_pulse_counter(uint16_t cnt)
{
	LCD_xy_pos(0, 56);
	sprintf(buf, "Pulse: %d", cnt);
	LCD_printf(buf);
}


// Отображение кол-ва ячеек батареи
void _lcd_show_batcell_setup(uint16_t cells)
{
	LCD_xy_pos(0, 0);
	LCD_printf("BATT CELL  SETUP");
	
	LCD_xy_pos(0, 56);
	sprintf(buf, "Cells: %d ", cells);
	LCD_printf(buf);
}