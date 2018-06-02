#include <stdlib.h>
#include "event.h"
#include <stdio.h>//aemre_debug

#define getListenerIndex(x) ((x < EVENT_LISTENER_CAPACITY && x >= 0) ? x : EVENT_LISTENER_CAPACITY)
#define getNextEventIndex(x) ((x + 1) % EVENT_CAPACITY)
#define getPrevEventIndex(x) ((x - 1) % EVENT_CAPACITY)

struct EventListenerContainer{
  uint8_t target;
  EventCallback callback;
  struct EventListenerContainer *next;
};
typedef struct EventListenerContainer EventListenerContainer;

static EventListenerContainer *eventListenerList[EVENT_LISTENER_CAPACITY + 1];
static Event *eventList[EVENT_CAPACITY];

static uint8_t front = 0;
static uint8_t end = 0;
static uint8_t initialized = 0;

void event_init(){
  if (initialized){
    return;
  }
  front = 0;
  end = 0;
  initialized = !initialized;
  //memset(eventListenerList, 0, sizeof(eventListenerList));
  for ( int i = 0; i < EVENT_LISTENER_CAPACITY; i++){
    eventListenerList[i] = 0;
  }
}

void event_stateMachine(){
  if (event_getSize()) {

    Event *e = eventList[front];
    front = getNextEventIndex(front);

    printf("Checking whether a listener for target %d\n",e->target);

    if (e->target < EVENT_LISTENER_CAPACITY) {
      EventListenerContainer *c = eventListenerList[e->target];
      while (c){
        if (c->callback){
          (*c->callback)(e);
        }
        c = c->next;
      }
    }
    else {
      EventListenerContainer *c = eventListenerList[EVENT_LISTENER_CAPACITY];
      while (c){
        if (c->callback && (c->target == e->target)) {
          (*c->callback)(e);
        }
        c = c->next;
      }
    }
    free(e);
  }
}

void event_triggerWithData(uint8_t source, uint8_t target, uint32_t int_data, void *data){
  Event *e;
  uint8_t next_index;
  printf("Trying to trigger new data with source %d and target %d and int_data %d.\n", source, target, int_data);
  next_index = getNextEventIndex(end);
  if (next_index == front) {
    //no space for new event
    printf("ERROR: There is no space for new event!\n");
    return;
  }
  e = (Event *) malloc(sizeof(Event));
  if (e) {
    //if allocation is successful
    e->source = source;
    e->target = target;
    e->int_data = int_data;
    e->data = data;
    eventList[end] = e;
    end = next_index;
  }
}

void event_trigger(uint8_t source, uint8_t target){
  event_triggerWithData(source, target, -1, 0);
}

void event_addListener(uint8_t target, EventCallback callback){
  uint8_t index;
  EventListenerContainer *n;//for iterations
  EventListenerContainer *c = (EventListenerContainer *) malloc(sizeof(EventListenerContainer));

  index = getListenerIndex(target);
  
  if (c) {//if new space is allocated
    c->target = target;
    c->callback = callback;
    c->next = 0;
    
    n = eventListenerList[index];
    if(!n){ //if stack is empty
      printf("Initialized for index %d\n",c->target);
      eventListenerList[index] = c;
    } else {
      while (n->next){
        n = n->next;
      }
      printf("Appended to index %d\n",c->target);
      n->next = c;
    }
  }

}

void event_removeListenerForTarget(uint8_t target, EventCallback callback){
  uint8_t index;
  EventListenerContainer *c;
  EventListenerContainer *tmp;
  EventListenerContainer *head;
  index = getListenerIndex(target);
  head = eventListenerList[index];
  printf("Will be checked for listener with the target and callback\n");
  if (head){
    c = head;
    while(c->next){
      tmp = c->next;
      if ((tmp->target == target) && (tmp->callback == callback)) {
        printf("INFO: Found exact callback, now removing...\n");
        c->next = tmp->next;
        free(tmp);
      } else {
        c = c->next;
      }
    }
    if ((head->target == target) && (head->callback == callback)) {
      printf("INFO: Found exact callback in the head, now removing...\n");
      eventListenerList[index] = head->next;
      free(head);
    }
  }
}

void event_removeAllListenersForTarget(uint8_t target){
  uint8_t index;
  EventListenerContainer *c;
  EventListenerContainer *tmp;
  EventListenerContainer *head;
  index = getListenerIndex(target);
  head = eventListenerList[index];
  printf("Will be checked for listener with the target and callback\n");
  if (head){
    c = head;
    while(c->next){
      tmp = c->next;
      if (tmp->target == target) {
        printf("INFO: Found exact callback, now removing...\n");
        c->next = tmp->next;
        free(tmp);
      } else {
        c = c->next;
      }
    }
    if (head->target == target) {
      printf("INFO: Found exact callback in the head, now removing...\n");
      eventListenerList[index] = head->next;
      free(head);
    }
  }
}

void event_removeAllListeners(){

}

void event_deinit(){
}

uint8_t event_getSize(){
  return (end >= front) ? end - front : front - end + EVENT_CAPACITY;
}