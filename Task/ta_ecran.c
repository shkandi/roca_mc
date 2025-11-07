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
void led_sig(void);
char arc_draw(u16 zero_x, u16 zero_y, u8 radius, u16 ang, u16 ang_ofs, u16 color);
char radius_drw(u16 zero_x, u16 zero_y, u8 radius, u16 ang, u16 color);
char arrow_drw(u16 pox, u16 poy, u16 *ang, u16 *pang);

u8 show_n;
u32 l_cnt;
u8 l_flip;
u16 cpn_ang;
u16 cpp_ang;
u16 rang;
u16 corang;

// Задача экран
void ta_ecran(void *pvParameters){
  
  ecran_black();
  ec_first(); // Первичное заполнение
  delay(TIME_MS * 1000);   
   
  while(1){
 
    ec_status(); // Обновляем статус
    led_sig(); // Мигание светодиода
 
    //arrow_drw(90, 60, &cpn_ang, &cpp_ang);
    
    delay(TIME_MS * 10);
    
  }
}



// Первичное заполнение главного экрана
void ec_first(void){
  u8 e;
  u16 show_ang[3] = {75, 105, 90};
  
  // Пишем в нулнвой строке 
  for (e = 0; e < 21; e++) 
    draw_symbol(eline_text[0][e], ecr_arr[0][e]);
  
  // Пишем в строке статуса
  for (e = 0; e < 21; e++) 
    draw_symbol(eline_text[1][e], ecr_arr[1][e]);
  
  for (e = 0; e < 21; e++)
    el_buf[e] = eline_text[1][e];
  
  // Рисуем компасс
  arc_draw(90, 60, 30, 360, 0, SRC_COMPASS_CIRCLE_COLOR);
  cpp_ang = 360 - 0;
  cpn_ang = 360 - 90;
  arrow_drw(90, 60, &cpn_ang, &cpp_ang);
  
  
  
  /*
  for (e = 0; e < 3; e++){
    cpn_ang = 360 - show_ang[e];
    arrow_drw(90, 60, &cpn_ang, &cpp_ang);
  }
  */
  
  
    
  return;
}



// Обновляем стутус
void ec_status(void){
  u8 e;
  u8 tmp;
  
  /*
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
  */    
        
 // for (e = 0; e < 21; e++)
   // draw_symbol(el_buf[e], ecr_arr[1][e]);
  
  /*
  // Пишем в строке статуса
  for (e = 0; e < 21; e++) 
    draw_symbol(eline_text[e][*p_status], ecr_arr[e][1]);
  */
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