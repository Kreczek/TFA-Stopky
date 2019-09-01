//  TFA Stopky s nRF24L01
//  verze 1.03
//  2019.09.01

//opraveno pocitani po zapnuti a zobrazovani rozdilnych casu displej vs casomira




#include <LiquidCrystal.h>
#include <Arduino.h>
#include <RF24.h>
#include "elapsedMillis.h"


RF24 radio(8, 9); // CE, CSN

elapsedMillis casomira;
elapsedMillis displayRefreshTime;
elapsedMillis stopRefresh;
elapsedMillis deBounce;

const byte address[6] = "00001";

LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);
volatile unsigned long buffer;

boolean zastaveno;
boolean resetFlag;
boolean zobrazStop;

char vystup[8];
char vystupRF[7];

void start () {
  if (resetFlag) {
    casomira = 0;
    zastaveno = false;
  }
  else {
    casomira = buffer;
    zastaveno = false;
  }
  resetFlag = false;
}

void stop () {
  buffer = casomira;
  zastaveno = true;
  zobrazStop = true;
}

void vynulovat () {
  lcd.clear();
  if ((digitalRead(3)) == LOW) {
      // lcd.clear();
lcd.setCursor(0, 0);
 lcd.print("stisknuto STOP!!");
 lcd.setCursor(0, 1);
 lcd.print("!!! odblokuj !!!");
 delay (100);
 vynulovat();}
 else 
  
  {
  // lcd.clear();
   lcd.setCursor(0, 0);
  lcd.print("   TFA Stopky   ");
   

  buffer = 0;
  zastaveno = true;
  resetFlag = true;
  
}
}
void setup() {
  radio.begin();
 // Serial.begin(9600);
 if (digitalRead(7) == LOW){
  radio.setChannel (125) ;}
 else
 {radio.setChannel (0);}

  radio.setPALevel (RF24_PA_MAX);
 radio.setDataRate( RF24_250KBPS );
   
    
  radio.stopListening();
  radio.openWritingPipe(address);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
   pinMode(7, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), start, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), stop, FALLING);
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("TFA Stopky - RF");
  lcd.setCursor(0, 1);
  lcd.print("verze 2019.09.01");
  delay(2500);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("TFA Stopky");
  vynulovat ();
}

void loop() {
  if (!digitalRead(4) && deBounce > 500) {
      deBounce = 0;
      start();
    }
    if (!digitalRead(6) && deBounce > 500) {
      deBounce = 0;
      stop();
    }

    
    if (!digitalRead(5) && deBounce > 500) {
      deBounce = 0;
      vynulovat();
    }
    uint8_t minuty = buffer / 60000;
      uint8_t sekundy = (buffer % 60000) / 1000;
      uint8_t milisekundy = buffer % 1000 / 10;
    if (zobrazStop) {
     
      zobrazStop = false;
         
    }
    if(zastaveno && stopRefresh > 1000){
     
    
      stopRefresh = 0;
    }
     if (!zastaveno && displayRefreshTime) {
     
      buffer = casomira;
    
    }
    sprintf(vystupRF, "%02u%02u%02u:", minuty, sekundy, milisekundy);
    sprintf(vystup, "%02u:%02u:%02u", minuty, sekundy, milisekundy);
    lcd.setCursor(0, 1);
  lcd.print("CAS:");
      lcd.setCursor(8, 1);
      lcd.print(vystup);
    radio.write(vystupRF, 7);
}
