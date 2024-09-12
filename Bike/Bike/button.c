#include "button.h"

#include "avr/io.h"


uint8_t _value;
BUTTON_STATE_T _state = UNPRESSED;
BUTTON_PRESS_TYPE_T _press_type = NO_PRESS;
uint16_t _counter = 0;


static uint8_t button_state(){return ((PIND&_BV(PD3)) == 0);}


uint8_t button_is_pressed(BUTTON_PRESS_TYPE_T press_type){if(_press_type == press_type){_press_type = NO_PRESS; return 1;} return 0;}
void button_reset(){_press_type = NO_PRESS;}


BUTTON_PRESS_TYPE_T button_process()
{
	_value = button_state();

	switch (_state)
	{
		case UNPRESSED:

		_counter = (_value) ? _counter+1 : 0;

		if(_counter > DEBOUNCE_TIME_MS)
		{
			_state = PRESSED;
			_press_type = B_CLICK;
		}
		break;
		
		case PRESSED:

		if(_value)
		{
			_counter++;

			if(_counter >= LONG_PRESS_TIME_MS)
			{
				_press_type = B_LONG;
				_state = WAIT_FOR_RELEASE;
			}
		}
		else
		{
		//	if(_click_release_en)
			_press_type = B_CLICK_RELEASE;
			
			_counter = 0;
			_state = UNPRESSED;
		}
		break;
		
		case WAIT_FOR_RELEASE:
		if(!_value)
		{
			_counter = 0;
			_state = UNPRESSED;
		}
		break;
	}
	
	return _press_type;
}