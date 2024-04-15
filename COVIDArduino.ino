#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Average.h>
#include <Math.h>

// LCD Display Setup
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Default voltage input values
const double outputReferenceMaxVoltage = 5;
const double analogArduinoInput = 1023;

// Buttons & Led Setup
const int buttonPin1 = 2;  // SI
const int buttonPin2 = 3;  // NO
const int buttonPin3 = 4;  // RESET
const int ledPin = 12;

bool buttonState1 = 0;
bool buttonState2 = 0;
bool buttonState3 = 0;

// Delays
const int resetTime = 3000;
const int messageDisplayDuration = 1000;
const int interactionDisplayDuration = 3000;
const int shortDelay = 250;
const int minimumDelay = 50;

// Value units:
String voltageUnits = "V";
const double INF = 9999.0;

// Calibration
const int calibrationSamples = 10;
const int calibrationSamplingDelay = 500;

// Sensor Input Voltage Lecture
double noiseVoltage = 0;

// Data Capture
const int dataCaptureLoop = 5;  // Production value: 5
const int copyCaptureLoop = 3;  // Production value: 3
const int scanningDataSamples = 50;

bool repeatAnalysis = true;
int analysisCounter = 0;

// Data Model
const double capturesSensor1[dataCaptureLoop] = { 0.30, 0.33, 0.36, 0.39, 0.42 };
const double capturesSensor2[dataCaptureLoop] = { 0.28, 0.33, 0.38, 0.43, 0.48 };
double myCaptures[dataCaptureLoop];
const int sensor = 2;  // Select Working Sensor

// Absorbance and Transmittance
const double baseTransmittance = 0;
double baseTransmittanceCaptures[dataCaptureLoop];

void (*resetFunc)(void) = 0;

void setup() {
  // Initialize Serial Port
  Serial.begin(9600);

  // 1 - Initialize LCD and Buttons
  lcdDisplayInit();
  buttonAndLedInit();

  // 2 - Show Presentation Message
  mainPresentation();

  // 3 - Calibrate PCB Amp OP variation
  sensorInputVoltageCalibration();
  transmittanceBaseCalibration();

  // Initiate CSV Generation
  csvTitleGenerator();

  // 4 - Medical Diagnosis & Data Interpretation
  while (repeatAnalysis) {
    medicalDiagnosis();
  }

  // 5 - Reset Program
  resetFunc();
}

void loop() {
  pushbuttonState();
}

/*
*************************************************
1 - Initialize LCD and Buttons
*************************************************
*/

void lcdDisplayInit() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void buttonAndLedInit() {
  pinMode(buttonPin1, INPUT);  // Botón Si
  pinMode(buttonPin2, INPUT);  // Botón No
  pinMode(buttonPin3, INPUT);  // Botón Reset

  pinMode(ledPin, OUTPUT);  // led
}

/*
*************************************************
2 - Show Presentation Message
*************************************************
*/

void mainPresentation() {
  lcd.clear();
  updateLCDDisplay(2, 1, "SECRETARIA DE LA");
  updateLCDDisplay(2, 2, "DEFENSA NACIONAL");
  delay(interactionDisplayDuration);
  lcd.clear();
  updateLCDDisplay(2, 1, "ESCUELA MILITAR");
  updateLCDDisplay(4, 2, "DE MEDICINA");
  delay(interactionDisplayDuration);
  lcd.clear();
}

/*
*************************************************
3 - Calibrate PCB Amp OP variation
*************************************************
*/

