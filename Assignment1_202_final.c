#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <cab202_graphics.h>
#include <cab202_timers.h>
// Insert other functions here, or include header files

bool game_over= false;
#define PI 3.14159265358979
bool pause=false;

//Jerry state
double jx,jy; // coordinate of Jerry
double a,b;  // read coordinate(x,y) from files for jerry
double jdx,jdy;
double t3,t4; // At move_jerry, stand for the difference each coordinate x,y between cheese and jerry
int score=0;// Score for manual jerry
int health=5;   // Lives for jerry
#define MAX_HEALTH 5
#define Jerry_IMG   'J'
bool activate_J;  // using to switch character

//Tom state
double tx,ty;  // coordinate of Tom
double tdx,tdy;// move_Tom
double c,d;     // read coordinate(x,y) from files for Tom
double t1,t2;//  stand for the difference each coordinate x,y between tom and jerry
double d2;   // the distance between tom and jerry
#define Tom_IMG 'T'
int thealth=5; // Lives for Tom
int tscore=0;       // Score for Tom

//Cheese state
double cx[5],cy[5],acx,acy; // Coordinate of cheese in manual Jerry and manual Tom, respectively. Store 5 cheese in cheese array 
int cheese;   // Count total cheese on screen
int cheese_piece; // Count cheese pieces for Jerry to set up door
bool order_cheese[5]; // Count the orther of each cheese
#define Cheese_IMG 'C'
#define MAX_CHEESE 5
bool spawn_cheese=false; //Create more cheese when player is Tom and Jerry ate Cheese

//Traps state
#define Trap_IMG 'O'
double trx[5],try[5]; // Store 5 traps in array
int tr;              //Count traps
bool order_trap[5]; // Count orther of each trap
bool spawn_trap=false;//Create more trap when player is Tom and Jerry touched on trap

//Weapon state
int wp;             //Count weapon
double wx,wy,wdx,wdy; // Coordinate of weapon
#define Weapon_IMG '='

//Level state
int l=1;
bool level_up=false;

//Door sate
double dx1,dy; // coordinate of door
#define Door_IMG 'X'

//timer
timer_id timer1; // create timer at 2s
timer_id timer3;  // create timer at 5s
timer_id timer2;   // create timer at 3s 

//Screen dimensions
int W,H;
int current_time;
int start_time;

//---------------------
double walls[50][4];
int num_walls=0;
bool firework_enable=false; // activate weapon
bool firework_back =false;  // Cant not press f before the firework touch on the walls or borders
FILE * stream_list[10]; 
//----------

// Check collision
bool collided(double x0, double y0, double x1 , double y1){
    if(round(x0)==round(x1)&& round(y0)== round(y1)){
        return true;
    }
    else{
        return false;
    }
}

void read_file( FILE * stream)
{
   
    while (!feof(stream))
    {
        char command;
        double arg1,arg2,arg3,arg4;
        int items_scanned= fscanf(stream,"%c %lf %lf %lf %lf ",&command,&arg1,&arg2,&arg3,&arg4);
        if (items_scanned==3)
        {  
             if (command=='J'){ 
            a=arg1* (screen_width()-1);  
            b=arg2 * (screen_height() -1 -4)+4;
             }
            else if (command=='T')
            { 
                c=arg1* (screen_width()-1); 
                d=arg2 * (screen_height() -1 -4)+4; 
            }
    
        }

        else if (items_scanned==5)
        { 
             
            if(command=='W'){
                walls[num_walls][0] = arg1 * (screen_width()-1);
                walls[num_walls][1] = arg2 * (screen_height() -1 -4)+4;
                walls[num_walls][2]= arg3 * (screen_width()-1);
                walls[num_walls][3] = arg4 * (screen_height() -1 -4)+4;
                    num_walls++;
            }
    }
    }
}
 void draw_wall(){
     for (int i=0;i<num_walls;i++){
         draw_line(walls[i][0], walls[i][1],walls[i][2],walls[i][3],'*');
     }
        

 }
