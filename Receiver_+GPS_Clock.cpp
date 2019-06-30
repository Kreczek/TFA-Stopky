// TFA STOPKY  displej
// verze 1.0 30.6.2019
//RF24 prijimac, MAX7219displej a GPS  NEO 6M hodiny


#include <AltSoftSerial.h>
#include <Arduino.h>
#include <RF24.h>
#include "LedControl.h"
#include <TinyGPS++.h>
#include "elapsedMillis.h"

RF24 radio(6, 7); // CE, CSN
const byte address[8] = "00001";

//
/***** These pin numbers will probably not work with your hardware *****
 pin 5 is connected to the DataIn 
 pin A5 is connected to the CLK 
 pin A4 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(5,A5,A4,1);


elapsedMillis lastRadio;
char sSegment[8];
byte utc1[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,0};
byte utc2[]={2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,0,1};
byte Second, Minute, Hour;
bool dot;
AltSoftSerial altSerial;
TinyGPSPlus gps;
float delaytime = (200);

void setup() {
  Serial.begin(9600);
  altSerial.begin(9600);
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,2  );
  /* and clear the display */
  lc.clearDisplay(0);
  //lc.setScanLimit(0,6);

   lc.setChar(0,5,'-',false);
  delay(delaytime);
  lc.setRow(0,4,B01110111);
  delay(delaytime);
  lc.setRow(0,3,B00110111);
  delay(delaytime);
  lc.setRow(0,2,B01111110);
  delay(delaytime);
  lc.setRow(0,1,B00111000);
  delay(delaytime);
  //lc.setRow(0,1,0x05);
  //delay(delaytime);
  lc.setChar(0,0,'-',false);
  delay(2500);
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
    Serial.print(text);
   if (text[6]==58) {
    lastRadio=0;
    Serial.print(text);
    for (int i=0, j = 5; i<6; i++, j--)
    {
      if(text[i]!=sSegment[i]){
        sSegment[i]=text[i];
        lc.setChar(0, j, sSegment[i], true);
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

        if (lastRadio>10000 && memcmp(sSegment, text, 7)){
           
          for (int i=0, j = 5; i<6; i++, j--)
          {
              sSegment[i]=text[i];
           dot= Second % 2 ;
         
              lc.setChar(0, j, sSegment[i], dot);
          }
        }
      }
    }
  }
}
