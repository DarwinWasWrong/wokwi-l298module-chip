
// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 Darwin WasWrong
// src/L298-module.chip.c

// thanks to
// Maverick - for saving my mind with PWM
//
// https://notisrac.github.io/FileToCArray/
// for the conversion for images
//




#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#define BOARD_HEIGHT 100
#define BOARD_WIDTH 100




// the various states the channel can be in
const char drive_state[][17]=
{
  "Backward       ",
  "Forward        ", 
  "Brake Stop     ",
  "Free Stop      "
  };

// basic RGBA color
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} rgba_t;

typedef struct {
  
  //Module Pins
  // channel A
  pin_t pin_IN1;
  pin_t pin_IN2;
  pin_t pin_ENA;
  pin_t pin_ENA_5v;   // 5V for enable
// Channel B
  pin_t pin_IN3;
  pin_t pin_IN4;
  pin_t pin_ENB;
  pin_t pin_ENB_5v;  // 5V for enable
 


// powers and ground
  pin_t pin_VCC;
  pin_t pin_GND;
  pin_t pin_Motor_V;

  // start of timing checks for the PWM
  uint8_t  use_PWM_ENA; //  pwm used
  uint8_t  use_PWM_ENB; //  pwm used
// Channel A  
  uint32_t  high_ENA;
  uint32_t  low_ENA;
  uint32_t  previous_high_ENA;
  uint32_t  previous_low_ENA;
  uint32_t  high_time_ENA;
  uint32_t  low_time_ENA;
// Channel B
  uint32_t  high_ENB;
  uint32_t  low_ENB;
  uint32_t  previous_high_ENB;
  uint32_t  previous_low_ENB;
  uint32_t  high_time_ENB;
  uint32_t  low_time_ENB;

// check for start state of jumpered enables
  uint32_t  start_state_ENA;
  uint32_t  start_state_ENB;

  uint32_t Vs_attr;  // power

// frame buffer details
  uint32_t fb_w;
  uint32_t fb_h;
  uint32_t row;
  buffer_t framebuffer;

// colors
  rgba_t   white;
  rgba_t   green;
  rgba_t   background;
  rgba_t   purple;
  rgba_t   black;
  rgba_t   red;
  rgba_t   blue;

 // text start and postion
  uint32_t vertical_start;
  uint32_t position_x;
  uint32_t position_y;
  
  uint8_t  speed_percent_A;
  uint8_t  speed_percent_B;
  uint8_t  previous_speed_percent_A;
  uint8_t  previous_speed_percent_B;

  uint8_t  drive_A_state;
  uint8_t  drive_B_state;
  uint8_t  previous_drive_A_state;
  uint8_t  previous_drive_B_state;
  
  // motor graphics position
  uint8_t motorAphase;
  uint8_t motorBphase;
  uint8_t motor_A_y;
  uint8_t motor_A_x;
  uint8_t motor_2_x;
  uint8_t motor_B_x;

// arrow graphics position
  uint8_t motor_1_2_arrow_y ;

  uint8_t motorA_right_arrow_x;
  uint8_t motorA_left_arrow_x;

  uint8_t motorB_right_arrow_x;
  uint8_t motorB_left_arrow_x;

  uint8_t bar_1_2_y;
  uint8_t bar_left_x;
  uint8_t bar_right_x;


} chip_state_t;


// screen functions
static void draw_state (chip_state_t *chip);
static void draw_board(chip_state_t *chip, uint32_t x_start,  uint32_t y_start) ;
static void draw_line(chip_state_t *chip, uint32_t row, rgba_t color);
static void draw_pixel(chip_state_t *chip, uint32_t x,  uint32_t y,  rgba_t color) ;
static void draw_cog(chip_state_t *chip, uint32_t x,  uint32_t y ,uint32_t phase);

static void draw_right_arrow(chip_state_t *chip, uint32_t x_start,  uint32_t y_start, uint8_t wipe);
static void draw_left_arrow(chip_state_t *chip, uint32_t x_start,  uint32_t y_start, uint8_t wipe);

