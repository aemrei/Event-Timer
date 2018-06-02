#include "event.h"

void timer_init();

void timer_tictoc();

uint32_t timer_getSystemClock();

void timer_stateMachine();

uint32_t timer_getDelay(uint32_t start_time);

uint8_t timer_setTimeoutWithData(uint32_t delay, EventCallback callback, uint32_t target, uint32_t int_data, void *data);

uint8_t timer_setTimeout(uint32_t delay, EventCallback callback);

uint8_t timer_setIntervalWithData(uint32_t delay, EventCallback callback, uint32_t target, uint32_t int_data, void *data);

uint8_t timer_setInterval(uint32_t delay, EventCallback callback);

void timer_clear(uint8_t id);

void timer_printAll();
