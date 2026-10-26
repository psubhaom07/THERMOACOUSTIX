#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <ESP32Servo.h>
#include <LiquidCrystal.h>

#define DHT_PIN 4
#define SERVO_PIN 15

#define GREEN_LED 12
#define RED_LED 13
#define BUZZER_PIN 14

#define LCD_RS 23
#define LCD_EN 19
#define LCD_D4 32
#define LCD_D5 33
#define LCD_D6 25
#define LCD_D7 26

#define SDA_PIN 21
#define SCL_PIN 22

#define SAMPLE_RATE 100
#define TOTAL_SAMPLES 200 

DHT dht(DHT_PIN, DHT11);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

LiquidCrystal lcd(
  LCD_RS,
  LCD_EN,
  LCD_D4,
  LCD_D5,
  LCD_D6,
  LCD_D7
);

Servo servo;

float samples[TOTAL_SAMPLES];

bool accelOK = false;
bool dhtOK = false;

float temperature = 0;
float humidity = 0;

float baselineRMS = 0;
float responseRMS = 0;
float vibrationPeak = 0;

int anomalyScore = 0;

float readAcceleration()
{
  sensors_event_t event;
  accel.getEvent(&event);

  return sqrt(
    event.acceleration.x * event.acceleration.x +
    event.acceleration.y * event.acceleration.y +
    event.acceleration.z * event.acceleration.z
  );
}

void readSensors()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (!isnan(humidity) && !isnan(temperature))
    dhtOK = true;
  else
    dhtOK = false;
}

void collectTest()
{
  float baselineSquares = 0;
  float responseSquares = 0;
  float peak = 0;

  Serial.println("TEST_START");

  servo.write(30);

  unsigned long nextSample = micros();

  for (int i = 0; i < TOTAL_SAMPLES; i++)
  {
    while ((long)(micros() - nextSample) < 0)
    {
      yield();
      delayMicroseconds(50);
    }

    nextSample += 10000;

    // Pulse sequence scaled for 200 samples
    if (i == 20)  servo.write(90);
    if (i == 50)  servo.write(30);
    if (i == 80)  servo.write(90);
    if (i == 110) servo.write(30);
    if (i == 140) servo.write(90);
    if (i == 170) servo.write(30);
    if (i == 199) servo.write(30);

    float acceleration = readAcceleration();

    samples[i] = acceleration - 9.81;

    if (i < 50) // Baseline window: first 50 samples (~0.5s)
    {
      baselineSquares += samples[i] * samples[i];
    }
    else // Response window: remaining 150 samples (~1.5s)
    {
      responseSquares += samples[i] * samples[i];

      if (abs(samples[i]) > peak)
        peak = abs(samples[i]);
    }
  }

  servo.write(30);

  baselineRMS = sqrt(baselineSquares / 50.0);
  responseRMS = sqrt(responseSquares / 150.0);
  vibrationPeak = peak;

  float increase = responseRMS - baselineRMS;

  anomalyScore = 0;

  if (increase > 0.5) anomalyScore += 15;
  if (increase > 1.0) anomalyScore += 20;
  if (increase > 2.0) anomalyScore += 25;
  if (vibrationPeak > 5) anomalyScore += 10;
  if (vibrationPeak > 8) anomalyScore += 15;

  if (anomalyScore > 100) anomalyScore = 100;

  bool anomaly = anomalyScore > 10;

  Serial.println("DATA_START");

  for (int i = 0; i < TOTAL_SAMPLES; i++)
  {
    Serial.print("SAMPLE,");
    Serial.print(i);
    Serial.print(",");
    Serial.println(samples[i], 2);
  }

  Serial.println("DATA_END");

  Serial.print("RESULT,");
  Serial.print(baselineRMS, 5);
  Serial.print(",");
  Serial.print(responseRMS, 5);
  Serial.print(",");
  Serial.print(vibrationPeak, 5);
  Serial.print(",");
  Serial.print(anomalyScore);
  Serial.print(",");

  if (anomaly)
    Serial.println("ANOMALY");
  else
    Serial.println("NORMAL");

  Serial.println("TEST_END");

  if (anomaly)
  {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
  }
  else
  {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  lcd.clear();

  if (anomaly)
  {
    lcd.setCursor(0, 0);
    lcd.print("MATERIAL NOT OK");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("MATERIAL OK");
  }

  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print(anomalyScore);
  lcd.print("/100");
}

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SYSTEM READY");

  lcd.setCursor(0, 1);
  lcd.print("Send TEST");

  printSensors();
}

void loop()
{
  if (Serial.available())
  {
    char command = Serial.read();

    if (command == 'T' || command == 't')
    {
      if (accelOK)
      {
        readSensors();
        collectTest();
      }
    }
  }
}