void sensorInputVoltageCalibration() {
  switch (sensor) {
    case 1:
      for (int i = 0; i < (sizeof(capturesSensor1) / sizeof(capturesSensor1[0])); i++) {
        myCaptures[i] = capturesSensor1[i];
      }
      break;
    case 2:
      for (int i = 0; i < (sizeof(capturesSensor2) / sizeof(capturesSensor2[0])); i++) {
        myCaptures[i] = capturesSensor2[i];
      }
      break;
  }

  lcd.clear();
  updateLCDDisplay(6, 1, "Sensor " + String(sensor));
  for (int i = 0; i < (sizeof(myCaptures) / sizeof(myCaptures[0])); i++) {
    updateLCDDisplay(i * 4 + 1, 2, String(int(myCaptures[i] * 100)));
  }

  delay(interactionDisplayDuration);

  lcd.clear();
  updateLCDDisplay(0, 0, "Iniciando ajuste");
  updateLCDDisplay(0, 1, "del Componente...");
  delay(messageDisplayDuration);

  lcd.clear();
  updateLCDDisplay(0, 0, "Apague el Sensor.");
  pressButtonToMessage(1, "para continuar.");
  while (pushbuttonState() != 1) {}

  lcd.clear();
  updateLCDDisplay(0, 0, "Calibrando ruido...");

  // Calibration
  noiseVoltage = getOutputAverageVoltage(calibrationSamples, calibrationSamplingDelay);

  lcd.clear();
  updateLCDDisplay(0, 0, "Ruido calibrado.");
  updateLCDDisplay(0, 2, "Ruido = " + String(noiseVoltage, 4) + " " + voltageUnits);

  delay(interactionDisplayDuration);

  lcd.clear();
  updateLCDDisplay(0, 0, "Encienda el Sensor.");
  pressButtonToMessage(1, "para continuar.");
  while (pushbuttonState() != 1) {}
}

void transmittanceBaseCalibration() {
  lcd.clear();
  updateLCDDisplay(0, 0, "Calibracion de la");
  updateLCDDisplay(0, 1, "transmitancia...");
  delay(interactionDisplayDuration);

  lcd.clear();
  updateLCDDisplay(0, 0, "Coloca porta muestra");
  updateLCDDisplay(0, 1, "limpia en el sensor.");
  pressButtonToMessage(1, "para continuar.");
  while (pushbuttonState() != 1) {}

  int dato = 0;

  Average<double> gatheredData(dataCaptureLoop);
  while (dato < dataCaptureLoop) {
    dato = dato + 1;

    lcd.clear();
    delay(shortDelay);
    updateLCDDisplay(0, 0, "Configure captura " + String(dato) + ".");
    updateLCDDisplay(0, 1, "(A) " + String(myCaptures[dato - 1], 4));
    pressButtonToMessage(1, "para continuar.");
    while (pushbuttonState() != 1) {}
    lcd.clear();
    delay(shortDelay);

    double crudeOutputVoltage = readSensorInputVoltage();
    double capturedOutputVoltage = constrain(crudeOutputVoltage - noiseVoltage, 0, INF);

    capturedDataStatus(dato, 0, myCaptures[dato - 1]);
    pressButtonToMessage(1, "para capturar.");
    while (pushbuttonState() != 1) {
      crudeOutputVoltage = readSensorInputVoltage();
      capturedOutputVoltage = constrain(crudeOutputVoltage - noiseVoltage, 0, INF);
      displayFilteredCrudeVoltage(crudeOutputVoltage, capturedOutputVoltage);
    }

    lcd.clear();
    capturedDataStatus(dato, 0, myCaptures[dato - 1]);
    updateLCDDisplay(0, 3, "Capturando...");

    Average<double> scanningVoltageData(scanningDataSamples);

    for (int i = 0; i < scanningDataSamples; i++) {
      crudeOutputVoltage = readSensorInputVoltage();
      capturedOutputVoltage = constrain(crudeOutputVoltage - noiseVoltage, 0, INF);
      scanningVoltageData.push(capturedOutputVoltage);
      displayFilteredCrudeVoltage(crudeOutputVoltage, capturedOutputVoltage);
    }

    capturedOutputVoltage = scanningVoltageData.maximum();

    gatheredData.push(capturedOutputVoltage);
    baseTransmittanceCaptures[dato - 1] = capturedOutputVoltage;
    delay(shortDelay);
  }

  lcd.clear();
  updateLCDDisplay(0, 0, "Transmitancia base:");
  const int columns = 2;
  const int rowsPerColumn = 3;

  for (int c = 0; c < columns; c++) {
    for (int r = 0; r < rowsPerColumn; r++) {
      if (r + (c * 3) == dataCaptureLoop) {
        break;
      }
      updateLCDDisplay(c * 11, r + 1, String(r + 1 + (c * 3)) + ": " + String(gatheredData.get(r + (c * 3)), 4));
    }
  }

  delay(shortDelay);
  while (pushbuttonState() != 1) {}

  lcd.clear();
}
/*
*************************************************
4 - Medical Diagnosis & Data Interpretation
*************************************************
*/

