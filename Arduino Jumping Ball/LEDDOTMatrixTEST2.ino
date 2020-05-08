#include <LedControl.h>
#include <time.h>
int DIN = D7;   // DIN pin of MAX7219 module
int CLK = D5;   // CLK pin of MAX7219 module
int CS = D4;    // CS pin of MAX7219 module
LedControl lc=LedControl(DIN,CLK,CS,0);

uint8_t s = 1;
long s_l = 1;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
int ball = 32;
int ball2 = 0;

int jumping = 0;
int height = 6;
int goup = 1;
int over = 0;

byte a[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x20,0xFF}; //First Step
byte ab[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x20,0xFF}; //First Step
byte t[8] = {0x38,0x04,0x04,0x38,0x04,0x04,0x38,0x00}; //3
byte w[8] = {0x38,0x04,0x04,0x38,0x20,0x20,0x1C,0x00}; //2
byte o[8] = {0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00}; //1
byte f[8] = {0x78,0x40,0x40,0x70,0x40,0x40,0x40,0x00}; //LOSE

void setup(){

  lc.shutdown(0,false);     //The MAX72XX is in power-saving mode on startup

  lc.setIntensity(0,1);     // Set the brightness to maximum value

  lc.clearDisplay(0);        // and clear the display

  pinMode(D0,INPUT);
  Serial.begin(9600);

  startup();
  printByte(a);
}
void loop() {
    if(digitalRead(D0) == LOW && jumping == 0){
    jumping = 1;
    }

    if(jumping == 1)
    {runJump();}
      
    runScene();
    if(over == 1) {printByte(f); if(digitalRead(D0) == LOW){over = 0; startup(); printByte(a); s = 1; s_l = 1;}}else
    {printByte(a);}
}
void printByte(byte character []){
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
}
void runScene(){
  unsigned long currentMillis = millis();
  if(currentMillis-previousMillis >= 200){
    previousMillis = currentMillis;

    if(ball+ball2 == 32 && s == 32){over = 1;}
    a[5] = s + ball2;
    a[6] = s + ball;
    if(s == 128 || s == 160){s_l = 1; s = 1;} else{
    s_l = s_l+s_l;
    s = s_l;}
    }
  }
void runJump(){
  unsigned long currentMillis = millis();
  if(currentMillis-previousMillis2 >= 200){
    previousMillis2 = currentMillis;
    if(height == 6 && goup == 1){ball = 0;  ball2 = 32; height = 5;}
    else if(height == 5 && goup == 1){a[4] = 32; ball2 = 0; height = 4;}
    else if(height == 4 && goup == 1){a[3] = 32; a[4] = 0; height = 3;}
    else if(height == 3){a[4] = 32; a[3] = 0; height = 4; goup = 0; }
    else if(height == 4 && goup == 0){ball2 = 32; a[4] = 0; height = 5; }
    else if(height == 5 && goup == 0){ball2 = 0; ball = 32; height = 6; jumping = 0; goup=1;}
    }
  }
 void startup(){
  printByte(t);
  delay(1000);
  printByte(w);
  delay(1000);
  printByte(o);
  delay(1000);
  }
