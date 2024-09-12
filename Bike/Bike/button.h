 #ifndef _BUTTON_H
 #define _BUTTON_H



#include "stdint.h"

#include "avr/io.h"

typedef enum _BUTTON_PRESS_TYPE
{
	NO_PRESS,
	B_CLICK,
	B_CLICK_RELEASE,
	B_LONG
}BUTTON_PRESS_TYPE_T;


enum _BUTTON_TIMING
{
	DEBOUNCE_TIME_MS = 3,
	LONG_PRESS_TIME_MS = 300
};
	
typedef enum _BUTTON_STATE
{
	UNPRESSED = 0,
	PRESSED = 1,
	WAIT_FOR_RELEASE = 2
}BUTTON_STATE_T;






BUTTON_PRESS_TYPE_T button_process(void);

uint8_t button_is_pressed(BUTTON_PRESS_TYPE_T press_type);
void button_reset();



#endif //_BUTTON_H

