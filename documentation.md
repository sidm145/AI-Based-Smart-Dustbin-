# Smart Dustbin Project: Documentation

## Overview
This project combines Computer Vision and physical sensors to classify and sort waste into three categories: Wet, Dry, and Metal. It utilizes an ESP32-CAM and a YOLOv8 object detection model on a PC for initial visual detection, followed by an Arduino-based continuous conveyor belt sorting mechanism relying on IR, Moisture, and Metal sensors.

## Bill of Materials (BOM)
1. **Arduino Microcontroller** (e.g., Uno or Nano)
2. **ESP32-CAM Module** (For initial object detection and streaming)
3. **PC/Laptop** (Running Command Prompt, Python, and YOLOv8)
4. **Conveyor Belt Assembly**
5. **DC Motor** (To drive the conveyor belt)
6. **L298N Motor Driver** (To control the DC motor)
7. **Servo Motor** (To route waste into corresponding bins)
8. **IR Sensor Module** (For physical hardware trigger)
9. **Inductive Proximity Sensor** (For metal detection)
10. **Moisture Sensor** (For wet waste detection)
11. **1.3" OLED Display** (SH1106 I2C, for status readout)
12. **Power Supply** (Adequate voltage/current for Arduino, motors, and ESP32-CAM)
13. **Jumper Wires & Breadboard**

## Pin Connections & Hardware Setup

### Arduino Connections
| Component | Arduino Pin | Notes |
| :--- | :--- | :--- |
| **IR Sensor** | D2 | Obstacle detection / Hardware trigger (Active LOW) |
| **Metal Sensor** | D3 | Inductive Proximity output (Active HIGH) |
| **Moisture Sensor**| D4 | Digital output for wetness (Active LOW) |
| **Motor Driver IN1**| D7 | Controls conveyor direction |
| **Motor Driver IN2**| D8 | Controls conveyor direction |
| **Servo Motor** | D9 | PWM Signal Pin |
| **OLED Display** | A4 (SDA), A5 (SCL)| I2C Data lines (Check specific board pins) |

### ESP32-CAM & PC Setup
* The **ESP32-CAM** is positioned to face the starting point of the conveyor belt. It streams video data to the PC over Wi-Fi (or Serial/USB if using an FTDI programmer).
* The PC runs a **Python script with YOLOv8** via the command prompt to perform initial object detection (identifying if an object is present on the belt and predicting its category: Wet, Dry, or Metal).
* *(Note: The primary physical sorting logic in the Arduino code relies on local hardware sensors to verify and execute the sorting. The PC vision system acts as the initial "smart" detection layer).*

## System Workflow

1. **Phase 1: Initial Vision Detection (YOLOv8 + ESP32-CAM)**
   - The ESP32-CAM streams the conveyor belt environment to the PC.
   - The YOLOv8 model (running in the command prompt via Python) detects a piece of waste being placed on the belt and performs preliminary categorization.

2. **Phase 2: Hardware Trigger (IR Sensor)**
   - As the waste moves down the running conveyor belt, it breaks the beam of the IR Sensor (Pin 2 goes LOW).
   - The Arduino immediately triggers the `processCycle()` and stops the conveyor belt to create a stable scanning window.

3. **Phase 3: Material Scanning (2 Seconds)**
   - **Moisture Scan (1s):** The Arduino monitors the Moisture Sensor for 1 second. If moisture is detected, the `foundWet` flag is set to true.
   - **Metal Scan (1s):** The Arduino monitors the Inductive Proximity Sensor for 1 second. If metal is detected, the `foundMetal` flag is set to true.

4. **Phase 4: Decision Logic (Intelligent Default)**
   - **Metal Priority:** If metal was found, the target category is `METAL` (Servo angle: 180°).
   - **Wet Priority:** If wet was found, the target category is `WET` (Servo angle: 120°).
   - **Default to Dry:** If *neither* metal nor wet was found, the system intelligently defaults to `DRY` (Servo angle: 60°). 

5. **Phase 5: Sorting and Resumption**
   - The Arduino updates the OLED display with the chosen category.
   - The Servo Motor rotates to the target angle to route the waste into the proper bin.
   - After a brief pause (500ms) to allow the servo to move, the Arduino restarts the conveyor belt.

6. **Phase 6: Clearance and Reset**
   - The Arduino enters a safety loop, waiting for the waste to completely clear the IR sensor path. This prevents the system from endlessly triggering on the exact same piece of trash.
   - Once clear, the code exits the cycle. The conveyor belt remains **ON**, and the system returns to its idle state, continuously running until the next object breaks the IR sensor beam.
