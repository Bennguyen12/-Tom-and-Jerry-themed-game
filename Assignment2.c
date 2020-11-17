#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <graphics.h>
#include <lcd.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <cpu_speed.h>
#include <graphics.h>
#include <macros.h>
#include "lcd_model.h"
#include <stdbool.h>
#include "cab202_adc.h"
#include "usb_serial.h"


bool start_move=false;
bool game_over= false;
#define PI 3.14159265358979
bool pause;
#define SQRT(x,y) sqrt(x*x + y*y)
int score;
int health;   
double adc_left;
double adc_right;
//Level state
int level=1;
bool level_up=false;
bool return_ini=false;
bool appear_bowl=false;
bool start_count_time_cheese=false;// appear cheese 2 senconds after consuming
bool start_count_time_trap=false;
bool start_count_time_bowl=false;
bool start_count_time_transform=false;
bool super_power=false;
bool appear_door;
bool increase_size=false;
bool game_over_activate=false;
#define BIT(x) (1 << (x))
#define OVERFLOW_TOP (1023)
char tx_buffer[32];
char tx_buffer1[32];
char tx_buffer2[32];
char tx_buffer3[32];
char tx_buffer4[32];
char tx_buffer5[32];
char tx_buffer6[32];
char tx_buffer7[32];
char tx_buffer8[32];
char tx_buffer9[32];
char tx_buffer10[32];
//----------------------------------------------------------------------------
double CalcDistanceBetween2Points(int x1, int y1, int x2, int y2)
{
    return SQRT((x1-x2), (y1-y2));
}
bool collided_point(int x1,int y1,int x2,int y2, double value){
     double dist1 = CalcDistanceBetween2Points(x1, y1, x2, y2);
    
    if (abs(dist1)<= value ){
        return true;
    }
    else{
        return false;
    }
}
bool collided (int x, int y, int x1, int y1, int x2, int y2, double value )
{
    double dist1 = CalcDistanceBetween2Points(x, y, x1, y1);
    double dist2 = CalcDistanceBetween2Points(x, y, x2, y2);
    double dist3 = CalcDistanceBetween2Points(x1, y1, x2, y2);
    
   if ( abs(dist3 - (dist1 + dist2)) <= value){
       return true;
   }
   else{
       return false;
   }
}
void usb_serial_read_string(char * message){
   int c = 0;
   int buffer_count=0;
      
   while (c != '\n'){
         c = usb_serial_getchar();
         message[buffer_count]=c;
         buffer_count++;
    }
         

}
//------------------------------------------------------------------
char buffer[2];
void draw_int(uint8_t x, uint8_t y, int value, colour_t colour) {
	snprintf(buffer, sizeof(buffer), "%d", value);
	draw_string(x, y, buffer, colour);
}
//-----------------------------------------------
void screen_game_over(){
    clear_screen();
    draw_string(0,10,"   Press right",FG_COLOUR);
    draw_string(0,20,"   button to ",FG_COLOUR);
    draw_string(0,30,"   restart ",FG_COLOUR);
    show_screen();

}
void usb_serial_send(char * message) {

	usb_serial_write((uint8_t *) message, strlen(message));
}