void medicalDiagnosis() {
  lcd.clear();
  updateLCDDisplay(0, 0, "Iniciando analisis");
  updateLCDDisplay(0, 1, "de muestras...");
  delay(messageDisplayDuration);

  String patientDiagnostic;
  lcd.clear();
  updateLCDDisplay(0, 0, "Ingrese diagnostico");
  updateLCDDisplay(0, 1, "del paciente:");
  patientDiagnostic = PositiveNegativeSelector();

  analysisCounter = analysisCounter + 1;

  int copia = 0;
  int dato = 0;

  while (copia < copyCaptureLoop) {
    copia = copia + 1;
    dato = 0;

    lcd.clear();
    updateLCDDisplay(5, 1, "Analisis " + String(copia));
    delay(messageDisplayDuration);

    Average<double> gatheredData(dataCaptureLoop);
    double printCaptures[dataCaptureLoop];
    while (dato < dataCaptureLoop) {
      dato = dato + 1;

      lcd.clear();
      delay(shortDelay);
      updateLCDDisplay(0, 0, "Configure captura " + String(dato) + ".");
      updateLCDDisplay(0, 1, "(A) " + String(myCaptures[dato - 1], 4));
      pressButtonToMessage(1, "para continuar.");
      while (pushbuttonState() != 1) {}
      lcd.clear();
      delay(shortDelay);

      double crudeOutputVoltage = readSensorInputVoltage();
      double capturedOutputVoltage = constrain(crudeOutputVoltage - noiseVoltage, 0, INF);

      capturedDataStatus(dato, copia, myCaptures[dato - 1]);
      pressButtonToMessage(1, "para capturar.");
      while (pushbuttonState() != 1) {
        crudeOutputVoltage = readSensorInputVoltage();
        capturedOutputVoltage = constrain(crudeOutputVoltage - noiseVoltage, 0, INF);
        displayFilteredCrudeVoltage(crudeOutputVoltage, capturedOutputVoltage);
      }

      lcd.clear();
      capturedDataStatus(dato, copia, myCaptures[dato - 1]);
      updateLCDDisplay(0, 3, "Capturando...");

      Average<double> scanningVoltageData(scanningDataSamples);

      for (int i = 0; i < scanningDataSamples; i++) {
        crudeOutputVoltage = readSensorInputVoltage();
        capturedOutputVoltage = constrain(crudeOutputVoltage - noiseVoltage, 0, INF);
        scanningVoltageData.push(capturedOutputVoltage);
        displayFilteredCrudeVoltage(crudeOutputVoltage, capturedOutputVoltage);
      }

      capturedOutputVoltage = scanningVoltageData.maximum();

      gatheredData.push(capturedOutputVoltage);
      printCaptures[dato - 1] = capturedOutputVoltage;
      delay(shortDelay);
    }

    // Captures
    const int columns = 2;
    const int rowsPerColumn = 3;

    // Voltage Captures
    lcd.clear();
    updateLCDDisplay(0, 0, "Capturas:");

    for (int c = 0; c < columns; c++) {
      for (int r = 0; r < rowsPerColumn; r++) {
        if (r + (c * 3) == dataCaptureLoop) {
          break;
        }
        updateLCDDisplay(c * 11, r + 1, String(r + 1 + (c * 3)) + ": " + String(gatheredData.get(r + (c * 3)), 4));
      }
    }

    delay(shortDelay);
    while (pushbuttonState() != 1) {}

    // Transmittance Captures
    lcd.clear();
    updateLCDDisplay(0, 0, "Transmitancias:");

    for (int c = 0; c < columns; c++) {
      for (int r = 0; r < rowsPerColumn; r++) {
        if (r + (c * 3) == dataCaptureLoop) {
          break;
        }
        const double transmittanceT = transmittance(baseTransmittanceCaptures[r + (c * 3)], gatheredData.get(r + (c * 3)));
        updateLCDDisplay(c * 11, r + 1, String(r + 1 + (c * 3)) + ": " + String(transmittanceT, 2) + "%");
      }
    }

    delay(shortDelay);
    while (pushbuttonState() != 1) {}

    // Absorbance Captures
    lcd.clear();
    updateLCDDisplay(0, 0, "Absorbancias:");

    for (int c = 0; c < columns; c++) {
      for (int r = 0; r < rowsPerColumn; r++) {
        if (r + (c * 3) == dataCaptureLoop) {
          break;
        }
        const double absorbanceA = absorbance(baseTransmittanceCaptures[r + (c * 3)], gatheredData.get(r + (c * 3)));
        updateLCDDisplay(c * 11, r + 1, String(r + 1 + (c * 3)) + ": " + String(absorbanceA, 4));
      }
    }

    delay(shortDelay);
    while (pushbuttonState() != 1) {}

    // CSV Generation
    csvDataGenerator(analysisCounter, patientDiagnostic, copia, printCaptures);

    lcd.clear();
  }

  lcd.clear();
  updateLCDDisplay(0, 0, "Repetir analisis?");
  delay(shortDelay);
  repeatAnalysis = YesNoSelector();
}

