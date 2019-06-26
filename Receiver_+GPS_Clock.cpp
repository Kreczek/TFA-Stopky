#include <Arduino.h>
#include <RF24.h>
#include "LedControl.h"
#include "altSoftSerial.h"
#include <TinyGPS++.h>
#include "elapsedMillis.h"
RF24 radio(6, 7); // CE, CSN
const byte address[8] = "00001";
LedControl lc=LedControl(5,19,18,1);;
elapsedMillis lastRadio;
char sSegment[8];
byte Second, Minute, Hour;
AltSoftSerial altSerial;
TinyGPSPlus gps;
//8 RX,9 TX GPS


void setup() {
  //Serial.begin(9600);
  altSerial.begin(9600);
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

  radio.begin();
  //radio.setChannel (0);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    char text[8];
    radio.read(text, 8);
    if (text[5]==58) {
    lastRadio=0;
    //Serial.println(text);
    for (int i=0, j = 8-1; i<8; i++, j--)
    {
      if(text[i]!=sSegment[i]){
        sSegment[i]=text[i];
        lc.setChar(0, j, sSegment[i], false);
            }
          }
        }
      }

  while (altSerial.available()){
    if (gps.encode(altSerial.read()))
    {
      if (gps.time.isValid())
      {
        char text[8];
        Minute = gps.time.minute();
        Second = gps.time.second();
        Hour   = gps.time.hour();
        sprintf(text, "%02u:%02u:%02u", Hour, Minute, Second);

        if (lastRadio>10000 && memcmp(sSegment, text, 8)){
          Serial.println(text);
          for (int i=0, j = 8-1; i<8; i++, j--)
          {
              sSegment[i]=text[i];
              lc.setChar(0, j, sSegment[i], false);
          }
        }
      }
    }
  }
}
