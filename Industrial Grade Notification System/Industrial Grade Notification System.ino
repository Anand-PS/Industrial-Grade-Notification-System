#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <DS3231.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27 // Add your I2C address here.
#define Rs_pin 0
#define Rw_pin 1
#define En_pin 2
#define BACKLIGHT_PIN 3
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

DS3231  rtc(SDA, SCL);
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
Time  t;
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


int Hor;
int Min;
int Sec;

int testStepDelay = 7000;

short testPlay = 0;
bool playFlag = 0;
unsigned long previousMillis = 0;
const long interval = 9000;
unsigned long currentMillis = 0;

short testPlay_1 = 0;
bool playFlag_1 = 0;
unsigned long previousMillis_1 = 0;
const long interval_1 = 5000;
/*
int norm = 2;
int exm = 3;
int emcy = 8;

int red = 5;
int green = 6;
int blue = 9;
*/

#define norm 2
#define exm 3
#define emcy 8

#define red 5
#define green 9
#define blue 6

bool normal;
bool exam;
bool emergency;
bool Select;
bool systemTest;
bool Loop = 0;

short mode=0;
short selectedMode=0;



void setup()
{  
  pinMode(A0, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A0,HIGH);
  digitalWrite(A2,LOW);
  pinMode(norm, INPUT);
  pinMode(exm, INPUT);
  pinMode(emcy, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  delay(100);
  
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE); // LCD Backlight ON
  lcd.setBacklight(HIGH);
 
  systemTest = digitalRead(A1);
  if(systemTest==1)
  {
    int a = 250;
    digitalWrite(blue,HIGH);
    delay(a);
    digitalWrite(blue,LOW);
    delay(a);
    digitalWrite(blue,HIGH);
    delay(a);
    digitalWrite(blue,LOW);
    delay(a);
  }
  
  lcd.home ();
  Wire.begin();
  rtc.begin();
  lcd.begin(16,2);
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);


  Serial.println();
  Serial.println(F("Initializing MP3 Module..."));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) 
  {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Communication Error!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
    Serial.println(F("MP3 Module is online."));
  
    myDFPlayer.setTimeOut(500);
  
    myDFPlayer.volume(20);  //(0~30)
    
//  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);

    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD); //Default Storage

  //pinMode(buz, OUTPUT);
  if(systemTest==1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Booting into");
    lcd.setCursor(0,1);
    lcd.print("System Test Mode");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    for(int i = 0;i<16;i++)
    {
    lcd.print(".");
    delay(150);
    }
    lcd.clear();
  }
  boot();
 // settime();
}

void loop()
{
 t = rtc.getTime();
 Hor = t.hour;
 Min = t.min;
 Sec = t.sec;

 currentMillis = millis();

 if(systemTest==1)
 {
 if (currentMillis - previousMillis >= interval) 
 {
  Serial.print("TestPlay Update : ");
  Serial.println(testPlay);
  playFlag = 1;
  previousMillis = currentMillis;
 }

 if (currentMillis - previousMillis_1 >= interval_1) 
 {
  Serial.print("TestPlay 1 Update : ");
  Serial.println(testPlay_1);
  playFlag_1 = 1;
  previousMillis_1 = currentMillis;
 }
 }
 
 if (myDFPlayer.available()) 
  {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the Error details
  }
 selectedMode = analyseInput();

 
 switch(selectedMode)
 {
  case 1:
        regularAlert();
        digitalWrite(red,LOW);
        digitalWrite(green,HIGH);
        digitalWrite(blue,LOW);
        lcd.setCursor(0,0);
        lcd.print("Mode: ");
        lcd.print("Regular");
        break;
  case 2:
        examAlert();
        digitalWrite(red,LOW);
        digitalWrite(green,LOW);
        digitalWrite(blue,HIGH);
        lcd.setCursor(0,0);
        lcd.print("Mode: ");
        lcd.print("Exam");
        break;
  case 3:
        emergencyAlert();
        digitalWrite(red,HIGH);
        digitalWrite(green,LOW);
        digitalWrite(blue,LOW);
        lcd.setCursor(0,0);
        lcd.print("Mode: ");
        lcd.print("Emergency");
        break;     
}

if(systemTest==0)
  {
    lcd.setCursor(0,1);
    lcd.print("Time: ");
    lcd.print(rtc.getTimeStr());
    //lcd.print("Date: ");
    //lcd.print(rtc.getDateStr());
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("System Test Mode");
  }
}

