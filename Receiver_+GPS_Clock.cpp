#include <Arduino.h>
#include <RF24.h>
#include "LedControl.h"
#include "altSoftSerial.h"
#include <TinyGPS++.h>
#include "elapsedMillis.h"
RF24 radio(6, 7); // CE, CSN
const byte address[8] = "00001";
LedControl lc=LedControl(5,19,18,1);
elapsedMillis lastRadio;
char sSegment[8];
byte utc1[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,0};
byte utc2[]={2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,0,1};
byte Second, Minute, Hour;
bool dot;
AltSoftSerial altSerial;
TinyGPSPlus gps;


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
    char text[7];
    radio.read(text, 7);
    if (text[6]==58) {
    lastRadio=0;
    //Serial.println(text);
    for (int i=0, j = 6; i<7; i++, j--)
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
        char text[6];
        Minute = gps.time.minute();
        Second = gps.time.second();
        Hour   = gps.time.hour();
        sprintf(text, "%02u%02u%02u", utc2[Hour], Minute, Second);

        if (lastRadio>10000 && memcmp(sSegment, text, 8)){
          dot =! dot;
          for (int i=0, j = 6; i<7; i++, j--)
          {
              sSegment[i]=text[i];
              lc.setChar(0, j, sSegment[i], false);
          }
        }
      }
    }
  }
}
