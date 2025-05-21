/**
****************************************************************************
* @file    EventsBuffer.c
* @author  Scherwey Roland
* @date    24 march 2018
* @brief   Implementation of Events Buffer.
****************************************************************************/

#include "EventsBuffer.h"

#include <assert.h>
#include <stdio.h>   // NULL

void EventsBuffer_addData(EventsBuffer_t *const pBuff, EventsTypes_t event)
{
  assert(0!=pBuff);
  assert(!EventsBuffer_isFull(pBuff));
  pBuff->myData[pBuff->myWrIndex] = event;
  pBuff->myWrIndex = (pBuff->myWrIndex+1) % eMAX_BUFFER_SIZE;
}

EventsTypes_t EventsBuffer_getData(EventsBuffer_t *const pBuff)
{
  assert(0!=pBuff);
  assert(!EventsBuffer_isEmpty(pBuff));
  EventsTypes_t const cEvent = pBuff->myData[pBuff->myRdIndex];

  pBuff->myRdIndex = (pBuff->myRdIndex+1) % eMAX_BUFFER_SIZE;

  return cEvent;
}

bool EventsBuffer_isEmpty(EventsBuffer_t *const pBuff)
{
  assert(0!=pBuff);
  return pBuff->myRdIndex == pBuff->myWrIndex;
}

bool EventsBuffer_isFull(EventsBuffer_t *const pBuff)
{
  assert(0!=pBuff);
  return (((pBuff->myWrIndex+1)%eMAX_BUFFER_SIZE) == pBuff->myRdIndex);
}

void EventsBuffer_init(EventsBuffer_t *const pBuf)
{
  pBuf->myWrIndex = 0;
  pBuf->myRdIndex = 0;
}


