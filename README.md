# Saliva-Based COVID-19 Detection using Quantum Cascade Laser (QCL) and Arduino

This repository contains the code for a project that utilizes an Arduino Uno platform to detect the presence of SARS-CoV-2 (COVID-19) in saliva samples using a Quantum Cascade Laser (QCL) coupled with an IR Sensor. This is a part of a larger project that employs Fourier-transform infrared spectroscopy (FTIR) to discriminate between negative and positive COVID-19 patients.

## Overview

The Arduino reads the voltage produced by the QCL as it traverses a saliva sample. The voltage readings are processed to determine the presence or absence of COVID-19 based on pre-defined cutoff points and controller values.

## Hardware Requirements

- Arduino Uno
- Quantum Cascade Laser (QCL)
- IR Sensor
- Liquid Crystal I2C Display

## Libraries Used

- [Liquid Crystal I2C](https://www.arduinolibraries.info/libraries/liquid-crystal-i2-c)
- [Average](https://github.com/MajenkoLibraries/Average)

## Tools Used

- [CoolTerm](https://freeware.the-meiers.org/): A serial port terminal application used for debugging and communication with the Arduino. Setup tutorial available [here](https://www.youtube.com/watch?v=RWgyCcnUxPY&ab_channel=ShuvangkarDas).

## Setup and Configuration

1. Connect the Arduino Uno to the QCL and IR Sensor.
2. Install the Liquid Crystal I2C and Average libraries in your Arduino IDE.
3. Upload the provided code to the Arduino Uno.
4. Use CoolTerm to monitor the serial output for diagnostic messages and data capture.