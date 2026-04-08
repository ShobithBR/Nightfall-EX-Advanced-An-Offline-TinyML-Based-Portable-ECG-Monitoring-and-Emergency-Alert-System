# Nightfall-EX-Advanced-An-Offline-TinyML-Based-Portable-ECG-Monitoring-and-Emergency-Alert-System

Portable offline ECG monitor with TinyML arrhythmia detection, GPS tracking, and GSM emergency SMS alerts. Built on ESP32. No internet required.

---

## 1. The Problem

Most existing ECG monitoring systems depend on smartphones, cloud servers, or stable internet connectivity to function. This makes them completely unreliable in:

- Rural and remote areas where network coverage is inconsistent
- Emergency situations where the patient is unconscious
- Environments where smartphones or Wi-Fi infrastructure is unavailable

Nightfall-EX Advanced was built specifically to fill this gap. The entire system — acquisition, analysis, and alerting — operates independently without any external dependency.

---

## 2. Key Features

- Fully offline operation — no internet, no smartphone, no cloud at any point
- Real-time on-device arrhythmia detection using a quantized TinyML model (Edge Impulse)
- Trained on the **MIT-BIH Arrhythmia Database** — 93.4% classification accuracy
- Automatic emergency SMS with embedded GPS coordinates via SIM800L cellular network
- Average inference latency of **18–22 ms** with RAM usage consistently below 120 kB
- Emergency SMS delivered within **8–15 seconds** after arrhythmia detection
- Live P-Q-R-S ECG waveform visualization on a 0.96" OLED display
- Time-stamped ECG data logging to MicroSD card for later medical review
- Manual emergency button — single press = vitals SMS, double press = full emergency SMS
- Dual 18650 Li-ion battery system for isolated, stable, long-duration operation
- Compact two-layer PCB design (15cm × 10cm, 5.5cm height)

---

## 3. System Performance

| Metric | Result |
| :--- | :--- |
| Overall Classification Accuracy | **93.4%** |
| Precision (Abnormal Detection) | **92.1%** |
| Recall (Abnormal Detection) | **91.3%** |
| Average Inference Latency | **18–22 ms per frame** |
| Peak RAM Usage | **< 120 kB** |
| SMS Delivery Time | **8–15 seconds** |
| GPS Satellite Lock Time | **10–20 seconds** |
| GPS Positioning Accuracy | **2–5 meters outdoors** |

---

## 4. Hardware Components

| Component | Role in System |
| :--- | :--- |
| **ESP32 DevKitC** | Central microcontroller — ADC sampling, EMA filtering, TinyML inference, UART control |
| **AD8232 ECG Module** | Captures cardiac biopotential signals via 3 body electrodes (RA, LA, RL) |
| **SIM800L GSM Module** | Sends emergency SMS alerts via cellular network — works with zero internet |
| **NEO-7M GPS Module** | Provides real-time latitude/longitude via NMEA protocol |
| **0.96" I2C OLED (SSD1306)** | Displays live scrolling ECG waveform, BPM, GPS status, system condition |
| **MicroSD Card Module** | Logs time-stamped ECG readings, classifications, and alert events via SPI |
| **18650 Li-ion Battery x2** | Battery 1 → ESP32, AD8232, OLED, MicroSD. Battery 2 → GPS, SIM800L |
| **2200µF Electrolytic Capacitor** | Stabilizes SIM800L VCC rail during 2A SMS transmission current spikes |
| **200kΩ + 100kΩ Resistors** | Voltage divider for battery level monitoring on GPIO35 |
| **Tactile Push Button** | Manual SOS trigger with single and double press detection |
| **Power Switch + LED Indicator** | Device on/off control and power status indication |

---

## 5. Pin Connections (ESP32 DevKitC)

### AD8232 ECG Sensor

| AD8232 Pin | ESP32 Pin | Description |
| :--- | :--- | :--- |
| VCC | 3.3V | Power from ESP32 onboard regulator |
| GND | GND | Common ground |
| OUTPUT | GPIO 34 | Analog ECG signal input (ADC) |
| LO+ | GPIO 32 | Leads-off detection positive |
| LO- | GPIO 33 | Leads-off detection negative |
| RL | GND | Right leg drive reference |

### NEO-7M GPS Module

| GPS Pin | Connection | Description |
| :--- | :--- | :--- |
| VCC | Battery 2 (3.7V) | Direct from Battery 2 |
| GND | GND | Common ground |
| TX | GPIO 16 | Serial1 RX on ESP32 |
| RX | GPIO 17 | Serial1 TX on ESP32 |

### SIM800L GSM Module

| GSM Pin | Connection | Description |
| :--- | :--- | :--- |
| VCC | Battery 2 (3.7V) | Direct from Battery 2 + 2200µF cap |
| GND | GND | Common ground |
| TX | GPIO 26 | Serial2 RX on ESP32 |
| RX | GPIO 27 | Serial2 TX on ESP32 |

### 0.96" OLED Display (I2C)

| OLED Pin | ESP32 Pin |
| :--- | :--- |
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### MicroSD Card Module (SPI)

| SD Pin | ESP32 Pin |
| :--- | :--- |
| CS | GPIO 5 |
| MOSI | GPIO 23 |
| MISO | GPIO 19 |
| SCK | GPIO 18 |

### Other Connections

| Component | ESP32 Pin |
| :--- | :--- |
| Tactile Push Button | GPIO 12 (INPUT_PULLUP) |
| Battery ADC (200kΩ → Node A → 100kΩ → GND) | GPIO 35 |

---

## 6. Power Architecture

The system uses two separate 18650 Li-ion batteries (3.7V each) to isolate 
the power-hungry GSM module from the core processing unit.

**Battery 1** powers the ESP32 via its VIN pin. The ESP32 onboard 3.3V 
regulator then supplies the AD8232 ECG sensor, 0.96" OLED display, and 
MicroSD card module.

**Battery 2** directly powers the NEO-7M GPS module and the SIM800L GSM 
module. A 2200µF electrolytic capacitor is placed across the SIM800L VCC 
and GND pins to absorb the 2A current spike during SMS transmission and 
prevent voltage brownout on the ESP32.

```text
Battery 1 (18650 Li-ion 3.7V)
    ├── ESP32 VIN
    └── ESP32 3.3V regulator
            ├── AD8232 VCC
            ├── OLED VCC
            └── MicroSD VCC

Battery 2 (18650 Li-ion 3.7V)
    ├── NEO-7M GPS VCC
    └── SIM800L VCC
            └── 2200µF capacitor across VCC & GND
```

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
│   ├── Top view of the assembled prototype.jpg
│   ├── Side view showing stacked PCB structure.jpg
│   ├── Emergency SMS with Location and BPM.jpg
│   ├── Example of SMS alert received on mobile phone.jpg
│   ├── GPS coordinates received during testing.jpg
│   ├── ECG waveform output displayed on OLED screen
│   ├── ECG Signal Feature Extraction
│   ├── Model Performance & Accuracy Metrics
│   ├── Feature Distribution & Data Explorer
│   └── ECG Raw Data Visualization (Edge Impulse).jpg
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