//---------------------------------------------------------------------------------
double wx[4][2],wy[4][2];
void setup_wall0(){
     wx[0][0]=0;
     wy[0][0]=35.0;
}
void setup_wall(){
    wx[0][0]=25.0; wy[0][0]=35.0;wx[0][1]=25.0;wy[0][1]=45.0;
    wx[1][0]=18.0; wy[1][0]=15.0;wx[1][1]=13.0;wy[1][1]=25.0;
    wx[2][0]=45.0; wy[2][0]=10.0;wx[2][1]=60.0;wy[2][1]=10.0;
    wx[3][0]=58.0;wy[3][0]=25.0; wx[3][1]=72.0;wy[3][1]=30.0;
}   
void update_wall(){
 wx[0][1]=wx[0][0], wy[0][1]=wy[0][0]+15.0;
 wx[1][0]=wx[1][1]+5.0, wy[1][0]=wy[1][1]-10.0;
 wx[2][1]=wx[2][0]+15.0,wy[2][1]=wy[2][0];
 wx[3][1]=wx[3][0]+14.0,wy[3][1]=wy[3][0]+5.0;
        double dwx0=0.2*adc_right;
        double dwy0=0;
        wx[0][0]+=dwx0;wy[0][0]+=dwy0;
        
       double dwx1=0.13*adc_right;
        double dwy1= -((wx[1][0]-wx[1][1])/(wy[1][0]-wy[1][1]))*dwx1;
        wx[1][1]+=dwx1; wy[1][1]+=dwy1;
       
        double dwx2=0;
        double dwy2=0.09*adc_right;
        wx[2][0]+=dwx2;wy[2][0]+=dwy2;
        
         double dwx3=-0.05*adc_right;
        double dwy3=-((wx[3][0]-wx[3][1])/(wy[3][0]-wy[3][1]))*dwx3;
        wx[3][0]+=dwx3;  wy[3][0]+=dwy3;
        
    if  (wx[0][0]>83.0){
      setup_wall0();
    }
    if ( wy[2][0]>47.0){
       wy[2][0]=10.0;
 wx[2][0]=45.0;
    }
    if  (wy[1][1]>47.0){
            wy[1][1]=18.5;
            wx[1][1]=0.0;
    }
    if (wy[3][1]>47.0){
        wy[3][0]=10.0;
        wx[3][0]=63.35;
    }
    if (wx[0][0]<0.0){
        wx[0][0]=83.0;
        wy[0][0]=35.0;
    }
    if (wy[2][0]<10.0){
        wx[2][0]=45.0;
        wy[2][0]=47.0;
    } 
    if (wx[1][1]<0){
          wy[1][1]=47.0;
            wx[1][1]=57.0;
    }
    if (wy[3][0]<10.0 ){
         wy[3][0]=42; 
        wx[3][0]=51.93;
    }

}
void draw_wall(){
    draw_line(wx[0][0],wy[0][0],wx[0][1],wy[0][1],FG_COLOUR);
    draw_line(wx[1][0],wy[1][0],wx[1][1],wy[1][1],FG_COLOUR);
    draw_line(wx[2][0],wy[2][0],wx[2][1],wy[2][1],FG_COLOUR);
    draw_line(wx[3][0],wy[3][0],wx[3][1],wy[3][1],FG_COLOUR);
}
//---------------------------------------------------------------------------------------
// Set up time
int time;
void setup_timer0(){
   
        	CLEAR_BIT(TCCR0B,WGM02);
		SET_BIT(TCCR0B,CS02);  //1
		CLEAR_BIT(TCCR0B,CS01); //0
		SET_BIT(TCCR0B,CS00);  

          SET_BIT(TIMSK0, TOIE0);
          sei();

}


char buffer1[3];
void draw_int_time(uint8_t x, uint8_t y, int value, colour_t colour) {
	snprintf(buffer1, sizeof(buffer1), "%02d", value);
	draw_string(x, y, buffer1, colour);
}
 volatile uint8_t state_count1=0;
 volatile  uint8_t switch_closed1;
  volatile uint8_t state_count2=0;
 volatile  uint8_t switch_closed2;
  volatile uint8_t state_count3=0;
 volatile  uint8_t switch_closed3;
  volatile uint8_t state_count4=0;
 volatile  uint8_t switch_closed4;
  volatile uint8_t state_count5=0;
 volatile  uint8_t switch_closed5;
  volatile uint8_t state_count6=0;
 volatile  uint8_t switch_closed6;
  volatile uint8_t state_count7=0;
 volatile  uint8_t switch_closed7;
