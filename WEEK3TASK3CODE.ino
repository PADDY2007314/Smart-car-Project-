 #include <LiquidCrystal.h>  
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int ENA = 3;   
const int ENB = 11;   
const int IN1 = 12;   
const int IN2 = 13;   
const int IN3 = A3;  
const int IN4 = 2;   
const int Trig = A2;  
const int Echo = A1;  
long dist;  
const int SAFE_DIST = 25; 
const int MOTOR_SPEED = 160; 
const int TURN_TIME = 700;   

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  lcd.begin(16, 2);
  lcd.print("Obstacle Dist:");
  analogWrite(ENA, MOTOR_SPEED);
  analogWrite(ENB, MOTOR_SPEED);
}

void loop() {
  dist = measureDistance();  
  updateLCD();               
  
  if (dist > SAFE_DIST) {  
    goForward();
  } else {                   
    goBackward(300);
    turnRight(TURN_TIME);   
  }
}
long measureDistance() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  return pulseIn(Echo, HIGH) * 0.034 / 2;
}

void updateLCD() {
  lcd.setCursor(0, 1);       
  lcd.print("                "); 
  lcd.setCursor(0, 1);
  lcd.print(dist);
  lcd.print(" cm");
}
void goForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void goBackward(int delayTime) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(delayTime);
}

void turnRight(int delayTime) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(delayTime);
}
void turnLeft(int delayTime) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(delayTime);
}