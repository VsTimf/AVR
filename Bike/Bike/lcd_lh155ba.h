#ifndef LCD_LH155BA_H_
#define LCD_LH155BA_H_

#include <avr/io.h>
#include <util/delay.h>



#define LCDPORT		PORTB
#define WDELAY 1
#define LCD_X_MAX	0x0F
#define LCD_Y_MAX	0x3F

#define SCL		(1 << PB3)		// serial clock line
#define SDA		(1 << PB4)		// serial Data line
#define RS		(1 << PB1)		// Used to identify data sent by MCU at D0 to D7
#define RES		(1 << PB2)		// Reset line		active low
//#define CS		(1 << PD4)		// Chip enable 		active low
//#define PS		(1 << PD5)		// H=Parallel/L=Serial line




/////////////////////////////////////////////////////////////////////////////////////////////////
void LCD_write_byte(unsigned char c, int data_command);

void LCD_init();								// Init Display, power it on etc.

void LCD_pos_0();								//set to upper left corner
void LCD_y_pos(unsigned char y);
void LCD_x_pos(unsigned char x);
void LCD_xy_pos(uint8_t x, uint8_t y);

void LCD_clear_display();

void LCD_putc(char c);
void LCD_printf(char* str);
void LCD_draw_image (unsigned char x,unsigned char y,char* Pimage);

/*void LCD_print_2x(uint8_t number);
void LCD_printf_2x(char* str);
void LCD_put2xc(char c);
void LCD_printf_2x(char* str);*/

void LCD_put_digit(uint8_t dig);

void LCD_ON(void);
void LCD_OFF(void);



typedef struct _LCD
{
	unsigned char x_pos;
	unsigned char y_pos; 		
}LCD_T;


#endif /* LCD_LH155BA_H_ */