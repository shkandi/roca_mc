// Roca файл интерфейсов
#include "stm32f1xx.h"
#include "include.h"


u8 i2c1_sbuf[SET_I2C1_SBUF_SIZE]; // Буфер передачи по i2c1
u8 i2c1_rbuf[SET_I2C1_RBUF_SIZE]; // Буфер приема по i2c1
u8 u1sb[SET_UART1_SBUF_SIZE]; // Буфер передачи по uart1
u8 u1rb[SET_UART1_RBUF_SIZE]; // Буфер приема по uart1
u8 u2sb[SET_UART2_SBUF_SIZE]; // Буфер передачи по uart2
u8 u2rb[SET_UART2_RBUF_SIZE]; // Буфер приема по uart2

u16 ur2w_count; // Счетчик ожиданния приема UART2

// Инициализация I2C1
void init_i2c1(void){
  I2C1->CR1 |= I2C_CR1_SWRST; // Ресетим
  I2C1->CR1 &= ~I2C_CR1_SWRST; // Отресетиваем
  GPIOB->CRL |= (GPIO_CRL_CNF6 | GPIO_CRL_MODE6_1); // Настраиваем ножку B6 на альт. функцию и на выход 2MHz (SCL)
  GPIOB->CRL |= (GPIO_CRL_CNF7 | GPIO_CRL_MODE7_1); // Настраиваем ножку B7 на альт. функцию и на выход 2MHz (SDA)
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // Включаем тактирование модуля
  I2C1->CR1 &= ~I2C_CR1_SMBUS; // Устанавливаем модуль в режим i2c вместо SMBUS 
  I2C1->CR2 &= ~I2C_CR2_FREQ; // Обнуляем настройки частоты
  I2C1->CR2 |= I2C1_MODUL_TAKT; // Устанавливаем настройки частоты для модуля
  I2C1->CCR &= ~I2C_CCR_FS; // Устанавливаем Slow режим до 100KHz
  I2C1->CCR |= I2C1_CCR_DATA; // Устанавливаем частоту SCL
  I2C1->TRISE = 0xE; // Устанавливаем время нарастания фронта
  //I2C1->CR2 |= I2C_CR2_ITEVTEN; // Включаем прерывания 
  //I2C1->CR2 |= I2C_CR2_ITBUFEN; // Включаем прерывания по Tx Rx
  //I2C1->CR2 |= I2C_CR2_ITERREN; // Включаем прерывания по ошбикам
  I2C1->CR1 |= I2C_CR1_PE; // Включаем модуль i2c1
  return;
}



// Сброс I2C1
void reset_i2c1(void){
  I2C1->CR1 |= I2C_CR1_SWRST; // Ресетим
  I2C1->CR1 &= ~I2C_CR1_SWRST; // Отресетиваем
}



// Запись по I2C1
u8 i2c1_write(u8 adr, u8 cnt, u8 *buf){
  u8 iw;
  while (I2C1->SR2 &= I2C_SR2_BUSY){}// ждем свободной шины i2c1
  I2C1->CR1 |= I2C_CR1_START; // Старт бит пошел				
  while ((I2C1->SR1 & I2C_SR1_SB) == 0){} // Проверяем статус регистра (старт бит завершен)
  I2C1->DR = adr; // Загружаем адрес
  while ((I2C1->SR1 & I2C_SR1_ADDR) == 0){} // Проверяем статус регистра (адрес завершен)
  (void)I2C1->SR1; // Срасываем флаг ADDR
  (void)I2C1->SR2; // Последовательным чтением регистров SR1, SR2
  for (iw = 0; iw < cnt; iw++){
    while (I2C1->SR1 != I2C1_SR1_TXENOBTF){} // Проверям регистр SR1 на флаг TXE без BTF - регистр данных пуст
    I2C1->DR = *(buf + iw); // Пишем данные
  }
  while (I2C1->SR1 != I2C1_SR1_TXEANDBTF){} // Проверяем регистр SR1 на флаг TXE и BTF - сдвиговый регистр пуст
  I2C1->CR1 |= I2C_CR1_STOP; // Останавливаем i2c1
  return 0;
}