void boot()
{   
  lcd.setCursor(6,0);
  lcd.print("Smart");
  lcd.setCursor(2,1);
  lcd.print("Talking Bell");
  delay(2000);
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("By Team");
  lcd.setCursor(4,1);
  lcd.print("Spectra");
  delay(2000);
  Serial.println("\nBooting Successful");
  lcd.clear();
}
void settime()
{
  rtc.setDOW(THURSDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(13, 00, 01);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(20, 2, 2020);   // Set the date to January 1st, 2014
}

int analyseInput()
{
  normal=digitalRead(norm);
  exam=digitalRead(exm);
  emergency=digitalRead(emcy);
  
/*/////////////////////////////////////////////////////////////////////////
  normal=0;
  exam=0;
  emergency=0;
  Select=0;
/////////////////////////////////////////////////////////////////////////*/

  if(normal==1 && exam==1 && emergency==1)
  {
    mode=4;
    error();
  }
  else if(normal==1 && exam==1)
  {
    mode=4;
    error();
  }
  else if(normal==1 && emergency==1)
  {
    mode=4;
    error();
  }
  else if(exam==1 && emergency==1)
  {
    mode=4;
    error();
  }/////////////////////////////////////////
  else if(normal==1)
  {
    mode=1; //Normal Mode Selected
  }
  else if(exam==1)
  {
    mode=2; //Exam Mode Selected
  }
  else if(emergency==1)
  {
    mode=3; //emergency Mode Selected
  }
  
  return mode;
}

void regularAlert()
{
  if(systemTest == 0)
  {
  if( Hor == 9 && Min == 15)
    {
          Loop = 1;
          myDFPlayer.play(1);  //Play the first mp3 file named as 1.mp3
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Period No:1");
          lcd.setCursor(0,1);
          lcd.print("Started at 9:15");
          Serial.println("Period No:1. Started at 9:15");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 9 && Min == 15)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
  else if( Hor == 10 && Min == 10)
  {
          Loop = 1;
          myDFPlayer.play(2);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Period No:2");
          lcd.setCursor(0,1);
          lcd.print("Started at 10:10");
          Serial.println("Period No:2. Started at 10:10");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 10 && Min == 10)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 11 && Min == 10)
  {
          Loop = 1;
          myDFPlayer.play(3); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Period No:3");
          lcd.setCursor(0,1);
          lcd.print("Started at 11:10");
          Serial.println("Period No:3. Started at 11:10");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 11 && Min == 10)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 12 && Min == 05)
  {
          Loop = 1;
          myDFPlayer.play(4);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Period No:4");
          lcd.setCursor(0,1);
          lcd.print("Started at 12:05");
          Serial.println("Period No:4. Started at 12:05");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 12 && Min == 05)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     
     else if( Hor == 13 && Min == 00)
  {
          Loop = 1;
          myDFPlayer.play(5); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Lunch Break");
          lcd.setCursor(0,1);
          lcd.print("Started at 01:00");
          Serial.println("Lunch Break. Started at 01:00");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 13 && Min == 00)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 13 && Min == 45)
  {
          Loop = 1;
          myDFPlayer.play(6);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Period No:5");
          lcd.setCursor(0,1);
          lcd.print("Started at 01:45");
          Serial.println("Period No:5. Started at 01:45");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 13 && Min == 45)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 14 && Min == 40)
  {
          Loop = 1;
          myDFPlayer.play(7);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Period No:6");
          lcd.setCursor(0,1);
          lcd.print("Started at 02:40");
          Serial.println("Period No:6. Started at 02:40");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 14 && Min == 40)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 15 && Min == 35)
  {
          Loop = 1;
          myDFPlayer.play(8); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Period No:7");
          lcd.setCursor(0,1);
          lcd.print("Started at 03:35");
          Serial.println("Period No:7. Started at 03:35");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 15 && Min == 35)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
  }//////////////////////////////////////////////////////////////////////////////////////////////////////////
  else
  {
    if(playFlag==1)
    {
     playFlag = 0;
     
    switch(testPlay)
     {
     case 0:
            myDFPlayer.play(1);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Period No:1");
            lcd.setCursor(0,1);
            lcd.print("Started at 9:15");
            Serial.println("Period No:1. Started at 9:15");
            delay(testStepDelay);
            lcd.clear();
            break;

     case 1:
            myDFPlayer.play(2);  
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Period No:2");
            lcd.setCursor(0,1);
            lcd.print("Started at 10:10");
            Serial.println("Period No:2. Started at 10:10");
            delay(testStepDelay);
            lcd.clear();
            break;

     case 2:
            myDFPlayer.play(3);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Period No:3");
            lcd.setCursor(0,1);
            lcd.print("Started at 11:10");
            Serial.println("Period No:3. Started at 11:10");
            delay(testStepDelay);
            lcd.clear();
            break;

     case 3:
            myDFPlayer.play(4);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Period No:4");
            lcd.setCursor(0,1);
            lcd.print("Started at 12:05");
            Serial.println("Period No:4. Started at 12:05");
            delay(testStepDelay);
            lcd.clear();
            break;

     case 4:
            myDFPlayer.play(5);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Lunch Break");
            lcd.setCursor(0,1);
            lcd.print("Started at 01:00");
            Serial.println("Lunch Break. Started at 01:00");
            delay(testStepDelay);
            lcd.clear();
            break;
     
     case 5:
            myDFPlayer.play(6);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Period No:5");
            lcd.setCursor(0,1);
            lcd.print("Started at 01:45");
            Serial.println("Period No:5. Started at 01:45");
            delay(testStepDelay);
            lcd.clear();
            break;         
          
     case 6:
            myDFPlayer.play(7);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Period No:6");
            lcd.setCursor(0,1);
            lcd.print("Started at 02:40");
            Serial.println("Period No:6. Started at 02:40");
            delay(testStepDelay);
            lcd.clear();
            break;
     case 7:
            myDFPlayer.play(8);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Period No:7");
            lcd.setCursor(0,1);
            lcd.print("Started at 03:35");
            Serial.println("Period No:7. Started at 03:35");
            delay(testStepDelay);
            lcd.clear();
            break;             
     }
     
     testPlay++;
     if(testPlay==8)
     {
       testPlay = 0;
     }
     
    }
  }
}
void examAlert()
{
  if(systemTest == 0)
  {
  if( Hor == 8 && Min == 45)
    {
          Loop = 1;
          myDFPlayer.play(9); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Student's ID");
          lcd.setCursor(0,1);
          lcd.print("Checking Process");
          Serial.println("Students ID Check");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 8 && Min == 45)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 9 && Min == 00)
     {
          Loop = 1;
          myDFPlayer.play(10);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Instructions for");
          lcd.setCursor(0,1);
          lcd.print("Filling OMR");
          Serial.println("Instructions for Filling OMR Sheet");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 9 && Min == 00)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 9 && Min == 10)
     {
          Loop = 1;
          myDFPlayer.play(11); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Exam");
          lcd.setCursor(0,1);
          lcd.print("Started");
          Serial.println("Exam Started");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 9 && Min == 10)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 9 && Min == 30)
       {
            Loop = 1;
            myDFPlayer.play(12);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Further canidates");
            lcd.setCursor(0,1);
            lcd.print("Not allowed");
            Serial.println("Further canidates Not allowed");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
              Sec = t.sec;
              Serial.print("Minute :");
              Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 9 && Min == 30)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 12 && Min == 00)
       {
            Loop = 1;
            myDFPlayer.play(13); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Only 10 Minutes");
            lcd.setCursor(0,1);
            lcd.print("Remaining");
            Serial.println("Only 10 Minutes Remaining");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
              Sec = t.sec;
              Serial.print("Minute :");
              Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 12 && Min == 00)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 12 && Min == 10)
       {
            Loop = 1;
            myDFPlayer.play(14); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("End of this Exam");
            lcd.setCursor(0,1);
            lcd.print("Pen Down");
            Serial.println("End of this Exam, Pen Down");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 12 && Min == 10)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }

     if(rtc.getDOWStr()=="Friday")
  {////////////////////////////////////////--------------------------------------------------------------
    if( Hor == 13 && Min == 45)
    {
          Loop = 1;
          myDFPlayer.play(9); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Student's ID");
          lcd.setCursor(0,1);
          lcd.print("Checking Process");
          Serial.println("Students ID Check");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 13 && Min == 45)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 14 && Min == 00)
     {
          Loop = 1;
          myDFPlayer.play(10);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Instructions for");
          lcd.setCursor(0,1);
          lcd.print("Filling OMR");
          Serial.println("Instructions for Filling OMR Sheet");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 14 && Min == 00)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 14 && Min == 10)
     {
          Loop = 1;
          myDFPlayer.play(11); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Exam");
          lcd.setCursor(0,1);
          lcd.print("Started");
          Serial.println("Exam Started");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 14 && Min == 10)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 14 && Min == 30)
       {
            Loop = 1;
            myDFPlayer.play(12);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Further canidates");
            lcd.setCursor(0,1);
            lcd.print("Not allowed");
            Serial.println("Further canidates Not allowed");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
              Sec = t.sec;
              Serial.print("Minute :");
              Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 14 && Min == 30)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 17 && Min == 00)
       {
            Loop = 1;
            myDFPlayer.play(13); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Only 10 Minutes");
            lcd.setCursor(0,1);
            lcd.print("Remaining");
            Serial.println("Only 10 Minutes Remaining");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
              Sec = t.sec;
              Serial.print("Minute :");
              Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 17 && Min == 00)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 17 && Min == 10)
       {
            Loop = 1;
            myDFPlayer.play(14); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("End of this Exam");
            lcd.setCursor(0,1);
            lcd.print("Pen Down");
            Serial.println("End of this Exam, Pen Down");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 17 && Min == 10)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
  }////////////////////////////////////////--------------------------------------------------------------
  else
  {
    if( Hor == 12 && Min == 45)
    {
          Loop = 1;
          myDFPlayer.play(9); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Student's ID");
          lcd.setCursor(0,1);
          lcd.print("Checking Process");
          Serial.println("Students ID Check");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 12 && Min == 45)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 13 && Min == 00)
     {
          Loop = 1;
          myDFPlayer.play(10);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Instructions for");
          lcd.setCursor(0,1);
          lcd.print("Filling OMR");
          Serial.println("Instructions for Filling OMR Sheet");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 13 && Min == 00)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
     else if( Hor == 13 && Min == 10)
     {
          Loop = 1;
          myDFPlayer.play(11); 
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Exam");
          lcd.setCursor(0,1);
          lcd.print("Started");
          Serial.println("Exam Started");
          while(Loop==1)
          {
            delay(1000);
            t = rtc.getTime();
            Hor = t.hour;
            Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 13 && Min == 10)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 13 && Min == 30)
       {
            Loop = 1;
            myDFPlayer.play(12);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Further canidates");
            lcd.setCursor(0,1);
            lcd.print("Not allowed");
            Serial.println("Further canidates Not allowed");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
              Sec = t.sec;
              Serial.print("Minute :");
              Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 13 && Min == 30)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 16 && Min == 00)
       {
            Loop = 1;
            myDFPlayer.play(13); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Only 10 Minutes");
            lcd.setCursor(0,1);
            lcd.print("Remaining");
            Serial.println("Only 10 Minutes Remaining");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
              Sec = t.sec;
              Serial.print("Minute :");
              Serial.print(Min);
              Serial.print(" Second :");
              Serial.println(Sec);
              if( Hor == 16 && Min == 00)
              {
                Loop = 1;
              }
              else
              {
                Loop = 0;
                lcd.clear();
              }
            }
       }
       else if( Hor == 16 && Min == 10)
       {
            Loop = 1;
            myDFPlayer.play(14); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("End of this Exam");
            lcd.setCursor(0,1);
            lcd.print("Pen Down");
            Serial.println("End of this Exam, Pen Down");
            while(Loop==1)
            {
              delay(1000);
              t = rtc.getTime();
              Hor = t.hour;
              Min = t.min;
            Sec = t.sec;
            Serial.print("Minute :");
            Serial.print(Min);
            Serial.print(" Second :");
            Serial.println(Sec);
            if( Hor == 16 && Min == 10)
            {
              Loop = 1;
            }
            else
            {
              Loop = 0;
              lcd.clear();
            }
          }
     }
  }
  }
  else
  {
    if(playFlag_1 == 1)
    {
      playFlag_1 = 0;
      switch(testPlay_1)
        {
    case 0:
          myDFPlayer.play(9);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Student's ID");
          lcd.setCursor(0,1);
          lcd.print("Checking Process");
          Serial.println("Students ID Check");
          delay(testStepDelay);
          lcd.clear();
          break;

    case 1:
          myDFPlayer.play(10);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Instructions for");
          lcd.setCursor(0,1);
          lcd.print("Filling OMR");
          Serial.println("Instructions for Filling OMR Sheet");
          delay(testStepDelay);
          lcd.clear();
          break;

    case 2:
          myDFPlayer.play(11);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Exam");
          lcd.setCursor(0,1);
          lcd.print("Started");
          Serial.println("Exam Started");
          delay(testStepDelay);
          lcd.clear();
          break;

    case 3:
          myDFPlayer.play(12);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Further canidates");
          lcd.setCursor(0,1);
          lcd.print("Not allowed");
          Serial.println("Further canidates Not allowed");
          delay(testStepDelay);
          lcd.clear();
          break;

    case 4:
          myDFPlayer.play(13);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Only 10 Minutes");
          lcd.setCursor(0,1);
          lcd.print("Remaining");
          Serial.println("Only 10 Minutes Remaining");
          delay(testStepDelay);
          lcd.clear();
          break;

    case 5:
          myDFPlayer.play(14);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("End of this Exam");
          lcd.setCursor(0,1);
          lcd.print("Pen Down");
          Serial.println("End of this Exam, Pen Down");
          delay(testStepDelay);
          lcd.clear();
          break;                                                  
     
        }
        testPlay_1++;
        if(testPlay_1 == 6)
        {
         testPlay_1 = 0;
        }
    }
  }
}
void emergencyAlert()
{
  myDFPlayer.play(15);
  Loop = 1;
  digitalWrite(green,LOW);
  digitalWrite(blue,LOW);
  while(Loop==1)
          {
          Serial.print("Emergency Mode");
          lcd.clear();
          delay(700);
          lcd.setCursor(3,0);
          lcd.print("Emergency");
          lcd.setCursor(5,1);
          lcd.print("Mode !");
          digitalWrite(red,HIGH);
          delay(1500);
          digitalWrite(red,LOW);
          }
}

void error()
{
  while(mode==4)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Error");
    Serial.println("Error");
    delay(1000);
    analyseInput();
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
