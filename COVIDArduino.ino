#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Average.h>

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

// Value units:
String voltageUnits = "V";

// Calibration
const int calibrationSamples = 10;
const int calibrationSamplingDelay = 500;

// Sensor Input Voltage Lecture
double noiseVoltage = 0;

// Data Capture
const int dataCaptureLoop = 5;  // Production value: 5
const int copyCaptureLoop = 3;  // Production value: 3

bool repeatAnalysis = true;

// Data Model
const double cutoffPoint = 0.7049;                              // Select Cutoff Point
const double cutoffPointThreshold = (2.0 / 3.0);                // Select Threshold Range
const double myCaptures[] = { 0.28, 0.33, 0.38, 0.43, 0.48 };   // Select (A) COntroller values
const double IRSensorMin = myCaptures[0];                       // Select Min SensorIR(A) lecture
const double IRSensorMax = myCaptures[4];                       // Select Max SensorIR(A) lecture

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

  // 4 - Medical Diagnosis & Data Interpretation
  while (repeatAnalysis) {
    medicalDiagnosis();
  }

  // 5 - Reset Program
  resetFunc();
}

void loop() {
  pushbuttonState();
  
  // printVoltageOutputAtA3();
}

// -------------------- //

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

void updateLCDDisplay(int cell, int row, String displayString) {
  lcd.setCursor(cell, row);
  lcd.print(displayString);
}

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

// -------------------- //

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

  updateLCDDisplay(4, 3, "SI");
  updateLCDDisplay(14, 3, "NO");

  while (pushbuttonState() < 1) {}

  if (pushbuttonState() == 1) {
    updateLCDDisplay(4, 2, "__");
    answer = 1;
    delay(shortDelay);
  } else if (pushbuttonState() == 2) {
    updateLCDDisplay(14, 2, "__");
    answer = 0;
    delay(shortDelay);
  }

  delay(shortDelay);
  return answer;
}

void capturedDataStatus(int data, int copy) {
  updateLCDDisplay(0, 0, "Dato: " + String(data));
  updateLCDDisplay(17, 0, String(copy) + "/" + String(copyCaptureLoop));
}

void mainPresentation() {
  lcd.clear();
  updateLCDDisplay(2, 1, "SECRETARIA DE LA");
  updateLCDDisplay(2, 2, "DEFENSA NACIONAL");
  delay(messageDisplayDuration);
  lcd.clear();
  updateLCDDisplay(2, 1, "ESCUELA MILITAR");
  updateLCDDisplay(4, 2, "DE MEDICINA");
  delay(messageDisplayDuration);
  lcd.clear();
}

void sensorInputVoltageCalibration() {
  lcd.clear();
  updateLCDDisplay(0, 0, "Iniciando ajuste");
  updateLCDDisplay(0, 1, "del Componente...");
  delay(interactionDisplayDuration);

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
  updateLCDDisplay(0, 1, "Ruido = " + String(noiseVoltage) + " " + voltageUnits);

  delay(interactionDisplayDuration);

  lcd.clear();
  updateLCDDisplay(0, 0, "Encienda el Sensor.");
  pressButtonToMessage(1, "para continuar.");
  while (pushbuttonState() != 1) {}
}

void medicalDiagnosis() {
  lcd.clear();
  updateLCDDisplay(0, 0, "Iniciando analisis");
  updateLCDDisplay(0, 1, "medico...");
  delay(interactionDisplayDuration);

  int copia = 0;
  int dato = 0;
  Average<double> resultados(copyCaptureLoop);

  int hasTos = 0;
  int hasCefalea = 0;
  int hasRinorrea = 0;
  /*
  lcd.clear();
  updateLCDDisplay(0, 0, "Tiene Tos?:");
  hasTos = YesNoSelector();

  lcd.clear();
  updateLCDDisplay(0, 0, "Tiene Cefalea?:");
  hasCefalea = YesNoSelector();

  lcd.clear();
  updateLCDDisplay(0, 0, "Tiene Rinorrea?:");
  hasRinorrea = YesNoSelector();

  lcd.clear();
  updateLCDDisplay(0, 0, "Respuestas:");
  updateLCDDisplay(0, 1, "- Tos:       " + String(hasTos));
  updateLCDDisplay(0, 2, "- Cefalea:   " + String(hasCefalea));
  updateLCDDisplay(0, 3, "- Rinorrea:  " + String(hasRinorrea));
  delay(interactionDisplayDuration);
*/


  while (copia < copyCaptureLoop) {
    copia = copia + 1;
    dato = 0;

    lcd.clear();
    updateLCDDisplay(5, 1, "Analisis " + String(copia));
    delay(interactionDisplayDuration);

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

      double capturedOutputVoltage = readSensorInputVoltage() - noiseVoltage;

      capturedDataStatus(dato, copia);
      pressButtonToMessage(1, "para capturar.");
      while (pushbuttonState() != 1) {
        capturedOutputVoltage = readSensorInputVoltage() - noiseVoltage;
        updateLCDDisplay(0, 1, "Medicion: " + String(capturedOutputVoltage, 4) + " " + voltageUnits);
      }
      gatheredData.push(capturedOutputVoltage);
      delay(shortDelay);
    }

    lcd.clear();
    updateLCDDisplay(0, 0, "Capturas:");
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

    // Data Interpretation

    double normMin = (gatheredData.get(0) - gatheredData.mean()) / gatheredData.stddev();
    double normMax = (gatheredData.get(dataCaptureLoop - 1) - gatheredData.mean()) / gatheredData.stddev();
    double slope = (normMax - normMin) / (IRSensorMax - IRSensorMin);

    lcd.clear();
    updateLCDDisplay(0, 0, "Dato 1 = " + String(gatheredData.get(0), 4) + " " + voltageUnits);
    // updateLCDDisplay(0, 1, "Dato " + String(dataCaptureLoop) + " = " + String(gatheredData.get(dataCaptureLoop - 1), 4) + voltageUnits);
    updateLCDDisplay(0, 1, "CutOff = " + String(cutoffPoint, 4) + " " + voltageUnits);

    updateLCDDisplay(0, 3, "Resultado " + String(copia) + ": ");
    if (gatheredData.get(0) < cutoffPoint) {
      updateLCDDisplay(12, 3, "POSITIVO");
      resultados.push(1);
    } else {
      updateLCDDisplay(12, 3, "NEGATIVO");
      resultados.push(0);
    }

    delay(shortDelay);
    while (pushbuttonState() != 1) {};

    lcd.clear();
  }

  lcd.clear();
  String finalResult;

  if (resultados.mean() >= cutoffPointThreshold) {
    finalResult = "POSITIVO";
  } else {
    finalResult = "NEGATIVO";
  }
  updateLCDDisplay(0, 0, "Promedio resultados:");
  updateLCDDisplay(0, 1, String(resultados.mean()));
  updateLCDDisplay(0, 2, "Resultado final:");
  updateLCDDisplay(0, 3, finalResult);

  delay(shortDelay);
  while (pushbuttonState() != 1) {};

  lcd.clear();
  updateLCDDisplay(0, 0, "Repetir analisis?");
  delay(shortDelay);
  repeatAnalysis = YesNoSelector();
}
