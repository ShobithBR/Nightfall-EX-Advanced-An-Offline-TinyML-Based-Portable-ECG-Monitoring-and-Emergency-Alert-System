# Nightfall-EX-Advanced-An-Offline-TinyML-Based-Portable-ECG-Monitoring-and-Emergency-Alert-System

Portable offline ECG monitor with TinyML arrhythmia detection, GPS tracking, and GSM emergency SMS alerts. Built on ESP32. No internet required.


> This dual-battery isolation ensures that SIM800L current surges
> during SMS transmission do not cause the ESP32 to reset or
> drop the ECG signal mid-acquisition.

---

## 7. System Architecture Diagrams

| Diagram | File |
| :--- | :--- |
| Block Diagram | `diagrams/block_diagram.png` |
| Circuit Schematic | `diagrams/circuit_schematic.png` |
| System Flowchart | `diagrams/architecture_flowchart.png` |
| Device Blueprint | `diagrams/device_blueprint.png` |

---

## 8. How the System Works

### 8.1 Signal Acquisition and Filtering

1. The AD8232 captures the cardiac biopotential signal through three electrodes placed on the body (RA, LA, RL).
2. The analog output is sampled at approximately 125Hz via the ESP32 ADC on GPIO34.
3. Leads-off detection on GPIO32 and GPIO33 monitors electrode contact status continuously.
4. The raw signal passes through an **EMA filter** (`α = 0.2`) in real time to remove motion artifacts and baseline drift.
5. Filtered samples are pushed into a circular buffer and rendered as a live scrolling waveform on the OLED.

### 8.2 TinyML Arrhythmia Detection

1. Filtered ECG samples are simultaneously fed into the **Edge Impulse inference engine** on the ESP32.
2. The quantized **1D-CNN + Fully Connected** model, trained on the MIT-BIH Arrhythmia Database, classifies each window as `Normal` or `Abnormal`.
3. If the `Abnormal` class confidence exceeds the threshold, the emergency workflow triggers.
4. Inference completes in **18–22 ms per frame** without interrupting display or logging.

### 8.3 Emergency Alert Workflow

1. Arrhythmia detected → ESP32 queries NEO-7M GPS for current coordinates.
2. Emergency SMS payload compiled:
   - Patient name and phone number
   - GPS coordinates with Google Maps link
   - Current BPM and detected condition
3. SIM800L sends the SMS using `AT+CMGF` and `AT+CMGS` AT commands.
4. OLED updates to display `Alert Sent`.
5. Event logged with timestamp to MicroSD card.
6. Entire alert cycle completes within **8–15 seconds**.

### 8.4 Button Controls

| Button Action | Response |
| :--- | :--- |
| Single press | Sends immediate location + vitals SMS |
| Double press | Sends full emergency ambulance SMS |

### 8.5 Data Logging

Every ECG reading, model classification, and alert event is recorded to the MicroSD card with a timestamp as a plain readable log file — no app or internet needed to access it.

---

## 9. TinyML Model Performance

Trained using **Edge Impulse** on the MIT-BIH Arrhythmia Database, deployed as a quantized C inference library on the ESP32.

| Test Sample | Input Condition | Model Output | Confidence | Result |
| :--- | :--- | :--- | :--- | :--- |
| Sample 1 | Normal Rhythm | Normal | 94% | Correct |
| Sample 2 | Slight Irregularity | Abnormal | 91% | Correct |
| Sample 3 | Sudden Peak / Noise | Normal | 88% | Correct (filtered) |
| Sample 4 | Arrhythmic Pattern | Abnormal | 96% | Correct |

### Sample Data Log File

| Time | ECG Value | Classification | GPS Status |
| :--- | :--- | :--- | :--- |
| 10:01:32 | 83 | Normal | Locked |
| 10:02:10 | 214 | Abnormal | Locked |
| 10:02:11 | — | SMS Sent | Locked |

---

## 10. Project Structure

