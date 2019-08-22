//#include <nRF24L01.h>
////#include <printf.h>
//#include <RF24.h>
#include <RF24_config.h>



// TFA STOPKY  displej
// verze 1.0  22.8.2019 Beta
//RF24 prijimac, MAX7219displej a GPS  NEO 6M hodiny
//adaptivni riyeni jasu


#include <AltSoftSerial.h>
#include <Arduino.h>
#include <RF24.h>
#include <nRF24L01.h>
#include "LedControl.h"
#include <TinyGPS++.h>
#include "elapsedMillis.h"
#include <BH1750.h>

BH1750 lightMeter;



RF24 radio(2, 3); // CE, CSN
const byte address[8] = "00001";

//
/***** These pin numbers will probably not work with your hardware *****
 pin 4 is connected to the DataIn 
 pin 6 is connected to the CLK 
 pin 5 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(4,6,5,1);


elapsedMillis lastRadio;
char sSegment[8];
byte utc1[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,0};
byte utc2[]={2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,0,1};
byte Second, Minute, Hour;
byte light = 15;
int lux ;
bool dot;

AltSoftSerial altSerial;
TinyGPSPlus gps;
float delaytime = (500);
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 200;


void setup() {

  pinMode(A0,INPUT); // letni /zimni cas
pinMode(A1,INPUT);   //adaptivni/max jas
pinMode(A2,INPUT);   // kanal 0/125
pinMode(A3,INPUT);  // luxmetr mode - toho casu fuck off

  Serial.begin(9600);
  altSerial.begin(9600);
  lightMeter.begin();
  Wire.begin();

  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,15);
  /* and clear the display */
  lc.clearDisplay(0);
  lc.setScanLimit(0,6);

   lc.setChar(0,5,'8',true);
  delay(delaytime);
  lc.setChar(0,4,'8',true);
  //lc.setRow(0,4,B00110111);
  delay(delaytime);
  lc.setChar(0,3,'8',true);
  //lc.setRow(0,3,B00110111);
  delay(delaytime);
  lc.setChar(0,2,'8',true);
  //lc.setRow(0,2,B01111110);
  delay(delaytime);
  lc.setChar(0,1,'8',true);
  //lc.setRow(0,1,B00111000);
  delay(delaytime);
  //lc.setChar(0,4,'8',false);
  //lc.setRow(0,1,0x05);
  //delay(delaytime);
  lc.setChar(0,0,'8',true);
  delay(2500);
 lc.clearDisplay(0);

  radio.begin();
   radio.openReadingPipe(0, address);
   if (digitalRead(A2) == LOW){
   radio.setChannel (125) ;}
   else
 {radio.setChannel (0);}
 radio.setDataRate( RF24_250KBPS );
 radio.setPALevel(RF24_PA_MAX);
   // radio.setRetries(3,5); // delay, count
 // radio.openReadingPipe(0, address);
 // radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
   startMillis = millis();  //initial start time
}


void loop() {
  if (digitalRead(A1) == LOW){
currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    lux = lightMeter.readLightLevel();
        light = map(lux, 0, 20000, 0, 15);
       
               lc.setIntensity(0,light);
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
  }
 else 
  {lc.setIntensity(0,15);}

  
   if (digitalRead(A3) == LOW){
  char txt[6];
   sprintf(txt, lux);
    for (int i=0, j = 5; i<6; i++, j--)
    {
      if(txt[i]!=sSegment[i]){
    sSegment[i]=txt[i];
         
        
        
       lc.setChar(0, j, sSegment[i], false);
          }
          }
 // Serial.print("Light: ");
 // Serial.println(lux);
  //Serial.println(" lx");
//delay(1000);
 //lux = map(lux, 0, 200, 0, 15);
// Serial.print(light);}
   }
   else
   {
   
  
  if (radio.available()) {
    char text[7];
    radio.read(text, 7);
    //Serial.print(text);
   if (text[6]==58) {
    lastRadio=0;
   // Serial.print(text);
   

   //Serial.print(text);
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
   // Serial.println(" lx");
    if (gps.encode(altSerial.read()))
    {
      if (gps.time.isValid())
      {
        char text[6];
        Minute = gps.time.minute();
        Second = gps.time.second();
        Hour   = gps.time.hour();
        
        if (digitalRead(A0) == LOW){
        sprintf(text, "%02u%02u%02u", utc2[Hour], Minute, Second);}
        else
        {sprintf(text, "%02u%02u%02u", utc1[Hour], Minute, Second);}
      
        if (lastRadio>10000 && memcmp(sSegment, text, 7)){
           
          for (int i=0, j = 5; i<6; i++, j--)
          {
              sSegment[i]=text[i];
           dot= Second % 2 ;
          
        
              lc.setChar(0, j, sSegment[i],  dot);
          }
        }
      }
    }
  }
   }
  }

     