volatile int overflow_counter0 = 0;
volatile int overflow_counter1 = 0;
volatile int overflow_counter2 = 0;
volatile int overflow_counter3 = 0;
volatile int overflow_counter4 = 0;
   ISR(TIMER0_OVF_vect) {
        uint8_t mask=0b00000111;

	 if (BIT_IS_SET(PIND,1)){
				
               	  	state_count1= ((state_count1<<1) & (0b00000111)) |1;
				 }
				 else 	state_count1= ((state_count1<<1) & (0b00000111)) |0;
				if (state_count1==mask)	switch_closed1=1;

                 
                 else if (state_count1==0)	switch_closed1=0; 
        
	 if (BIT_IS_SET(PINB,7)){
				
               	  	state_count2= ((state_count2<<1) & (0b00000111)) |1;
				 }
				 else 	state_count2= ((state_count2<<1) & (0b00000111)) |0;
				if (state_count2==mask)	switch_closed2=1;
                
                 else if (state_count2==0)	switch_closed2=0; 
        if (BIT_IS_SET(PINB,1)){
				
               	  	state_count3= ((state_count3<<1) & (0b00000111)) |1;
				 }
				 else 	state_count3= ((state_count3<<1) & (0b00000111)) |0;
				if (state_count3==mask)	switch_closed3=1;

                 
                 else if (state_count3==0)	switch_closed3=0; 
        if (BIT_IS_SET(PIND,0)){
				
               	  	state_count4= ((state_count4<<1) & (0b00000111)) |1;
				 }
				 else 	state_count4= ((state_count4<<1) & (0b00000111)) |0;
				if (state_count4==mask)	switch_closed4=1;

                 
                 else if (state_count4==0)	switch_closed4=0; 
    if (BIT_IS_SET(PINB,0)){
				
               	  	state_count5= ((state_count5<<1) & (0b00000111)) |1;
				 }
				 else 	state_count5= ((state_count5<<1) & (0b00000111)) |0;
				if (state_count5==mask)	switch_closed5=1;

                 
                 else if (state_count5==0)	switch_closed5=0; 
        if (BIT_IS_SET(PINF,6)){
				
               	  	state_count6= ((state_count6<<1) & (0b00000111)) |1;
				 }
				 else 	state_count6= ((state_count6<<1) & (0b00000111)) |0;
				if (state_count6==mask)	switch_closed6=1;

                 
                 else if (state_count6==0)	switch_closed6=0; 
        if (BIT_IS_SET(PINF,5)){
				
               	  	state_count7= ((state_count7<<1) & (0b00000111)) |1;
				 }
				 else 	state_count7= ((state_count7<<1) & (0b00000111)) |0;
				if (state_count7==mask)	switch_closed7=1;

                 
                 else if (state_count7==0)	switch_closed7=0; 
                
      
       if (pause==false){
             overflow_counter0 ++;

            if(start_count_time_cheese==true){
                   overflow_counter1=0;
                   start_count_time_cheese=false;
            }
            if (start_count_time_cheese==false){
                    overflow_counter1++;
            }
           if(start_count_time_trap==true){
                   overflow_counter2=0;
                   start_count_time_trap=false;
            }
            if (start_count_time_trap==false){
                    overflow_counter2++;
            }
           if(start_count_time_bowl==true){
                   overflow_counter3=0;
                   start_count_time_bowl=false;
            }
            if (start_count_time_bowl==false){
                    overflow_counter3++;
            }
            if(start_count_time_transform==true){
                   overflow_counter4=0;
                   start_count_time_transform=false;
            }
            if (start_count_time_transform==false){
                    overflow_counter4++;
            }

 
    }
    if (pause==true){
         overflow_counter2=overflow_counter2;
       overflow_counter1=overflow_counter1;
        overflow_counter0=overflow_counter0;
        overflow_counter3=overflow_counter3;
        overflow_counter4=overflow_counter4;
    }
}

 int elapsed_time(void){
    int time=(overflow_counter0 * 256.0+TCNT0)*1024/(8e6);
    return time;
}
double time1;
double elapsed_time1(){
     double time1=(overflow_counter1 * 256.0+TCNT0)*1024/(8e6);
        if (time1>2.0){
            overflow_counter1=0;
        }
     

        return time1;
}

double time2;
double elapsed_time2(){
     double time2=(overflow_counter2 * 256.0+TCNT0)*1024/(8e6);
        if (time2>3.0){
            overflow_counter2=0;
        }
        
        return time2;
}
double time3;
double elapsed_time3(){
     double time3=(overflow_counter3 * 256.0+TCNT0)*1024/(8e6);
        if (time3>5.0){
            overflow_counter3=0;
        }
        return time3;
}
double time4;
double elapsed_time4(){
     double time4=(overflow_counter4 * 256.0+TCNT0)*1024/(8e6);
        if (time4>10.0){
            overflow_counter4=0;
        }
        return time4;
}

//--------------------------------------------------------------------------------------
// Jerry
int jx[6];
int  jy[6];
void  setup_jerry(){
    jx[0]=0;
    jx[1]=3;
    jx[2]=2;
    jx[3]=2;
    jx[4]=0;
    jx[5]=2;
    jy[0]=9;
    jy[1]=9;
    jy[2]=10;
    jy[3]=11;
    jy[4]=12;
    jy[5]=12;
}
void setup_jerry1(){
        jx[0]=30;
    jx[1]=33;
    jx[2]=32;
    jx[3]=32;
    jx[4]=30;
    jx[5]=32;
    jy[0]=20;
    jy[1]=20;
    jy[2]=21;
    jy[3]=22;
    jy[4]=23;
    jy[5]=23;
}
  
