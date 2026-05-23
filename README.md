# ⚡ Arduino Hardware & Software Integrations

Welcome to my collection of C/C++ Arduino mini-projects! This repository contains a series of interactive, hardware-driven builds focusing on timing, sensor integration, and bridging microcontrollers with software scripts. 

## 🛠️ Tech Stack & Components
* **Language:** C / C++ 
* **Hardware:** Arduino Uno/Nano, PIR Sensors, Piezo Buzzers, 7-Segment Displays, LCD Screens, Push Buttons, LEDs.
* **Software Integration:** Python (for serial communication & mobile alerts)

---

## 🚀 Projects Overview

### 🏎️ 1. F1 Reaction Time Game
*Lights out and away we go!* This project simulates the iconic Formula 1 starting sequence. 
* **How it works:** Five LEDs light up sequentially. Once they all turn off after a randomized delay, the player must hit the button as fast as possible. 
* **Hardware:** 5x LEDs (Red), 1x Push Button.
* **Key Concepts:** `random()` functions, debounce logic, precision `millis()` timing for high-accuracy reaction tracking.

### ♟️ 2. Blitz Chess Timer
A fully functional dual-timer for competitive chess games, built to handle rapid and blitz time controls.
* **How it works:** Two buttons control the active timer. When a player finishes their move, hitting their button pauses their clock and starts the opponent's. A buzzer signals when someone flags (runs out of time).
* **Hardware:** 7-Segment Display / LCD, 2x Push Buttons, Piezo Buzzer.
* **Key Concepts:** State machines, multiplexing displays, interrupt handling.

### 🎂 3. Birthday Countdown Timer
A persistent countdown clock tracking the exact days, hours, minutes, and seconds until a targeted birthday.
* **How it works:** Continuously calculates the remaining time and formats it to a digital display.
* **Hardware:** LCD Screen / 7-Segment Display.
* **Key Concepts:** Time math, memory optimization, display refreshing without blocking the main loop.

### 🚨 4. Motion Detection Desk Alarm (`zayd_desk_alarm`)
A room security system to catch anyone sneaking around the workspace. 
* **How it works:** A PIR sensor detects movement. If tripped, the Arduino triggers a local piezo buzzer and sends a serial signal to a running Python script on the host machine. The Python script then pushes an alert directly to a mobile device.
* **Hardware:** PIR Motion Sensor, Piezo Buzzer.
* **Software:** Python (PySerial, push notification APIs).
* **Key Concepts:** Hardware-to-PC serial communication, sensor calibration, full-stack integration.

---

## ⚙️ Setup & Installation
1. Clone this repository to your local machine.
2. Open the respective project folder in the **Arduino IDE**.
3. Wire up your breadboard according to the pinouts defined at the top of each `.ino` file.
4. Verify the code (always double-check your function call parentheses!).
5. Flash the code to your Arduino board.
6. *For the Motion Alarm:* Ensure you run `pip install -r requirements.txt` and execute the companion Python script alongside the plugged-in Arduino.

## 👨‍💻 Author
**Zayd El Motassadeq** *Engineering at UBC*