// Game over screen
void screen_game_over() {
    clear_screen();

    const char *message[] = {
        "Press 'r' to Restart",
        "Press 'q' to Quit, please"
    };

    const int rows = 2;

    for (int i = 0; i < rows; i++) {
       
        int len = strlen(message[i]);
        int x = (W - len) / 2;
        int y = (H - rows) / 2 + i;
        draw_formatted(x, y, message[i]);
    }

    show_screen();

    while (get_char() > 0) {}
    wait_char();

    game_over = true;
    
}
//---------------------------------------------------
void setup_jerry(){
	jx=a;
    jy=b;
  
  
}
void draw_jerry(){
    draw_char(round(jx),round(jy),Jerry_IMG);
}



void update_jerry(int key_code) {
    // Check collision with walls
    if (scrape_char(jx+1,jy)=='*'){
        jx=jx-1;
        jy=jy;
    }
    if(scrape_char(jx,jy+1)=='*'){
        jx=jx;
        jy=jy-1;
    }
    if(scrape_char(jx-1,jy)=='*'){
        jx=jx+1;
        jy=jy;
    }
    if(scrape_char(jx,jy-1)=='*'){
        jx=jx;
        jy=jy+1;
    }

    //if player is jerry
     if(key_code=='a' && jx>0){ 
            jx--;
            }
        else if (key_code=='d' && jx<W-1){
            jx ++;
        }
        if (key_code=='w' && jy>4){  
            jy--;
        }
        else if (key_code=='s' && jy<H-1){
             jy++;
    }
}

//--------------------------------
void setup_tom(){
    tx=c;
    ty=d;

}
void draw_tom(){
    draw_char(round(tx),round(ty),Tom_IMG);
}
void move_tom(){ 
    double t1=jx - tx;
    double t2=jy - ty;
     d2= sqrt(t1*t1+t2*t2);

    double speed= 0.1; 
    tdx=t1/d2 *speed;
    tdy=t2/d2*speed;
    int new_x=round(tx+tdx),new_y=round(ty+tdy);

    if (new_x<1){
       
        tx++;
    }
    if (new_y<5){
        ty++;
    }
    if(new_x>W-1){
        tx--;
    }
    if(new_y>H-1){
            ty--;
        }
  
  if (scrape_char(tx+1,ty)=='*'){
        tx=tx-1;
        ty=ty-1;
    }
    if(scrape_char(tx,ty+1)=='*'){
        tx=tx-1;//
        ty=ty-1;
    }
    if(scrape_char(tx-1,ty)=='*'){
        tx=tx+1;
        ty=ty-1;//
    }
    if(scrape_char(tx,ty-1)=='*'){
        tx=tx-1;//
        ty=ty+1;
    }
    // Tom catch Jerry
    else{
    tx += tdx;
    ty += tdy;
    }
}
void update_tom(int key){
    if( key <0){
        move_tom(); 
    }
    if (collided(wx,wy,tx,ty)){
        wx=-1000;
        wy=-1000;
         setup_tom();
        score++;
        wp++;
        firework_enable=false;
         firework_back=true;
    
        }
     if (collided(jx,jy,tx,ty)){
        setup_tom();
        health --;
        if (health <= 0){
            game_over=true;
            screen_game_over();
        }
    }
}
// Manual Tom
void update_atom(int key_code) {
    if (scrape_char(tx+1,ty)=='*'){
        tx=tx-1;
        ty=ty;
    }
    if(scrape_char(tx,ty+1)=='*'){
        tx=tx;
        ty=ty-1;
    }
    if(scrape_char(tx-1,ty)=='*'){
        tx=tx+1;
        ty=ty;
    }
    if(scrape_char(tx,ty-1)=='*'){
        tx=tx;
        ty=ty+1;
    }
    if (collided(tx,ty,jx,jy)){
        tscore+=5;
        setup_jerry();
    }

    if (collided(tx,ty,dx1,dy)){
        dx1=-1;
        tscore=0;
        thealth=5;
        level_up=true;
           cheese_piece=0;
        score=0;
        tr=0;
        health=5;
        cheese=0;
        tscore=0;
    }
    if(key_code=='a' && tx>1){
            tx--;
            }
    else if (key_code=='d' && tx<W-2){
            tx ++;
        }
    if (key_code=='w' && ty>4){
            ty--;
        }
    else if (key_code=='s' && ty<H-2){
             ty++;
    }


}
   


