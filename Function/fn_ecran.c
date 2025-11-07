// Roca функции для экрана
#include "stm32f1xx.h"
#include "include.h"
#include "static_data.h"
#include "font_5x7.h"

extern int sinus_ang[91];
u16 ecr_arr[16][21]; // Массив координат символов экрана (при разбивке 21х16)
u8 el_buf[21];
char arc_draw(u16 zero_x, u16 zero_y, u8 radius, u16 ang, u16 ang_ofs, u16 color);
char radius_point(u16 osx, u16 osy, u8 radius, s16 ang, u16 *crdx, u16 *crdy);
char point_draw(u16 col, u16 coh, u16 color);
void ecran_com(u8 com);
void ecran_dat(u8 dat);


// Отправка команды по SPI на экран. Аргумент - команда типа 0x00
void ecran_com(u8 com){
  while (SPI2->SR & SPI_SR_BSY); // проверяем флаг BUSY 
  while (!(SPI2->SR & SPI_SR_TXE)); // ждем флаг TXE что буфер данных на отправку пуст	
  SPI2_A0_LOW; // включаем А0 в 0 для команды
  SPI2_NSS_LOW; // включаем выборку
  SPI2->DR = com; // загружаем данные в буфер	 
  while (!(SPI2->SR & SPI_SR_TXE)); // ждем флаг TXE что буфер данных на отправку пуст
  while (SPI2->SR & SPI_SR_BSY); // проверяем флаг BUSY 
  SPI2_A0_HIGH; // выключаем А0 
  SPI2_NSS_HIGH; // выключаем выборку
}



// Отправка данных на экран. Аргумент - данные 0x00
void ecran_dat(u8 dat){
  while (SPI2->SR & SPI_SR_BSY);
  while (!(SPI2->SR & SPI_SR_TXE)); //ждем флаг TXE что буфер данных на отправку пуст
  SPI2_A0_HIGH; // включаем А0 в 1 для данных
  SPI2_NSS_LOW; // включаем выборку
  SPI2->DR = dat; // загружаем данные в буфер  
  while (!(SPI2->SR & SPI_SR_TXE)); // ждем флаг TXE что буфер данных на отправку пуст
  while (SPI2->SR & SPI_SR_BSY); // проверяем флаг BUSY 
  SPI2_NSS_HIGH; // выключаем выборку
}



// Инициализация экрана
void init_ecran(void){
  u16 tmp = 0;
  u8 x_pos = 0x03;
  u8 y_pos = 0x05;
  
  // заполнение массива координат символов экрана (при разбивке 21х16)
  for (int j = 0; j < 16; j++){
    for (int i = 0; i < 21; i++){
      tmp = 0x00;
      tmp |= x_pos;
      tmp <<= 8;
      tmp &= 0xff00;
      tmp |= y_pos;
      ecr_arr[j][i] = tmp;
      x_pos += 0x06;
    }
    x_pos = 0x03;
    y_pos += 0x0a;
  }

  // Инициализация
  GPIOB->BSRR |= GPIO_BSRR_BS8; //включаем подсветку
  ecran_com(0x01); // программный сброс экрана
  delay(960000);
  ecran_com(0x11); // выход из режима сна
  delay(960000);	
  ecran_com(0x3A);// задаем формат цифрового пространства
  ecran_dat(0x05);// RGB444
  delay(1000);
  //ecran_com(0xB1);// частота обновления дисплея
  //ecran_dat(0x00);// данные
  delay(1000);
  ecran_com(0x29);// включение дисплея
}



//Заполнение экрана черным цветом
void ecran_black(void){
  ecran_com(CASET); //комманда на х1 и х2 координаты клетки
  ecran_dat(0x00);
  ecran_dat(0x00);
  ecran_dat(0x00);
  ecran_dat(0x7f);
  
  ecran_com(RASET); //комманда на y1 и y2 координаты клетки
  ecran_dat(0x00);
  ecran_dat(0x00);
  ecran_dat(0x00);
  ecran_dat(0xa0);

  ecran_com(RAMWR); // комманда на отправку данных 
  // отправка данных для заполнения черными точками
  for (int i = 0; i < 20480 ; i++){
    ecran_dat(0x00);
    ecran_dat(0x00);		
  }
}