// Чтение по I2C1 
u8 i2c1_read(u8 adr, u8 cnt, u8 *buf){
  u8 ir;
  while (I2C1->SR2 &= I2C_SR2_BUSY){}// ждем свободной шины i2c1
  I2C1->CR1 |= I2C_CR1_ACK; // Подтверждение АСК включено
  I2C1->CR1 |= I2C_CR1_START; // Старт бит пошел
  while ((I2C1->SR1 & I2C_SR1_SB) == 0){} // Проверяем статус регистра (старт бит завершен)
  I2C1->DR = adr; // Загружаем адрес
  while ((I2C1->SR1 & I2C_SR1_ADDR) == 0){} // Проверяем статус регистра (адрес завершен)
  (void)I2C1->SR1; // Срасываем флаг ADDR
  (void)I2C1->SR2; // Последовательным чтением регистров SR1, SR2
  for (ir = 0; ir < cnt; ir++){
    while ((I2C1->SR1 & I2C_SR1_RXNE) == 0){} // Проверяем регистр SR1 на флаг RxNE - прием данных завершен
    *(buf + ir) = I2C1->DR; // Пишем данные
  }
  I2C1->CR1 &= ~I2C_CR1_ACK; // Выключаем ACK подтверждение
  I2C1->CR1 |= I2C_CR1_STOP; // Останавливаем i2c1
  return 0;
}



 // Инициализация SPI2 (экран)
void init_spi2(void){
  GPIOB->CRH &= ~(GPIO_CRH_CNF15 | GPIO_CRH_MODE15); // сбрасываем настройки В15 порта
  GPIOB->CRH &= ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14); // сбрасываем настройки В14 порта
  GPIOB->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13); // сбрасываем настройки В13 порта
  GPIOB->CRH &= ~(GPIO_CRH_CNF12 | GPIO_CRH_MODE12); // сбрасываем настройки В12 порта
  GPIOB->CRH |= (GPIO_CRH_CNF15_1 | GPIO_CRH_MODE15); // настраиваем порт В15 на альт. функцию и push-pull (MOSI)
  GPIOB->CRH |= GPIO_CRH_CNF14_0; // настраиваем порт В14 на вход (MISO)
  GPIOB->CRH |= (GPIO_CRH_CNF13_1 | GPIO_CRH_MODE13); // настраиваем порт В13 на альт. функцию и push-pull (SCK)
  GPIOB->CRH |= GPIO_CRH_MODE12_1; // настраиваем порт В12 на выход и push-pull (программный NSS)
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // включаем тактирование SPI2
  //SPI2->CR1 |= SPI_CR1_BR; // делитель частоты Fpclk/256
  //SPI2->CR1 |= 0x20;
  SPI2->CR1 |= SPI_CR1_SSM; // подача значения с SSI на NSS
  SPI2->CR1 |= SPI_CR1_SSI;// ставим 1 чтобы подать на NSS
  SPI2->CR1 |= SPI_CR1_MSTR; // режим - мастер
  SPI2->CR1 |= SPI_CR1_SPE; // включаем сам SPI
  return;
}



// Инициализация UART1 (связь с PLM)
void init_uart1(void){
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включаем тактирование uart1
  
  GPIOA->CRH &= ~(GPIO_CRH_CNF9_0); // Сбрасываем настройки А9 порта
  GPIOA->CRH |= GPIO_CRH_CNF9_1; // Настраиваем порт A9 на альт. функцию и push-pull (TX)
  GPIOA->CRH |= GPIO_CRH_MODE9; // Ставим порт А9 на output и 50 МГц
  
  GPIOA->CRH &= ~(GPIO_CRH_CNF10_0); // Сбрасываем настройки А10 порта
  GPIOA->CRH |= GPIO_CRH_CNF10_1; // Настраиваем порт A10 на прием с подтяжкой (RX)
  GPIOA->CRH &= ~(GPIO_CRH_MODE10);
  GPIOA->BSRR |= GPIO_ODR_ODR10; // Включаем подтяжку
  
  USART1->CR1 = USART_CR1_UE; // Включаем uart1 (сбрасываем остальные биты)
  USART1->BRR = SET_UART1_BAUD_RATE; // Устанавливаем скорость 
  
  USART1->CR1 |= USART_CR1_TE; // Включаем передачу
  USART1->CR1 |= USART_CR1_RE; // Включаем прием
  
  USART1->CR2 = 0;
  USART1->CR3 = 0;
  
  
  return;
}


// Инициализация UART2 (связь с ПК)
void init_uart2(void){
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Включаем тактирование uart2
  
  GPIOA->CRL &= ~(GPIO_CRL_CNF2_0); // Сбрасываем настройки А2 порта
  GPIOA->CRL |= GPIO_CRL_CNF2_1; // Настраиваем порт A2 на альт. функцию и push-pull (TX)
  GPIOA->CRL |= GPIO_CRL_MODE2; // Ставим порт А2 на output и 50 МГц
  
  GPIOA->CRL &= ~(GPIO_CRL_CNF3_0); // Сбрасываем настройки А3 порта
  GPIOA->CRL |= GPIO_CRL_CNF3_1; // Настраиваем порт A3 на прием с подтяжкой (RX)
  GPIOA->CRL &= ~(GPIO_CRL_MODE3);
  GPIOA->BSRR |= GPIO_ODR_ODR3; // Включаем подтяжку
  
  USART2->CR1 = USART_CR1_UE; // Включаем uart2 (сбрасываем остальные биты)
  USART2->BRR = SET_UART2_BAUD_RATE; // Устанавливаем скорость 
  
  USART2->CR3 |= (1 << USART_CR3_DMAR_Pos); // Включаем DMA на прием
  USART2->CR3 |= (1 << USART_CR3_DMAT_Pos); // Включаем DMA на передачу
  
  USART2->CR1 |= USART_CR1_TE; // Включаем передачу
  USART2->CR1 |= USART_CR1_RE; // Включаем прием
  
  return;
}



