// Roca задача работы с компасом
#include "stm32f1xx.h"
#include "include.h"

extern u16 cpn_ang;

// Задача работы с компассом
void ta_compa(void *pvParameters){
  
    
  while(1){
    
    cpn_ang = *p_compa;
    
    delay(TIME_MS * 10);
  }
}
