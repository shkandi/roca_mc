// Roca файл интерфейсов
#include "stm32f1xx.h"
#include "include.h"

u8 i2c1_sbuf[SET_I2C1_SBUF_SIZE]; // Буфер передачи по i2c1
u8 i2c1_rbuf[SET_I2C1_RBUF_SIZE]; // Буфер приема по i2c1
u8 u1sb[SET_UART1_SBUF_SIZE]; // Буфер передачи по uart1
u8 u1rb[SET_UART1_RBUF_SIZE]; // Буфер приема по uart1



// Инициализация i2c1
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



// Сброс i2c1
void reset_i2c1(void){
  I2C1->CR1 |= I2C_CR1_SWRST; // Ресетим
  I2C1->CR1 &= ~I2C_CR1_SWRST; // Отресетиваем
}



// Запись по i2c1
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



// Чтение по i2c1 
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



 // Инициализация spi2 (экран)
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



// Инициализация uart1 (для PLM)
void init_uart1(void){
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Включаем тактирование uart1
  USART1->BRR = 0x1a1; // Устанавливаем скорость 19200 (при 8 МГц)
  GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9); // Сбрасываем настройки А9 порта
  GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10); // Сбрасываем настройки А10 порта
  GPIOA->CRH |= GPIO_CRH_CNF9_1; // Настраиваем порт A9 на альт. функцию и push-pull (TX)
  GPIOA->CRH |= GPIO_CRH_MODE9_0; // Ставим порт А9 на output и 10 МГц
  GPIOA->CRH |= GPIO_CRH_CNF10_1; // Настраиваем порт A10 на прием с подтяжкой (RX)
  GPIOA->BSRR |= GPIO_ODR_ODR10; // Включаем подтяжку
  USART1->CR1 |= USART_CR1_RE; // Включаем прием
  USART1->CR1 |= USART_CR1_TE; // Включаем передачу
  USART1->CR1 |= USART_CR1_UE; // Включаем uart1
  return;
}



// Отправка по uart1
void uart1_snd(u8 *buf, u8 count){
  u8 scn;
  for (scn = 0; scn < count; scn++){
    while ((USART1->SR & USART_SR_TXE) == 0){}
    USART1->DR = *(buf + scn);
  }
  return;
}  
  


// Прием по uart1
char uart1_rcv(u8 *buf){
  u8 rcn;
  u32 w_count;
  while (w_count < UART1_REC_WAIT){
    if (USART1->SR & USART_SR_RXNE){
      *(buf + rcn) = USART1->DR;
      rcn++;
      w_count = 0;
    }
    else
      w_count++;
  }
  return rcn;
}  
  
  
  

    
    