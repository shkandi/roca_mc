// Roca задача работы с экраном
#include "stm32f1xx.h"
#include "include.h"
#include "static_data.h"

extern u8 ar_reg[SET_REG_SIZE];
extern u16 ecr_arr[16][21];
extern u8 el_buf[21];

void draw_symbol(u8 sym, u16 pos);
void ec_first(void);
void ec_status(void);
void ecran_black(void);
void ec_data(void);
void led_sig(void);
char arc_draw(u16 zero_x, u16 zero_y, u8 radius, u16 ang, u16 ang_ofs, u16 color);
char radius_drw(u16 zero_x, u16 zero_y, u8 radius, u16 ang, u16 color);
char arrow_drw(u16 pox, u16 poy, u16 *n_ang, u16 *p_ang);

u16 cpn_ang;
u16 cpp_ang;

u8 show_n;
u32 l_cnt;
u8 l_flip;
u16 rang;
u16 corang;


// Задача экран
void ta_ecran(void *pvParameters){
  
  ecran_black();
  ec_first(); // Первичное заполнение
  delay(TIME_MS * 10);   
   
  while(1){
 
    cpn_ang = 360 - *(u16*)(ar_reg + 0x14); // Берем угол для компаса
    
    ec_status(); // Обновляем статус
    ec_data(); // Обнавляем данные
    led_sig(); // Мигание светодиода

    arrow_drw(90, 60, &cpn_ang, &cpp_ang); // Анимация компаса
        
  }
}



// Первичное заполнение главного экрана
void ec_first(void){
  u8 e;
  u8 f;
    
  // Пишем в нулнвой строке 
  for (e = 0; e < 21; e++) 
    draw_symbol(eline_text[0][e], ecr_arr[0][e]);
  
  // Пишем в строке статуса
  for (e = 0; e < 21; e++) 
    draw_symbol(eline_text[1][e], ecr_arr[1][e]);
  
  // Переменные для отладки A
  for (f = 3; f < 9; f++){
    for (e = 0; e < 21; e++){ 
      if (e == 1)
        draw_symbol(f + 13, ecr_arr[f][e]);
      else
        draw_symbol(eline_text[2][e], ecr_arr[f][e]);
    }
  }
  
  // Переменные для отладки B и С
  for (f = 10; f < 16; f++){
    for (e = 0; e < 21; e++){ 
      if ((e == 1) || (e == 10))
        draw_symbol(f + 6, ecr_arr[f][e]);
      else
        draw_symbol(eline_text[3][e], ecr_arr[f][e]);
    }
  }
  
  
  for (e = 0; e < 21; e++)
    el_buf[e] = eline_text[1][e];
  
  // Рисуем компасс
  arc_draw(90, 60, 30, 360, 0, SRC_COMPASS_CIRCLE_COLOR);
  
  cpp_ang = 360 - 55;
  cpn_ang = 360 - 125;
  arrow_drw(90, 60, &cpn_ang, &cpp_ang);
  cpn_ang = 360 - 55;
  arrow_drw(90, 60, &cpn_ang, &cpp_ang);
  
  *p_compa = 90;
  
  return;
}



// Обновляем стутус
void ec_status(void){
  
  /*
  u8 e;
  u8 tmp;
  
  for (e = 0; e < 21; e++){
    if (e == 0){
      tmp = el_buf[0];
      el_buf[0] = el_buf[1];
    }
    else if (e > 0 && e < 20)
      el_buf[e] = el_buf[e + 1];
    else if (e == 20)
      el_buf[20] = tmp;
  }
      
  for (e = 0; e < 21; e++)
    draw_symbol(el_buf[e], ecr_arr[1][e]);
  
 
  // Пишем в строке статуса
  for (e = 0; e < 21; e++) 
    draw_symbol(eline_text[e][*p_status], ecr_arr[e][1]);
  */
  
  return;
}



// Вывод данных на экран
void ec_data(void){
  u8 e, f;
  u8 cnt;
  u8 tmp;
  u16 data;
  u16 *p_dat;
  
  // Вывод значения компаса
  data = *(short*)(ar_reg + 0x14);
  tmp = data / 100;   
  if (tmp)
    draw_symbol(tmp + 16, ecr_arr[1][17]);
  else
    draw_symbol(0, ecr_arr[1][17]);
  
  tmp = data % 100;   
  tmp /= 10;  
  
  if (tmp)
    draw_symbol(tmp + 16, ecr_arr[1][18]);
  else
    draw_symbol(0, ecr_arr[1][18]);
  
  tmp = data % 10;   
  draw_symbol(16 + tmp, ecr_arr[1][19]);
  draw_symbol(7, ecr_arr[1][20]);
  
  // Вывод переменных A
  p_dat = p_a0;
  for (f = 3; f < 9; f++){
    data = *p_dat;
    cnt = 12;
    for (e = 3; e < 7; e++){
      tmp = data >> cnt;   
      tmp &= 0xf;
    
      if (tmp > 9)
        tmp += 23;
      else
        tmp += 16;
    
      draw_symbol(tmp, ecr_arr[f][e]);
      cnt -= 4;
    }
    p_dat += 1;
  }
  
  // Вывод переменных B
  p_dat = p_b0;
  for (f = 10; f < 16; f++){
    data = *p_dat;
    cnt = 12;
    for (e = 3; e < 7; e++){
      tmp = data >> cnt;   
      tmp &= 0xf;
    
      if (tmp > 9)
        tmp += 23;
      else
        tmp += 16;
    
      draw_symbol(tmp, ecr_arr[f][e]);
      cnt -= 4;
    }
    p_dat += 1;
  }

  // Вывод переменных C
  p_dat = p_c0;
  for (f = 10; f < 16; f++){
    data = *p_dat;
    cnt = 12;
    for (e = 12; e < 16; e++){
      tmp = data >> cnt;   
      tmp &= 0xf;
    
      if (tmp > 9)
        tmp += 23;
      else
        tmp += 16;
    
      draw_symbol(tmp, ecr_arr[f][e]);
      cnt -= 4;
    }
    p_dat += 1;
  }

  return;
}




void led_sig(void){
    
  if (l_cnt < 20000)
    l_cnt++;
  else {
    l_cnt = 0;
    if (l_flip){
      GPIOC->BSRR |= GPIO_BSRR_BR13;
      l_flip = 0;
    }
    else {
      GPIOC->BSRR |= GPIO_BSRR_BS13;
      l_flip = 1;
    }
  }
 
  return;
}