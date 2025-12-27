#include <LiquidCrystal.h>
const int ENA = 3;
const int ENB = 11;
const int IN1 = 12;
const int IN2 = 13;
const int IN3 = 1;
const int IN4 = 2;
const int leftIR = A1 ;
const int rightIR = A2 ;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int leftEncoderA = 18;
const int leftEncoderB = 19;
const int rightEncoderA = 20;
const int rightEncoderB = 21;
volatile long leftEncoderCount = 0;
volatile long rightEncoderCount = 0;
const float WHEEL_DIAMETER = 6.5;
const float ENCODER_RESOLUTION = 12;
int straightLeftSpeed = 65;
int straightRightSpeed = 65;
int turnSlowSpeed = 255;
int turnFastSpeed = 255;
const int TARGET_DISTANCE = 470;
const int RESTART_SPEED = 200;
const int RESTART_STOP_TIME = 7;
unsigned long startTime = 0;
float totalDistance = 0;
bool isDistanceStopped = false;
bool isRestarting = false;
unsigned long restartTime = 0;
bool hasCompletedStop = false;
unsigned long restartCountTime = 0;
bool isRestartCounting = false;
bool hasRestartStopped = false;
bool isRestartFinished = false;
bool isTimeFrozen = false;
float frozenRunTime = 0.0;
void setup() {
pinMode(ENA, OUTPUT);pinMode(ENB, OUTPUT);
pinMode(IN1, OUTPUT);pinMode(IN2, OUTPUT);
pinMode(IN3, OUTPUT);pinMode(IN4, OUTPUT);
pinMode(leftIR, INPUT);pinMode(rightIR, INPUT);
pinMode(leftEncoderA, INPUT_PULLUP);
pinMode(leftEncoderB, INPUT_PULLUP);
pinMode(rightEncoderA, INPUT_PULLUP);
pinMode(rightEncoderB, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(leftEncoderA), countLeftEncoder, RISING);
attachInterrupt(digitalPinToInterrupt(rightEncoderA), countRightEncoder, RISING);
lcd.begin(16, 2);
lcd.print("Time:0s Dist:0cm");
lcd.setCursor(0, 1);
lcd.print("IR:L0 R0 Mode:--");
startTime = millis();
}
void loop() {
unsigned long currentTime = millis();
float runTime = isTimeFrozen ? frozenRunTime : (currentTime - startTime) / 1000.0;
float wheelCircumference = WHEEL_DIAMETER * PI;
float leftDistance = (leftEncoderCount / ENCODER_RESOLUTION) * wheelCircumference;
float rightDistance = (rightEncoderCount / ENCODER_RESOLUTION) * wheelCircumference;
totalDistance = (leftDistance + rightDistance) / 2;
if (!isDistanceStopped && !hasCompletedStop) {
if (totalDistance >= TARGET_DISTANCE) {
stopCar();
lcd.setCursor(12, 1);lcd.print("Stop");
frozenRunTime = runTime;
isTimeFrozen = true;
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
isRestartFinished = true;
restartCountTime = currentTime;
isRestartCounting = true;
}
}
if (isRestartCounting && !hasRestartStopped && isRestartFinished) {
float restartRunTime = (currentTime - restartCountTime) / 1000.0;
if (restartRunTime >= RESTART_STOP_TIME) {
stopCar();
lcd.setCursor(12, 1);lcd.print("R-Stop");
hasRestartStopped = true;
isRestartCounting = false;
}
}
int leftIRVal = digitalRead(leftIR);
int rightIRVal = digitalRead(rightIR);
lcd.setCursor(5, 0);lcd.print(runTime, 1);lcd.print("s ");
lcd.setCursor(11, 0);lcd.print((int)totalDistance);lcd.print("cm");
lcd.setCursor(3, 1);lcd.print(leftIRVal);
lcd.setCursor(6, 1);lcd.print(rightIRVal);
lcd.setCursor(12, 1);lcd.print(" ");
lcd.setCursor(12, 1);
if (!hasRestartStopped) {
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
}
void countLeftEncoder() {
leftEncoderCount++;
}
void countRightEncoder() {
rightEncoderCount++;
}
void goStraight(int leftSpd, int rightSpd) {
digitalWrite(IN1, HIGH);digitalWrite(IN2, LOW);analogWrite(ENA, leftSpd);
digitalWrite(IN3, HIGH);digitalWrite(IN4, LOW);analogWrite(ENB, rightSpd);
}
void turnLeft() {
digitalWrite(IN1, LOW);digitalWrite(IN2, HIGH);analogWrite(ENA, turnSlowSpeed);
digitalWrite(IN3, HIGH);digitalWrite(IN4, LOW);analogWrite(ENB, turnFastSpeed);
}
void turnRight() {
digitalWrite(IN1, HIGH);digitalWrite(IN2, LOW);analogWrite(ENA, turnFastSpeed);
digitalWrite(IN3, LOW);digitalWrite(IN4, HIGH);analogWrite(ENB, turnSlowSpeed);
}
void stopCar() {
analogWrite(ENA, 0);analogWrite(ENB, 0);
digitalWrite(IN1, LOW);digitalWrite(IN2, LOW);
digitalWrite(IN3, LOW);digitalWrite(IN4, LOW);
}