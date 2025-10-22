// Roca файл интерфейсов
#include "stm32f1xx.h"
#include "include.h"

#define I2C1_MODUL_TAKT 0x4 // Частота тактирования модуля I2C
#define I2C1_CCR_DATA 0xfff // Биты частоты SCL. 1/2T = CCR * Tpclk

#define I2C_SR1_TXENOBTF 0x80 // Флаг TXE без флага BTF
#define I2C_SR1_TXEANDBTF 0x84 // Флаг TXE c флагом BTF

/* Интерфейс i2c номер 1 */
u8 i2c1_sbuf[64]; // Буфер передачи по i2c1
u8 i2c1_rbuf[64]; // Буфер приема по i2c1

 
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
    while (I2C1->SR1 != I2C_SR1_TXENOBTF){} // Проверям регистр SR1 на флаг TXE без BTF - регистр данных пуст
    I2C1->DR = *(buf + iw); // Пишем данные
  }
  while (I2C1->SR1 != I2C_SR1_TXEANDBTF){} // Проверяем регистр SR1 на флаг TXE и BTF - сдвиговый регистр пуст
  I2C1->CR1 |= I2C_CR1_STOP; // Останавливаем i2c1
  return 0;
}



// Чтение по i2c1 
u8 i2c1_read(u8 adr, u8 cnt, u8 *buf){
  u8 ir;
  return 0;
}



 // Инициализация SPI2 под экран
void init_spi2(void){
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // включаем тактирование альтернативных функций на порт B
  GPIOB->CRH &= ~(GPIO_CRH_CNF15 | GPIO_CRH_MODE15); // сбрасываем настройки В15 порта
  GPIOB->CRH &= ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14); // сбрасываем настройки В14 порта
  GPIOB->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13); // сбрасываем настройки В13 порта
  GPIOB->CRH &= ~(GPIO_CRH_CNF12 | GPIO_CRH_MODE12); // сбрасываем настройки В12 порта
  GPIOB->CRH |= (GPIO_CRH_CNF15_1 | GPIO_CRH_MODE15); // настраиваем порт В15 на альт. функцию и push-pull (MOSI)
  GPIOB->CRH |= GPIO_CRH_CNF14_0; // настраиваем порт В14 на вход (MISO)
  GPIOB->CRH |= (GPIO_CRH_CNF13_1 | GPIO_CRH_MODE13); // настраиваем порт В13 на альт. функцию и push-pull (SCK)
  GPIOB->CRH |= GPIO_CRH_MODE12_1; // настраиваем порт В12 на выход и push-pull (программный NSS)
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // включаем тактирование SPI2
  SPI2->CR1 |= SPI_CR1_BR; // делитель частоты Fpclk/256
  SPI2->CR1 |= SPI_CR1_SSM; // подача значения с SSI на NSS
  SPI2->CR1 |= SPI_CR1_SSI;// ставим 1 чтобы подать на NSS
  SPI2->CR1 |= SPI_CR1_MSTR; // режим - мастер
  SPI2->CR1 |= SPI_CR1_SPE; // включаем сам SPI
  return;
}
   
    
    