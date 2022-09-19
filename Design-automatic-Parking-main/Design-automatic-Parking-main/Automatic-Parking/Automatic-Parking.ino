#include<UltraDistSensor.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
// define pin sensor
#define TRIG_PIN1 3
#define ECHO_PIN1 2
#define TRIG_PIN2 4
#define ECHO_PIN2 5
#define TIME_OUT 5000
// define pin rfid
const byte SS_PIN[] =  {8,10};
const byte RST_PIN = 9;

const byte numReaders = 2;


LiquidCrystal_I2C lcd(0x27,20,4); 
UltraDistSensor mysensor;
UltraDistSensor mysensor2;
float reading;
//const int trigPin = 8;
//const int echoPin = 7;
long duration;
int distanceCm1, distanceCm2;

Servo myservo;

int pos=0;
MFRC522 mfrc522[numReaders];   // Create MFRC522 instance.

void setup()
{
  // lcd, sensor config
  pinMode(6, OUTPUT);
  pinMode(A0,OUTPUT);
  lcd.init();                    
  lcd.backlight();
  Serial.begin(9600);
  mysensor.attach(3,2);
  mysensor2.attach(4,5);
  lcd.setCursor(4,0);
  lcd.print("SMART PARKING");
  
  // barier config
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
     // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  myservo.attach(7);
}

void Distance1()
{
  digitalWrite(TRIG_PIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN1, LOW);
  duration = pulseIn(ECHO_PIN1, HIGH);
  distanceCm1= duration*0.034/2;
}
void Distance2()
{
  digitalWrite(TRIG_PIN2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN2, LOW);
  duration = pulseIn(ECHO_PIN2, HIGH);
  distanceCm2= duration*0.034/2;
}
void open1(){
    for (pos = 0; pos <=90; pos +=1) {
    myservo.write(pos);
    delay(5);
    }
  }
void close1(){
  for (pos = 90; pos >= 0; pos -=1) {
     myservo.write(pos);
    }
  }
int t=0;
void sensor(){
    Distance1();
    Distance2();
  lcd.setCursor(1,1);
  lcd.print("vi tri con trong: ");
    if(distanceCm1 > 10){
      if(distanceCm2 > 10){
        digitalWrite(6, HIGH); 
        digitalWrite(A0, HIGH);
        lcd.setCursor(1,2);
        lcd.print("A1 A2          ");
      }
      else{
        digitalWrite(6, HIGH);
        digitalWrite(A0, LOW);
        lcd.setCursor(1,2);
        lcd.print("A1            ");
        t = distanceCm2;
      }
    }
    else{
      if(distanceCm2 > 10){
        digitalWrite(6, LOW); 
        digitalWrite(A0, HIGH);
        lcd.setCursor(1,2);
        lcd.print("A2            ");  
        t = distanceCm1;    
      }
      else{
      digitalWrite(A0, LOW); 
      digitalWrite(6, LOW);
      lcd.setCursor(1,2);
      lcd.print(" HET CHO TRONG ");
      }
    }
}
void rfid()
{
  myservo.write(0);
  for(uint8_t i = 0; i < numReaders; i++){

    mfrc522[i].PCD_Init(SS_PIN[i], RST_PIN);
//    if ( ! mfrc522[i].PICC_IsNewCardPresent()) 
//  {
//    return;
//  }
//  // Select one of the cards
//  if ( ! mfrc522[i].PICC_ReadCardSerial()) 
//  {
//    return;
//  }
  
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  if(mfrc522[i].PICC_IsNewCardPresent() &&  mfrc522[i].PICC_ReadCardSerial()){
  for (byte j = 0; j < mfrc522[i].uid.size; j++) 
    {
     Serial.print(mfrc522[i].uid.uidByte[j] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522[i].uid.uidByte[j], HEX);
     content.concat(String(mfrc522[i].uid.uidByte[j] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522[i].uid.uidByte[j], HEX));
    }
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "F3 9E B0 18" | content.substring(1) == "7C 12 18 49" ) //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    Serial.println();
    open1();
    delay(3000);
    close1();
  }


   else   {
    Serial.println(" Access denied");
    delay(1000);
  }
  mfrc522[i].PCD_DumpVersionToSerial();
 }
}
void loop(){
  // rfid, servo
  rfid();
  // lcd, sensor, led
  sensor();
  }
