#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#define EVENT_LISTENER_CAPACITY 15
#define EVENT_CAPACITY 256

#define TARGET_TIMER 1

// typedef __int32 int32_t;
// typedef unsigned __int32 uint32_t;
// typedef __int8 int8_t;
// typedef unsigned __int8 uint8_t;

typedef struct{
  uint8_t source;
  uint8_t target;
  uint32_t int_data;
  void *data;
}Event;

typedef void (*EventCallback)(Event*);

void event_init();

void event_stateMachine();

void event_triggerWithData(uint8_t source, uint8_t target, uint32_t int_data, void *data);

void event_trigger(uint8_t source, uint8_t target);

void event_addListener(uint8_t target, EventCallback callback);

void event_removeListenerForTarget(uint8_t target, EventCallback callback);

void event_removeAllListenersForTarget(uint8_t target);

void event_removeAllListeners();

void event_deinit();

uint8_t event_getSize();

#endif
