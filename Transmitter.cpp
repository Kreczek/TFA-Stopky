#include <Arduino.h>
#include <RF24.h>
#include "elapsedMillis.h"
#include <LiquidCrystal_PCF8574.h>

RF24 radio(8, 9); // CE, CSN
LiquidCrystal_PCF8574 lcd(0x27);
elapsedMillis casomira;
elapsedMillis displayRefreshTime;
elapsedMillis deBounce;

const byte address[6] = "00001";

//LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);
volatile unsigned long buffer;

boolean zastaveno;
boolean resetFlag = false;
boolean zobrazStop = false;

void start () {
  if (resetFlag) {
    casomira = 0;
    zastaveno = false;
  }
  else {
    casomira = buffer;
    zastaveno = false;
  }
}

void stop () {
  buffer = casomira;
  zastaveno = true;
  zobrazStop = true;
}

void vynulovat () {
  //Serial.println("00:00:00");
  lcd.setCursor(4, 1);
  lcd.print("00:00:00");
  radio.write("00:00:00",8);
  zastaveno = true;
  resetFlag = true;
}


void setup() {
  radio.begin();
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
  lcd.print("TFA Stopky- r0.3");
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
      buffer = casomira;
      uint8_t minuty = buffer / 60000;
      uint8_t sekundy = (buffer % 60000) / 1000;
      uint8_t milisekundy = buffer % 1000 / 10;
      sprintf(vystup, "%02u:%02u:%02u", minuty, sekundy, milisekundy);
      radio.write(vystup, 8);
      lcd.setCursor(4, 1);
      lcd.print(vystup);
      displayRefreshTime = 0;
    }
    if (!digitalRead(5) && deBounce > 500) {
      deBounce = 0;
      vynulovat();
    }
    if (zobrazStop) {
      char vystup[8];
      zobrazStop = false;
      uint8_t minuty = buffer / 60000;
      uint8_t sekundy = (buffer % 60000) / 1000;
      uint8_t milisekundy = buffer % 1000 / 10;
      sprintf(vystup, "%02u:%02u:%02u", minuty, sekundy, milisekundy);
      radio.write(vystup, 8);
      lcd.setCursor(4, 1);
      lcd.print(vystup);
    }
}