//вывод на экран символа из массива font5x7[]
void draw_symbol(u8 sym, u16 pos){
  
  u8 ey1_pos = pos - 0x03;
  u8 ey2_pos = pos + 0x03;
  pos >>= 8;
  u8 ex1_pos = pos - 0x02;
  u8 ex2_pos = pos + 0x02;
  
  ecran_com(CASET); //комманда на х1 и х2 координаты клетки
  ecran_dat(0x00);
  ecran_dat(ex1_pos);
  ecran_dat(0x00);
  ecran_dat(ex2_pos);
  ecran_com(RASET); //комманда на y1 и y2 координаты клетки
  ecran_dat(0x00);
  ecran_dat(ey1_pos);
  ecran_dat(0x00);
  ecran_dat(ey2_pos);
    
  // Отрисовка символа согласно массиву
  ecran_com(RAMWR); // комманда на отправку данных 
  for (int row = 0; row < 7; row++){
    for(char point = 0x10; point > 0x00; point >>= 1){
      if (font_5x7[sym][row] & point){
        ecran_dat(0xff);
        ecran_dat(0xff);
      }
      else {
        ecran_dat(0x00);
        ecran_dat(0x00);
      }
    }
  }
}



//Заполнение клетки черным цветом
void draw_black(u16 pos){ // pos - позиция символа, задается из массива ecr_arr[x][x]
  u8 ey1_pos = pos - 0x05;
  u8 ey2_pos = pos + 0x05;
  pos >>= 8;
  u8 ex1_pos = pos - 0x03;
  u8 ex2_pos = pos + 0x03;
  ecran_com(CASET); //комманда на х1 и х2 координаты клетки
  ecran_dat(0x00);
  ecran_dat(ex1_pos);
  ecran_dat(0x00);
  ecran_dat(ex2_pos);
  ecran_com(RASET); //комманда на y1 и y2 координаты клетки
  ecran_dat(0x00);
  ecran_dat(ey1_pos);
  ecran_dat(0x00);
  ecran_dat(ey2_pos);
  // Заполняем  черными точками
  ecran_com(RAMWR); // комманда на отправку данных 
  for (int row = 0; row < 11; row++){
    for(char point = 0x40; point != 0x00; point >>= 1){
      ecran_dat(0x00);
      ecran_dat(0x00);
    }
  }
}



// Отрисовка точки
char point_draw(u16 col, u16 coh, u16 color){
  ecran_com(CASET); //комманда на х координаты клетки
  ecran_dat(col >> 8);
  ecran_dat(col);
  ecran_com(RASET); //комманда на y координаты клетки
  ecran_dat(coh >> 8);
  ecran_dat(coh);
  ecran_com(RAMWR); // комманда на отправку цвета  
  ecran_dat(color >> 8);
  ecran_dat(color);
  return 0;
}



// Отрисовка дуги окружности
char arc_draw(u16 zero_x, u16 zero_y, u8 radius, u16 ang, u16 ang_ofs, u16 color){

  u16 ang_rl;
  u16 ang_cn;
  u32 hilf_l;
  u32 hilf_h;
  u16 cord_l;
  u16 cord_h;

  u16 i;
  u8 cyf;

  if (ang > 360) 
    return 1;
  if (ang_ofs > 360) 
    return 1;

  ang_rl = ang_ofs; 
  if (ang_rl == 360) 
    ang_rl = 0;

  for (i = 0; i < ang; i++){

    cord_l = zero_x;
    cord_h = zero_y;

    if (ang_rl == 0 || ang_rl == 90 || ang_rl == 180 || ang_rl == 270)
      ang_cn = 0;
    else
      ang_cn = ang_rl % 90;

    hilf_h = (radius * sinus_ang[ang_cn]) / 10000;
    hilf_l = (radius * sinus_ang[90 - ang_cn]) / 10000;

    if (hilf_h == radius) 
      hilf_h--;
    if (hilf_l == radius) 
      hilf_l--;

    cyf = ang_rl / 90;
    switch (cyf){
      case 0:
        cord_l += hilf_l;
	cord_h += hilf_h;
	break;
      case 1:
        cord_l -= hilf_h;
        cord_h += hilf_l;
        break;
      case 2:
        cord_l -= hilf_l;
        cord_h -= hilf_h;
        break;
      case 3:
        cord_l += hilf_h;       
        cord_h -= hilf_l;
        break;
      default:
        break;
    }

    ecran_com(CASET); //комманда на х координаты клетки
    ecran_dat(0x00);
    ecran_dat(cord_l);
    ecran_com(RASET); //комманда на y координаты клетки
    ecran_dat(0x00);
    ecran_dat(cord_h);
    ecran_com(RAMWR); // комманда на отправку цвета  
    ecran_dat(color >> 8);
    ecran_dat(color);
            
    if (ang_rl >= 359)
      ang_rl = 0;
    else
      ang_rl++;
  }
  return 0;	
}