static void draw_rectangle(chip_state_t *chip, uint32_t x_start, uint32_t y_start, uint32_t x_len, uint32_t y_len,  rgba_t color, uint8_t fill);
static void draw_speed(chip_state_t *chip, uint32_t x_start, uint32_t y_start, uint32_t x_len, uint32_t y_len,  rgba_t color, uint8_t fill) ;


// timer for graphics
static void chip_timer_event_motorA(void *user_data);
static void chip_timer_event_motorB(void *user_data);

// timer for watchdog
static void chip_timer_event_Awatchdog(void *user_data);
static void chip_timer_event_Bwatchdog(void *user_data);

// pin change watches
static void chip_pin_change(void *user_data, pin_t pin, uint32_t value);
static void chip_pin_change_PWM_A(void *user_data, pin_t pin, uint32_t value);
static void chip_pin_change_PWM_B(void *user_data, pin_t pin, uint32_t value);

void chip_init(void) {
  printf("*** L298chip initialising...\n");
  
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->pin_ENA = pin_init("ENA",INPUT);
  chip->pin_IN1 = pin_init("IN1",INPUT);
  chip->pin_IN2 = pin_init("IN2",INPUT);
 

  chip->pin_ENB = pin_init("ENB",INPUT);
  chip->pin_IN3 = pin_init("IN3",INPUT);
  chip->pin_IN4 = pin_init("IN4",INPUT);


  chip->pin_VCC = pin_init("VCC",INPUT);
  chip->pin_GND = pin_init("GND",INPUT);
  chip->pin_Motor_V = pin_init("Motor_V",INPUT);

  // logic for detecting jumper
  // set the 5v jumper pins to 5v
  chip->pin_ENA_5v= pin_init("ENA_5v",OUTPUT_HIGH );
  chip->pin_ENB_5v= pin_init("ENB_5v",OUTPUT_HIGH );

  // read then ENA pin - if jumper is on - it will be high
  chip->start_state_ENA =pin_read(chip->pin_ENA);
  // read then ENB pin - if jumper is on - it will be high
  chip->start_state_ENB =pin_read(chip->pin_ENB);

 //
  printf( "ENA linked to 5v %d\n",chip->start_state_ENA);
  printf( "ENB linked to 5v %d\n",chip->start_state_ENB);


  
  // if there is no link have to check for PWM or switching
  chip->use_PWM_ENA= !chip->start_state_ENA;
  chip->use_PWM_ENB= !chip->start_state_ENB;




  // pwm timings
  unsigned long  high_ENA;
  unsigned long  low_ENA;
  unsigned long  previous_high_ENA;
  unsigned long  previous_low_ENA;
  unsigned long  high_ENB;
  unsigned long  low_ENB;
  unsigned long  previous_high_ENB;
  unsigned long  previous_low_ENB;



   unsigned long  high_time_ENA;
   unsigned long  low_time_ENA;
 
   unsigned long  high_time_ENB;
   unsigned long  low_time_ENB;
  
  // Display values
  chip->speed_percent_A=0;
  chip->speed_percent_B=0;

  // display colors
  chip-> white      = (rgba_t) { .r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff };
  chip-> green      = (rgba_t) { .r = 0x08, .g = 0x7f, .b = 0x45, .a = 0xff };
  chip-> background = (rgba_t) { .r = 0xf7, .g = 0xf7, .b = 0xf7, .a = 0xff };
  chip-> purple    = (rgba_t) { .r = 0xff, .g=0x00,   .b=0xff,   .a=0xff   };
  chip-> black    =  (rgba_t) { .r = 0x00, .g=0x00,   .b=0x00,   .a=0x00   };
  chip-> red    =  (rgba_t) { .r = 0xff, .g=0x00,   .b=0x00,   .a=0xff   };
  chip-> blue    =  (rgba_t) { .r = 0x00, .g=0x00,   .b=0xff,   .a=0xff   };
  chip->row = 0;
  // get the screen size
  chip->framebuffer = framebuffer_init(&chip->fb_w, &chip->fb_h);
  printf("Framebuffer: fb_w=%d, fb_h=%d\n", chip->fb_w, chip->fb_h);
 
 // settings for gears and displays
 // phase of gear display
  chip->motorAphase=0;
  chip->motorBphase=0;
  // positioning
  chip->motor_A_y=20;
  chip->motor_A_x=113;
  chip->motor_2_x=20;
  chip->motor_B_x=187;
  // arrow and speed base position
  chip->motor_1_2_arrow_y = 70;
  // Motor A arrow positions  relative to motor A position
  chip->motorA_right_arrow_x =chip->motor_A_x + 40;
  chip->motorA_left_arrow_x = chip->motor_A_x  ;
  // Motor B arrow positions  relative to motor B position
  chip->motorB_right_arrow_x = chip->motor_B_x +  40;
  chip->motorB_left_arrow_x = chip->motor_B_x;
  // bars position relative to motor
  chip-> bar_1_2_y=  chip->motor_1_2_arrow_y + 20;
  chip-> bar_left_x=chip->motor_A_x ;
  chip-> bar_right_x=chip->motor_B_x ;
   printf("Framebuffer: fb_w=%d, fb_h=%d\n", chip->fb_w, chip->fb_h);
//  box around displays
   draw_rectangle(chip, 101,10,75,100,chip->green ,0);
   draw_rectangle(chip, 175,10,75,100,chip->green ,0);
// wires top
   draw_rectangle(chip, 21,3,130,1, chip-> white   ,0); 
   draw_rectangle(chip, 21,4,25,5, chip-> white   ,0);
   draw_rectangle(chip, 126,4,25,5, chip-> white  ,0);
// wires base   
   draw_rectangle(chip, 21,115,207,1, chip-> white  ,0);
   draw_rectangle(chip, 21,110,25,5, chip-> white   ,0);
   draw_rectangle(chip, 203,110,25,5, chip-> white   ,0);
// board
  printf("Draw the board ...\n");
  draw_board(chip, 10,1);
  printf("Draw the speed holders ...\n");
  draw_speed(chip, chip-> bar_left_x,chip-> bar_1_2_y,50,15, chip-> purple  ,0);
  draw_speed(chip, chip-> bar_right_x,chip-> bar_1_2_y,50,15, chip-> purple  ,0);
  //Draw the cogs
  printf("Draw the cogs ...\n");
  draw_cog(chip, chip->motor_A_y,chip->motor_A_x,0);
  draw_cog(chip, chip->motor_A_y,chip->motor_B_x,0);



const timer_config_t timer_config_Awatchdog = {
    .callback = chip_timer_event_Awatchdog,
    .user_data = chip,
  };
  timer_t timer_Awatchdog = timer_init(&timer_config_Awatchdog);
  timer_start(timer_Awatchdog,100000, true);

const timer_config_t timer_config_Bwatchdog = {
    .callback = chip_timer_event_Bwatchdog,
    .user_data = chip,
  };
  timer_t timer_Bwatchdog = timer_init(&timer_config_Bwatchdog);
  timer_start(timer_Bwatchdog,100000, true);



const timer_config_t timer_config_motorA = {
    .callback = chip_timer_event_motorA,
    .user_data = chip,
  };
  timer_t timer_motorA = timer_init(&timer_config_motorA);
  timer_start(timer_motorA,100000, true);

const timer_config_t timer_config_motorB = {
    .callback = chip_timer_event_motorB,
    .user_data = chip,
  };
  timer_t timer_motorB = timer_init(&timer_config_motorB);
  timer_start(timer_motorB, 10000, true);


// config for PWM A watch
const pin_watch_config_t watch_config_PWM_A = {
    .edge = BOTH,
    .pin_change = chip_pin_change_PWM_A,
    .user_data = chip
  };

// config for PWM B watch
const pin_watch_config_t watch_config_PWM_B = {
    .edge = BOTH,
    .pin_change = chip_pin_change_PWM_B,
    .user_data = chip
  };

  // PWM watches
  printf("PWM watches ...\n");
  pin_watch(chip->pin_ENA, &watch_config_PWM_A );
  pin_watch(chip->pin_ENB, &watch_config_PWM_B );
  
  // config for other pins IN1 IN2 IN3 IN4
  const pin_watch_config_t watch_config = {
    .edge = BOTH,
    .pin_change = chip_pin_change,
    .user_data = chip
  };

  // pins watches
  printf("pins watches ...\n");
  pin_watch(chip->pin_IN1, &watch_config);
  pin_watch(chip->pin_IN2, &watch_config);
  pin_watch(chip->pin_IN3, &watch_config);
  pin_watch(chip->pin_IN4, &watch_config);





}

