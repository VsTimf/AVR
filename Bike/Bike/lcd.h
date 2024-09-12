#ifndef LCD_H_
#define LCD_H_

#include "lcd_lh155ba.h"



//#define SCREEN_LIGHT_ON		(PORTD |= _BV(PD1))
//#define SCREEN_LIGHT_OFF	(PORTD &= ~_BV(PD1))
//#define SCREEN_LIGHT_TOGGLE (PORTD ^= _BV(PD1))



void _lcd_init(void);

void _lcd_show_speed(uint8_t speed);
void _lcd_show_curr(float curr, float volt);
void _lcd_show_cap(float cap);
void _lcd_show_odometr(float odo);

void _lcd_show_time(void);
void _lcd_hide_time_pointers(void);

void _lcd_show_charging(void);
void _lcd_hide_charging(void);

void _lcd_show_battery(float low, float high, float curr);

void _lcd_show_settings(uint8_t setting, uint16_t param);
void _lcd_show_speed_setup(void);
void _lcd_show_pulse_counter(uint16_t cnt);
void _lcd_show_batcell_setup(uint16_t cells);

#endif // LCD_H_