// Отрисовка радиуса
char radius_drw(u16 zero_x, u16 zero_y, u8 radius, u16 ang, u16 color){
  
  u16 ang_cn;
  u32 hilf_l;
  u32 hilf_h;
  u16 cord_l;
  u16 cord_h;
  u16 i;
  u8 hilf_r;
  u8 cyf;

  if (ang > 360) 
    return 1;
  
  hilf_r = 0;
  for (i = 0; i < radius; i++){

    cord_l = zero_x;
    cord_h = zero_y;
    
    if (ang == 0 || ang == 90 || ang == 180 || ang == 270 || ang == 360)
      ang_cn = 0;
    else
      ang_cn = ang % 90;
    
    
    hilf_h = (hilf_r * sinus_ang[ang_cn]) / 10000;
    hilf_l = (hilf_r * sinus_ang[90 - ang_cn]) / 10000;

    cyf = ang / 90;
    switch (cyf){
      case 4:
      case 0:
        cord_l += hilf_l;
        cord_h += hilf_h;
        break;
      case 1:
        cord_l -= hilf_h;
        cord_h += hilf_l;
        break;
      case 2:
        cord_l -= hilf_l;
        cord_h -= hilf_h;
        break;
      case 3:
        cord_l += hilf_h;
        cord_h -= hilf_l;
        break;
      default:
        break;
    }
    
    ecran_com(CASET); //комманда на х координаты клетки
    ecran_dat(0x00);
    ecran_dat(cord_l);
    ecran_com(RASET); //комманда на y координаты клетки
    ecran_dat(0x00);
    ecran_dat(cord_h);
    ecran_com(RAMWR); // комманда на отправку цвета  
    ecran_dat(color >> 8);
    ecran_dat(color);
   
    hilf_r++;
  }
  return 0;
}



// Считает координаты конца радиуса
char radius_point(u16 osx, u16 osy, u8 radius, s16 ang, u16 *crdx, u16 *crdy){

  s16 alf;
  u16 cir;
  u32 hilf_l;
  u32 hilf_h;

  alf = ang;
  
  if (alf >= 0){
    if (alf > 360)
      alf %= 360;
  }
  else {
  alf = ~alf + 1;
  if (alf % 360)
    alf = 360 - alf % 360;
  else
  alf = 0;
  }

  cir = alf / 90;

  if (alf % 90)
    alf %= 90;
  else 			
    alf = 0;

  hilf_h = (radius * sinus_ang[alf]) / 10000;
  hilf_l = (radius * sinus_ang[90 - alf]) / 10000;

  if (hilf_h == radius) 
    hilf_h--;
  if (hilf_l == radius) 
    hilf_l--;		

  switch (cir){
    case 4:
    case 0:
      *crdx = osx + hilf_l;
      *crdy = osy + hilf_h;
      break;
    case 1:
      *crdx = osx - hilf_h;
      *crdy = osy + hilf_l;
      break;
    case 2:
      *crdx = osx - hilf_l;
      *crdy = osy - hilf_h;
      break;
    case 3:
      *crdx = osx + hilf_h;
      *crdy = osy - hilf_l;
      break;
    default:
      break;
  }
 
  
}



