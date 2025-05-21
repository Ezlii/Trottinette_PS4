/**
  ****************************************************************************
  * @file    EventsBuffer.h
  * @author  Roland Scherwey
  * @date    24 march 2018
  * @brief   Declaration of Events Buffer.
  ****************************************************************************/

#ifndef EVENTS_BUFFER_H_
#define EVENTS_BUFFER_H_

#include "EventsTypes.h"

#include <stdint.h>
#include <stdbool.h>

enum
{
  eMAX_BUFFER_SIZE = 100
};

typedef struct
{
  EventsTypes_t myData[eMAX_BUFFER_SIZE];
  uint16_t myWrIndex;
  uint16_t myRdIndex;

} EventsBuffer_t;
/// @brief  This method allows to add new event to the ring buffer
/// @note   It must be checked prior if the buffer is not full,
///         otherwise an assertion will occur
/// @param  pBuf - ring buffer pointer
/// @param  event - event to add
void EventsBuffer_addData(EventsBuffer_t *const pBuf, EventsTypes_t event);

/// @brief  This method allows to get data from the ring buffer
/// @note   It must be checked prior if the buffer is not empty,
///         otherwise an assertion will occur
/// @param  pBuf - ring buffer pointer
/// @return available event from the ring buffer
EventsTypes_t EventsBuffer_getData(EventsBuffer_t *const pBuf);

/// @brief  This method allows to check if the ring buffer is empty
///         so that getDataFromBuffer() can be safely called
/// @param  pBuf - ring buffer pointer
/// @return true if the ring buffer does not hold any stored data
///         false if the ring buffer holds stored data
bool EventsBuffer_isEmpty(EventsBuffer_t *const pBuf);

/// @brief  This method allows to check if the ring buffer is full
/// @param  pBuf - ring buffer pointer
/// @return true if the ring buffer is full
///         false if the ring buffer is not yet full so that
///         addDataToBuffer() can be safely called
bool EventsBuffer_isFull(EventsBuffer_t *const pBuf);

/// @brief  This method allows to initialize the ring buffer
/// @param  pBuf - ring buffer pointer
void EventsBuffer_init(EventsBuffer_t *const pBuf);

#endif /* EVENTS_BUFFER_H_ */
