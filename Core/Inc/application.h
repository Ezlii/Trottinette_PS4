/*
 * application.h
 *
 *  Created on: Apr 24, 2025
 *      Author: elias
 */

#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include "EventsTypes.h"
#include "FSM_Handler.h"
#include "EventsBuffer.h"
#include "stm32wbxx_it.h"

#define SW_PRESSED 0
#define nullptr 0
#define TICK_COUNT_10ms 10

void application(void);

void TrotinettControlTask(EventsBuffer_t* pBuff);

// FSM-Funktion zur Verarbeitung von Events
void Trotinette_FSM(EventsTypes_t event);

void TrotinettControlTask(EventsBuffer_t* pBuff);

// Gibt den aktuellen FSM-Zustand zurück
FSM_States_t MotorControl_getActualState(void);

#endif /* INC_APPLICATION_H_ */
