#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize two LCDs with different addresses
LiquidCrystal_I2C lcd1(0x27, 16, 2); // First LCD
LiquidCrystal_I2C lcd2(0x26, 16, 2); // Second LCD (change address if needed)

// Pin setup
const int lightPin = 3;
const int pirPin = 2;
const int photoResistorPin = A1;
const int tmpPin = A2;
const int gasPin = A0;
const int motorPin = 4;
const int gateMotorPin = 6;
const int ultrasonicTrigPin = 8;
const int ultrasonicEchoPin = 9;
const int buzzerPin = 10;

// Thresholds
const int lightThreshold = 600;
const int temperatureThreshold = 25;  // Minimum temperature for fan activation
const int smokeThreshold = 400;
const int smokeDangerThreshold = 500;
const int smokeVeryDangerousThreshold = 800;

void setup() {
  Serial.begin(9600);

  // Pin modes
  pinMode(lightPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(gateMotorPin, OUTPUT);
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Initialize both LCDs
  lcd1.init();
  lcd1.backlight();
  lcd2.init();
  lcd2.backlight();

  lcd1.print("Smart Home LCD1");
  lcd2.print("Smart Home LCD2");
  delay(2000); // Show startup message for 2 seconds
}
void loop() {
  // Automatic lighting system
  int lightLevel = analogRead(photoResistorPin);
  int motionDetected = digitalRead(pirPin);
  bool isLightOn = (lightLevel < lightThreshold && motionDetected == HIGH);
  digitalWrite(lightPin, isLightOn ? HIGH : LOW);

  // Temperature-based fan system
  int rawTemperature = analogRead(tmpPin);
  float voltage = rawTemperature * (5.0 / 1023.0);
  float temperature = (voltage - 0.5) * 100;

  // Map temperature to fan speed
  int fanSpeed = 0;
  if (temperature > temperatureThreshold) {
    fanSpeed = map(temperature, temperatureThreshold, 35, 50, 255);
    fanSpeed = constraint(fanSpeed, 50, 255); // Ensure speed is within the valid PWM range
  }
  analogWrite(motorPin, fanSpeed);

  // Gas detection system
  int gasLevel = analogRead(gasPin);
  String gasStatus;
  if (gasLevel > smokeVeryDangerousThreshold) {
    gasStatus = "VERY HIGH";
  } else if (gasLevel > smokeDangerThreshold) {
    gasStatus = "DANGEROUS";
  } else if (gasLevel > smokeThreshold) {
    gasStatus = "SLIGHTLY HIGH";
  } else {
    gasStatus = "SAFE";
  }
  bool isSmokeDetected = (gasLevel > smokeThreshold);
  digitalWrite(buzzerPin, isSmokeDetected ? HIGH : LOW);

  // Smart gate system
  digitalWrite(ultrasonicTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTrigPin, LOW);
  long duration = pulseIn(ultrasonicEchoPin, HIGH);
  int distance = duration * 0.0344 / 2;
  bool isGateOpen = (distance < 100);
  digitalWrite(gateMotorPin, isGateOpen ? HIGH : LOW);

  // Display on LCD1 (Light, Fan Speed, Temperature)
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Light: ");
  lcd1.print(isLightOn ? "ON " : "OFF");
  lcd1.setCursor(0, 1);
  lcd1.print("Temp: ");
  lcd1.print(temperature, 1);
  lcd1.print("C");
  delay(2000);

  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Fan Speed:");
  lcd1.setCursor(0, 1);
  lcd1.print(fanSpeed);
  delay(2000);

  // Display on LCD2 (Gate, Gas)
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print("Gate: ");
  lcd2.print(isGateOpen ? "OPEN " : "CLOSED");
  lcd2.setCursor(0, 1);
  lcd2.print("Gas: ");
  lcd2.print(gasStatus);
  delay(3000);

  // Serial Monitor Output
  Serial.println("------ Sensor Data ------");
  Serial.print("Light Level: ");
  Serial.println(lightLevel);
  Serial.print("Motion Detected: ");
  Serial.println(motionDetected == HIGH ? "Yes" : "No");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Fan Speed: ");
  Serial.println(fanSpeed);
  Serial.print("Gas Level: ");
  Serial.println(gasLevel);
  Serial.print("Smoke Status: ");
  Serial.println(gasStatus);
  Serial.print("Distance to Object: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Gate Status: ");
  Serial.println(isGateOpen ? "OPEN" : "CLOSED");
  Serial.println("-------------------------");

  delay(1000); // Refresh every second
}