void draw_jerry(){
   
    draw_line(jx[0],jy[0],jx[1],jy[1],FG_COLOUR);
    draw_line(jx[2],jy[2],jx[3],jy[3],FG_COLOUR);
    draw_line(jx[4],jy[4],jx[5],jy[5],FG_COLOUR);
}
void update_jerry(int char_code){
        double time4=elapsed_time4();
      if ((switch_closed4 !=0)&& (jx[1]<LCD_X-2) ){
         
           for (int i=0;i<6;i++){
               jx[i]=jx[i]+adc_left;
           }
             
        }
        if ( (switch_closed1 != 0) && (jy[1]>10 )){
          
               for (int i=0;i<6;i++){
               jy[i]=jy[i]-adc_left;  
           }
            
        }	
        if ((switch_closed2 !=0) && (jy[5]<LCD_Y-2) ){   
               for (int i=0;i<6;i++){
               jy[i]=jy[i]+adc_left;
           }

        }	
        if ((switch_closed3 !=0) && (jx[0]>0) ){
           
            for (int i=0;i<6;i++){
               jx[i]=jx[i]-adc_left;
           }

      } 
       if ( jx[1]<LCD_X-1 && char_code=='d' ){
         
           for (int i=0;i<6;i++){
                 jx[i]=jx[i]+adc_left;
           }
             snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
             
        }
        if ( jy[1]>9 && char_code=='w'){
          
               for (int i=0;i<6;i++){
                jy[i]=jy[i]-adc_left;
                    
           }
             snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
            
        }	
        if ( jy[5]<LCD_Y-1 && char_code=='s'){   
               for (int i=0;i<6;i++){
                 jy[i]=jy[i]+adc_left;
           }
   snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
        }	
        if ( jx[0]>0 && char_code=='a' ){
           
            for (int i=0;i<6;i++){
               jx[i]=jx[i]-adc_left;
           }
   snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
      } 
      if(super_power==false){  
         for (int j=0;j<4;j++){
        int jy1=jy[0]+j;
        int jx1=jx[0]+j;
        if (collided(jx[0]-1,jy1,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)){
           for (int i=0;i<6;i++){
               jx[i]=jx[i]+2;
           }

        }
        if (collided(jx1,jy[0]-1,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)){
             for (int i=0;i<6;i++){
               jy[i]=jy[i]+2; 
             }

        }
        if (collided(jx[1]+1,jy1,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5) ){
                 for (int i=0;i<6;i++){
               jx[i]=jx[i]-2;
                 }           
        }
        if(collided(jx1,jy[5]-1,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)){
               for (int i=0;i<6;i++){
               jy[i]=jy[i]-2;
               }
    }
      }
      }
    if (super_power==true){
        
        if((time4<9.8) && (increase_size==true) ){
            jx[1]++;
            jx[2]++;
            jx[3]++;
            jy[3]++;
            jy[4]++;
            jx[5]++;
            jy[5]++;
            increase_size=false;
        }
    
    
    if((time4>9.9) && (increase_size==false) ){
     
        super_power=false;
         jx[1]--;
            jx[2]--;
            jx[3]--;
            jy[3]--;
            jy[4]--;
            jx[5]--;
            jy[5]--;
            
    } 
    }
}
    

//------------------------------------------------------------------------------------------
// Tom
double tx[4];
double ty[4];
void setup_tom_ini(){
    tx[0]=LCD_X-6;
    tx[1]=LCD_X-2;
    tx[2]=LCD_X-4;
    tx[3]=LCD_X-4;
    ty[0]=LCD_Y-9;
    ty[1]=LCD_Y-9;
    ty[2]=LCD_Y-8;
    ty[3]=LCD_Y-5;
}
void draw_tom(){
    draw_line(tx[0],ty[0],tx[1],ty[1],FG_COLOUR);
    draw_line(tx[2],ty[2],tx[3],ty[3],FG_COLOUR);
}
double tdx,tdy;
void setup_tom(){
    double dir=rand() % 160+130;
    double speed=rand() % 76+65;
    tdx=speed/100*cos(dir*PI/100)/2;
    tdy=speed/100*sin(dir*PI/100)/2;
   
}
 int new_tx[4];
    int new_ty[4];
void move_tom(){
   
    for (int i=0;i<4;i++){
    new_tx[i]=round(tx[i]+tdx);
    new_ty[i]=round(ty[i]+tdy);
    }
    bool bounced = false;
    
    if (new_tx[0]<1||new_tx[1]>LCD_X-1){
        bounced = true;
        setup_tom();
    }
    if (new_ty[0]<11||new_ty[3]>LCD_Y-1){
        bounced = true;
        setup_tom();
    }
  
    for(int m=0;m<5;m++){
         int ty1=ty[0]+m;
        int ty2=ty[1]+m;
        int tx1=tx[0]+m;
        for (int j=0;j<4;j++){
       
    if(collided(tx[0]-2,ty1,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)||collided(tx[1]+2,ty2,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)){
        tdx=-tdx;
     
    }
    if(collided(tx1,ty[0]-2,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)||collided(tx1,ty[3]+2,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)){
        tdy=-tdy;
    
    }
    if ((collided(tx[0]-2,ty1,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)) && tx[1]>82 && tx[1]<85){
        setup_tom_ini();
    }
    if(collided(tx[1]+2,ty2,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5) && tx[0]<1 && tx[0]>-1 ){
        setup_tom_ini();
    }
    if(collided(tx1,ty[0]-2,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5) && ty[3]>46 && ty[3]<49){
        setup_tom_ini();
    } 
    if (collided(tx1,ty[3]+2,wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5) && ty[0]<10 && ty[0]>8){
        setup_tom_ini();
    }



    }
    }
      if (!bounced){
        for(int i=0;i<4;i++){
        tx[i] += tdx*adc_left;
        ty[i] += tdy*adc_left;
    }
    }
}

