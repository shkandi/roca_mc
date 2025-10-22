
#ifndef __INCLUDE
#define __INCLUDE

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short int s16;
typedef signed int s32;

// Общие дефайны
#define TRUE 0xff
#define FALSE 0x00
#define FLAG_RISE 0x01
#define FLAG_DOWN 0x00


// Настройки проекта
#define FW_VERSION 1
#define FW_SUB_VERSION 0
#define LOG_SIZE 128 // Количество записей в логе

// Рабочие регистры
extern u8 arr_reg[255];

void delay(u32 count); // Функция задержки

#endif