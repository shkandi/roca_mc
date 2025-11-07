// Roca функции для коммуникации
#include "stm32f1xx.h"
#include "include.h"

extern u8 u1sb[SET_UART1_SBUF_SIZE];
extern u8 u1rb[SET_UART1_RBUF_SIZE];
extern u8 ar_log[SET_LOG_SIZE];

char uart1_rcv(u8 *buf);
void uart1_snd(u8 *buf, u8 count);


// Проверить!!!
// Работа с запросами uart 
void ua_work(void){
  u8 cn;
  u8 rn;
  u8 r_flag;
  u8 code;
  u8 bcn;
  u16 adr;
  u16 loc;
  u8 *pl;
  
  rn = uart1_rcv(u1rb);
  if (rn > 4){
      code = u1rb[1] & 0x07;
      bcn = u1rb[1] & 0xf8;
      bcn >>= 3;
      adr = u1rb[3];
      adr <<= 8;
      adr |= u1rb[2];
      
      if (u1rb[0] != 0xaa)
        return;
      if (code + bcn < 3)
        return;
                  
      switch (code){
      case UART_CODE_READ: // Чтение
        
        if (adr <= 0xff){ // Проверяем адрес на проектные регистры МК
          r_flag = FLAG_RISE;
          for (cn = 0; cn < bcn + 1; cn++)
            u1sb[cn + 4] = ar_reg[adr + cn]; // Заполняем буфер
        }
        else if (adr >= 0x1000 && adr <= 0x1fff){ // Проверяем адрес на лог событий
          r_flag = FLAG_RISE;
          loc = adr & 0xfff; // Приводим адрес к локальному
          pl = ar_log;
          for (cn = 0; cn < bcn + 1; cn++) // Заполняем буфер
            u1sb[cn + 4] = *(pl + loc + cn);
        }
        else if (adr >= 0x3000 && adr <= 0x3fff){ // Проверяем адрес на регистры ПЛИС
          r_flag = FLAG_RISE;
          loc = adr & 0xff; // Приводим адрес к локальному
          for (cn = 0; cn < bcn + 1; cn++) // Заполняем буфер
            u1sb[cn + 4] = 0x11;
        }

        if (r_flag){
          u1sb[0] = 0xaa;
          u1sb[1] = bcn << 3;
          u1sb[1] |= UART_CODE_ANSWER;
          u1sb[2] = adr;
          u1sb[3] = adr >> 8;
          uart1_snd(u1sb, bcn + 1 + 4); // Отправляем обратно
        }
        break;
       case UART_CODE_WRITE: // Запись (простая)
        if (adr <= 0xff){ // Проверяем адрес на проектные регистры МК
          if (adr & 0x10){ // Проверям адрес на маску записи
            for (cn = 0; cn < bcn + 1; cn++)
              ar_reg[adr + cn] = u1rb[cn + 4];
          }
        }
        else if (adr >= 0x3000 && adr <= 0x3fff){ // Проверяем адрес на регистры ПЛИС
          if (adr & 0x3010){ // Проверям адрес на маску записи
            loc = adr & 0xff; // Приводим адрес к локальному
            for (cn = 0; cn < bcn + 1; cn++)
              r_flag = u1rb[cn + 4]; // Пишем в ПЛИС тут
          }
        }
        break;
      case UART_CODE_AND: // Запись (лог И)
        if (adr <= 0xff){ // Проверяем адрес на проектные регистры МК
          if (adr & 0x10){ // Проверям адрес на маску записи
            for (cn = 0; cn < bcn + 1; cn++)
              ar_reg[adr + cn] &= u1rb[cn + 4];
          }
        }
        else if (adr >= 0x3000 && adr <= 0x3fff){ // Проверяем адрес на регистры ПЛИС
          if (adr & 0x3010){ // Проверям адрес на маску записи
            loc = adr & 0xff; // Приводим адрес к локальному
            for (cn = 0; cn < bcn + 1; cn++)
              r_flag &= u1rb[cn + 4]; // Пишем в ПЛИС тут
          }
        }
        break;
      case UART_CODE_OR: // Запись (лог ИЛИ)
        if (adr <= 0xff){ // Проверяем адрес на проектные регистры МК
          if (adr & 0x10){ // Проверям адрес на маску записи
            for (cn = 0; cn < bcn + 1; cn++)
              ar_reg[adr + cn] |= u1rb[cn + 4];
          }
        }
        else if (adr >= 0x3000 && adr <= 0x3fff){ // Проверяем адрес на регистры ПЛИС
          if (adr & 0x3010){ // Проверям адрес на маску записи
            loc = adr & 0xff; // Приводим адрес к локальному
            for (cn = 0; cn < bcn + 1; cn++)
              r_flag |= u1rb[cn + 4]; // Пишем в ПЛИС тут
          }
        }
        break;
      default:
        break;
      }
  }
  return;
}