// Отправка по (связь с PLM)
void uart1_snd(u8 *buf, u8 count){
  
  // Отправляем изо всех сил 
  // Отправляем изо всех сил 
  // Отправляем изо всех сил 
  
  return;
}  
  


// Прием по (связь с PLM)
char uart1_rcv(u8 *buf){
  
  // Принимаем изо всех сил
  // Принимаем изо всех сил
  // Принимаем изо всех сил
  
  return 0;
}  
  


// Отправка по UART2 (связь с ПК)
void uart2_snd(u8 *buf){
  u8 n;
  u8 len;
  u16 pruf;
  
  while (DMA1_Channel7->CNDTR != 0) {} //Если идет отправка, то ждем
  DMA1_Channel7->CCR &= ~(1 << DMA_CCR_EN_Pos); // Отключаем DMA
  
  *(buf) = MODBUS_ADR_PC;
  *(buf + 1) = MODBUS_FUNCTION;
  *(buf + 2) = *(buf + 5) + 6; // Поле длина data 
  
  // Тут считаем CRC16
  // Тут считаем CRC16
  // Тут считаем CRC16
  
  // Записываем CRC16
  pruf = 0xc1c0;
  len = *(buf + 2); // Поле длина data 
  *(short*)(buf + len + 2) = pruf;
  
  
  len = *(buf + 2) + 4; // Длина всего кадра 
  // Копируем данные в буфер отправки
  for (n = 0; n < len; n++)
    u2sb[n] = *(buf + n);
  
        
  DMA1_Channel7->CNDTR = len; // Записываем количество байт в счетчик
  DMA1_Channel7->CCR |= (1 << DMA_CCR_EN_Pos); // Включаем DMA
  
  return;
}  
  


// Прием по UART2 (связь с ПК)
char uart2_rcv(u8 *buf){
  u8 n;
  u8 ret = 0;
  u8 b_rec = 0;
  u8 b_num = 0;
  u16 pruf;
  
  if (DMA1_Channel6->CNDTR < SET_UART2_RBUF_SIZE){ // Если есть принятые байты
    
    // Счетчик ожидания всего пакета
    if (ur2w_count < UART2_RECEIVE_WAIT)
      ur2w_count++;
    else {
      ur2w_count = 0;
      goto mark;  
    }
        
    // Смотрим количество принятых байт
    b_rec = SET_UART2_RBUF_SIZE - DMA1_Channel6->CNDTR;
    if (b_rec >= 10){ // Если принято 10 и более байт
      if (u2rb[0] != MODBUS_ADR_MCU) // Проверяем первый байт
        goto mark;
      
      if (u2rb[1] != MODBUS_FUNCTION) // Проверяем второй байт
        goto mark;

      b_num = u2rb[2]; // Берем количество байт из пакета
      if (b_rec == b_num + 4){ // Проверяем что все байты пакета приняты
        
        for (n = 0; n < b_rec; n++) // Копируем пакет в буфер обработки
          *(buf + n) = u2rb[n];
        
        // Тут считаем CRC16
        // Тут считаем CRC16
        // Тут считаем CRC16
        
        // Проверяем CRC16
        pruf = *(short*)(buf + b_rec - 2);
        if (pruf == 0xc1c0)
          ret = b_rec; // Записываем количество принятых байт
        
        goto mark;
      }
    }
  }
  
  return 0;
    
  mark:  
    DMA1_Channel6->CCR &= ~(1 << DMA_CCR_EN_Pos); // Отключаем DMA
    DMA1_Channel6->CNDTR = SET_UART2_RBUF_SIZE; // Задаем снова количество байт
      
    // Очищаем буфер
    for (n = 0; n < SET_UART2_RBUF_SIZE; n++)
      u2rb[n] = 0;
            
    ur2w_count = 0; // Обнуляем счетчик ожидания
    DMA1_Channel6->CCR |= (1 << DMA_CCR_EN_Pos); // Включаем DMA
  
  return ret; // Выходим и возвращаем количетво принятых байт
  
}  

  

    
    