//-----------------------------------------------
// Set up inital place for Cheese
void setup_inicheese(){
    for (int i=0;i<5;i++){
    cx[i]=-1;
    cy[i]=-1;
    order_cheese[i]=false;
}
}

void setup_cheese(int i){
    cx[i]= rand() % (W-1);
    cy[i]=5+ rand() % (H-5);
    order_cheese[i]= true;
    cheese++;

}
void draw_cheese(int i){
    draw_char(round(cx[i]),round(cy[i]),Cheese_IMG);
}
void draw_cheeses(){
    for (int i=0;i<5;i++){
        draw_cheese(i);
    }
}


void update_cheese(){  
   if(pause==true){
      for (int i=0;i<5;i++){
                if (collided(jx,jy,cx[i],cy[i])){
                score++;
                cheese--;
                health--;
                cx[i]=-1;
                cheese_piece++;
                }
            }
        } 
    if (pause==false){
      for (int i=0;i<5;i++){
            if (collided(jx,jy,cx[i],cy[i])){
                score++;
                cheese--;
                cx[i]=-1;
                 cheese_piece++;
                 order_cheese[i]=false;      // After Jerry ate Cheese, another Cheese will appear after 2s
                 if(timer_expired(timer1)) {
                    if(order_cheese[i]==false){
                             setup_cheese(i);
                            return;
                    }
                }
             }
            // Check collision cheese with wall 
             if (scrape_char(cx[i]+1,cy[i])=='*'){
                    cx[i]=cx[i]-1;
                    cy[i]=cy[i];
                }
            if(scrape_char(cx[i],cy[i]+1)=='*'){
                    cx[i]=cx[i];
                    cy[i]=cy[i]-1;
                }
           if(scrape_char(cx[i],cy[i])=='*'){
                    cx[i]=cx[i]+1;
                    cy[i]=cy[i];
                }
            if(scrape_char(cx[i],cy[i]-1)=='*'){
                    cx[i]=cx[i];
                    cy[i]=cy[i]+1;
                }
                
                }
                
            if(timer_expired(timer1)) {
                for (int i =0;i<5;i++){
                    if(order_cheese[i]==false){
                    setup_cheese(i);
                    return;
                     }
                    
                }
            }
    }
}
//-------------------------------------------------------------------------
//Set up cheese for switch character to Tom
void setup_acheese(int i)
{
    cx[i]=tx+1;
    cy[i]=ty;
    order_cheese[i]= true;
    cheese++;
}

void update_acheese(){
    for (int i =0;i<5;i++){   
        if(order_cheese[i]==false){
                setup_acheese(i);
                return;
        }
    }
}
//-------------------------------------------------------
// Auto Jerry
void move_jerry(){ 
    // Set up in order Tom find Cheese
    for(int i=0;i<5;i++){
        if (order_cheese[i]==true){
        acx=cx[i];
        acy=cy[i];
        }
    }
    double t3=acx - jx;
    double t4=acy - jy;
    double d1= sqrt(t3*t3+t4*t4);
    double speed= 0.12;
    jdx=t3/d1 *speed;
    jdy=t4/d1*speed;
   
 
   // Jeerry escapes Tom 
    double t1=jx - tx;
    double t2=jy - ty;
     d2= sqrt(t1*t1+t2*t2);
    tdx=t1/d2 *0.14;
    tdy=t2/d2*0.14;

    if (d2<28){ //In Cicle has radius 28, Jerry will escape Tom

         int new_x=round(jx+tdx),new_y=round(jy+tdy);

    if (new_x<1){
       
        jx++;
    }
    if (new_y<5){
        jy++;
    }
    if(new_x>W-1){
        jx--;
    }
    if(new_y>H-1){
            jy--;
        }
     jx+=tdx;
     jy+=tdy;
    }
    

    //.................
    //Collision with border
    int new_x=round(jx),new_y=round(jy);

    if (new_x==2){
       
        jx++;
    }
    if (new_y==4){
        jy++;
    }
    if(new_x==W-1){
        jx--;
    }
    if(new_y==H-1){
            jy--;
        }
    // Collision with wall
  if (scrape_char(jx+1,jy)=='*'){
        jx=jx-1;
        jy=jy-1;
    }
    if(scrape_char(jx,jy+1)=='*'){
        jx=jx-1;//
        jy=jy-1;
    }
    if(scrape_char(jx-1,jy)=='*'){
        jx=jx+1;
        jy=jy-1;//
    }
    if(scrape_char(jx,jy-1)=='*'){
        jx=jx-1;//
        jy=jy+1;
    }
    // Jerry find Cheese
    else{
    jx += jdx;
    jy += jdy;
    }

}
// Auto Jerry
void update_ajerry(int key){
    if( key <0){
        move_jerry();
        for (int i=0;i<5;i++){
            if (collided(jx,jy,cx[i],cy[i])){
                cheese--;
                cx[i]=-1;               // Check collision Jerry with cheese
                cheese_piece++;
                spawn_cheese=true;
                order_cheese[i]=false;
                }
            
            if (collided(jx,jy,trx[i],try[i])){
                tscore++;               // Check collision Jerry with trap
                tr--;
                trx[i]=-1;
                 setup_jerry();
                spawn_trap=true;
                order_trap[i]=false;
                if(thealth<=0){
                    game_over=true;
                    screen_game_over();
                }
                }
        }
    }
}
    

