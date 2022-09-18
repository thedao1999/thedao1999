/*
  author: Nguyen The Dao
*/
#include <Wire.h>
#include "RTClib.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#define Led_red 16 //D0
#define Led_green 15 //D8
#define SS_PIN 2  //D4
#define RST_PIN 0 //D3

RTC_DS1307 rtc;
char Time[10][30] = {"5p", "10p", "15p", "30p", "45p", "1 tieng", "1 tieng 30p", "2 tieng", "nua buoi",""};
char a[] = "Nguyen The Dao";
char b[] = "Nguyen Tien Manh";
char c[] = "Tran Van Giau";
char d[] = "Doong Xuan Cao";
char e[] ="Sep";
char f[] ="ID chua duoc dang ky";

const char* ssid = "B-LINK_Guest";
const char* password = "123456789";
const char* mqtt_server = "broker.hivemq.com";
WiFiClient mqtt;
PubSubClient client(mqtt);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance/object.
unsigned long interval = 5000;
unsigned long previousMillis = 0;
int variable = 0;

void setupRTC ()
{
 Serial.begin(115200);
 if (! rtc.begin())
 {
   Serial.print("Couldn't find RTC");
   while (1);
 }

  if (! rtc.isrunning())
 {
   Serial.print("RTC is NOT running!");
   Serial.println();
 }
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // tu dong cap nhat gio tai may tinh
   //rtc.adjust(DateTime(2021, 5, 14, 21, 0, 0)); //cai dat gio thu cong
}
void setup_wifi() {

  delay(100);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void setup() 
{
  pinMode(Led_red, OUTPUT);
  pinMode(Led_green,OUTPUT);
  Serial.begin(115200);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Show your card:");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  reconnect();
  setupRTC();
}

void reconnect() {
  // loop toi khi ket noi toi mqtt
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "mqtt-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void publish_string(char t[100], char r[100]) 
{  
if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if ((unsigned long)(millis() - previousMillis) >= interval) {
    previousMillis = millis();
    Serial.print(t);
    client.publish("home/timekeeper", t);// ten thanh vien
    Serial.print("  ");
    Serial.print(r);
    client.publish("home/RTC", r);// thoi gian muon
    delay(500);
  }
}

void RTC(char T[100])
{
  char k[10][30];
  int t=9;
   DateTime now = rtc.now();
   if(now.hour() == t){
    if(now.minute()==0 ){
      publish_string(T,"0");
      }
    if(now.minute() > 0 & now.minute() <= 5 ){
      publish_string(T,Time[0]);
      }
    if(now.minute() > 5 & now.minute() <= 10 ){
      publish_string(T,Time[1]);
      }
    if(now.minute() > 10 & now.minute() <= 15 ){
      publish_string(T,Time[2]);
      }
    if(now.minute() > 15 & now.minute() <= 30 ){
      publish_string(T,Time[3]);
      }
    if(now.minute() > 30 & now.minute() <= 45 ){
      publish_string(T,Time[4]);
      }
    if(now.minute() > 45 & now.minute() <= 60 ){
      publish_string(T,Time[5]);
      }
    }
    else{
      if( now.hour()<t){
        publish_string(T,"0");
        }
      if(now.hour()==t+1){
        if(now.minute() > 0 & now.minute() <=30){
          publish_string(T,Time[6]);
          }
        if(now.minute() >30 & now.minute()<=60){
          publish_string(T,Time[7]);
          }
        }
      if(now.hour()==t+2){
        if(now.minute() > 0 & now.minute() <=5){
          publish_string(T,Time[7]);
          }
          else{
            publish_string(T,Time[8]);
            }
          }
      else{
        publish_string(T,Time[8]);
        }
      }
  }
 void Timekeeper()
 {
  bool access = false;
    // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase(); // chuyen tat ca cac ki tu thanh in hoa
  Serial.println();
  delay(500);
  if(content.substring(1) == "1C 6E 50 49"){
    digitalWrite(Led_green, HIGH);
    access =true;
    RTC(a);
    digitalWrite(Led_green, LOW);
    }
  if(content.substring(1)== "CC 01 8D 18"){
    digitalWrite(Led_green, HIGH);
    access =true;
    RTC(b);
    digitalWrite(Led_green, LOW);
    }
  if(content.substring(1)== "6C 98 23 18"){
    digitalWrite(Led_green, HIGH);
    access =true;
    RTC(c);
    digitalWrite(Led_green, LOW);
  }
  if(content.substring(1)== "CC 84 9A 16"){
    digitalWrite(Led_green, HIGH);
     access =true;
     RTC(d);
     digitalWrite(Led_green, LOW);
  }
  if(content.substring(1)== "3C 55 50 18"){
    digitalWrite(Led_green, HIGH);
    access =true; 
    RTC(e);
    digitalWrite(Led_green, LOW);
  }
  if(access == false){
    digitalWrite(Led_red, HIGH);
    publish_string(f,Time[9]);// id chua duoc dang ky, nen khong tinh thoi gian
    digitalWrite(Led_red, LOW);
  }
 }
void loop() 
{
  Timekeeper();
} 
