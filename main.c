// Roca main file

#include "stm32f1xx.h"
#include "include.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void roca_init(void);
void ta_ecran(void *pvParameters);
void ta_compass(void *pvParameters);

int main(void){
  
  roca_init(); // Инициализация МК
  SystemInit(); // Инициализация freeRTOS  

  // Регистрируем наши задачи
  xTaskCreate(ta_ecran,"ecran", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  xTaskCreate(ta_compass,"compass", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

  // Запускаем планировщик
  vTaskStartScheduler();

}