void update_tom(){
   
        move_tom();
    if(super_power==false){
    for (int i=0;i<4;i++){
        int  jx1=jx[0]+i;
        int jy1=jy[0]+i;
    if (collided(jx1,jy[0],tx[3]-1,ty[3],tx[3]+1,ty[3],10e-5)||collided(jx1,jy[5],tx[0],ty[0],tx[1],ty[1],10e-5)||collided(jx[0],jy1,tx[1],ty[1],tx[3],ty[3],10e-5)||collided(jx[1],jy1,tx[0],ty[0],tx[3],ty[3],10e-5)){
                setup_jerry();
                setup_tom_ini();
                health--;
                if(health<1){
                        game_over_activate=true;
    }
    }
    }
}
}
//----------------------------------------------------
//Cheese
int cheese;
int cheese_appear;
bool order_cheese[5];
int cx[5],cy[5];
void setup_inicheese(){
      
    for (int i=0;i<5;i++){
    cx[i]=-5;
    order_cheese[i]=false;
}
}

void setup_cheese(int i){
                    
        cx[i]=rand()% 75;
        cy[i]=13+ rand()% 32;
         order_cheese[i]= true;
    }
void draw_cheese(){
    for (int i=0;i<5;i++){
        draw_line(cx[i],cy[i],cx[i]+2,cy[i]-2,FG_COLOUR);
        draw_line(cx[i]+2,cy[i]-2,cx[i]+4,cy[i],FG_COLOUR);
        draw_line(cx[i],cy[i],cx[i]+4,cy[i],FG_COLOUR);
    }
}

void update_cheese(){
    double time1=elapsed_time1();
    if(pause==true){
      for (int i=0;i<5;i++){
                if (collided_point(jx[0]+1,jy[0]+2,cx[i]+2,cy[i]+1,3)){
                score++;
                cheese++;
                cheese_appear--;
                cx[i]=-10;
                }
            }
        } 
                
    if (pause==false){
      for (int i=0;i<5;i++){
            if (collided_point(jx[0]+1,jy[0]+2,cx[i]+2,cy[i]+1,3)){
                score++;
                cheese++;
                cheese_appear--;
                cx[i]=-10;
                 order_cheese[i]=false;      // After Jerry ate Cheese, another Cheese will appear after 2s
                 start_count_time_cheese=true;
                 if(time1>1.98) {
                    if(order_cheese[i]==false){
                        cheese_appear++;
                             setup_cheese(i);
                             start_count_time_cheese=false;
                            return;
                    }
                }
             }
      }

                for (int i =0;i<5;i++){
                    if(time1>1.98){
                    if(order_cheese[i]==false){
                        cheese_appear++;
                    setup_cheese(i);
                    return;
                     }
                    
                }
            }
    }
            
  }


//-------------------------------------------------------------------------------
//Trap
int trap;
bool order_trap[5];
int trx[5],try[5];
void setup_initrap(){
        for (int i=0;i<5;i++){
            trx[i]=-10;
            order_trap[i]=false;
        }
}
void setup_trap(int i){
   
       trx[i]=tx[2];
        try[i]=ty[2]+1;
        order_trap[i]=true;
    
}
void draw_trap(){
    for (int i=0;i<5; i++){
        draw_pixel(trx[i],try[i],FG_COLOUR);
        draw_pixel(trx[i]-1,try[i]-1,FG_COLOUR);
        draw_pixel(trx[i]-1,try[i]+1,FG_COLOUR);
        draw_pixel(trx[i]+1,try[i]+1,FG_COLOUR);
        draw_pixel(trx[i]+1,try[i]-1,FG_COLOUR);
    }
}

void update_trap(){
    double time2=elapsed_time2();
     if(pause==true){
      for (int i=0;i<5;i++){
               if (collided_point(jx[0]+1,jy[0]+1,trx[i],try[i],3)){
                     health--;
                     trap--;
                     trx[i]=-10;
                     if(health<1){
                        game_over_activate=true;
                    }
                }
        }
     } 
                
    if (pause==false){
         if(super_power==false){
            for (int i=0;i<5;i++){
         
                 if (collided_point(jx[0]+1,jy[0]+1,trx[i],try[i],3)){
                health--;
                trap--;
                trx[i]=-10;
                 order_trap[i]=false; 
                 start_count_time_trap=true;     
                 if(time2>2.99) {
                    if(order_trap[i]==false){
                        trap++;
                             setup_trap(i);
                             start_count_time_trap=false;
                            return;
                    }
                }
                if(health<1){
                        
                        game_over_activate=true;
             }
      }
      }

                for (int i =0;i<5;i++){
                    if(time2>2.99){
                    if(order_trap[i]==false){
                        trap++;
                    setup_trap(i);
                    return;
                     }
                    
                }
            }
    }
            
  }
}
//--------------------------------------------------------------------------------------
//Weapon
int weapon;
bool order_weapon[20];
// activate weapon

