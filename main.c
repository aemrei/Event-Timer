#include <stdio.h>
#include "timer.h"

void callback(Event *e){
  printf("Callback1: Event source:%x, target:%x, int_data:%d\n", e->source, e->target, e->int_data);
}
void callback2(Event *e){
  printf("Callback2###: Event source:%x, target:%x, int_data:%d\n", e->source, e->target, e->int_data);
}
void callback3(Event *e){
  printf("CallbackDEBUG:3:::::::::: for debug Event source:%x, target:%x, int_data:%d\n", e->source, e->target, e->int_data);
}
void callback4(Event *e){
  printf("CallbackDEBUG:4:::::::::: for debug Event source:%x, target:%x, int_data:%d\n", e->source, e->target, e->int_data);
}
void callback5(Event *e){
  printf("CallbackDEBUG:5:::::::::: for debug Event source:%x, target:%x, int_data:%d\n", e->source, e->target, e->int_data);
}
int main(int argc, char const *argv[])
{
  printf("\n\n\n\n\n\n-----------------RESTARTING------------------------\n");
  //*
  timer_init();
  uint32_t current_time = timer_getSystemClock();
  timer_setTimeoutWithData(1, &callback,0x55,44,0);
  timer_setTimeout(4, &callback2);
  timer_setIntervalWithData(8, &callback4,0x66,42,0);
  timer_setTimeout(13, &callback5);
  timer_setTimeout(20, &callback5);
  timer_printAll();

  for(int i = 0; i < 22; i++){

    timer_tictoc();
    printf("-------------------------------------------------\n");
    printf("Current time is %d\n",timer_getDelay(current_time));
  	timer_printAll();
    timer_stateMachine();
    event_stateMachine();
    printf("End of time %d\n",timer_getDelay(current_time));
    printf("-------------------------------------------------\n");

  }
  /*/
  event_init();

  event_trigger(0x01,0x02);
  event_addListener(0x02, &callback);
  event_addListener(0x02, &callback2);

  event_addListener(0x20, &callback3);
  event_addListener(0x20, &callback4);
  event_addListener(0x30, &callback3);
  event_addListener(0x30, &callback3);
  event_addListener(0x30, &callback3);
  event_addListener(0x20, &callback5);
  printf("\nEvent stacksize : %d\n", event_getSize());
  for(int i = 0; i < 5; i++){
    event_stateMachine();
    if (i < 2){
      event_trigger(i,0x02);
      event_trigger(i,0x20);
    } else if (i == 2) {
      //*
      event_removeListenerForTarget(0x20, &callback3);
      event_removeListenerForTarget(0x20, &callback4);
      event_removeListenerForTarget(0x20, &callback5);
      
      event_removeAllListenersForTarget(0x20);
      
    }
  }
  //*/
  printf("\n----------------- THE END ---------------------\n\n\n\n\n");
  return 0;
}