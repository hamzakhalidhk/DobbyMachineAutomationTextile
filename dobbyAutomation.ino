#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SD.h>
#include <SPI.h>
#define ARRAYSIZE 300

int MAXCOUNTER;
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

const byte interruptPin = 2;
byte rowPins[ROWS] = {0, 1, 8, 3};
byte colPins[COLS] = {4, 5, 6, 7};

String c="0";
String temp;
char customKey; 
int block = 0;
int relayPin = 28;
int i=0;
int pinCS = 53; // Pin 10 on Arduino Uno
int sensorValue, tempTurns, turns, search;
unsigned int value[ARRAYSIZE];
boolean activate=false;
const int  sensorPin = 22;  
const int  ledPin = A2;  
int sensorPushCounter = 0;   
int sensorState = 0;         
int lastsensorState = 0;   

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  
File myFile;


void setup(){
  //Interrupt
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), mode, FALLING);

  //IR Sensor
  pinMode(sensorPin, INPUT_PULLUP);

  //Relay Pin
  pinMode(relayPin,OUTPUT); 

  //IR Sensor
  pinMode(ledPin, OUTPUT);

  //LCD
  lcd.begin(16,2);
  lcd.backlight();

  //SD Card
  pinMode(pinCS, OUTPUT);

  if (SD.begin())
  {
    lcd.setCursor(0,0);
    lcd.print("SD card Ready.");
  } else
  {
    lcd.setCursor(0,0);
    lcd.print("SD card failed.");
    return;
  }
  delay(1000);
  
  lcd.clear();

  myFile = SD.open("textile.txt");
  
  if (myFile) {
    while (myFile.available()) {
      c = myFile.readStringUntil('\n');
      lcd.setCursor(0,0);
      lcd.print("Reading SD card:");
      lcd.setCursor(0,1);
      lcd.print(c.toInt());
      delay(100);
      lcd.clear();
      value[i] = c.toInt();
      i++;
   }
   
    myFile.close();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("File read done.");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No. of turns:");
    lcd.setCursor(0,1);
    lcd.print(sensorPushCounter);
    MAXCOUNTER = value[0];
  }
  else {
    lcd.setCursor(0,0);
    lcd.print("File read failed.");
    lcd.setCursor(0,1);
    lcd.print("Restart system.");
  }
  
}

void loop(){


 sensorState = digitalRead(sensorPin);
  
  if (sensorState != lastsensorState) {
   
    if (sensorState == HIGH) {
   
        
      sensorPushCounter++;
        
    } 
   
    //delay(50);
  }

  lastsensorState = sensorState;
  
  if(sensorPushCounter > MAXCOUNTER){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("No. of Turns:");
        lcd.setCursor(0,1);
        sensorPushCounter = 1;     
        }
    
if(block==0 && sensorPushCounter > 0 ){
 
   lcd.setCursor(0,1); 
   lcd.print(sensorPushCounter);
 
    for(search=1; search<=ARRAYSIZE; search++){
     if(value[search] == sensorPushCounter){
      
      digitalWrite(relayPin,HIGH);
      search=ARRAYSIZE;
      
      }
      else{
        digitalWrite(relayPin,LOW);
        }
       tempTurns=sensorPushCounter;
     }
    delay(500);

  }    

//Keypad  
if(block==1) {
    keys();
  }

}


void mode() {
    block=1;
    search=ARRAYSIZE;
    digitalWrite(A2, HIGH);
}

void keys(){

  customKey = customKeypad.getKey();
  if(customKey){
    if (customKey != '#' && customKey != '*' && customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D'){
        temp+=customKey;}
        lcd.setCursor(0,0); 
        lcd.clear(); 
        lcd.print("Initial Value:");
        lcd.setCursor(0, 1);
        lcd.print(temp); 
         
        //Accept value Button
        if (customKey == 'A'){
            lcd.clear();
            sensorPushCounter=temp.toInt();
            temp="";
            lcd.setCursor(0,0);
            lcd.print("No. of turns:");
            lcd.setCursor(0,1);
            lcd.print(sensorPushCounter);
            block=0;
            digitalWrite(A2, LOW);
         }

         //Backspace Button
         else if (customKey == 'B'){
            lcd.setCursor(0,0); 
            lcd.clear(); 
            lcd.print("No. of turns:");
            lcd.setCursor(0,1);
            temp=temp.substring(0,temp.length()-1);
            lcd.print(temp);
         }
         
         //Cancel Button
         else if (customKey == 'C'){
            lcd.setCursor(0,0); 
            lcd.clear(); 
           lcd.print("No. of turns:");
            lcd.setCursor(0,1);
            sensorPushCounter=tempTurns;
            lcd.print(tempTurns);
            block=0;
            digitalWrite(A2, LOW);
         }    
//
//          //Backward Button
         if (customKey == '*'){
            lcd.setCursor(0,0); 
            lcd.clear(); 
            lcd.print("No. of turns:");
            lcd.setCursor(0,1);
            tempTurns-=1;
            sensorPushCounter=tempTurns;
            lcd.print(tempTurns);
            activate=true;
         } 
       //Backward Button
         if (customKey == '#'){
            lcd.setCursor(0,0); 
            lcd.clear(); 
            lcd.print("No. of turns:");
            lcd.setCursor(0,1);
            tempTurns+=1;
            sensorPushCounter=tempTurns;
            lcd.print(tempTurns);
            activate=true;
         }
         //Backward Button
         else if (customKey == 'D' && activate==true){
            block=0;
            activate=false;
            digitalWrite(A2, LOW);
        }
           
    }
}