// PWM A pin change function for watch
void chip_pin_change_PWM_A(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;
  uint8_t ENA = pin_read(chip->pin_ENA);

// channel A using PWM

  if (ENA){
    chip->high_ENA = get_sim_nanos();
    chip->low_time_ENA = chip->high_ENA - chip->low_ENA;
  } 
  else 
  {
    chip->low_ENA = get_sim_nanos();
    chip->high_time_ENA = chip->low_ENA - chip->high_ENA ;
  }

  float total_ENA = chip->high_time_ENA + chip->low_time_ENA;
  
  int duty_cycle_ENA = (chip->high_time_ENA / total_ENA) * 100.0;
  chip->speed_percent_A=duty_cycle_ENA;

  // if a change then redisplay
  if ( chip->previous_speed_percent_A != chip->speed_percent_A)
  {
   draw_state(chip);
   chip->previous_speed_percent_A = chip->speed_percent_A;
  }



  
}

// PWM B pin change function for watch
void chip_pin_change_PWM_B(void *user_data, pin_t pin, uint32_t value) {
  
  chip_state_t *chip = (chip_state_t*)user_data;
  uint8_t ENB = pin_read(chip->pin_ENB);
  

// channel B using PWM

  if (ENB){
    chip->high_ENB= get_sim_nanos();
    chip->low_time_ENB= chip->high_ENB- chip->low_ENB;
  } else {
    chip->low_ENB= get_sim_nanos();
    chip->high_time_ENB= chip->low_ENB- chip->high_ENB;
  }
  float total = chip->high_time_ENB+ chip->low_time_ENB;
  int duty_cycle_ENB = (chip->high_time_ENB / total) * 100.0;
  chip->speed_percent_B=duty_cycle_ENB;
 

 // if a change then redisplay
 if ( chip->previous_speed_percent_B != chip->speed_percent_B  )
  {
   draw_state(chip);
   chip->previous_speed_percent_B = chip->speed_percent_B;
  }
 
 
}


