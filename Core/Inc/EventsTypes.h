/*
 * EventsTypes.h
 *
 *  Created on: May 10, 2025
 *      Author: elias
 */

#ifndef INC_EVENTSTYPES_H_
#define INC_EVENTSTYPES_H_

typedef enum{
	eButton_start_lifting_pressed,
	eButton_start_descent_pressed,
	eButton_stop_lifting_pressed,
	eRotaryEncoder_pressed,
	eRotaryEncoder_moved_right,
	eRotaryEncoder_moved_left,
	eTimeTickElapsed_10ms,
	eNewDistanzValue,
	eNewVoltagemeasured
}EventsTypes_t;


#endif /* INC_EVENTSTYPES_H_ */
