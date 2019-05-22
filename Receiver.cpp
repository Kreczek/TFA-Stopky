#include <Arduino.h>
#include <RF24.h>
#include "LedControl.h"
RF24 radio(8, 9); // CE, CSN
const byte address[8] = "00001";
LedControl lc=LedControl(5,19,18,1);;
char sSegment[8];


void setup() {
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  while (radio.available()) {
    char text[8];
    radio.read(text, 8);

    for (int i=0, j = 8-1; i<8; i++, j--)
    {
      if(text[i]!=sSegment[i]){
        sSegment[i]=text[i];
        lc.setChar(0, j, sSegment[i], false);
      }
    }
  }
}
