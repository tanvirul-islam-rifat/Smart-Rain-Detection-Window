# Smart Rain Detection and Automatic Window-Closer Robot

An autonomous window actuation system built on **Arduino Uno R3** that monitors real-time environmental conditions using multi-sensor fusion and automatically closes a window when rain is detected — with no human intervention required.

Built as a course project for **CSE461 – Introduction to Robotics** at BRAC University.
Section 2 | Group 8 | Instructor: Md. Khalilur Rahman, PhD

---

## Team Members

| Name | Student ID |
|---|---|
| Rudra Joyti Maitra Soham | 22301056 |
| Sadar Ahmed | 22301716 |
| **Md. Tanvirul Islam Rifat** | **22101311** |
| Rahmin Raieef | 24341221 |

---

## Project Overview

<p align="center">
  <img src="./Smart%20Rain%20DEtection%20Project%20.jpg" alt="Smart Window Robot" width="450"/>
</p>

Unpredictable rainfall causes interior water damage when manual windows are left unattended. This project solves that problem with a low-cost, autonomous robot that:

1. Continuously monitors rain, temperature, humidity, and light using sensor fusion
2. Immediately drives a servo motor to close the window upon rain detection
3. Sounds a buzzer alert for 6 seconds during rain events
4. Displays live system status and sensor readings on a 16×2 I2C LCD
5. Allows manual override via limit switches at any time

The system demonstrates core robotics principles: **sense → decide → actuate** — the fundamental closed-loop feedback cycle of any autonomous robotic system.

---

## Hardware Components

| Component | Quantity | Role |
|---|---|---|
| Arduino Uno R3 | 1 | Central microcontroller — runs all control logic |
| Rain Sensor Module | 1 | Detects precipitation; triggers immediate window close |
| DHT11 Sensor | 1 | Monitors real-time temperature and humidity |
| LDR + 10kΩ Resistor | 1 | Measures ambient light level (analog input) |
| SG90 360° Servo Motor | 1 | Actuator driving the window open/close mechanism |
| 16×2 I2C LCD (0x27) | 1 | Real-time display of system status and sensor data |
| Active Buzzer Module | 1 | Audio alert during rain detection events |
| Green LED | 1 | Window OPEN indicator |
| Red LED | 1 | Window CLOSED / rain alert indicator |
| Micro Limit Switches | 2 | Safety stops preventing mechanical over-rotation |
| 6×AA Battery Holder (NiMH) | 1 | Portable power supply for motors and logic |
| Solderless Breadboard | 1 | Circuit assembly without soldering |
| Jumper Wires | 1 set | Signal and power routing |

---

## Pin Mapping

| Pin | Component | Direction |
|---|---|---|
| D2 | Rain Sensor | INPUT_PULLUP |
| D3 | DHT11 Data | INPUT |
| D6 | Limit Switch (CLOSE) | INPUT_PULLUP |
| D7 | Limit Switch (OPEN) | INPUT_PULLUP |
| D9 | SG90 Servo Signal | OUTPUT |
| D10 | Green LED | OUTPUT |
| D11 | Red LED | OUTPUT |
| D12 | Active Buzzer | OUTPUT |
| A0 | LDR (analog) | INPUT |
| SDA/SCL | I2C LCD | I2C Bus |

---

## System Logic

### Sense → Decide → Actuate Cycle

```
Rain Sensor ──┐
DHT11       ──┤──→ Arduino Uno ──→ Decision Logic ──→ Servo Motor (open/close)
LDR         ──┤                         │
Limit SW    ──┘                         ├──→ LED Indicators (red/green)
                                        ├──→ Buzzer Alert
                                        └──→ 16×2 LCD Display
```

### Decision Table

| Rain Detected | Limit SW (Close) | Limit SW (Open) | Action |
|:---:|:---:|:---:|---|
| YES | — | — | Close window + Red LED + Buzzer (6s) |
| NO | — | — | Open window + Green LED |
| — | Pressed | — | Force close (manual override) + Red LED |
| — | — | Pressed | Force open (manual override) + Green LED |

