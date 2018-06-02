#include <stdlib.h>
#include <stdio.h>
#include "timer.h"
#include "event.h"


struct TimerObject {
  uint8_t id;
  uint32_t originalDelay;
  uint32_t remainingDelay;
  EventCallback callback;
  uint32_t int_data;
  uint32_t target;
  void *data;
  struct TimerObject *next;
};

typedef struct TimerObject TimerObject;

static uint32_t sys_clk = 0;
static TimerObject *head = 0;

TimerObject *timer_getPrevById(uint8_t id){
  if (head) {
    TimerObject *tmp = head;
    while (tmp->next) {
      if (tmp->next->id != id) {
        return tmp;
      }
      tmp = tmp->next;
    }
  }
  return 0;
}

TimerObject *timer_getById(uint8_t id){
  TimerObject *tmp = head;
  while(tmp){
    if(tmp->id == id){
      return tmp;
    }
    tmp = tmp->next;
  }
  return 0;
}

void timer_addToList(TimerObject *o){
  if (head == 0) {
    printf("Adding to head as a single element!\n");
    head = o;
  } else {
    TimerObject *tmp = head;
    if (o->remainingDelay <= head->remainingDelay) {
      printf("Adding to head!\n");
      head->remainingDelay -= o->remainingDelay;
      o->next = head;
      head = o;
    } else {
      o->remainingDelay -= head->remainingDelay;
      printf("Adding to somewhere!\n");
      while ((tmp->next) && (o->remainingDelay > tmp->next->remainingDelay)) {
    		printf("Updating this one's remainingDelay (subtracting)!\n");
        o->remainingDelay -= tmp->next->remainingDelay;
        tmp = tmp->next;
      }
      if(tmp->next){
      	printf("Updating the next one's remainingDelay!\n");
        tmp->next->remainingDelay -= o->remainingDelay;
      }
      o->next = tmp->next;
      tmp->next = o;
    }
  }
  printf("Now: head->id:%d!\n",head->id);
}

void timer_callback(Event *e){
  TimerObject *tmp = (TimerObject *) e->data;
  e->data = (void *) tmp->data;
  e->source = e->target;
  e->target = tmp->target;
  e->int_data = tmp->int_data;

  (*tmp->callback)(e);

  if(tmp->originalDelay){//if this is an interval definition
  	tmp->remainingDelay = tmp->originalDelay;
    timer_addToList(tmp);
  } else {
    free(tmp);
  }
  printf("Now in callback EOF!\n");
}

void timer_init(){
  event_init();
  event_addListener(TARGET_TIMER, &timer_callback);
}

void timer_tictoc(){
  sys_clk++;
  if((head) && (head->remainingDelay > 0)){
    head->remainingDelay--;
  }
}

uint32_t timer_getSystemClock(){
  return sys_clk;
}

void timer_mDelay(uint32_t delay){
  uint32_t start_time = timer_getSystemClock();
  while(timer_getSystemClock() < delay){
  }
}

void timer_stateMachine(){
  TimerObject *tmp;
  if (head && (head->remainingDelay == 0)){
    printf("INFO: Timeout for id %d..\n",head->id);
    event_triggerWithData(0, TARGET_TIMER, (uint32_t)head->id, (void *)head);
    printf("INFO: Timer event id is triggered. %d\n",head->id);
    head = head->next;
  }
}

uint32_t timer_getDelay(uint32_t start_time){
  return (sys_clk >= start_time) ? (sys_clk - start_time) : (sys_clk + (uint32_t)(-1)) - start_time;
}

uint8_t timer_createGenericTimerObject(uint32_t originalDelay, uint32_t remainingDelay, EventCallback callback, uint32_t target, uint32_t int_data, void *data){
  static uint8_t id_counter = 1;
  TimerObject *tmp = (TimerObject *) malloc(sizeof(TimerObject));

  while(timer_getById(id_counter)){
    id_counter++;//if exists before then increase the counter
  }
  printf("Creating new timer object with id %d...\n", id_counter);
  if(tmp){//if allocation is successfully completed
    tmp->id = id_counter;
    tmp->originalDelay = originalDelay;
    tmp->remainingDelay = remainingDelay;
    tmp->callback = callback;
    tmp->target = target;
    tmp->int_data = int_data;
    tmp->data = data;
    tmp->next = 0;
    timer_addToList(tmp);
  }
}

uint8_t timer_setTimeoutWithData(uint32_t delay, EventCallback callback, uint32_t target, uint32_t int_data, void *data){
  timer_createGenericTimerObject(0, delay, callback, target, int_data, data);
}

uint8_t timer_setTimeout(uint32_t delay, EventCallback callback){
  return timer_setTimeoutWithData(delay, callback, 0, -1, 0);
}

uint8_t timer_setIntervalWithData(uint32_t delay, EventCallback callback, uint32_t target, uint32_t int_data, void *data){
  timer_createGenericTimerObject(delay, delay, callback, target, int_data, data);
}

uint8_t timer_setInterval(uint32_t delay, EventCallback callback){
  return timer_setIntervalWithData(delay, callback, 0, -1, 0);
}

void timer_clear(uint8_t id){
  TimerObject *tmp = head;
  TimerObject *tmp2 = 0;
  if(!tmp){//if nothing is there, just return
    return;
  }
  if(head->id == id){
    if (head->next) {
      head->next->remainingDelay += head->remainingDelay;
    }
    head = head->next;
  } else {
    while(tmp->next->id != id){
      tmp = tmp->next;
      if(!tmp->next){//if could not found, then return;
        return;
      }
    }
    tmp2 = tmp;
    tmp2->next = tmp2->next->next;
    tmp2->next->remainingDelay += tmp->remainingDelay;
  }
  free(tmp);
}

void timer_printAll() {
	TimerObject *tmp = head;
	printf("{\n");
	while (tmp) {
		printf("    %p:{id: %d, originalDelay: %d, remainingDelay: %d, int_data: %d, target: %d, next: %p}\n"
			,tmp, tmp->id, tmp->originalDelay, tmp->remainingDelay, tmp->int_data, tmp->target, tmp->next);
		tmp = tmp->next;
	}
	printf("};\n");
}
