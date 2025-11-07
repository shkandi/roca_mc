// Roca файл инициализации
#include "stm32f1xx.h"
#include "include.h"

char init_clk(void);
void init_variable(void);
void init_gpio(void);
void init_i2c1(void);
void init_spi2(void);
void init_ecran(void);


// Функция запцуска и инициализации
void roca_init(void){
  
  init_clk(); // Настраиваем тактирование
  init_variable(); // Инициализируем переменные
  init_gpio(); // Инициализируем пины
  init_i2c1(); // Инициализируем i2c1
  init_spi2(); // Инициализируем spi2 под экран
  init_ecran(); // Инициализируем экран
  
  event_log(EVENT_IN, EVENT_SYSTEM_START, 0x01, 0); // Делаем запись в логе
  return;
}



// Инициализация глобальных переменных
void init_variable(void){

  // Заполняем рабочие регистры
  ar_reg[0x00] = SET_ID_DATA;
  ar_reg[0x01] = SET_FW_VERSION; 
  ar_reg[0x02] = SET_FW_SUB_VERSION;
  
  ar_reg[0x09] = (SET_LOG_SIZE & 0xff00) >> 8;
  ar_reg[0x10] = SET_LOG_SIZE & 0xff;
  
  ar_reg[0x15] = (SET_COMPASS_START & 0xff00) >> 8;
  ar_reg[0x16] = SET_COMPASS_START;
  
  // Задаем указатели
  p_ufdate = (u32*)&ar_reg[0x11]; // Указатель на дату и время
  p_status = &ar_reg[0x06]; // Указатель на Status (состояние)
  p_compa = (u16*)&ar_reg[0x15]; // Указатель на значение компасса
  
  return;
}



// Инициализация пинов кроме интерфейсных
void init_gpio(void){
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // включаем тактирование порта А
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;	// включаем тактирование порта B
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;	// включаем тактирование порта C
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // включаем тактирование альтернативных функций

  // пины для разного
  //GPIOA->CRL |= GPIO_CRL_MODE7_1; // настроили ножку A7 на выход
  //GPIOA->CRL &= ~GPIO_CRL_CNF7; // настроили ножку A7 на основное назначение и push-pull	
  //GPIOA->CRL |= GPIO_CRL_MODE6_1; // настроили ножку A6 на выход
  //GPIOA->CRL &= ~GPIO_CRL_CNF6; // настроили ножку A6 на основное назначение и push-pull	

  // Led
  GPIOC->CRH |= GPIO_CRH_MODE13_1; // настроили ножку C13 на выход
  GPIOC->CRH &= ~GPIO_CRH_CNF13; // настроили ножку B0 на основное назначение и push-pull
  GPIOC->BSRR |= GPIO_BSRR_BS13; // выставили начальное значение для led 
    
  //GPIOB->CRL |= GPIO_CRL_MODE0_1; // настроили ножку B0 на выход
  //GPIOB->CRL &= ~GPIO_CRL_CNF0; // настроили ножку B0 на основное назначение и push-pull
  //GPIOB->CRL |= GPIO_CRL_MODE1_1; // настроили ножку В1 на выход
  //GPIOB->CRL &= ~GPIO_CRL_CNF1; // настроили ножку B1 на основное назначение и push-pull	
  //GPIOB->BSRR |= GPIO_BSRR_BS0; //выставили начальное значение для двуцветного led 
  //GPIOB->BSRR |= GPIO_BSRR_BR1;	//выставили начальное значение для двуцветного led
  
  // Пины для работы с экраном
  GPIOB->CRH |= GPIO_CRH_MODE8_1; // настроили ножку B8 на выход
  GPIOB->CRH &= ~GPIO_CRH_CNF8; // настроили ножку В8 на основное значение и push-pull
  GPIOB->CRH |= GPIO_CRH_MODE9_1; // настроили ножку B9 на выход
  GPIOB->CRH &= ~GPIO_CRH_CNF9; // настроили ножку В9 на основное значение и push-pull
  GPIOB->CRL |= GPIO_CRL_MODE5_1; // настроили ножку B5 на выход
  GPIOB->CRL &= ~GPIO_CRL_CNF5; // настроили ножку В5 на основное значение и push-pull	
  GPIOB->BSRR |= GPIO_BSRR_BR8;	//выставили начальное значение 0 для В8
  GPIOB->BSRR |= GPIO_BSRR_BR9;	//выставили начальное значение 0 для В9
  GPIOB->BSRR |= GPIO_BSRR_BS5;	//выставили начальное значение 1 для В5 потому что RESET для экрана
  return;
}



char init_clk(void){
  u16 cnt;
  
  // Запускаем HSE
  RCC->CR |= (1 << RCC_CR_HSEON_Pos);   
  for (cnt = 0; cnt < TOUT_CLK_SETUP; cnt++){
    if (RCC->CR & (1 << RCC_CR_HSERDY_Pos))
      break;
  }
  if (cnt >= TOUT_CLK_SETUP){
    RCC->CR &= ~(1 << RCC_CR_HSEON_Pos);   
    return 1;
  }
  
  
  // Запускаем PLL
  RCC->CFGR |= (0x07 << RCC_CFGR_PLLMULL_Pos) | (0x01 << RCC_CFGR_PLLSRC_Pos);
  RCC->CR |= (1 << RCC_CR_PLLON_Pos);
  for (cnt = 0; cnt < TOUT_CLK_SETUP; cnt++){
    if (RCC->CR & (1 << RCC_CR_PLLRDY_Pos))
      break;
  }
  if (cnt >= TOUT_CLK_SETUP){
    RCC->CR &= ~(1 << RCC_CR_HSEON_Pos);   
    RCC->CR &= ~(1 << RCC_CR_PLLON_Pos);   
    return 2;
  }
  
  
  // Настраиваем Flash
  FLASH->ACR |= (0x02 << FLASH_ACR_LATENCY_Pos);
  RCC->CFGR |= (0x00 << RCC_CFGR_PPRE2_Pos) | (0x04 << RCC_CFGR_PPRE1_Pos) | (0x00 << RCC_CFGR_HPRE_Pos);
  
  RCC->CFGR |= (0x02 << RCC_CFGR_SW_Pos);
  
  // Ждем пока переключится на внешний источник
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0x02 << RCC_CFGR_SWS_Pos)) { }
  
  // Отключаем внутренний RC
  RCC->CR &= ~(1 << RCC_CR_HSION_Pos);
  
  return 0;
   
}

