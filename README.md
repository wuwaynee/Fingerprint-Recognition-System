# Fingerprint Recognition System

## Overview
This project is a fingerprint recognition system utilizing the AS608 fingerprint sensor module and a PIC18F microcontroller. It provides the following functionalities:
- **Fingerprint Enrollment:** Register new fingerprints into the database.
- **Fingerprint Matching:** Identify stored fingerprints and trigger an LED upon a successful match.
- **Fingerprint Deletion:** Remove specific fingerprints from the database.
- **Status Display:** Display operation steps and results on an LCD screen.

This system is designed for applications requiring fingerprint authentication, such as access control.

---

## Features
- Fingerprint management using a compact keypad interface.
- Real-time operation feedback displayed on a 16x2 LCD.
- Adjustable LCD brightness using a 10K potentiometer.
- Stable performance ensured by an 18.4 MHz oscillator and capacitors.

---

## Technical Details
### Hardware Components
- AS608 Fingerprint Sensor
- PIC18F Microcontroller
- 16x2 LCD Display
- 1x4 Membrane Keypad
- LED Indicator
- 10K Potentiometer
- 18.4 MHz Oscillator

### Software Tools
- MPLAB® X IDE v5.20 for development
- XC8 Compiler
- PICKit4 Programmer

### Key Technologies
- **UART:** Communication between the microcontroller and fingerprint module.
- **Interrupts:** Real-time event handling.
- **ADC:** Control LCD brightness.

---

## How to Use
1. **Setup:** Connect all hardware components according to the design and supply stable power.
2. **Programming:** Upload the provided firmware to the PIC18F microcontroller using a PICKit4 programmer.
3. **Operation:**
   - Use the keypad to enroll, match, or delete fingerprints.
   - Follow the instructions displayed on the LCD screen for each action.
4. **Integration:** Adapt this system for specific applications such as access control or secure authentication.
