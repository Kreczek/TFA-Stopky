// TFA STOPKY  vysilac
// verze 0.8 30.6.2019
//RF24 vysilac, puvodni pajsl

#include <Arduino.h>
#include <RF24.h>
#include "elapsedMillis.h"
#include <LiquidCrystal_PCF8574.h>

RF24 radio(8, 9); // CE, CSN
LiquidCrystal_PCF8574 lcd(0x27);
elapsedMillis casomira;
elapsedMillis displayRefreshTime;
elapsedMillis stopRefresh;
elapsedMillis deBounce;

const byte address[6] = "00001";

//LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);
volatile unsigned long buffer;

volatile boolean zastaveno;
volatile boolean resetFlag;
volatile boolean zobrazStop;
volatile uint8_t repeat = 0;

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
  stopRefresh = 0;
  repeat = 0;
  
}

void vynulovat () {
  lcd.setCursor(4, 1);
  lcd.print("00:00:00");
  radio.write("000000:",7);
  zastaveno = true;
  resetFlag = true;
}

void setup() {
  radio.begin();
 // Serial.begin(9600);
  //radio.setChannel(0);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), start, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), stop, FALLING);
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("TFA Stopky- r0.8");
  delay(1000);
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

    if (!zastaveno && displayRefreshTime) {
      char vystup[8];
      char vystupRF[7];
      buffer = casomira;
      uint8_t minuty = buffer / 60000;
      uint8_t sekundy = (buffer % 60000) / 1000;
      uint8_t milisekundy = buffer % 1000 / 10;
      //sprintf(vystupRF, "%02u%02u%02u:", minuty, sekundy, milisekundy);
    sprintf(vystup, "%02u:%02u:%02u", minuty, sekundy, milisekundy);
       sprintf(vystupRF, "%02u%02u%02u:", minuty, sekundy, milisekundy);
    // Serial.println(vystupRF);
      radio.write(vystupRF,7);
      lcd.setCursor(4, 1);
      lcd.print(vystup);
      displayRefreshTime = 0;
    }
    if (!digitalRead(5) && deBounce > 500) {
      deBounce = 0;
      vynulovat();
    }
    if (zobrazStop && repeat < 3) {
      char vystup[8];
      char vystupRF[7];
      uint8_t minuty = buffer / 60000;
      uint8_t sekundy = (buffer % 60000) / 1000;
      uint8_t milisekundy = buffer % 1000 / 10;
       //sprintf(vystupRF, "%02u%02u%02u:", minuty, sekundy, milisekundy);
      sprintf(vystup, "%02u:%02u%:02u", minuty, sekundy, milisekundy);
      sprintf(vystupRF, "%02u%02u%02u:", minuty, sekundy, milisekundy);
      radio.write(vystupRF, 7);
      lcd.setCursor(4, 1);
      lcd.print(vystup);
      repeat++;
      stopRefresh = 0;
    }
    if(zastaveno && stopRefresh > 1000){
      char vystup[8];
       char vystupRF[7];
      uint8_t minuty = buffer / 60000;
      uint8_t sekundy = (buffer % 60000) / 1000;
      uint8_t milisekundy = buffer % 1000 / 10;
      //sprintf(vystupRF, "%02u%02u%02u:", minuty, sekundy, milisekundy);
      sprintf(vystup, "%02u:%02u:%02u:", minuty, sekundy, milisekundy);
     sprintf(vystupRF, "%02u%02u%02u:", minuty, sekundy, milisekundy);
      radio.write(vystupRF, 7);
      stopRefresh = 0;
    }
}
