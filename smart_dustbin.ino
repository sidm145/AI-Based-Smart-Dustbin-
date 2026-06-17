/*
 * =====================================================
 * SMART DUSTBIN - CONTINUOUS FLOW WORKFLOW
 * =====================================================
 * 1. IR Trigger -> 1s Wet Scan -> 1s Metal Scan
 * 2. If neither -> Category is DRY
 * 3. Move Servo AND Start Belt
 * 4. Continuous rotation until NEXT IR trigger
 * =====================================================
 */

#include <Wire.h>
#include <U8g2lib.h>
#include <Servo.h>

U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define IR_SENSOR_PIN       2
#define METAL_PIN           3  
#define MOISTURE_PIN        4
#define SERVO_PIN           9
#define MOTOR_IN1           7
#define MOTOR_IN2           8

#define SERVO_HOME          0    
#define SERVO_DRY           60   
#define SERVO_WET           120  
#define SERVO_METAL         180  

Servo sorterServo;

void setup() {
  Serial.begin(9600);
  
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(METAL_PIN,     INPUT); 
  pinMode(MOISTURE_PIN,  INPUT);
  pinMode(MOTOR_IN1,     OUTPUT);
  pinMode(MOTOR_IN2,     OUTPUT);

  display.begin();
  sorterServo.attach(SERVO_PIN);
  sorterServo.write(SERVO_HOME);
  
  stopConveyor();
  updateDisplay("SYSTEM READY");
}

void loop() {
  // Wait for new waste to trigger the IR sensor
  if (digitalRead(IR_SENSOR_PIN) == LOW) {
    processCycle();
  }
}

void processCycle() {
  // 1. IR TRIGGERED: Stop belt immediately to scan
  stopConveyor();
  updateDisplay("IR TRIGGERED");
  bool foundMetal = false;
  bool foundWet = false;

  // 2. SCAN WINDOW 1: MOISTURE (1 Second)
  updateDisplay("SCANNING WET...");
  unsigned long startW = millis();
  while (millis() - startW < 1000) {
    if (digitalRead(MOISTURE_PIN) == LOW) foundWet = true; 
  }

  // 3. SCAN WINDOW 2: METAL (1 Second)
  updateDisplay("SCANNING METAL...");
  unsigned long startM = millis();
  while (millis() - startM < 1000) {
    if (digitalRead(METAL_PIN) == HIGH) foundMetal = true; 
  }

  // 4. LOGIC: DECIDE MATERIAL
  int targetAngle;
  const char* result;

  if (foundMetal) {
    result = "METAL";
    targetAngle = SERVO_METAL;
  } 
  else if (foundWet) {
    result = "WET";
    targetAngle = SERVO_WET;
  } 
  else {
    // If NOT metal and NOT wet, categorize as DRY
    result = "DRY";
    targetAngle = SERVO_DRY;
  }
  
  // 5. EXECUTION: Move Servo and Start Belt
  updateDisplay(result);
  sorterServo.write(targetAngle);
  delay(500); // Small pause to let servo start moving
  
  startConveyor();
  Serial.print("Sorting: ");
  Serial.println(result);

  // 6. CLEARANCE: Wait for trash to move away from IR sensor
  // This prevents the code from re-triggering on the same object
  while(digitalRead(IR_SENSOR_PIN) == LOW) {
    delay(10);
  }
  
  // EXIT: The belt remains ON and loop() continues.
}

// --- Motor Control ---
void startConveyor() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
}

void stopConveyor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}

// --- OLED Update ---
void updateDisplay(const char* msg) {
  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tf); 
    display.drawStr(25, 12, "SMART DUSTBIN");
    display.drawHLine(0, 16, 128);
    display.setFont(u8g2_font_8x13_tf); 

    int x = 20;
    if (strcmp(msg, "WET") == 0 || strcmp(msg, "DRY") == 0) x = 52;
    else if (strcmp(msg, "METAL") == 0) x = 45;

    display.drawStr(x, 42, msg);
  } while (display.nextPage());
}
