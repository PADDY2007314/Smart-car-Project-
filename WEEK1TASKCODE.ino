#include <LiquidCrystal.h>  
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int ENA = 3;
const int ENB = 11;
const int IN1 = 12;
const int IN2 = 13;
const int IN3 = 1;
const int IN4 = 2;
unsigned long startTime;
bool isMoving = false;
void setup() {
 pinMode(ENA, OUTPUT);
 pinMode(ENB, OUTPUT);
 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);
 
 Serial.begin(9600);
 Serial.println("Car ready to move forward for 10 seconds.");
}
void loop() {
 if (!isMoving) {
 carForward();
 startTime = millis();
 isMoving = true;
 Serial.println("Start moving forward. Time elapsed (seconds):");
 }
 unsigned long elapsedTime = (millis() - startTime) / 1000;
 static unsigned long lastPrintTime = 0;
 if (isMoving && (millis() - lastPrintTime >= 1000)) {
 Serial.print("Elapsed: ");
 Serial.print(elapsedTime);
 Serial.println("s");
 lastPrintTime = millis();
 }
 if (isMoving && elapsedTime >= 10) {
 carStop();
 Serial.println("10 seconds reached! Car stopped.");
 isMoving = false;
 while (true);
 }
}
void carForward() {
 digitalWrite(ENA, HIGH);
 digitalWrite(IN1, HIGH);
 digitalWrite(IN2, LOW);
 
 digitalWrite(ENB, HIGH);
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4, LOW);
}
void carStop() {
 digitalWrite(ENA, LOW);
 digitalWrite(ENB, LOW);
}