/**
  ****************************************************************************
  * @file    EventsManagement.c
  * @author  Elias Jüni
  * @date    24 march 2018
  * @brief   Manage the system events
  ****************************************************************************/

#include "EventsManagement.h"

#include <stdint.h>
#include <assert.h>
#include <stdio.h>   // NULL

//----------------------------------------------------------------------------
// local definitions
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Implementations
//----------------------------------------------------------------------------

void eventsManagement_Push(EventsBuffer_t* const pBuff, EventsTypes_t event)
{
  assert(0!=pBuff);
  // Add event if not full
  if(!EventsBuffer_isFull(pBuff))
  {
    EventsBuffer_addData(pBuff,event);
  }
}

bool eventsManagement_Pop(EventsBuffer_t* const pBuff, EventsTypes_t * const pEvent)
{
  assert(NULL!=pBuff);
  assert(NULL!=pEvent);

  bool const cHasEvent = !EventsBuffer_isEmpty(pBuff);

  // If an event is pending
  if(cHasEvent)
  {
    *pEvent = EventsBuffer_getData(pBuff);
  }

  return cHasEvent;
}