// state = 0b00000000;
// set_bit(state, 0)
double wpx[20],wpy[20];
double wdx,wdy;
void setup_iniweapon(){
  for (int i=0;i<20;i++){
    wpx[i]=-10;
    wpy[i]=1;
    order_weapon[i]=false;

}
}
void setup_weapon(int i){
  
wpx[i]=jx[0]+3;
wpy[i]=jy[0]+2;
double w1=tx[2] - wpx[i];
double w2=(ty[2]+1) - wpy[i];
double d= sqrt(w1*w1+w2*w2);
    wdx=w1/d*1.5;
    wdy=w2/d*1.5;
order_weapon[i]=true;
    
}
void draw_weapon(){
    for (int i=0;i<20;i++){
        draw_pixel(wpx[i],wpy[i],FG_COLOUR);
}
}
void update_weapon(){
  for (int i=0;i<20;i++){
      for(int j=0;j<4;j++){
       if (collided(wpx[i],wpy[i],wx[j][0],wy[j][0],wx[j][1],wy[j][1],10e-5)) {
       wpx[i]=-10;
       order_weapon[i]=false;
       weapon--;
       }
      }
     if(collided(wpx[i],wpy[i],tx[0],ty[0],tx[0]+4,ty[0],10e-5)||collided(wpx[i],wpy[i],tx[0]+2,ty[0]+1,tx[0]+2,ty[0]+4,10e-5)){
            score++;
            setup_tom_ini();
            wpx[i]=-10;
            wpy[i]=1;
            weapon--;
    } 

    if (wpx[i]<0 && wpx[i]>-2){
         wpx[i]=-10;
         wpy[i]=1;
    order_weapon[i]=false;
    weapon--;
    }
if(wpy[i]<9 && wpy[i]>7){
       wpx[i]=-10;
         wpy[i]=1;
    order_weapon[i]=false;
    weapon--;
}

    if(wpx[i]>83||wpy[i]>47){
       wpx[i]=-10;
       wpy[i]=1;
    order_weapon[i]=false;
    weapon--;
    }
    if(order_weapon[i]==true){
     wpx[i] +=wdx;
     wpy[i] +=wdy;
 
    
        }   

}
}
//--------------------------------------------------------------------------------------
//Door
int dox,doy;
void setup_inidoor(){
    dox=-10;
}
void setup_door(){
    dox=rand() % 80;
    doy=14+rand()% 27; 
}
void draw_door(){
    
        draw_line(dox,doy,dox+3,doy,FG_COLOUR);
        draw_line(dox,doy,dox,doy+4,FG_COLOUR);
        draw_line(dox,doy+4,dox+3,doy+4,FG_COLOUR);
        draw_line(dox+3,doy+4,dox+3,doy,FG_COLOUR);
        draw_pixel(dox+2,doy+2,FG_COLOUR);

}
void update_door(){
    if((cheese>=5) && (appear_door==true)){
        setup_door();
        appear_door=false;
    }
  
}
//----------------------------------------------------------------------------
//Milk Bowl
int bx,by;
void setup_inibowl(){
    bx=-10;
}
void setup_bowl(){
    bx=tx[0];
    by=ty[0]+2;
     appear_bowl=false;
}
void draw_bowl(){
    draw_line(bx,by,bx,by-1,FG_COLOUR);
    draw_line(bx+1,by+1,bx+3,by+1,FG_COLOUR);
    draw_line(bx+4,by,bx+4,by-1,FG_COLOUR);
}
void update_bowl(){
    double time3=elapsed_time3();
    if(time3>4.99){
        if (appear_bowl==true){
        setup_bowl();
       
    }
    }
    if (collided_point(jx[0]+1,jy[0]+2,bx+2,by+1,3)){
                bx=-10;
                super_power=true;
                increase_size=true;
                start_count_time_bowl=true;
                appear_bowl=true;
                start_count_time_transform=true;
                if (appear_bowl==true){
                    if (time3>4.98){
                        setup_bowl();
                        start_count_time_bowl=false;

                    }
                }
    }
}
//--------------------------------------------------------------------------------------
// draw health
uint8_t health_original[8] = {
    0b00000000,
    0b01101100,
    0b10010010,
    0b10000010,
    0b01000100,
    0b00101000,
    0b00010000,
    0b00000000,
};

