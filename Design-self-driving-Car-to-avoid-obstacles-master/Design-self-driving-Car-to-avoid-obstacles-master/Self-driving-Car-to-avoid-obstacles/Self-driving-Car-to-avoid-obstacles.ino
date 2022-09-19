/*
 * author: Nguyen The Dao
 */
#include <Servo.h>  
#include<RF24.h>
#define trig 2 //chân trig của HC-SR05
#define echo 3//chân echo của HC-SR05  
#define TIME_OUT 5000
Servo srf05;
int pos = 0;
#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4
#define MAX_SPEED 255 //từ 0-255
#define MIN_SPEED 0
int speed_1;
void setup()
{
  Serial.begin(9600);
  srf05.attach(13);  
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

int distance_cm(byte angle)
{
  srf05.write(angle);
  delay(500);
  unsigned long duration;//biến đo thời gian
  int distance;//biến lưu khoảng cách

  /* phát xung từ chân trig */
  digitalWrite(trig,0);//tắt chân trig
  delayMicroseconds(2);
  digitalWrite(trig,1);// phát xung từ chân trig
  delayMicroseconds(5);// xung có độ dài 5 microSeconds
  digitalWrite(trig,0);//tắt chân trig
  
  /* tính toán thời gian */
  duration = pulseIn(echo,HIGH);//đo độ rộng xung HIGH ở chân echo. 
  distance = int(duration/2/29.412);//tính khoảng cách đến vật.
  return distance;
}
void motor_1_Dung() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}
 
void motor_2_Dung() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
 
void motor_1_Tien(int speed_1) { //speed: từ 0 - MAX_SPEED
  speed_1 = constrain(speed_1, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED 
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

}
 
void motor_1_Lui(int speed_1) {
  speed_1 = constrain(speed_1, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED 
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

}
 
void motor_2_Tien(int speed_1) { //speed: từ 0 - MAX_SPEED
  speed_1 = constrain(speed_1, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED 
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

}
 
void motor_2_Lui(int speed_1) {
  speed_1 = constrain(speed_1, MIN_SPEED, MAX_SPEED);//đảm báo giá trị nằm trong một khoảng từ 0 - MAX_SPEED 
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void dungyen(){
  motor_1_Dung();
  motor_2_Dung();
}
void dithang(){
  motor_1_Tien(speed_1);
  motor_2_Tien(speed_1);
} 
void dilui(){
  motor_2_Lui(speed_1);
  motor_1_Lui(speed_1);

}
void retrai(){
  motor_2_Dung();
  motor_1_Tien(speed_1);
}
void rephai(){
  motor_2_Tien(speed_1);
  motor_1_Dung();
}

int allow_distance=50; // khoang cach cho phep la 50cm 

int tranhvatcan2(){ // neu khoang cach cho phep nho hon khoang cach truoc mat thi kiem tra khoang cach 2 ben, neu van nho hon khoang cach cho phep thi lui.
  int max_distance;
  int distance_left = distance_cm(180);
  int distance_right = distance_cm(0);
  max_distance = max(distance_left, distance_right); // tra ve khoang cach lon nhat 
    if(max_distance == distance_left){
        if(max_distance > allow_distance){ // neu khoang canh ben trai lon hon khoang cach cho phep thi re trai
        retrai();
        delay(50);
        tranhvatcan1(); // tiep tuc di thang
        }
        else{ 
          dilui();
          delay(50);
          return tranhvatcan2();
        }
      }
    if(max_distance == distance_right){
      if(max_distance > allow_distance){
        rephai();
        delay(50);
        tranhvatcan1();
        }
      else{
        dilui();
        delay(50);
        return tranhvatcan2();
        }
      }
  }
void tranhvatcan1(){ 
  int distance_1=distance_cm(90);
  if(distance_1 > allow_distance){
    dithang();
    delay(10);
    }
  else{
    dungyen();
    delay(100);
    tranhvatcan2();
    }
  }
void loop()
{ 
  tranhvatcan1();

}