//--------------------------------------------
void setup_level(){
    
    level_up = false;
}
//---------------------------------------------------
// Set up initial trap
void setup_initrap(){
    for (int i=0;i<5;i++){
    trx[i]=-1;
    try[i]=-1;
    order_trap[i]=false;
    }
}

void setup_trap(int i){
    trx[i]= rand() % (W-1);
    try[i]=5+ rand() % (H-5);
    order_trap[i]= true;
    tr++;

}
void draw_trap(int i){
    draw_char(round(trx[i]),round(try[i]),Trap_IMG);
}
void draw_traps(){
    for (int i=0;i<5;i++){
        draw_trap(i);
    }
}


void update_trap(){  
   if(pause==true){
      for (int i=0;i<5;i++){
                if (collided(jx,jy,trx[i],try[i])){
                     tr--;
                     health--;
                     trx[i]=-1;
                     if(health<0){
                         game_over=true;
                        screen_game_over();
                    }
                }
        }
     } 
    if (pause==false){
      for (int i=0;i<5;i++){
        if (collided(jx,jy,trx[i],try[i])){
               tr--;
                trx[i]=-1;
               order_trap[i]=false;           // after jerry touch on a trap, spawn 1 trap interval 3s
                 if(timer_expired(timer2)) {
                   if(order_trap[i]==false){
                      setup_trap(i);
                      return;
                    }
                }
                if(health<0){
                    game_over=true;
                    screen_game_over();
                }
        }                   
        if (scrape_char(trx[i],try[i])=='*'){
                    trx[i]=trx[i]+1;
                    try[i]=try[i]+1;
        } 
    }
    // Create each trap for 2 seconds  
     if(timer_expired(timer2)) {
            for (int i =0;i<5;i++){
                if(order_trap[i]==false){
                  setup_trap(i);
                    return;
                } 
            }
        }
    }
}
//Set up trap when player is Tom
void setup_atrap(int i)
{
    trx[i]=tx+1;
    try[i]=ty;
    order_trap[i]= true;
    tr++;
}

void update_atrap(){
        for (int i =0;i<5;i++){    
            if(order_trap[i]==false){
                setup_atrap(i);
                return;
                tscore++;
            }
        }
}
//---------------------------------------------------
//Set up initial weapon
void setup_initialweapon(){
    wx=-1000;
    wy=-1000;
    wp=1;
}
void setup_weapon(){   
wx=jx+1;
wy=jy;
double w1=tx - wx;
double w2=ty - wy;
double d= sqrt(w1*w1+w2*w2);
    wdx=w1/d*0.25;
    wdy=w2/d*0.25;
firework_enable=true;
    }
void draw_weapon(){
        draw_char(round(wx),round(wy),Weapon_IMG);
}

