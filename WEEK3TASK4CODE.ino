#include <SoftwareSerial.h>
#define ENA 3    
#define ENB 11  
#define IN1 12  
#define IN2 13 
#define IN3 2   
#define IN4 A3   
#define BT_RX A5 
#define BT_TX A4 
SoftwareSerial btSerial(BT_RX, BT_TX);

char cmd; 

void setup() {
  Serial.begin(9600);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  btSerial.begin(9600);
  stopCar();
}

void loop() {
  if (btSerial.available() > 0) {
    cmd = btSerial.read();
    controlCar(cmd);  
  }
}
void controlCar(char c) {
  int speed = 220;  
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);

  switch(c) {
    case 'F': 
      forward();
      Serial.println("forward");
      break;
    case 'B':  
    Serial.println("back");
      backward();
      break;
    case 'L': 
      Serial.println("left");
      left();
      break;
    case 'R':
      Serial.println("right");
      right();
      break;
    case 'S': 
      Serial.println("stop");
      stopCar();
      break;
  }
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void right(){
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}