void chip_pin_change(void *user_data, pin_t pin, uint32_t value) {
  chip_state_t *chip = (chip_state_t*)user_data;
  uint8_t ENA = pin_read(chip->pin_ENA);
  uint8_t ENB = pin_read(chip->pin_ENA);
  uint8_t IN1 = pin_read(chip->pin_IN1);
  uint8_t IN2 = pin_read(chip->pin_IN2);
  uint8_t IN3 = pin_read(chip->pin_IN3);
  uint8_t IN4 = pin_read(chip->pin_IN4);
 
  // read control for PWM used
  // needs to change to detect held HIGH or LOW for NO PWM
  uint8_t use_PWM_ENA= attr_read_float(chip->use_PWM_ENA);
  uint8_t use_PWM_ENB= attr_read_float(chip->use_PWM_ENB);

  if (use_PWM_ENA )
  {
  if ( ENA && IN1 && !IN2) chip-> drive_A_state =  0;
  if ( ENA && !IN1 && IN2) chip-> drive_A_state =  1;
  if ( ENA && IN1 == IN2) chip-> drive_A_state =   2;
  if ( !ENA ) chip-> drive_A_state =               3;
  }
  else
  {
  //drive 1 states
  if ( IN1 && !IN2) chip-> drive_A_state =  0;
  if (!IN1 && IN2) chip->  drive_A_state =  1;
  if ( IN1 == IN2) chip->  drive_A_state =  2;

  }

if (use_PWM_ENB )
  {
  // drive 2 states
  if ( ENB && IN3 && !IN4) chip-> drive_B_state = 0;
  if ( ENB && !IN3 && IN4) chip-> drive_B_state = 1;
  if ( ENB && IN3 == IN4) chip-> drive_B_state =  2;
  if ( !ENB ) chip-> drive_A_state =              3;
  }
  else
 {
  if (IN3 && !IN4) chip-> drive_B_state = 0;
  if ( !IN3 && IN4) chip-> drive_B_state = 1;
  if (IN3 == IN4) chip-> drive_B_state =  2;
 }
  draw_state(chip);
}






