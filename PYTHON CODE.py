import serial
import time
import numpy as np
import matplotlib.pyplot as plt

PORT = "COM5"
BAUD = 230400
SAMPLE_RATE = 100
N = 200 # Reduced from 500 to 200

print("Connecting to ESP32...")

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
except Exception as e:
    print("Could not open", PORT)
    print(e)
    exit()

time.sleep(2)
ser.reset_input_buffer()

print("Connected to:", PORT)
print()
input("Press ENTER to start the 2-second material test...")

ser.reset_input_buffer()
ser.write(b"T\n")
ser.flush()

samples = []
temperature = 0
humidity = 0
baseline_rms = 0
response_rms = 0
peak = 0
score = 0
status = ""

test_started = False
test_finished = False

last_data_time = time.time()

while not test_finished:
    if time.time() - last_data_time > 10:
        print()
        print("ERROR: ESP32 communication timed out.")
        break

    if ser.in_waiting > 0:
        line = ser.readline().decode("utf-8", errors="ignore").strip()
        
        if not line:
            continue

        last_data_time = time.time()

        if line == "TEST_START":
            test_started = True
            samples = []
            print()
            print("TEST STARTED")
            print("Servo test running for 2 seconds...")

        elif line.startswith("SENSOR"):
            parts = line.split(",")
            if len(parts) >= 3:
                try:
                    temperature = float(parts[1])
                    humidity = float(parts[2])
                except:
                    pass

        elif line.startswith("SAMPLE"):
            parts = line.split(",")
            if len(parts) >= 3:
                try:
                    value = float(parts[2])
                    samples.append(value)

                    if len(samples) % 50 == 0:
                        print("Samples received:", len(samples), "/", N)
                except:
                    pass

        elif line.startswith("RESULT"):
            parts = line.split(",")
            if len(parts) >= 6:
                try:
                    baseline_rms = float(parts[1])
                    response_rms = float(parts[2])
                    peak = float(parts[3])
                    score = int(parts[4])
                    status = parts[5]
                except:
                    pass

        elif line == "TEST_END":
            test_finished = True

ser.close()

print()
print("======================================")
print("DATA COLLECTION COMPLETE")
print("======================================")

print("Total samples received:", len(samples))

if len(samples) < N:
    print()
    print("ERROR: Not enough samples received.")
    print("Expected:", N)
    print("Received:", len(samples))
    print()
    print("Check:")
    print("1. Flash the updated ESP32 Arduino code.")
    print("2. Ensure Arduino Serial Monitor is CLOSED.")
    exit()

samples = np.array(samples[:N])
time_axis = np.arange(N) / SAMPLE_RATE

window = np.hanning(N)
windowed = samples * window
fft_values = np.fft.rfft(windowed)
frequencies = np.fft.rfftfreq(N, 1 / SAMPLE_RATE)

magnitude = np.abs(fft_values)
magnitude[0] = 0

dominant_index = np.argmax(magnitude)
dominant_frequency = frequencies[dominant_index]
fft_rms = np.sqrt(np.mean(samples ** 2))

response_mag = np.abs(response_fft)
if len(response_mag) > 0:
    response_mag[0] = 0

response_index = np.argmax(response_mag)
response_frequency = response_freq[response_index]

print()
print("======================================")
print("FFT ANALYSIS")
print("======================================")

print("Temperature        :", temperature, "C")
print("Humidity           :", humidity, "%")

print()
print("Edge Anomaly Score :", score, "/ 100")
print("ESP32 Status       :", status)

print()
if status == "ANOMALY":
    print(">>> MATERIAL NOT OK <<<")
else:
    print(">>> MATERIAL OK <<<")

print("======================================")

# --- PLOTTING ---
plt.figure(figsize=(10, 5))
plt.plot(time_axis, samples)
plt.axvline(0.5, linestyle="--", color='r', label="Baseline ends (0.5s)")
plt.xlabel("Time (seconds)")
plt.ylabel("Vibration (m/s²)")
plt.title("Thermo-Acoustic Vibration Response (200 Samples)")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
