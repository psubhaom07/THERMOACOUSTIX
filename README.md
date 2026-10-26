# 📊 ThermoAcoustix: Edge-Based Anomaly Detection System

---

## 🔍 Overview
ThermoAcoustix is a low-cost, automated vibration-based material screening prototype designed to detect hidden internal defects, structural damage, and material variations beneath normal surfaces. Developed by team **Voltage Mafia** (Saksham Dhawan, Patnala Subhaom Kumar Patro, and Ayaan Garg) at VIT Vellore, the system converts physical vibration responses into measurable digital data and displays real-time signal analysis through graphical dashboards to automate the non-destructive testing (NDT) logic.

---

## 🎯 The Problem & Our Approach
* **The Challenge:** Identical external surfaces can mask internal structural differences across brackets, mechanical components, and material samples. Traditional inspection relies on subjective human evaluation (manual hammer strikes and auditory/visual checks), which suffers from high variability and limited automation.
* **Our Solution:** ThermoAcoustix bridges physical response with digital evidence by combining repeatable mechanical excitation with automated frequency-domain feature extraction and visual graph plots.

---

## ⚙️ System Architecture & Hardware Components
The prototype is orchestrated by an **ESP32** microcontroller working alongside precise sensors and electromechanical actuators:
* **ESP32 (Central Brain):** Handles core control, sensor data acquisition, system communication, user input handling, and test sequencing.
* **Servo Motor (Actuator):** Delivers controlled, repeatable mechanical strikes to eliminate manual testing variance.
* **ADXL345 Accelerometer:** Measures time-domain acceleration responses, transient amplitudes, oscillation behavior, and decay characteristics.
* **DHT11 Sensor:** Captures environmental temperature and humidity context for condition-aware calibration.
* **Potentiometer:** Provides an analog interface for test profile selection.
* **LCD Screen & Python Dashboard:** Acts as the human-machine interface (HMI) for real-time status displays, alerts, and **graphical frequency spectrum plots**.

---

## 🔄 Automated Test Workflow
The system executes a strict 13-step automated inspection loop:
1. Load Sample
2. Presence Check
3. Select Profile
4. Verify Chamber
5. Servo Strike
6. Capture Vibration
7. Log Environment
8. Filter + Window
9. Fast Fourier Transform (FFT)
10. Extract Features
11. Compare Baseline
12. Check Tolerance
13. Display + Graphical Plot Alert

---

## 📈 Signal Processing & Graph Visualization
* **Time-to-Frequency Domain:** Raw acceleration data undergoes digital filtering, windowing, and **Fast Fourier Transform (FFT)** analysis to expose spectral strengths.
* **Graphical Representation:** The processed data is pushed to a Python-linked interface, rendering clear graphical waveforms and frequency response charts so operators can visually inspect peak amplitudes, dominant frequencies, and anomaly deviations instantly.

---

## 🚀 Key Innovations
* **Controlled Excitations:** Automated mechanical strikes remove human variability.
* **Visual Digital Fingerprinting:** Converts physical vibration behavior into computable numerical features and live graphs.
* **Environmental Context Integration:** Logs ambient temperature and humidity data per test cycle.
* **Embedded Safety Interlocks:** Ensures physical chamber conditions securely gate the testing sequence.

---

## 🌐 Real-World Applications & Roadmap
* **Applications:** Manufacturing quality screening, machine condition monitoring, structural response research, small workshop NDT testing, and educational demonstrations.
* **Development Roadmap:** Progressing from foundational hardware sensing (Level 1) and optimized digital filtering/FFT (Level 2) toward real machine learning and TinyML edge inference on the ESP32 (Level 3), culminating in industrial-scale multi-sensor deployment and database dashboards (Level 4).
