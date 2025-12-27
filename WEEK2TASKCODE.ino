#include <LiquidCrystal.h>
const int ENA = 3;
const int ENB = 11;
const int IN1 = 12;
const int IN2 = 13;
const int IN3 = 1;
const int IN4 = 2;
const int leftIR = A1;
const int rightIR = A2;
const int leftEncA = 5;
const int leftEncB = A3;
const int rightEncA = 6;
const int rightEncB = A4;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int straightLeftSpeed = 65;
int straightRightSpeed = 65;
int turnSlowSpeed = 255;
int turnFastSpeed = 255;
const float wheelDia = 6.5;
const float encPulse = 13;
volatile long leftEncCnt = 0;
volatile long rightEncCnt = 0;
float totalDistance = 0.0;
const int TARGET_DISTANCE = 470;
const int RESTART_SPEED = 150;
const int DISTANCE_TOLERANCE = 5;
unsigned long startTime = 0;
bool isDistanceStopped = false;
bool isRestarting = false;
unsigned long restartTime = 0;
bool hasCompletedStop = false;
void setup() {
pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);
pinMode(IN1, OUTPUT);
pinMode(IN2, OUTPUT);
pinMode(IN3, OUTPUT);
pinMode(IN4, OUTPUT);
pinMode(leftIR, INPUT);
pinMode(rightIR, INPUT);
pinMode(leftEncA, INPUT);
pinMode(leftEncB, INPUT);
pinMode(rightEncA, INPUT);
pinMode(rightEncB, INPUT);
attachInterrupt(digitalPinToInterrupt(leftEncA), LeftEncCount, RISING);
attachInterrupt(digitalPinToInterrupt(rightEncA), RightEncCount, RISING);
lcd.begin(16, 2);
lcd.print("Time:0s Dist:0cm");
lcd.setCursor(0, 1);
lcd.print("IR:L0 R0 Mode:--");
startTime = millis();
}
void loop() {
unsigned long currentTime = millis();
float runTime = (currentTime - startTime) / 1000.0;
CalculateDistance();
if (!isDistanceStopped && !hasCompletedStop) {
if ((digitalRead(IN1) != digitalRead(IN2)) || (digitalRead(IN3) != digitalRead(IN4))) {
totalDistance = ((abs(leftEncCnt) + abs(rightEncCnt)) / 2.0) / encPulse * PI * wheelDia;
}
if (totalDistance >= (TARGET_DISTANCE + DISTANCE_TOLERANCE)) {
stopCar();
lcd.setCursor(12, 1);lcd.print("Stop");
delay(2000);
hasCompletedStop = true;
isDistanceStopped = true;
isRestarting = true;
restartTime = currentTime;
}
}
int currentLeftSpeed = straightLeftSpeed;
int currentRightSpeed = straightRightSpeed;
if (isRestarting) {
float restartElapsed = (currentTime - restartTime) / 1000.0;
if (restartElapsed <= 1.0) {
currentLeftSpeed = RESTART_SPEED;
currentRightSpeed = RESTART_SPEED;
} else {
currentLeftSpeed = straightLeftSpeed;
currentRightSpeed = straightRightSpeed;
isRestarting = false;
}
}
int leftIRVal = digitalRead(leftIR);
int rightIRVal = digitalRead(rightIR);
lcd.setCursor(5, 0);lcd.print(runTime, 1);lcd.print("s ");
lcd.setCursor(11, 0);lcd.print(totalDistance, 1);lcd.print("cm");
lcd.setCursor(3, 1);lcd.print(leftIRVal);
lcd.setCursor(6, 1);lcd.print(rightIRVal);
lcd.setCursor(12, 1);lcd.print(" ");
lcd.setCursor(12, 1);
if (leftIRVal == 0 && rightIRVal == 0) {
goStraight(currentLeftSpeed, currentRightSpeed);
lcd.print("Go");
}
else if (leftIRVal == 1 && rightIRVal == 0) {
turnLeft();
lcd.print("Left");
}
else if (leftIRVal == 0 && rightIRVal == 1) {
turnRight();
lcd.print("Right");
}
}
void LeftEncCount() {
if (digitalRead(leftEncB) == HIGH) leftEncCnt++;
else leftEncCnt--;
}
void RightEncCount() {
if (digitalRead(rightEncB) == HIGH) rightEncCnt++;
else rightEncCnt--;
}
void CalculateDistance() {
if (!isDistanceStopped) {
totalDistance = ((abs(leftEncCnt) + abs(rightEncCnt)) / 2.0) / encPulse * PI * wheelDia;
}
}
void goStraight(int leftSpd, int rightSpd) {
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);
analogWrite(ENA, leftSpd);
digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
analogWrite(ENB, rightSpd);
}
void turnLeft() {
digitalWrite(IN1, LOW);
digitalWrite(IN2, HIGH);
analogWrite(ENA, turnSlowSpeed);
digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
analogWrite(ENB, turnFastSpeed);
}
void turnRight() {
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);
analogWrite(ENA, turnFastSpeed);
digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
analogWrite(ENB, turnSlowSpeed);
}
void stopCar() {
analogWrite(ENA, 0);
analogWrite(ENB, 0);
digitalWrite(IN1, LOW);
digitalWrite(IN2, LOW);
digitalWrite(IN3, LOW);
digitalWrite(IN4, LOW);
}