/*
*************************************************
TOOLS AND FUNCTIONS
*************************************************
*/

// BUTTONS AND SELECTORS

int pushbuttonState() {
  int pressedButton = -1;

  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);

  if (buttonState1 == 1) {
    pressedButton = 1;  // SI
  } else if (buttonState2 == 1) {
    pressedButton = 2;  // NO
  } else if (buttonState3 == 1) {
    pressedButton = 0;  // RESET
    resetFunc();
  }

  if (pressedButton > -1) {
    digitalWrite(ledPin, 1);
  } else {
    digitalWrite(ledPin, 0);
  }

  return pressedButton;
}

void pressButtonToMessage(bool set, String message) {
  String setValue = "";
  if (set) {
    setValue = "SI";
  } else {
    setValue = "NO";
  }

  updateLCDDisplay(0, 3, setValue + " " + String(message));
}

int YesNoSelector() {
  int answer = 0;

  updateLCDDisplay(14, 3, "SI");
  updateLCDDisplay(4, 3, "NO");

  while (pushbuttonState() < 1) {}

  if (pushbuttonState() == 1) {
    updateLCDDisplay(14, 2, "__");
    answer = 1;
    delay(shortDelay);
  } else if (pushbuttonState() == 2) {
    updateLCDDisplay(4, 2, "__");
    answer = 0;
    delay(shortDelay);
  }

  delay(shortDelay);
  return answer;
}

String PositiveNegativeSelector() {
  String answer;

  updateLCDDisplay(11, 3, "POSITIVO");
  updateLCDDisplay(1, 3, "NEGATIVO");

  while (pushbuttonState() < 1) {}

  if (pushbuttonState() == 1) {
    updateLCDDisplay(11, 2, "________");
    answer = "Positivo";
    delay(shortDelay);
  } else if (pushbuttonState() == 2) {
    updateLCDDisplay(1, 2, "________");
    answer = "Negativo";
    delay(shortDelay);
  }

  delay(shortDelay);
  return answer;
}

// VOLTAGES