void draw_state(chip_state_t *chip) {
  
  //turn off the two timers
  timer_stop(0);
  timer_stop(1);

// backwards
 if (chip-> drive_A_state == 0) 
 {
   // remove left - place right
    draw_right_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorA_right_arrow_x,0);
    draw_left_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorA_left_arrow_x,1);

 }
 //forwards
 if (chip-> drive_A_state == 1) 
 {
    // remove right - place left
  draw_left_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorA_left_arrow_x,0);
  draw_right_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorA_right_arrow_x,1);
 }
 //stopped
 if (chip-> drive_A_state == 2 || chip-> drive_A_state == 3)
 {
  // remove left and right
  draw_right_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorA_right_arrow_x,1);
  draw_left_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorA_left_arrow_x,1); 
 }




 if (chip-> drive_B_state == 0)
 { 
    // remove left - place right
    draw_right_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorB_right_arrow_x,0);
    draw_left_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorB_left_arrow_x,1);
 }


 if (chip-> drive_B_state == 1) 
 {
  // remove right - place left
  draw_right_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorB_right_arrow_x,1);
  draw_left_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorB_left_arrow_x,0);
 }

 if (chip-> drive_B_state == 2 || chip-> drive_B_state == 3)
 {
  // remove both arrows
  draw_right_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorB_right_arrow_x,1);
  draw_left_arrow(chip,chip->motor_1_2_arrow_y ,chip->motorB_right_arrow_x,1); 
 }




  if (chip-> drive_A_state == 0 || chip-> drive_A_state == 1)
  {
   timer_start(0, (100 - chip->speed_percent_A) *1000 * 3 , 1);
  }
   if (chip-> drive_B_state == 0 || chip-> drive_B_state == 1)
   {
   timer_start(1, (100 - chip->speed_percent_B) *1000 * 3, 1);
   }

    printf( "   chip->speed_percent_A %d chip->speed_percent_b  %d\n",chip->speed_percent_A,chip->speed_percent_B);
  
   draw_speed(chip, chip-> bar_left_x,chip-> bar_1_2_y,50,15, chip-> purple  ,chip->speed_percent_A);
   draw_speed(chip, chip-> bar_right_x,chip-> bar_1_2_y,50,15, chip-> purple  ,chip->speed_percent_B);
}







// graphics for motor A
void chip_timer_event_motorA(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;
  draw_cog(chip, chip->motor_A_y,chip->motor_A_x,chip->motorAphase);
  if ( chip-> drive_A_state == 0 ) chip->motorAphase=chip->motorAphase - 1;
  if ( chip-> drive_A_state == 1) chip->motorAphase=chip->motorAphase + 1;
  if (chip->motorAphase < 0) chip->motorAphase =8;
  if (chip->motorAphase >8) chip->motorAphase = 0;

}

// graphics for motor B
void chip_timer_event_motorB(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;
  draw_cog(chip, chip->motor_2_x,chip->motor_B_x,chip->motorBphase);
  if ( chip-> drive_B_state == 0 ) chip->motorBphase=chip->motorBphase - 1;
  if ( chip-> drive_B_state == 1) chip->motorBphase=chip->motorBphase + 1;
  if (chip->motorBphase < 0) chip->motorBphase = 8;
  if (chip->motorBphase > 8) chip->motorBphase = 0;

}

// watch dog A
void chip_timer_event_Awatchdog(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;
  printf("chip->low_time_ENA %d chip->high_time_ENA %d \n",chip->low_time_ENA ,chip->high_time_ENA );

}