### Servo Control
The SG90 360° servo is driven with timed pulses rather than position angles (since it's a continuous rotation servo):
- **Close:** Write 120 → rotate CW for 1000ms → Write 90 (stop)
- **Open:** Write 60 → rotate CCW for 1000ms → Write 90 (stop)
- Limit switches provide physical hard stops to prevent mechanical over-rotation damage

### LCD Display (16×2)
```
Row 0: Rain:YES Win:CLS      (always shown)
Row 1: Temp: 28.5°C          (rotates every 3s)
       Humid: 65.2%
       Light: 423
```

---

## How to Run

### Requirements
- Arduino IDE (1.8+ or 2.x)
- Libraries (install via Library Manager):
  - `Servo` (built-in)
  - `DHT sensor library` by Adafruit
  - `LiquidCrystal_I2C` by Frank de Brabander

### Steps
1. Clone this repository
2. Open `smart_window.ino` in Arduino IDE
3. Install required libraries via **Tools → Manage Libraries**
4. Connect hardware as per the pin mapping table above
5. Select **Board: Arduino Uno** and correct COM port
6. Upload the sketch
7. Open Serial Monitor at **9600 baud** to see live sensor readings

---

## Repository Contents

```
smart-rain-detection-window/
├── smart_window.ino       # Final Arduino sketch (v2.0 — corrected servo directions)
├── images/
│   └── smart-window-robot.png   # Physical build photograph
├── poster/
│   └── CSE461_Group8_Poster.pdf # Project poster (abstract, circuit diagram, results)
└── README.md
```

> **Note on code versions:** Two code iterations were developed during the project.
> v1.0 had the servo rotation directions inverted.
> `smart_window.ino` is **v2.0** — the corrected final version with a comment
> `// SWAPPED` marking the fixed servo direction assignments.

---

## Applications

- Smart home automation — automatic window control based on weather
- Greenhouse climate management — prevent moisture damage to crops
- Assistive living — helps elderly or physically impaired individuals
- Low-cost retrofit — alternative to expensive commercial smart window systems

---

## Technical Architecture

- **Language:** C++ (Arduino / AVR)
- **Paradigm:** Event-driven embedded systems with closed-loop feedback control
- **Communication Protocols:** I2C (LCD display), Digital I/O (sensors/actuators), PWM (servo)
- **Microcontroller:** Arduino Uno R3 (ATmega328P, 16MHz, 5V logic)
- **Interface:** 16×2 I2C LCD (physical) + Serial Monitor (debug)

## Core Engineering Practices Demonstrated

- **Sense-Decide-Actuate Loop:** The system continuously polls sensors, evaluates rain state, and drives the servo actuator — implementing the fundamental autonomous robotics control cycle
- **Multi-Sensor Fusion:** Rain detection is supplemented by temperature, humidity, and light readings to provide a complete environmental picture, enabling future smart ventilation logic
- **State-Based Control:** The `lastRainState` flag prevents redundant servo activations — the window only moves on state *transitions* (dry→wet, wet→dry), not on every loop iteration
- **Safety Mechanism Design:** Limit switches act as physical hard stops preventing mechanical over-rotation — a standard embedded systems protection pattern for actuator-driven systems
- **I2C Protocol Usage:** LCD communication handled over a 2-wire I2C bus (SDA/SCL) via the `LiquidCrystal_I2C` library, demonstrating serial communication protocol integration
- **Manual Override Architecture:** Limit switch inputs are checked before autonomous logic, giving human operators absolute priority over automated behavior — standard in safety-critical robotic systems
- **Debounce Handling:** A 300ms delay after button reads prevents false-trigger jitter from mechanical switch bounce

## Author

**Md. Tanvirul Islam Rifat**

* **GitHub:** [@tanvirul-islam-rifat](https://github.com/tanvirul-islam-rifat)
* **LinkedIn:** [Tanvirul Islam Rifat](https://www.linkedin.com/in/tanvirul-islam-rifat)
