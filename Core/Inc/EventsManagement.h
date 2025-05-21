/**
  ****************************************************************************
  * @file    EventsManagement.h
  * @author  Elias Jüni
  * @date    24 march 2020
  * @brief   Manage the system events for the FSM using LIFO (stack)
  ****************************************************************************/

#ifndef EVENTSMANAGEMENT_H_
#define EVENTSMANAGEMENT_H_

#include "EventsBuffer.h"

#include <stdint.h>
#include <stdbool.h>

/// @brief  Used to get a button event from the stack
/// @param  pBuff pointer to Events Buffer to use
/// @param  pEvent Address to store the event popped if any is available
/// @return true if there is a new event, false otherwise
bool eventsManagement_Pop(EventsBuffer_t* const pBuff, EventsTypes_t * const pEvent);

/// @brief  Used to store an event to the indicated buffer
/// @param  pBuff pointer to Events Buffer to use
/// @param  event which needs to be stored
void eventsManagement_Push(EventsBuffer_t* const pBuff, EventsTypes_t event);

#endif /* EVENTSMANAGEMENT_H_ */