// Рисуем стрелку
char arrow_drw(u16 pox, u16 poy, u16 *n_ang, u16 *p_ang){
	
        u16 mov_ang;
        u16 n_posi;
        u16 s_posi;
        u16 delta;
        u8 ccv_flag;
        u16 ar_npos[4]; 
        u16 ar_spos[4]; 
        u16 i, t; 
	u16 cord_l;
	u16 cord_h;
	
        // Если нет разницы с предыдущим углом, выходим
        if (*n_ang == *p_ang)
          return 0;
       
        // Определяем сторону вращения и дельту с новым углом
        if (*n_ang > *p_ang){
          delta = *n_ang - *p_ang;
          if (delta < 180)
            ccv_flag = 1;
          else {
            ccv_flag = 0;
            delta = 360 - delta;
            }
        } 
        else {
          delta = *p_ang - *n_ang;
            if (delta < 180)
              ccv_flag = 0;
            else {
              ccv_flag = 1;
              delta = 360 - delta;
            }
          }

        mov_ang = *p_ang;

      while(delta){
        delta--;
        // Если поворот против часовой стрелки (CVV)
        if (ccv_flag){
            
            mov_ang++;
            if (mov_ang == 360)
              mov_ang = 0;
            
            // Берем N позицию и откатываем назад
            n_posi = mov_ang;
            for (i = 0; i < 2; i++){
              if (n_posi == 0)
                n_posi = 359;
              else
                n_posi--;
            }
            
            // Берем S позицию
            s_posi = mov_ang + 178;
            if (s_posi > 360)
              s_posi = s_posi - 360;

            // Записываем углы для N и S в массив
            for (i = 0; i < 4; i++){
              ar_npos[i] = n_posi;
              ar_spos[i] = s_posi;
              n_posi++;
              s_posi++;
              if (n_posi == 360)
                n_posi = 0;
              if (s_posi == 360)
                s_posi = 0;
            }

            // Отрисовываем N и S
            for (t = 0; t < 4; t ++){
              for (i = 23; i < 30; i++){
                radius_point(pox, poy, i, ar_npos[t], &cord_l, &cord_h);
                point_draw(cord_l, cord_h, ar_nclr[t]);
              }
              for (i = 23; i < 30; i++){
                radius_point(pox, poy, i, ar_spos[t], &cord_l, &cord_h);
                point_draw(cord_l, cord_h, ar_sclr[t]);
              }
            }
          }
        
         // Если поворот по часовой стрелки (CV)
         else {   
            
           if (mov_ang == 0)
            mov_ang = 359; 
           else
            mov_ang--;
            
            // Берем N позицию и двигаем вперед
            n_posi = mov_ang;
            for (i = 0; i < 2; i++){
              n_posi++;
              if (n_posi == 360)
                n_posi = 0;
            }
            
            // Берем S позицию
            s_posi = mov_ang + 182;
            if (s_posi > 360)
              s_posi = s_posi - 360;

            // Записываем углы для N и S в массив
            for (i = 0; i < 4; i++){
              ar_npos[i] = n_posi;
              ar_spos[i] = s_posi;
              
              if (n_posi == 0)
                n_posi = 359;
              else
                n_posi--;
                  
              if (s_posi == 0)
                s_posi = 359;
              else
                s_posi--;
              
            }

            // Отрисовываем N и S
            for (t = 0; t < 4; t ++){
              for (i = 23; i < 30; i++){
                radius_point(pox, poy, i, ar_npos[t], &cord_l, &cord_h);
                point_draw(cord_l, cord_h, ar_nclr[t]);
              }
              for (i = 23; i < 30; i++){
                radius_point(pox, poy, i, ar_spos[t], &cord_l, &cord_h);
                point_draw(cord_l, cord_h, ar_sclr[t]);
              }
            }
          
         }
       }
                  
        
           
	*p_ang = *n_ang;
	
      	
	return 0;

}