// watch dog A
void chip_timer_event_Bwatchdog(void *user_data) {
  chip_state_t *chip = (chip_state_t*)user_data;
  printf("chip->low_time_ENB %d chip->high_time_ENB %d \n",chip->low_time_ENB ,chip->high_time_ENB );

}





void draw_line(chip_state_t *chip, uint32_t row, rgba_t color) {
  uint32_t offset = chip->fb_w * 4 * row;
  for (int x = 0; x < chip->fb_w * 4; x += 4) {
    buffer_write(chip->framebuffer, offset + x, (uint8_t*)&color, sizeof(color));
  }
}

 void draw_pixel(chip_state_t *chip, uint32_t x,  uint32_t y,  rgba_t color) {
      if (x < 0 || x >= chip->fb_w || y < 0 || y >= chip->fb_h) {
        return;
    }
  uint32_t offset = chip->fb_w * 4 * y ;
  buffer_write(chip->framebuffer, offset + x*4, (uint8_t*)&color, sizeof(color));

}

void draw_cog(chip_state_t *chip, uint32_t x_start,  uint32_t y_start,uint32_t phase) {
// size of our graphic
uint8_t square_size = 50;

uint32_t pixel_spot_data = 0;
for (int x=x_start;x < square_size + x_start; x++)
   {
     for (int y=y_start;y<square_size + y_start ;y++)
    {
        uint32_t pixel_data = all_cogs[phase][pixel_spot_data] ;
        rgba_t  color =
            { 
            .r= (pixel_data  & 0xFF000000) >> 24,
            .g = (pixel_data & 0x00FF0000) >> 16,
            .b = (pixel_data & 0x0000FF00) >> 8,
            .a = (pixel_data & 0x000000FF) 
            };
        uint32_t offset = chip->fb_w * 4 ;  
        buffer_write(chip->framebuffer, (offset * x) + y * 4, (uint8_t*)&color , 4); 
        pixel_spot_data++;
    }
   }
}  

void draw_board(chip_state_t *chip, uint32_t x_start,  uint32_t y_start) {
// size of our graphic
uint8_t square_size = 100;
uint32_t pixel_spot_data = 0;
for (int x=x_start;x < square_size + x_start; x++)
   {
     for (int y=y_start;y<square_size + y_start ;y++)
    {

    // rgba_t  color1 =  rgba_t(test_pattern[pixel_spot_data] );
      uint32_t pixel_data =  board[pixel_spot_data] ; 
        //uint32_t pixel_data = all_cogs[pos][pixel_spot_data] ;
        rgba_t  color =
            { 
            .r= (pixel_data  & 0xFF000000) >> 24,
            .g = (pixel_data & 0x00FF0000) >> 16,
            .b = (pixel_data & 0x0000FF00) >> 8,
            .a = (pixel_data & 0x000000FF) 
            };
        
  
        uint32_t offset = chip->fb_w * 4 ;  
        buffer_write(chip->framebuffer, (offset * x) + y * 4, (uint8_t*)&color , 4); 
        pixel_spot_data++;
    }
   }
} 



void draw_rectangle(chip_state_t *chip, uint32_t x_start, uint32_t y_start, uint32_t x_len, uint32_t y_len,  rgba_t color, uint8_t fill) {
  // draw the top and base lines
  uint32_t offset = chip->fb_w * 4 * y_start;
  uint32_t offset2 = chip->fb_w * 4 * (y_len + y_start) ;
  for (int x = x_start * 4 ; x < x_start * 4 + x_len * 4; x += 4) {
    buffer_write(chip->framebuffer, offset + x, (uint8_t*)&color, sizeof(color));
    buffer_write(chip->framebuffer, offset2 + x, (uint8_t*)&color, sizeof(color));
  }
// draw the left and right lines
  for (int y = y_start  ; y < y_start + y_len ; y++) {
    buffer_write(chip->framebuffer, y * chip->fb_w * 4 + x_start * 4 , (uint8_t*)&color, sizeof(color));
    buffer_write(chip->framebuffer, y * chip->fb_w * 4 +x_start * 4 + (x_len * 4)-4 , (uint8_t*)&color, sizeof(color));
  }
}


