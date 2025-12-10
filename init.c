// Roca файл инициализации
#include "stm32f1xx.h"
#include "include.h"

extern u8 u2sb[SET_UART2_SBUF_SIZE];
extern u8 u2rb[SET_UART2_RBUF_SIZE];

char init_clk(void);
void init_variable(void);
void init_gpio(void);
void init_i2c1(void);
void init_spi2(void);
void init_ecran(void);
void init_uart1(void);
void init_uart2(void);
void init_dma(void);


// Функция запцуска и инициализации
void roca_init(void){
  
  init_clk(); // Настраиваем тактирование
  init_variable(); // Инициализируем переменные
  init_gpio(); // Инициализируем пины
  init_i2c1(); // Инициализируем I2C1
  init_spi2(); // Инициализируем SPI2 под экран
  init_uart1(); // Инициализируем UART1 для PLM
  init_uart2(); // Инициализируем UART2 для ПК
  init_dma(); // Иницилизируем DMA
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
  
  // Задаем указатели
  p_status = &ar_reg[0x03]; // Указатель на Status (состояние)
  p_ufdate = (u32*)&ar_reg[0x10]; // Указатель на дату и время
  p_compa = (u16*)&ar_reg[0x14]; // Указатель на значение компасса
  
  // Отладка
  p_a0 = (u16*)&ar_reg[0x20];
  p_a1 = (u16*)&ar_reg[0x22];
  p_a2 = (u16*)&ar_reg[0x24];
  p_a3 = (u16*)&ar_reg[0x26];
  p_a4 = (u16*)&ar_reg[0x28];
  p_a5 = (u16*)&ar_reg[0x2a];
  
  p_b0 = (u16*)&ar_reg[0x2c];
  p_b1 = (u16*)&ar_reg[0x2e];
  p_b2 = (u16*)&ar_reg[0x30];
  p_b3 = (u16*)&ar_reg[0x32];
  p_b4 = (u16*)&ar_reg[0x34];
  p_b5 = (u16*)&ar_reg[0x36];
  
  p_c0 = (u16*)&ar_reg[0x38];
  p_c1 = (u16*)&ar_reg[0x3a];
  p_c2 = (u16*)&ar_reg[0x3c];
  p_c3 = (u16*)&ar_reg[0x3e];
  p_c4 = (u16*)&ar_reg[0x40];
  p_c5 = (u16*)&ar_reg[0x42];
  
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
  
  // Вывод такта на MCO
  /*
  GPIOA->CRH &= ~(GPIO_CRH_MODE8_Msk | GPIO_CRH_CNF8_Msk);
  GPIOA->CRH |= ((0x03 << GPIO_CRH_MODE8_Pos) | (0x02 << GPIO_CRH_CNF8_Pos));
  RCC->CFGR &= ~(RCC_CFGR_MCO_Msk);
    
  RCC->CFGR &= ~(0x07 << RCC_CFGR_MCO_Pos); // Сбрасываем MCO
  RCC->CFGR |= (0x04 << RCC_CFGR_MCO_Pos); // Выставляем MCO
  */

  
  return;
}



char init_clk(void){
  u16 cnt;
  
  // Запускаем HSE
  RCC->CR &= ~(1 << RCC_CR_HSEON_Pos);   
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
  RCC->CFGR |= (0x02 << RCC_CFGR_PLLMULL_Pos); // Множитель PLL (стоит 8MHz * 4 = 32Mhz)
  RCC->CFGR |= (0x01 << RCC_CFGR_PLLSRC_Pos); // Источник PLL
  RCC->CR |= (0x01 << RCC_CR_PLLON_Pos); // Включаем PLL
    
  for (cnt = 0; cnt < TOUT_CLK_SETUP; cnt++){
    if (RCC->CR & (1 << RCC_CR_PLLRDY_Pos))
      break;
  }
  if (cnt >= TOUT_CLK_SETUP){
    RCC->CR &= ~(1 << RCC_CR_HSEON_Pos);   
    RCC->CR &= ~(1 << RCC_CR_PLLON_Pos);   
    return 2;
  }
  
  FLASH->ACR |= (0x01 << FLASH_ACR_LATENCY_Pos); // Настройка flash (стоит для 32MHz)
  //RCC->CFGR |= (0x00 << RCC_CFGR_PPRE2_Pos); // Делитель для шины APB2 (стоит 0)
  //RCC->CFGR |= (0x00 << RCC_CFGR_PPRE1_Pos); // Делитель для шины APB1 (стоит 0)
  //RCC->CFGR |= (0x00 << RCC_CFGR_HPRE_Pos); // Делитель для шины AHB (стоит 0)
  RCC->CFGR |= (0x02 << RCC_CFGR_SW_Pos); // Источник тактирования MCU (стоит PLL)
   
  // Ждем пока переключится на внешний источник
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0x02 << RCC_CFGR_SWS_Pos)) { }
  
  // Отключаем внутренний RC
  RCC->CR &= ~(1 << RCC_CR_HSION_Pos);
    
  return 0;
   
}



void init_dma(void){
  
  RCC->AHBENR |= (1 << RCC_AHBENR_DMA1EN_Pos); // Включаем тактирование DMA1
  
  // Настраиваем для UART2 прием
  DMA1_Channel6->CCR |= (1 << DMA_CCR_MINC_Pos); // Инкремент памяти
  DMA1_Channel6->CNDTR = SET_UART2_RBUF_SIZE; // Сколько байт принимает
  DMA1_Channel6->CPAR = (uint32_t)&USART2->DR; // Адрес переферии
  DMA1_Channel6->CMAR = (uint32_t)u2rb; // Адрес памяти
  DMA1_Channel6->CCR |= (1 << DMA_CCR_EN_Pos); // Включаем DMA
  
  // Настраиваем для UART2 передача
  DMA1_Channel7->CCR |= (1 << DMA_CCR_MINC_Pos); // Инкремент памяти
  DMA1_Channel7->CCR |= (1 << DMA_CCR_DIR_Pos); // Направление из памяти в UART2
  DMA1_Channel7->CNDTR = 0; // Сколько байт передаем (ставим в 0)
  DMA1_Channel7->CPAR = (uint32_t)&USART2->DR; // Адрес переферии
  DMA1_Channel7->CMAR = (uint32_t)u2sb; // Адрес памяти
  //DMA1_Channel7->CCR |= (1 << DMA_CCR_EN_Pos); // Включаем DMA
   
  return;
   
}

