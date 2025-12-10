// Roca функции для коммуникации
#include "stm32f1xx.h"
#include "include.h"

extern u8 u1sb[SET_UART1_SBUF_SIZE];
extern u8 u1rb[SET_UART1_RBUF_SIZE];

extern u8 u2sb[SET_UART2_SBUF_SIZE];
extern u8 u2rb[SET_UART2_RBUF_SIZE];

extern u8 ar_log[SET_LOG_SIZE];


u8 rbuf[SET_UART2_RBUF_SIZE];
u8 sbuf[SET_UART2_RBUF_SIZE];
  

char uart1_rcv(u8 *buf);
void uart1_snd(u8 *buf);

char uart2_rcv(u8 *buf);
void uart2_snd(u8 *buf);


// Проверить!!!
// Работа с запросами uart 
void ua_work(void){
  
  u8 cn;
  u8 rn;
  u8 ans_flag = FLAG_DOWN;
  u8 err_flag = FLAG_DOWN;
  u8 code;
  u8 bcn;
  u16 adr;
  u8 *pl;
  
  // Смотрим буфер приема
  rn = uart2_rcv(rbuf);
  
  // Если есть пакет
  if (rn){
      
      // Читаем из пакета команду, количество байт и адрес
      code = rbuf[4];
      bcn = rbuf[5];
      adr = *(short*)(rbuf + 6); 
      
      // Выполняем комманду
      switch (code){
      case FRAME_CODE_READ: //  Если чтение
        
        if (adr <= 0xff){ // Проверяем адрес на проектные регистры МК
          if ((adr + bcn - 1) <= 0xff){ // Провряем что адрес не выйдет за пределы области
            ans_flag = FLAG_RISE; // Поднимаем флаг ответа
            for (cn = 0; cn < bcn; cn++)
              sbuf[cn + 8] = ar_reg[adr + cn]; // Заполняем буфер ответа
          }
          else
            err_flag = FLAG_RISE;
        }       
        else if (adr >= 0x1000 && adr <= 0x1fff){ // Проверяем адрес на протокол событий
          if ((adr + bcn - 1) <= 0x1fff){ // Провряем что адрес не выйдет за пределы области
            ans_flag = FLAG_RISE; // Поднимаем флаг ответа
            adr &= 0xfff; // Приводим адрес к локальному
            pl = ar_log;
            for (cn = 0; cn < bcn; cn++) // Заполняем буфер ответа
              sbuf[cn + 8] = *(pl + adr + cn);
          }
          else
            err_flag = FLAG_RISE;
        }
        else if (adr >= 0x3000 && adr <= 0x30ff){ // Проверяем адрес на регистры ПЛИС
          if ((adr + bcn - 1) <= 0x30ff){ // Провряем что адрес не выйдет за пределы области
            ans_flag = FLAG_RISE;
            adr &= 0xff; // Приводим адрес к локальному
          
            // Тут запускаем таймер обращения
            // Тут делаем запрос на плис
            // Тут дждем ответ и проверяем таймер
                     
            for (cn = 0; cn < bcn; cn++) // Заполняем буфер
              sbuf[cn + 8] = 0x11;
          }
          else
            err_flag = FLAG_RISE;
        }
        else
            err_flag = FLAG_RISE;

        break;
       
      case FRAME_CODE_WRITE: // Если запись 
      case FRAME_CODE_AND:
      case FRAME_CODE_OR:
        
        if (adr <= 0xff){ // Проверяем адрес на проектные регистры МК
          if (adr > 0x0f){ // Проверям что регистры можно записыватиь
            if ((adr + bcn - 1) <= 0x30ff){ // Провряем что адрес не выйдет за пределы области
              if (code == FRAME_CODE_WRITE){ // Если обычная запись
                for (cn = 0; cn < bcn; cn++)
                  ar_reg[adr + cn] = rbuf[cn + 8];
              }
              else if (code == FRAME_CODE_AND){ // Если запись через AND
                for (cn = 0; cn < bcn; cn++)
                  ar_reg[adr + cn] &= rbuf[cn + 8];
              }
              else if (code == FRAME_CODE_OR){ // Если запись через OR
                for (cn = 0; cn < bcn; cn++)
                  ar_reg[adr + cn] |= rbuf[cn + 8];
              }
            }
            else
            err_flag = FLAG_RISE;
          }
          else
            err_flag = FLAG_RISE;
        }
             
        else if (adr >= 0x3000 && adr <= 0x3fff){ // Проверяем адрес на регистры ПЛИС
          if (adr > 0x0f){ // Проверям что регистры можно записыватиь
            if ((adr + bcn - 1) <= 0x30ff){ // Провряем что адрес не выйдет за пределы области
              
              // Тут делаем запись в регистры ПЛИС
              // Тут делаем запись в регистры ПЛИС
              // Тут делаем запись в регистры ПЛИС
              
            }
            else
            err_flag = FLAG_RISE;
          }
          else
            err_flag = FLAG_RISE;
        }
        else 
          err_flag = FLAG_RISE;
        
        break;

      default:
        err_flag = FLAG_RISE;
        break;
      
      } // End of switch (code)
  } // End of if (rn)
  
  // Если поднят флаг ошибки
  /*
  if (err_flag){
    sbuf[0] = 0xaa;
    sbuf[1] = 0;
    sbuf[1] |= FRAME_CODE_ERROR;
    sbuf[2] = rbuf[2];
    sbuf[3] = rbuf[3];
    uart2_snd(sbuf, 4); // Отправляем ошибку
  }
  */
  
  // Если поднят флаг ответа
  if (ans_flag){
    sbuf[4] |= FRAME_CODE_ANSWER;
    sbuf[5] = bcn;
    sbuf[6] = rbuf[6];
    sbuf[7] = rbuf[7];
    uart2_snd(sbuf); // Отправляем ответ
  }
  
  return;
}