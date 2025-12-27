#include <Wire.h>
#include <LiquidCrystal.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

const int ENA = 3;    
const int ENB = 11;   
const int IN1 = 12;   
const int IN2 = 13;  
const int IN3 = 1;    
const int IN4 = 2;    
const int MPU_INT_PIN = A2;


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;


const int DRIVE_SPEED = 165;       
const int ROTATE_SPEED = 150;      
const int DOWN_SPEED = 170;          
const float TOP_ANGLE_MIN = -10.0;    
const float TOP_ANGLE_MAX = 10.0;
const unsigned long TOP_STABLE_DUR = 250; 
const unsigned long STOP_DURATION = 4000; 
const float ROTATE_ANGLE = 350.0;    
const unsigned long ROTATE_MAX_TIME = 2700; 
float currentSlope = 0.0;            
float rotateTotal = 0.0;           
unsigned long rotateStartTime = 0;   


bool hasStartedDriving = true;      
bool isStopped = false;             
bool isSlopeFixed = false;          
bool isCountingDown = false;       
bool isRotating = false;             
bool hasRotatedOnce = false;      
bool isGoingDown = false;            
float fixedSlopeVal = 0.0;         
const unsigned long STABLE_DUR = 100; 
const float SLOPE_FLUCT = 3.0;      
unsigned long countDownStart = 0;    

void setup() {
 
  pinMode(ENA, OUTPUT);pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);pinMode(IN4, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("State:Climbing");
  lcd.setCursor(0, 1);
  lcd.print("Slope:0.0 Deg");

  pinMode(MPU_INT_PIN, INPUT_PULLUP);
  if (!mpu.begin()) {
    lcd.clear();
    lcd.print("MPU Err! Check");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(200);
  driveForward(DRIVE_SPEED);
}

void loop() {
  mpu.getEvent(&a, &g, &temp);
  calculateSlope();
  calculateRotateAngle();

  if (!isSlopeFixed) {
    static unsigned long stableStart = 0;
    static float slopeMin = 100.0, slopeMax = -100.0;
    
    if (currentSlope < slopeMin) slopeMin = currentSlope;
    if (currentSlope > slopeMax) slopeMax = currentSlope;
    
    if (stableStart == 0) stableStart = millis();
    
    if (millis() - stableStart >= STABLE_DUR) {
      if (slopeMax - slopeMin <= SLOPE_FLUCT) {
        isSlopeFixed = true;
        fixedSlopeVal = (slopeMin + slopeMax) / 2.0;
      } else {
        stableStart = 0;
        slopeMin = 100.0;
        slopeMax = -100.0;
      }
    }
  }

  if (hasStartedDriving && !isStopped && !isCountingDown && !isRotating && !isGoingDown) {
    driveForward(DRIVE_SPEED); 
    
    static unsigned long topDetectTime = 0;
    if (currentSlope >= TOP_ANGLE_MIN && currentSlope <= TOP_ANGLE_MAX) {
      if (topDetectTime == 0) topDetectTime = millis();
      if (millis() - topDetectTime >= TOP_STABLE_DUR) {
        stopCar();
        isStopped = true;
        isCountingDown = true;
        countDownStart = millis();
        lcd.setCursor(0, 0);lcd.print("State:Stop 4s ");
      }
    } else {
      topDetectTime = 0;
    }
  }
  else if (isCountingDown) {
    unsigned long elapsed = millis() - countDownStart;
    unsigned long remaining = STOP_DURATION - elapsed;
    
    lcd.setCursor(0, 0);
    lcd.print("Remain:");
    lcd.print(remaining / 1000);
    lcd.print("s       ");
    
    if (elapsed >= STOP_DURATION) {
      isCountingDown = false;
      isRotating = true;
      rotateTotal = 0.0;
      rotateStartTime = millis(); 
      lcd.setCursor(0, 0);lcd.print("State:Rotating");
    }
  }
  else if (isRotating && !hasRotatedOnce) {
    rotate360();
    if (rotateTotal >= ROTATE_ANGLE || millis() - rotateStartTime >= ROTATE_MAX_TIME) {
      stopCar();
      isRotating = false;
      hasRotatedOnce = true;
      isGoingDown = true;
      lcd.setCursor(0, 0);lcd.print("State:Going Down");
    }
  }
  else if (isGoingDown) {
    driveForward(DOWN_SPEED); 
    
    static unsigned long downStopTime = 0;
    if (currentSlope >= TOP_ANGLE_MIN && currentSlope <= TOP_ANGLE_MAX) {
      if (downStopTime == 0) downStopTime = millis();
      if (millis() - downStopTime >= TOP_STABLE_DUR) {
        stopCar();
        isGoingDown = false;
        lcd.setCursor(0, 0);lcd.print("State:All Done ");
      }
    } else {
      downStopTime = 0;
    }
  }

  lcd.setCursor(0, 1);
  lcd.print("Slope:");
  if (isSlopeFixed) {
    lcd.print(fixedSlopeVal, 1);
  } else {
    lcd.print(currentSlope, 1);
  }
  lcd.print(" Deg   ");
}

void calculateSlope() {
  float ax = a.acceleration.x;
  float az = a.acceleration.z;
  float rawSlope = atan2(ax, az) * (180.0 / PI);
  static float lastSlope = 0.0;
  currentSlope = (rawSlope * 0.2) + (lastSlope * 0.8);
  lastSlope = currentSlope;
}

void calculateRotateAngle() {
  float gyroZ = g.gyro.z;
  static unsigned long lastRotateTime = 0; 
  unsigned long currentTime = millis();
  float dt = (currentTime - lastRotateTime) / 1000.0;
  
  if (isRotating && !hasRotatedOnce) {
    rotateTotal += gyroZ * dt;
    rotateTotal = max(rotateTotal, 0.0); 
    if (rotateTotal > 360.0) rotateTotal = 360.0;
  }
  lastRotateTime = currentTime;
}
void driveForward(int speed) {
  digitalWrite(IN1, HIGH);digitalWrite(IN2, LOW); 
  digitalWrite(IN3, HIGH);digitalWrite(IN4, LOW);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

void rotate360() {
  digitalWrite(IN1, LOW);digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);digitalWrite(IN4, LOW);
  analogWrite(ENA, ROTATE_SPEED);
  analogWrite(ENB, ROTATE_SPEED);
}


void stopCar() {
  analogWrite(ENA, 0);analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);digitalWrite(IN4, LOW);
}