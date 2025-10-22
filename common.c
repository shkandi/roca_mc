// Roca общие функции
#include "stm32f1xx.h"
#include "include.h"

#define EVENT_STOP_MASK 0xe0
#define EVENT_RESTART_MASK 0xf0

// Глобальные переменные
u16 st_date; // Переменная для даты
u16 st_time; // Переменная для времени


// Рабочие регистры
u8 arr_reg[255];


// Структура записи события
typedef struct {
  u16 date;
  u32 time;
  u8 event;
  u8 code;
  u32 data;
} _slog;

_slog ar_log[LOG_SIZE]; // Буфер для записи событий
char sn_log; // Индекс записи



// Логирование событий и ошибок
void err_log(u8 event, u8 code, u32 data){

  // Делаем запись
  ar_log[sn_log].date = st_date;
  ar_log[sn_log].time = st_time;
  ar_log[sn_log].event = event;
  ar_log[sn_log].code = code;
  ar_log[sn_log].data = data;
  
  // Сдвигаем позицию
  if (sn_log < LOG_SIZE)
    sn_log++;
  else
    sn_log = 0;
  
  // Проверяем событие на неотложные действия
  if (event & EVENT_STOP_MASK){
    // Инициируем остановку механизмов
  } 
  if (event & EVENT_RESTART_MASK){
    // Инициируем перезагрузку
  } 
  
  // Ваши данные очень важны для нас
  return;
}



// Функция задержки
void delay(u32 count){for (int i=0; i < count; i++);}