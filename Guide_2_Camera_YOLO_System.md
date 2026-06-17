# Phase 2: AI Border Security - Camera & YOLOv8 Detection Guide

## Overview
This subsystem uses an **ESP32-CAM** module to stream live video over a local Wi-Fi network. A PC running **Python** captures this MJPEG video stream and applies a deep-learning AI model (**YOLOv8**) to detect humans in real-time. This acts as the visual verification layer of the border security system, ensuring accurate human identification over the internet.

## Components Required
- **ESP32-CAM Module** (AI-Thinker model with OV2640 camera)
- **FTDI Programmer** (USB to TTL serial converter for programming)
- **Jumper Wires**
- **5V Power Supply** (For stable camera operation)
- **PC/Laptop** with Python installed

## Circuit Diagram / Wiring Connections (For Programming)
To upload code to the ESP32-CAM, you need an FTDI programmer.

| FTDI Programmer | ESP32-CAM |
|-----------------|-----------|
| 5V              | 5V        |
| GND             | GND       |
| TX              | U0R       |
| RX              | U0T       |

> **IMPORTANT**: To put the ESP32-CAM into programming flash mode, you MUST connect `GPIO 0` to `GND` before powering it on. After uploading the code, remove the jumper between `GPIO 0` and `GND` and press the Reset button on the ESP32-CAM board.

## Software Prerequisites (PC Setup)
1. **Install Python**: Ensure Python 3.8+ is installed on your computer.
2. **Install Required Libraries**: Open your Command Prompt (CMD) or PowerShell and run:
   ```bash
   pip install opencv-python ultralytics
   ```

## Workflow & Logic
1. **ESP32-CAM (IoT Node)**: Connects to the specified Wi-Fi network and starts an HTTP MJPEG server on port 80. It continuously captures frames from the camera and streams them.
2. **Python Script (AI Server)**: Uses OpenCV (`cv2.VideoCapture`) to connect to the ESP32-CAM's streaming IP address over the network.
3. **YOLOv8 AI Inference**: Ultralytics YOLOv8 processes each frame. We pass `classes=0` to filter the AI to only look for Persons/Humans in the COCO dataset.
4. **Alerting**: If a human is detected in the frame, bounding boxes are drawn on the stream display, and an alert is printed to the terminal console showing the number of detected individuals.

## Setup Instructions
### Step 1: Upload the ESP32-CAM Server
1. Open `ESP32_CAM_Server/ESP32_CAM_Server.ino` in the Arduino IDE.
2. **Edit the Wi-Fi credentials**: Replace `"YOUR_WIFI_SSID"` and `"YOUR_WIFI_PASSWORD"` with your actual network details.
3. Connect the FTDI programmer and ensure `GPIO 0` is connected to `GND`.
4. In the Arduino IDE, go to Tools -> Board -> "AI Thinker ESP32-CAM".
5. Click **Upload**.
6. Once done, remove the `GPIO 0` to `GND` jumper and press the RESET button on the ESP32-CAM.
7. Open the Serial Monitor (baud rate `115200`) to find the assigned IP address (e.g., `http://192.168.1.100`).

### Step 2: Run the AI Human Detection via CMD
1. Open the `YOLOv8_Human_Detection/detect_human.py` file in a text editor (like VS Code or Notepad).
2. Change the `ESP32_CAM_URL` variable to match the IP address printed on your Arduino Serial Monitor (e.g., `ESP32_CAM_URL = "http://192.168.1.100"`).
3. Open CMD or PowerShell, navigate to the `YOLOv8_Human_Detection` folder, and run the script:
   ```bash
   python detect_human.py
   ```
4. A window will open showing the live camera feed with boxes drawn around detected humans! The CMD will log alerts when individuals cross the camera's path. Press `q` to quit the stream.
