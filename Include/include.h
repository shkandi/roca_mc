
#ifndef __INCLUDE
#define __INCLUDE

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

// Общие дефайны
#define TRUE 0xff
#define FALSE 0x00
#define FLAG_RISE 0x01
#define FLAG_DOWN 0x00


// Пакет соединения UART
#define FRAME_CODE_READ 0x00
#define FRAME_CODE_WRITE 0x01
#define FRAME_CODE_AND 0x02
#define FRAME_CODE_OR 0x03
#define FRAME_CODE_ANSWER 0xa0
#define FRAME_CODE_ERROR 0xef


// Настройки счетчиков
#define CPU_FREQ_MHZ 32 // Частоста в МГц 
#define CPU_FREQ_HZ CPU_FREQ_MHZ * 32000000 // Частоста в Гц 
#define TIME_MS 3210 // Счетчик для 1мс (эксперементально)
#define TOUT_CLK_SETUP 4000 // Таймаут настроек тактирования


// Настройки общие
#define SET_ID_DATA 0xca // Байт идентификации
#define SET_FW_VERSION 1 // Версия ПО
#define SET_FW_SUB_VERSION 0 // Подверсия ПО
#define SET_REG_SIZE 256 //  Количество рабочих регистров
#define SET_COMPASS_START 90 // Начальный угол компасса


// Настройки логирования
#define SET_LOG_SIZE 64 // Количество записей в логе
#define EVENT_STOP_MASK 0xe0 // Маска для событий остановки
#define EVENT_RESTART_MASK 0xf0 // Маска для событий перезагрузки


// Настройки интерфейса i2c1
#define SET_I2C1_SBUF_SIZE 32 // Размер буфера передачи по i2c1
#define SET_I2C1_RBUF_SIZE 32 // Размер буфера приема по i2c1
#define I2C1_MODUL_TAKT 0x4 // Частота тактирования модуля I2C
#define I2C1_CCR_DATA 0xfff // Биты частоты SCL. 1/2T = CCR * Tpclk
#define I2C1_SR1_TXENOBTF 0x80 // Флаг TXE без флага BTF
#define I2C1_SR1_TXEANDBTF 0x84 // Флаг TXE c флагом BTF


// Настройки интерфейса spi2 (под экран)
#define SPI2_A0_LOW	GPIOB->BSRR |= GPIO_BSRR_BR9 // задаем 0 на сигнале экрана А0
#define SPI2_A0_HIGH	GPIOB->BSRR |= GPIO_BSRR_BS9 // задаем 1 на сигнале экрана А0
#define SPI2_NSS_LOW    GPIOB->BSRR |= GPIO_BSRR_BR12 // задаем 0 на B12 (NSS)
#define SPI2_NSS_HIGH   GPIOB->BSRR |= GPIO_BSRR_BS12 // задаем 1 на B12 (NSS)
#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C


// Настройки интерфейса uart1
#define SET_UART1_BAUD_RATE 417 // 19200
#define SET_UART1_SBUF_SIZE 64 // Размер буфера передачи по uart1
#define SET_UART1_RBUF_SIZE 64 // Размер буфера приема по uart1
#define UART1_REC_WAIT TIME_MS * 10 // Ожидание приема uart1

// Настройки интерфейса uart2
#define SET_UART2_BAUD_RATE 417 // 19200
#define SET_UART2_SBUF_SIZE 64 // Размер буфера передачи по uart2
#define SET_UART2_RBUF_SIZE 64 // Размер буфера приема по uart2
#define UART2_RECEIVE_WAIT 1000 // Счетчик ожидания приема uart2

// Коды Modbus
#define MODBUS_ADR_MCU 0xF1 // Адрес MCU
#define MODBUS_ADR_PC 0xB1 // Адрес PC
#define MODBUS_FUNCTION 0x00 // Modbus function

//Журнал событий
#define EVENT_IN 1 // Приходящее событие
#define EVENT_OUT 0 // Уходящие событие 
#define EVENT_SYSTEM_START 0x0a // Загрузка системы

// Экран
#define SRC_BACK_COLOR 0x0000
#define SRC_N_ARROW_COLOR 0x5e9f
#define SRC_S_ARROW_COLOR 0xfac6
#define SRC_COMPASS_CIRCLE_COLOR 0x5e9f

// Рабочие регистры
extern u8 ar_reg[SET_REG_SIZE];

// Общие глобальные переменные
extern u32 *p_ufdate; // Указатель на дату и время
extern u8 *p_status; // Указатель на статус
extern u16 *p_compa; // Указатель на угол компаса

// Отладка
extern u16 *p_a0;
extern u16 *p_a1;
extern u16 *p_a2;
extern u16 *p_a3;
extern u16 *p_a4;
extern u16 *p_a5;

extern u16 *p_b0;
extern u16 *p_b1;
extern u16 *p_b2;
extern u16 *p_b3;
extern u16 *p_b4;
extern u16 *p_b5;

extern u16 *p_c0;
extern u16 *p_c1;
extern u16 *p_c2;
extern u16 *p_c3;
extern u16 *p_c4;
extern u16 *p_c5;


void event_log(u8 inout, u8 event, u8 code, u32 data); // Логироване событий
void delay(u32 count); // Функция задержки


#endif