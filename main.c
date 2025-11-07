// Roca main file

#include "stm32f1xx.h"
#include "include.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void roca_init(void);
void ta_contact(void *pvParameters);
void ta_ecran(void *pvParameters);
void ta_compa(void *pvParameters);

int main(void){
  
  roca_init(); // Инициализация МК
  SystemInit(); // Инициализация freeRTOS  

  // Регистрируем наши задачи
  xTaskCreate(ta_contact,"contact", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(ta_ecran,"ecran", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(ta_compa,"compass", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  
  // Запускаем планировщик
  vTaskStartScheduler();
    
  return 0;
    
}

