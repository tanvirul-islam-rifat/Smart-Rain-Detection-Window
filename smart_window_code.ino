/*
 * Smart Rain Detection and Automatic Window-Closer Robot
 * =========================================================
 * CSE461 — Introduction to Robotics
 * BRAC University | Section 2, Group 8
 *
 * Team Members:
 *   Rudra Joyti Maitra Soham  (22301056)
 *   Sadar Ahmed               (22301716)
 *   Md. Tanvirul Islam Rifat  (22101311)
 *   Rahmin Raieef             (24341221)
 *
 * Instructor: Md. Khalilur Rahman, PhD
 *
 * Description:
 *   An autonomous window actuation system that monitors real-time
 *   meteorological data (rain, temperature, humidity, light) using
 *   multi-sensor fusion and automatically closes the window upon
 *   rain detection via a servo motor. Includes manual override via
 *   limit switches and real-time status display on a 16x2 I2C LCD.
 *
 * Hardware:
 *   - Arduino Uno R3
 *   - Rain Sensor Module
 *   - DHT11 Temperature & Humidity Sensor
 *   - LDR (Light Dependent Resistor) with 10kΩ resistor
 *   - SG90 360-degree Servo Motor
 *   - 16x2 I2C LCD (address 0x27)
 *   - Active Buzzer Module
 *   - Green LED + Red LED
 *   - 2x Micro Limit Switches
 *   - 6xAA Battery Holder (NiMH batteries)
 *   - Solderless Breadboard + Jumper Wires
 *
 * Version: v2.0 (Final — servo directions corrected)
 */

#include <Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= PIN DEFINITIONS =================
#define SERVO_PIN    9
#define RAIN_PIN     2
#define GREEN_LED   10
#define RED_LED     11
#define BUZZER      12
#define DHTPIN       3
#define LDRPIN      A0
#define LIMIT_CLOSE  6   // Limit switch — force CLOSE window
#define LIMIT_OPEN   7   // Limit switch — force OPEN window

// ================= CONSTANTS =================
const int ROTATE_TIME    = 1000;  // ms — tune experimentally for ~90° travel
const int OPEN_POSITION  = 90;    // Logical: servo position representing OPEN
const int CLOSED_POSITION = 0;    // Logical: servo position representing CLOSED

// ================= OBJECTS =================
Servo windowServo;
DHT dht(DHTPIN, DHT11);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD at I2C address 0x27

// ================= STATE VARIABLES =================
bool lastRainState   = false;
int  currentServoPos = OPEN_POSITION;  // Assume window starts open
int  displayMode     = 0;              // Cycles: 0=Temp, 1=Humidity, 2=Light


// =====================================================
// SERVO CONTROL
// =====================================================

void closeWindow() {
  if (currentServoPos <= CLOSED_POSITION) {
    Serial.println("[SERVO] Window already CLOSED — no movement.");
    return;
  }
  Serial.print("[SERVO] Closing from position: ");
  Serial.println(currentServoPos);

  windowServo.write(120);     // Rotate CW
  delay(ROTATE_TIME);         // Travel ~90°
  windowServo.write(90);      // Stop

  currentServoPos = CLOSED_POSITION;
  Serial.println("[SERVO] Window CLOSED.");
}

void openWindow() {
  if (currentServoPos >= OPEN_POSITION) {
    Serial.println("[SERVO] Window already OPEN — no movement.");
    return;
  }
  Serial.print("[SERVO] Opening from position: ");
  Serial.println(currentServoPos);

  windowServo.write(60);      // Rotate CCW
  delay(ROTATE_TIME);         // Travel ~90°
  windowServo.write(90);      // Stop

  currentServoPos = OPEN_POSITION;
  Serial.println("[SERVO] Window OPEN.");
}


// =====================================================
// SETUP
// =====================================================

