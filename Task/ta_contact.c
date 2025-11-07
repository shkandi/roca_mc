// Roca задача коммуникации
#include "stm32f1xx.h"
#include "include.h"

extern void ua_work(void);


// Задача коммуникации
void ta_contact(void *pvParameters){
  
    
  while(1){
    
    ua_work(); // Обрабатываем запросы по uart
    
    delay(TIME_MS * 10);
    
    asm("nop");
    asm("nop");
    asm("nop");
    
  }
}