void update_weapon(){

    if (scrape_char(wx,wy)=='*'){
       wx=-1000;
       wy=-1000;
       firework_enable=false;
       firework_back=true;
       wp++;
    }    
    if (round(wx)==0||round(wx)==W-1){
         wx=-1;
      firework_back=true;
      firework_enable=false;
      wp++;
    }
    if (round(wy)==4||round(wy)==H-4){
        wx=-1;
       firework_back=true;
       firework_enable=false;
       wp++;
    }
     wx +=wdx;
     wy +=wdy;
}   
// Player is Tom
void update_aweapon(){
       if ((scrape_char(wx,wy)=='*')||(scrape_char(tx,ty)=='*')||(scrape_char(tx,ty)=='*')||(scrape_char(tx,ty)=='*')){
       wx=-1000;
       wy=-1000;
       firework_enable=false;
       wp++;
    }    
     if (round(wx)==0||round(wx)==W-1){
         wx=-1;
      firework_enable=false;
      wp++;
    }
    if (round(wy)==4||round(wy)==H-4){
        wx=-1;
       firework_enable=false;
       wp++;
    }
    if (collided(wx,wy,tx,ty)){
        wx=-1;
        wy=-1;
        thealth--;
        firework_enable=false;
        wp++;
        if (thealth<=0){
            game_over=true;
            screen_game_over();
        }
    }
        wx +=wdx;
        wy +=wdy;
}

//----------------------------------------------
void setup_door(){
   
     dx1= rand() % (W-1);
    dy=5+ rand() % (H-5);
    
}
void draw_door(){
    if( cheese_piece>4){
    draw_char(round(dx1),round(dy),Door_IMG);
}
}
void update_door(){
    if (collided(jx,jy,dx1,dy)&& (cheese_piece>=5)){
        l++;
        dx1=-1;
        dy=-1000;
        level_up=true;
        cheese_piece=0;
        score=0;
        tr=0;
        health=5;
        cheese=0;
        tscore=0;
        thealth=5;

        
    }
    if (scrape_char(dx1,dy)=='*'){
        dx1=dx1-1;
        dy=dy+1;//
    }
    if(scrape_char(dx1,dy)=='*'){
        dx1=dx1+1;//
        dy=dy-1;
    }
    if(scrape_char(dx1,dy)=='*'){
        dx1=dx1+1;
        dy=dy+1;//
    }
    if(scrape_char(dx1,dy)=='*'){
        dx1=dx1+1;//
        dy=dy+1;
    }

}
void draw_adoor(){
       if( tscore>4){
    draw_char(round(dx1),round(dy),Door_IMG);
}
}
//----------------------------
// Player is Jerry
void draw_border(){
    draw_line(0,3,W-1,3,'-');
}
void draw_status(int start_time){
   { int current_time= round(get_current_time())-start_time;
    int m=0;
    for (int i=0;i<61;i++){
         if (current_time>=60) {   
            m++;
            current_time= current_time-60;
        }
        draw_formatted(round(4*(W-1)/5)+5,0,"Time: %02d:%02d", m, current_time);
    }
   }
    draw_formatted(0,0,"Student Number: n10275355");
    draw_formatted(round((W-1)/5)+15,0,"Score: %d",score);
    draw_formatted(round(2*(W-1)/5)+5,0,"Lives: %d",health);
    draw_formatted(round(3*(W-1)/5)+5,0,"Player: J");
    draw_formatted(0,2,"Cheese: %d",cheese_piece);
    draw_formatted(round((W-1)/5)+15,2,"Traps: %d",tr);
    draw_formatted(round(2*(W-1)/5)+5,2,"Fireworks: %d",wp);
    draw_formatted(round(3*(W-1)/5)+5,2,"Level:%d",l);

}
// Player is Tom
void draw_astatus(int start_time){
      int m=0;
      int current_time= round(get_current_time())-start_time;
    for (int i=0;i<61;i++){
         if (current_time>=60) {   
            m++;
            current_time= current_time-60;
        }
        draw_formatted(round(4*(W-1)/5)+5,0,"Time: %02d:%02d", m, current_time);
    }
   
    draw_formatted(0,0,"Student Number: n10275355");
    draw_formatted(round((W-1)/5)+15,0,"Score: %d",tscore);
    draw_formatted(round(2*(W-1)/5)+5,0,"Lives: %d",thealth);
    draw_formatted(round(3*(W-1)/5)+5,0,"Player: T");
    draw_formatted(0,2,"Cheese: %d",cheese);
    draw_formatted(round((W-1)/5)+15,2,"Traps: %d",tr);
    draw_formatted(round(2*(W-1)/5)+5,2,"Fireworks: %d",wp);
    draw_formatted(round(3*(W-1)/5)+5,2,"Level:%d",l);

}
//------------------------------------------------------

