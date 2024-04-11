# Saliva-Based COVID-19 Detection using Quantum Cascade Laser (QCL) and Arduino

This repository contains the code for a project that utilizes an Arduino Uno platform to detect the presence of SARS-CoV-2 (COVID-19) in saliva samples using a Quantum Cascade Laser (QCL) coupled with an IR Sensor. This is a part of a larger project that employs Fourier-transform infrared spectroscopy (FTIR) to discriminate between negative and positive COVID-19 patients.

This Arduino UNO programming project was developed for research conducted at the Military Medical School, which is part of the National Defense Secretariat (SEDENA) in Mexico.

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

- [CoolTerm](https://freeware.the-meiers.org/): A serial port terminal application used for debugging and communication with the Arduino.

## Setup and Configuration

### 1 Arduino Setup

To use this program effectively, follow these steps:

1. **Connections and Components:**
   - Ensure you have the required components connected properly, including the LCD display, buttons, LED, and any sensors.
   - Connect the Arduino Uno to the QCL and IR Sensor.

2. **Upload the Code:**
   - Copy the provided code into your Arduino IDE or any compatible platform.
   - Upload the code to your Arduino board.

3. **Setup:**
   - Once the code is uploaded, the setup function will initialize the program.
   - Ensure that the LCD display and buttons are initialized correctly.

4. **Presentation Message:**
   - Upon starting, the program will display a presentation message on the LCD screen. This is just a visual indication that the program has started correctly.

5. **Calibration:**
   - Follow the instructions on the LCD screen to calibrate the sensor input voltage.
   - The calibration process involves turning off the sensor, calibrating noise, and then turning on the sensor again.
   - Follow the on-screen instructions and press the buttons as required to proceed through the calibration process.

6. **Data Interpretation:**
   - After calibration, the program will enter the main loop for medical diagnosis and data interpretation.
   - Enter the diagnosis of the patient using the buttons on the board.
   - The program will then perform a series of analyses based on user input and sensor readings.
   - Follow the on-screen instructions to capture data and proceed with the analysis.

    Important Instructions for Voltage Output Analysis:

    1) Activate the Laser Diode / Temperature Controller:
        * Turn on the laser diode or temperature controller and wait for the diode to activate.
        * You will hear a relay click, indicating the activation of the diode.
        * Additionally, observe the LD ON switch LED; it will change from blinking to remaining continuously on, signaling the activation of the diode.
    
    2) Initiate Scanning Capture on the Arduino:
        * Once the diode is activated, promptly begin the scanning capture on the Arduino.
        * The program will continuously analyze all voltage outputs registered during the data capture process.

7. **Data Output:**
   - The program will output data to the LCD display as well as the serial monitor.
   - If needed, you can capture the data from the serial monitor for further analysis or processing.

8. **Repeat Analysis:**
   - After each analysis, the program will ask if you want to repeat the analysis.
   - Use the buttons to select 'SI' (Yes) or 'NO' (No) accordingly.

9. **Reset:**
   - If you need to reset the program at any point, press the reset button on your Arduino board.

### 2 CSV Generation

1. First, download the CoolTerm executable and unzip the file.
[Download](https://github.com/hectorzayas/SEDENACovid/blob/master/READMECaptures/1.jpg)
2. When using CoolTerm, stick to the default settings.
3. Make sure to close the Arduino IDE Capture Monitor to free up the communication port.
4. Now, open the CoolTerm executable.
5. Navigate to Settings and double-check that the Communication Port and Baudrate match the settings used in the Arduino IDE and your program.
[Settings](https://github.com/hectorzayas/SEDENACovid/blob/master/READMECaptures/2.jpg)
6. To establish a connection, go to Connection > Capture to Text/Binary File > Start...
[File](https://github.com/hectorzayas/SEDENACovid/blob/master/READMECaptures/3.jpg)
7. When saving the file, select "All Files" and give it a .csv extension.
8. Hit Connect to start using the Arduino Program. The file will automatically save when new data is captured.
[Connection](https://github.com/hectorzayas/SEDENACovid/blob/master/READMECaptures/4.jpg)
9. When you're finished, stop the connection by going to Connection > Capture to Text/Binary File > Stop...
10. Finally, disconnect from CoolTerm and exit the program.