void setup() {
  pinMode(RAIN_PIN,    INPUT_PULLUP);
  pinMode(LIMIT_CLOSE, INPUT_PULLUP);
  pinMode(LIMIT_OPEN,  INPUT_PULLUP);
  pinMode(GREEN_LED,   OUTPUT);
  pinMode(RED_LED,     OUTPUT);
  pinMode(BUZZER,      OUTPUT);

  windowServo.attach(SERVO_PIN);
  windowServo.write(90);  // Stop signal on startup

  dht.begin();

  // LCD startup splash
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(" Smart Window");
  lcd.setCursor(0, 1); lcd.print(" Initializing...");
  delay(2000);
  lcd.clear();

  Serial.begin(9600);
  Serial.println("=== Smart Rain Detection Window v2.0 ===");
  Serial.println("Servo: 0=CLOSED, 90=OPEN");
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
  bool rainDetected    = (digitalRead(RAIN_PIN)    == LOW);
  bool closePressed    = (digitalRead(LIMIT_CLOSE) == LOW);
  bool openPressed     = (digitalRead(LIMIT_OPEN)  == LOW);

  // ── Manual Override (Limit Switches) ──────────────
  if (closePressed) {
    closeWindow();
    digitalWrite(RED_LED,   HIGH);
    digitalWrite(GREEN_LED, LOW);
    delay(300);  // Debounce
  }

  if (openPressed) {
    openWindow();
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED,   LOW);
    noTone(BUZZER);
    delay(300);  // Debounce
  }

  // ── Automatic Rain Detection ───────────────────────
  if (rainDetected != lastRainState) {
    if (rainDetected) {
      closeWindow();
      digitalWrite(RED_LED,   HIGH);
      digitalWrite(GREEN_LED, LOW);
      tone(BUZZER, 1000);  // Alert tone at 1kHz
      delay(6000);
      noTone(BUZZER);
    } else {
      openWindow();
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED,   LOW);
      noTone(BUZZER);
    }
    lastRainState = rainDetected;
  }

  // ── Read Sensors ───────────────────────────────────
  float temp  = dht.readTemperature();
  float hum   = dht.readHumidity();
  int   light = analogRead(LDRPIN);

  // ── Update LCD ─────────────────────────────────────
  lcd.clear();

  // Row 0: Rain status + Window state
  lcd.setCursor(0, 0);
  lcd.print("Rain:");
  lcd.print(rainDetected ? "YES" : "NO ");
  lcd.print(" Win:");
  lcd.print(currentServoPos == CLOSED_POSITION ? "CLS" : "OPN");

  // Row 1: Rotating sensor data (Temp / Humidity / Light)
  lcd.setCursor(0, 1);
  if (displayMode == 0) {
    lcd.print("Temp: ");
    if (isnan(temp)) { lcd.print("--.-"); }
    else { lcd.print(temp, 1); lcd.print((char)223); lcd.print("C  "); }
  } else if (displayMode == 1) {
    lcd.print("Humid: ");
    if (isnan(hum)) { lcd.print("--.-"); }
    else { lcd.print(hum, 1); lcd.print("%   "); }
  } else {
    lcd.print("Light: ");
    lcd.print(light);
    lcd.print("      ");
  }

  // Rotate display mode every 3 seconds
  static unsigned long lastModeChange = 0;
  if (millis() - lastModeChange > 3000) {
    displayMode = (displayMode + 1) % 3;
    lastModeChange = millis();
  }

  // ── Serial Monitor Output ──────────────────────────
  Serial.print("Rain:");  Serial.print(rainDetected ? "YES" : "NO");
  Serial.print(" | Light:"); Serial.print(light);
  Serial.print(" | Temp:");  Serial.print(temp);   Serial.print("C");
  Serial.print(" | Hum:");   Serial.print(hum);    Serial.print("%");
  Serial.print(" | Window:"); Serial.println(currentServoPos == CLOSED_POSITION ? "CLOSED" : "OPEN");

  delay(500);  // Poll every 500ms
}
