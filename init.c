// Roca файл инициализации
#include "stm32f1xx.h"
#include "include.h"

void init_variable(void);
void init_gpio(void);
void init_i2c1(void);
void init_spi2(void);
void init_ecran(void);


// Функция запцуска и инициализации
void roca_init(void){
  
  init_variable(); // Инициализируем переменные
  init_gpio(); // Инициализируем пины
  
  init_i2c1(); // Инициализируем i2c1
  init_spi2(); // Инициализируем spi2 под экран
    
  
  
  init_ecran(); // Инициализируем экран
  
  return;
}



// Инициализация глобальных переменных
void init_variable(void){

  
  // Заполняем рабочие регистры
  arr_reg[0] = FW_VERSION;
  arr_reg[1] = FW_SUB_VERSION;
  
  
}



// Инициализация вне аппаратных пинов для МК тут
void init_gpio(void){
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // включаем тактирование порта А
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	// включаем тактирование порта B
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // включаем тактирование альтернативных функций на порт B

  // пины для разного
  GPIOA->CRL |= GPIO_CRL_MODE7_1; // настроили ножку A7 на выход
  GPIOA->CRL &= ~GPIO_CRL_CNF7; // настроили ножку A7 на основное назначение и push-pull	
  GPIOA->CRL |= GPIO_CRL_MODE6_1; // настроили ножку A6 на выход
  GPIOA->CRL &= ~GPIO_CRL_CNF6; // настроили ножку A6 на основное назначение и push-pull	

  // Led
  GPIOB->CRL |= GPIO_CRL_MODE0_1; // настроили ножку B0 на выход
  GPIOB->CRL &= ~GPIO_CRL_CNF0; // настроили ножку B0 на основное назначение и push-pull
  GPIOB->CRL |= GPIO_CRL_MODE1_1; // настроили ножку В1 на выход
  GPIOB->CRL &= ~GPIO_CRL_CNF1; // настроили ножку B1 на основное назначение и push-pull	
  GPIOB->BSRR |= GPIO_BSRR_BS0; //выставили начальное значение для двуцветного led 
  GPIOB->BSRR |= GPIO_BSRR_BR1;	//выставили начальное значение для двуцветного led
  
  // дополнительные пины для работы с экраном
  GPIOB->CRH |= GPIO_CRH_MODE8_1; // настроили ножку B8 на выход
  GPIOB->CRH &= ~GPIO_CRH_CNF8; // настроили ножку В8 на основное значение и push-pull
  GPIOB->CRH |= GPIO_CRH_MODE9_1; // настроили ножку B9 на выход
  GPIOB->CRH &= ~GPIO_CRH_CNF9; // настроили ножку В9 на основное значение и push-pull
  GPIOB->CRL |= GPIO_CRL_MODE5_1; // настроили ножку B5 на выход
  GPIOB->CRL &= ~GPIO_CRL_CNF5; // настроили ножку В5 на основное значение и push-pull	
  GPIOB->BSRR |= GPIO_BSRR_BR8;	//выставили начальное значение 0 для В8
  GPIOB->BSRR |= GPIO_BSRR_BR9;	//выставили начальное значение 0 для В9
  GPIOB->BSRR |= GPIO_BSRR_BS5;	//выставили начальное значение 1 для В5 потому что RESET для экрана
}