void draw_speed(chip_state_t *chip, uint32_t x_start, uint32_t y_start, uint32_t x_len, uint32_t y_len,  rgba_t color, uint8_t percent_up) {
  // draw the top and base lines
  uint32_t offset = chip->fb_w * 4 * y_start;
  uint32_t offset2 = chip->fb_w * 4 * (y_len + y_start) ;

  for (int x = x_start * 4 ; x < x_start * 4 + x_len * 4; x += 4) {
    buffer_write(chip->framebuffer, offset + x, (uint8_t*)&color, sizeof(color));
    buffer_write(chip->framebuffer, offset2 + x, (uint8_t*)&color, sizeof(color));
  }
// draw the left and right lines
  for (int y = y_start  ; y < y_start + y_len ; y++) {
    buffer_write(chip->framebuffer, y * chip->fb_w * 4 + x_start * 4 , (uint8_t*)&color, sizeof(color));
    buffer_write(chip->framebuffer, y * chip->fb_w * 4 +x_start * 4 + (x_len * 4)-4 , (uint8_t*)&color, sizeof(color));
  }
 rgba_t color_black = chip->black;
 for (int y = y_start +1 ; y < y_start + y_len  ; y++) {

    for ( int z= (x_start +1) * 4 ; z < (x_start) * 4 + ((x_len-1)* 4) ; z+= 4)
    {
    buffer_write(chip->framebuffer, y * chip->fb_w * 4 + z , (uint8_t*)&color_black , sizeof(color_black ));
    
    }
 }



    rgba_t color2 = chip->white;
    percent_up = percent_up /2;
    //printf(" x_len %d       percent_up %d\n",x_len, percent_up);
    for (int y = y_start +1 ; y < y_start + y_len  ; y++) {

    for ( int z= (x_start +1) * 4 ; z < (x_start) * 4 + ((percent_up)* 4) ; z+= 4)
    {
    buffer_write(chip->framebuffer, y * chip->fb_w * 4 + z , (uint8_t*)&color2, sizeof(color2));
    
    }
  }
}

void draw_right_arrow(chip_state_t *chip, uint32_t x_start,  uint32_t y_start, uint8_t wipe) {
uint32_t pixel_spot_data = 0;
for (int x=x_start;x < RIGHT_HEIGHT + x_start; x++)
   {
     for (int y=y_start;y<  RIGHT_WIDTH+ y_start ;y++)
    {
       uint32_t pixel_data = right_arrow[pixel_spot_data] ;
        rgba_t  color =
            { 
            .r=  (pixel_data  & 0xFF000000) >> 24,
            .g = (pixel_data & 0x00FF0000) >> 16,
            .b = (pixel_data & 0x0000FF00) >> 8,
            .a = (pixel_data & 0x000000FF) 
            };
        
     if(wipe == 1)
{
  color =  chip-> black;
}   
        uint32_t offset = chip->fb_w * 4 ;  
        buffer_write(chip->framebuffer, (offset * x) + y * 4, (uint8_t*)&color , 4); 
        pixel_spot_data++;
    }
   }
}  

void draw_left_arrow(chip_state_t *chip, uint32_t x_start,  uint32_t y_start, uint8_t wipe) {
uint32_t pixel_spot_data = 0;
for (int x=x_start;x < RIGHT_HEIGHT + x_start; x++)
   {
     for (int y=y_start;y<  RIGHT_WIDTH+ y_start ;y++)
    {
        uint32_t pixel_data = left_arrow[pixel_spot_data] ;
        rgba_t  color =
            { 
            .r= (pixel_data  & 0xFF000000) >> 24,
            .g = (pixel_data & 0x00FF0000) >> 16,
            .b = (pixel_data & 0x0000FF00) >> 8,
            .a = (pixel_data & 0x000000FF) 
            };
        
        
if(wipe == 1)
{
  color =  chip-> black;
}
        uint32_t offset = chip->fb_w * 4 ;  
        buffer_write(chip->framebuffer, (offset * x) + y * 4, (uint8_t*)&color , 4); 
        pixel_spot_data++;
    }
   }
}  