uint8_t health_direct[8];
uint8_t hx, hy;
void setup_health(void) {
   
    for (int i = 0; i < 8; i++) {

       
        for (int j = 0; j < 8; j++) {
          
            uint8_t bit_val = BIT_VALUE(health_original[j], (7 - i));
            WRITE_BIT(health_direct[i], j, bit_val);
        }
    }
    hx = 13;
    hy = 0;
}
void draw_health() {
    LCD_CMD(lcd_set_function, lcd_instr_basic | lcd_addr_horizontal);
    LCD_CMD(lcd_set_x_addr, hx);
    LCD_CMD(lcd_set_y_addr, hy) ;

    for (int i = 0; i < 8; i++) {
        LCD_DATA(health_direct[i]);
    }
}
//--------------------------------------------------------------

 
//-------------------------------------------------------------
void draw_border(){
    draw_line(0,8,LCD_X-1,8,FG_COLOUR);
}
int time_clock;
int m;
void draw_time(){
    time_clock = elapsed_time();
   m=0;
      for (int i=0;i<61;i++){
         if (time_clock>=60) {   
            m++;
            time_clock= time_clock-60;
       
        }
        }
    draw_int_time(55,0,m,FG_COLOUR);
    draw_pixel(67,3,FG_COLOUR);
    draw_pixel(67,5,FG_COLOUR);
    draw_int_time(69,0,time_clock,FG_COLOUR);

    
}
void draw_status(){
    //Level
    draw_line(0,0,0,5,FG_COLOUR);
    draw_line(0,5,2,5,FG_COLOUR);
    draw_pixel(4,3,FG_COLOUR);
    draw_pixel(4,5,FG_COLOUR);
    draw_int(5,0,level,FG_COLOUR);
    //Lives
    draw_pixel(21,3,FG_COLOUR);
    draw_pixel(21,5,FG_COLOUR);
    draw_int(23,0,health,FG_COLOUR);
    //Score
    draw_line(32,3,36,3,FG_COLOUR);
    draw_line(34,1,34,5,FG_COLOUR);
    draw_pixel(38,3,FG_COLOUR);
    draw_pixel(38,5,FG_COLOUR);
    draw_int_time(40,0,score,FG_COLOUR);
        // Time
        draw_time();
}
//----------------------------------------------------------------------
void setup_usb_serial(void) {
	usb_init();

	while ( !usb_configured() ) {
		
	}
}


//-------------------------------------------------------------------------
void setup_game(){
  setup_inidoor();
    setup_inibowl();
    setup_timer0();
    setup_wall();
	setup_health();
    setup_tom();
    setup_initrap();
    setup_inicheese();
    setup_jerry();
    setup_tom_ini();
     setup_iniweapon();
   weapon=0;
    level_up=false;
    health=5;
    score=0;
    cheese=0;
    trap=0;
    cheese_appear=0;
    overflow_counter0=0;
    overflow_counter1=0;
    overflow_counter2=0;
    overflow_counter3=0;
    overflow_counter4=0;
    appear_door=true;
     super_power=false;
     increase_size=false;
      start_count_time_bowl=false;
                start_count_time_trap=false;
                 start_count_time_cheese=false;
    start_count_time_transform=false;
    game_over_activate=false;
 
}



void setup( void ) {
	set_clock_speed(CPU_8MHz);
     lcd_init(LCD_DEFAULT_CONTRAST);

     adc_init();
     
         CLEAR_BIT(DDRF,5);
        CLEAR_BIT(DDRF,6);
  
                CLEAR_BIT(DDRB,0);
                CLEAR_BIT(DDRB,1);
                CLEAR_BIT(DDRB,7);
   
             CLEAR_BIT(DDRD,0);
              CLEAR_BIT(DDRD,1);

              SET_BIT(DDRB, 2);
	            SET_BIT(DDRB, 3);

                draw_string(0,10,"n10275355",FG_COLOUR);
                draw_string(0,20,"Sang Nguyen Phuoc",FG_COLOUR);
                draw_string(0,30,"Tom vs Jerry",FG_COLOUR);
                 show_screen();
                 
                 return_ini=true;
                 level_up=false;
                  setup_usb_serial();
               pause=true;
             level=1;
}

void draw_all(){
    if (game_over_activate==false){
    clear_screen();
    draw_status();
    draw_border();
    draw_wall();
    draw_trap();
   draw_cheese();
   draw_weapon();
   draw_bowl();
   draw_door();
    draw_jerry();
    draw_tom();
    
    show_screen();
    }
    if (game_over_activate==true){
        screen_game_over();
    }
    
}