```text
nightfall-ex-advanced/
├── code/
│   └── nightfall_ex_advanced.ino
├── diagrams/
│   ├── block_diagram.png
│   ├── circuit_schematic.png
│   ├── architecture_flowchart.png
│   └── device_blueprint.png
├── images/
│   ├── oled_ecg_waveform.jpg
│   ├── ecg_testing_volunteers.jpg
│   ├── gps_coordinates_test.jpg
│   ├── sms_alert_screenshot.jpg
│   ├── pcb_side_view.jpg
│   └── pcb_top_view.jpg
├── docs/
│   └── MINI_PROJECT_REPORT.pdf
└── README.md
```

---

## 11. How to Reproduce This Project

1. **Install Arduino IDE** and add the ESP32 board package from Espressif.
2. **Install required libraries** via Arduino Library Manager:
   - `Adafruit GFX Library`
   - `Adafruit SSD1306`
   - `TinyGPSPlus`
   - Your Edge Impulse exported inference library
3. **Train your TinyML model:**
   - Create a free account at [edgeimpulse.com](https://edgeimpulse.com)
   - Upload ECG segments (normal and abnormal) for training
   - Select a 1D-CNN architecture, train, and export as an Arduino library
   - Place the exported library folder inside your Arduino `libraries/` directory
4. **Update the firmware:**
   - Open `nightfall_ex_advanced.ino` in Arduino IDE
   - Replace the emergency phone number inside `AT+CMGS` with your actual registered number
5. **Wire all hardware** according to the pin connection tables in Section 5.
6. **Connect the two batteries** as described in the Power Architecture section.
7. **Upload the sketch:** Select `ESP32 Dev Module` from the Boards menu and upload.
8. **Attach ECG electrodes** to the body — RA (right arm), LA (left arm), RL (right leg/ankle).
9. **Power on** and confirm the OLED shows the live ECG waveform and BPM.
10. **Test the alert:** Single-press the button to verify the SIM800L sends an SMS to your registered number.

>  Disclaimer: Nightfall-EX Advanced is an academic research prototype. It has not been clinically validated and is not a certified medical device. Do not use it as a substitute for professional medical equipment or clinical diagnosis.

---

## 12. Comparison with Existing Systems

| Feature | Cloud/Smartphone Systems | Nightfall-EX Advanced |
| :--- | :--- | :--- |
| Internet Required | Yes | No |
| Smartphone Required | Yes | No |
| On-Device AI | No | Yes (TinyML) |
| Autonomous Emergency Alert | No | Yes (GSM SMS) |
| GPS Location in Alert | Rarely | Yes (always) |
| Works When Patient Unconscious | No | Yes |
| Rural/Remote Usability | Poor | Excellent |

---

## 13. Possible Improvements

- Expand TinyML model to classify specific arrhythmia types (AFib, VTach, Bradycardia)
- Add multi-lead ECG support for higher diagnostic accuracy
- Conduct full clinical validation on a larger patient dataset
- Develop a companion mobile app for log review and device configuration
- Miniaturize to a custom wrist-worn PCB form factor
- Add solar charging and ultra-low-power sleep modes for extended field deployment

---

## 14. Applications

- Rural and remote healthcare centers with limited infrastructure
- Elderly patient monitoring for individuals living alone
- Disaster relief and field medical emergency operations
- Ambulances and first-aid units for early ECG screening during transport
- Military and remote workforce cardiac safety monitoring

---

## 15. References

1. V. K. et al., "Artificial Intelligence Enhanced Portable ECG Monitor," IEEE ICCCNP, 2025.
2. M. H. Abid and I. A. Talin, "IoT-Based ECG Monitoring System on Smartphones," IEEE iCACCESS, 2024.
3. A. Vishwanatham et al., "Smart and Wearable ECG Monitoring System as a POC Device," IEEE ANTS, 2018.
4. M. Garcia and S. Kumar, "AI-Based Detection for Remote ECG Monitoring System," IEEE AIIoT, 2024.
5. S. Jalaja et al., "Smart Health IoT Monitoring for Real-Time Arrhythmia Detection Using AI," IEEE ICDSBS, 2025.

For the complete list of 11 referenced papers, see `docs/MINI_PROJECT_REPORT.pdf`.

---

## 16. Documentation

For complete hardware methodology, TinyML training pipeline, circuit schematics, and experimental results:
👉 [View Full Project Report (PDF)](./docs/MINI_PROJECT_REPORT.pdf)