double readSensorInputVoltage() {
  const double inputVoltage = analogRead(A3) * (outputReferenceMaxVoltage / analogArduinoInput);
  return inputVoltage;
}

double printVoltageOutputAtA3() {
  lcd.clear();
  while (pushbuttonState() < 1) {
    updateLCDDisplay(2, 1, "V = " + String(readSensorInputVoltage()));
    delay(shortDelay);
  }
}

double getOutputAverageVoltage(int avSize, int avDelay) {
  Average<double> averageValue(avSize);
  int i = avSize;
  while (i > 0) {
    averageValue.push(readSensorInputVoltage());
    delay(avDelay);
    i = i - 1;
  }

  return averageValue.mean();
}

void displayFilteredCrudeVoltage(double crudeOutputVoltage, double capturedOutputVoltage) {
  updateLCDDisplay(0, 1, "Filtro: " + String(capturedOutputVoltage, 4));
  updateLCDDisplay(15, 1, "V");
  updateLCDDisplay(0, 2, "Crudo:  " + String(crudeOutputVoltage, 4));
  updateLCDDisplay(15, 2, "V");
  delay(minimumDelay);
}

// DISPLAY

void updateLCDDisplay(int cell, int row, String displayString) {
  lcd.setCursor(cell, row);
  lcd.print(displayString);
}

void capturedDataStatus(int data, int copy, double capture) {
  updateLCDDisplay(0, 0, String(data));
  updateLCDDisplay(7, 0, String(capture, 2));
  if (copy > 0) {
    updateLCDDisplay(17, 0, String(copy) + "/" + String(copyCaptureLoop));
  } else {
    updateLCDDisplay(17, 0, "1/1");
  }
}

// CSV GENERATION

void csvTitleGenerator() {
  Serial.println("");

  Serial.print(" , , , , ");
  int myCapturesLength = (int)sizeof(myCaptures) / sizeof(myCaptures[0]);
  for (int i = 0; i < myCapturesLength; i++) {
    Serial.print(String(myCaptures[i], 4) + ", ");
  }
  Serial.println("");
  Serial.print(" , , , Transmitancia base, ");
  for (int i = 0; i < dataCaptureLoop; i++) {
    Serial.print(String(baseTransmittanceCaptures[i], 4) + ", ");
  }
  Serial.println("");

  Serial.println("");
  Serial.print("Paciente, Diagnostico, Analisis, , ");
  for (int i = 0; i < myCapturesLength; i++) {
    Serial.print(String(myCaptures[i], 4) + ", ");
  }
}

void csvDataGenerator(int analysisCounter, String patientDiagnostic, int copia, double printCaptures[]) {
  Serial.println("");
  Serial.print(String(analysisCounter) + ", ");
  Serial.print(patientDiagnostic + ", ");
  Serial.print(String(copia) + ", ");
  Serial.print(", ");
  for (int i = 0; i < dataCaptureLoop; i++) {
    Serial.print(String(printCaptures[i], 4) + ", ");
  }

  Serial.println("");
  Serial.print(", ");
  Serial.print(", ");
  Serial.print(", ");
  Serial.print("Transmitancia (%), ");
  for (int i = 0; i < dataCaptureLoop; i++) {
    Serial.print(String(transmittance(baseTransmittanceCaptures[i], printCaptures[i]), 4) + ", ");
  }

  Serial.println("");
  Serial.print(", ");
  Serial.print(", ");
  Serial.print(", ");
  Serial.print("Absorbancia, ");
  for (int i = 0; i < dataCaptureLoop; i++) {
    Serial.print(String(absorbance(baseTransmittanceCaptures[i], printCaptures[i]), 4) + ", ");
  }
}

// FORMULAS

double transmittance(double io, double it) {
  const double transmittance = constrain((it / io) * 100, 0, 100);
  return transmittance;
}

double absorbance(double io, double it) {
  const double absorbance = constrain(log10(io / it), 0, 2);
  return absorbance;
}