// Roca общие функции
#include "stm32f1xx.h"
#include "include.h"

// Глобальные переменные
u32 *p_ufdate; // Указатель на датц и время 
u8 *p_status; // Указатель на статус


// Рабочие регистры
u8 ar_reg[SET_REG_SIZE];


// Структура записи события
typedef struct {
  u16 position;
  u32 uf_date;
  u8 res;
  u8 inout;
  u8 event;
  u8 code;
  u32 data;
  u16 reserv;
} _slog;

_slog ar_log[SET_LOG_SIZE]; // Буфер для записи событий
u16 sn_log; // Индекс записи



// Логирование событий и ошибок
void event_log(u8 inout, u8 event, u8 code, u32 data){

  // Делаем запись
  ar_log[sn_log].position = sn_log;
  ar_log[sn_log].uf_date = *p_ufdate;
  ar_log[sn_log].res = 0;
  ar_log[sn_log].inout = inout;
  ar_log[sn_log].event = event;
  ar_log[sn_log].code = code;
  ar_log[sn_log].data = data;
  ar_log[sn_log].reserv = 0; 
  
  // Сдвигаем позицию
  if (sn_log < SET_LOG_SIZE)
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