import cv2
from ultralytics import YOLO

# Load the YOLOv8 model (yolov8n is the nano model, fast and good for realtime)
model = YOLO('yolov8n.pt') 

# Replace with the IP address printed on your ESP32-CAM Serial Monitor
ESP32_CAM_URL = "http://192.168.1.100" 

# Open the video stream
cap = cv2.VideoCapture(ESP32_CAM_URL)

if not cap.isOpened():
    print("Error: Could not open the video stream. Check your ESP32-CAM IP address.")
    exit()

print("Starting Human Detection... Press 'q' to quit.")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame. Retrying...")
        continue

    # Run YOLOv8 inference on the frame
    # classes=0 filters the detection to only 'person' (class 0 in COCO dataset)
    results = model(frame, classes=0, conf=0.5) 

    # Visualize the results on the frame
    annotated_frame = results[0].plot()

    # If humans are detected, print an alert
    num_persons = len(results[0].boxes)
    if num_persons > 0:
        print(f"ALERT: {num_persons} Human(s) Detected at the Border!")

    # Display the annotated frame
    cv2.imshow("ESP32-CAM YOLOv8 Human Detection", annotated_frame)

    # Press 'q' to exit the loop
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Clean up
cap.release()
cv2.destroyAllWindows()