void setup(){
    start_time=round(get_current_time());
	W = screen_width();
	H = screen_height();
    srand(get_current_time());
	setup_jerry();
    setup_initrap();
    setup_level();
    setup_tom();
    setup_door();
    setup_inicheese();
    setup_initialweapon();
     firework_enable=false;
    firework_back=true;
    activate_J = true;
    spawn_cheese=false;
    cheese=0;
    cheese_piece=0;
    tr=0;
    tscore=0;
    health=5;
    thealth=5;
    score=0;
}


void loop(){
    int key= get_char();
    if (key == 'q') {
        game_over = true;
        return;
    }
    if (key=='r'){
        game_over=false;
        setup();
    }
   if (key=='p'){
        pause=!pause;
       
   }
    if (key=='l'){
       level_up=true;
       l++;
       health=5;
       thealth=5;
       score=0;          // Move to next level
       tscore=0;
       cheese=0;
       cheese_piece=0;
       tr=0;

   }
   if (key == 'z') {
       activate_J = !activate_J; // Switch character
   }
    if (activate_J){ //When player is Jerry
            if(firework_back==true){  // After touch on the wall can press f to shoot again
                if (key=='f' && (l>=2)){
                firework_back=false;
                wp--;
                setup_weapon();
                }
            }
            
            if(firework_enable==true && (l>=2)){
                update_weapon();
            }

            if(!pause){
            update_tom(key);
            update_door();
            
            } 
           
            update_trap();
            update_cheese();
             update_jerry(key);
 }
  else { //When player is Tom
        if (!pause) {
            update_atom(key);
            update_ajerry(key);
            if (key=='c'){
                update_acheese(); // Generate more cheese when number of cheese less than 5             
                if(spawn_cheese==true){    // Press c to create more cheese after Jerry ate Cheese
                    for (int i=0;i<5;i++){
                        if(order_cheese[i]==false){
                             setup_acheese(i);
                            return;
                            cheese++;
                        }
                    }  
                }
            }

            if (key=='m'){
                update_atrap();// Create trap when trap less than 5
                  if(spawn_trap==true){  // Press m to create more trap after Jerry touch on
                    for (int i=0;i<5;i++){
                        if(order_trap[i]==false){ 
                             setup_atrap(i);
                            return;
                            tr++;
                        }
                    }  
                }
             }     
             if (timer_expired(timer3)&& l>1){
                    if (firework_enable==false){ 
                        setup_weapon(); // Set up weapon to shoot current location of Tom
                        wp--;
                    }
            }
            if (firework_enable==true){
                 update_aweapon(); // Activate and shoot immediately
            
            }
        }
    }
 }


void draw_all(int start_time){
    clear_screen(); 
    if(activate_J){ // Player is Jerry
        draw_door();
        draw_border();
        draw_cheeses();
      draw_weapon();
        draw_traps();
          draw_wall();
        draw_jerry();
        draw_tom();
            draw_status(start_time); 
    }
    else{ //Player is Tom
        draw_astatus(start_time);
        draw_adoor();
        draw_border();
        draw_jerry();
        draw_cheeses();
        draw_tom();
        draw_traps();
        draw_weapon();
        draw_wall();
    
    }
    show_screen();

}

int main(int argc, char * argv[]) {
    start_time= round(get_current_time());

    const int DELAY = 10;
    setup_screen();
    int i=1;
while(i < argc) 
    {
        stream_list[i-1]  = fopen(argv[i], "r");
    if (stream_list[i-1]  != NULL)
    {
        read_file(stream_list[i-1] );
    }else{
        return 0;
    }
    timer3=create_timer(5000);
    timer1=create_timer(2000);
    timer2=create_timer(3000);
    setup();

   
    while (!game_over && !level_up) {
        loop();
        draw_all(start_time);
    
        //wait_char();
        timer_pause(DELAY);
    
    }
        
    i++ ;
    
   num_walls=0;
    }
  
    return 0;
}


/*
main
    while(game_not_exit){
        setup everything
        while(!game over){
            do game
        }
        show game over options
            if 'q' game not exit = false
            if 'r' game over = true
    }

*/