void process( void ) {

 
    if ((BIT_IS_SET(PINF,5)) && (return_ini==true))
    {   
        setup_game();
        start_move=true;// no need to press right button for movement
    }
    if (start_move==true){

        	//read both potentiometers
	int left_adc = adc_read(0);
	int right_adc = adc_read(1);
   
    adc_left=left_adc/500;   
    if (right_adc<512){
    adc_right=-(512-right_adc)/512;
    }
    if (right_adc>=512){
        adc_right=right_adc/512;
    }
  	int16_t char_code = usb_serial_getchar();
        
          
        if(	(switch_closed6!= 0)|| char_code=='l'){

            level++;
          if (level==2){
             level_up=true;
           setup_game();
           
       appear_bowl=true;
       }
       if (level==3){
            game_over_activate=true;
       }
                  snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
        }
        if(super_power==true){
        for (int i=0;i<4;i++){
        int  jx1=jx[0]+i;
        int jy1=jy[0]+i;
    if (collided(jx1,jy[0],tx[3]-1,ty[3],tx[3]+1,ty[3],10e-5)||collided(jx1,jy[5],tx[0],ty[0],tx[1],ty[1],10e-5)||collided(jx[0],jy1,tx[1],ty[1],tx[3],ty[3],10e-5)||collided(jx[1],jy1,tx[0],ty[0],tx[3],ty[3],10e-5)){
                setup_tom_ini();
               score++;
    }
        }
        }
            update_bowl();
     if ( collided_point(jx[0]+1,jy[0]+2,dox+2,doy+2,3)){ //collided door
        level++;
          if (level==2){
             level_up=true;
           setup_game();
       appear_bowl=true;
       }
       if (level==3){
            game_over_activate=true;
       }
       
   }   
            
         update_jerry(char_code);
          update_cheese();
          update_trap();
        update_door();
        if (game_over_activate==false){
         if ((switch_closed7 != 0)||(char_code=='p')){
            
             pause=!pause;
                snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
         }
        }
        if(game_over_activate==true){
            if (switch_closed7 !=0){
                level_up=true;
                setup_game();
                level=1;
            }
        }
         if(!pause){
            update_tom();
          update_wall();
           if ((switch_closed5 != 0) || char_code=='f'){
               if (cheese>2){
                weapon++;
                  for (int i=0;i<20;i++){	
                     
                if (order_weapon[i]==false){
                setup_weapon(i);
                return;
                }
                }
               }
                 
         snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
            }
                update_weapon();
           
         }
        if (char_code=='i'){

    snprintf( tx_buffer, sizeof(tx_buffer), "received '%c'\r\n", char_code );
	  usb_serial_send( tx_buffer );
      snprintf( tx_buffer10, sizeof(tx_buffer10), "Time:%d:%d \r\n",m,time_clock );
    usb_serial_send( tx_buffer10 );

       snprintf( tx_buffer1, sizeof(tx_buffer1), "level:%d\r\n", level );
    usb_serial_send( tx_buffer1 );
          snprintf( tx_buffer2, sizeof(tx_buffer2), "Jerry's lives:%d\r\n", health );
    usb_serial_send( tx_buffer2 );
snprintf( tx_buffer3, sizeof(tx_buffer3), "Score:%d\r\n", score );
    usb_serial_send( tx_buffer3 );
    snprintf( tx_buffer4, sizeof(tx_buffer4), "Consumed cheeses:%d\r\n", cheese );
    usb_serial_send( tx_buffer4 );
     snprintf( tx_buffer5, sizeof(tx_buffer5), "Cheeses on screen:%d\r\n", cheese_appear );
    usb_serial_send( tx_buffer5 );
     snprintf( tx_buffer6, sizeof(tx_buffer6), "Traps on screen:%d\r\n", trap);
    usb_serial_send( tx_buffer6 );
     snprintf( tx_buffer7, sizeof(tx_buffer7), "Fireworks on screen:%d\r\n", weapon);
    usb_serial_send( tx_buffer7 );
    if(pause==true){
        snprintf( tx_buffer8, sizeof(tx_buffer8), "Pause is true\r\n");
    usb_serial_send( tx_buffer8 );
        }
    if (pause==false){
         snprintf( tx_buffer8, sizeof(tx_buffer8), "Pause is false\r\n");
    usb_serial_send( tx_buffer8 );
    }
     if(super_power==true){
        snprintf( tx_buffer9, sizeof(tx_buffer9), "Super-mode is true\r\n");
    usb_serial_send( tx_buffer9 );
        }
    if (super_power==false){
         snprintf( tx_buffer9, sizeof(tx_buffer9), "Super-mode is false\r\n");
    usb_serial_send( tx_buffer9 );
    }
    }
            draw_all();
            if(game_over_activate==false){
            draw_health();
            }
           return_ini=false;// do not reset the initial characters
}
}
int main(void) {

    setup();

    while (!game_over && !level_up) {
       
        process();
     _delay_ms(100);
    }

    return 0;
}