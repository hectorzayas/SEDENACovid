#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Average.h>

// LCD Display Setup
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Buttons & Led Setup
const int buttonPin1 = 2;   // SI
const int buttonPin2 = 3;   // NO
const int buttonPin3 = 4;   // RESET
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
String voltageUnits = "mV";

// Calibration
const int calibrationSamples = 10;
const int calibrationSamplingDelay = 500;

// Sensor Input Voltage Lecture
double noiseVoltage = 0;

// Data Capture
const int dataCaptureLoop = 5;
const int copyCaptureLoop = 3;

void(* resetFunc) (void) = 0;

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

  // 4 - Initialize Medical Consultation
  medicalConsultation();

  lcd.clear();
}

void loop() {
  pushbuttonState();

  // lcd.clear();
  // updateLCDDisplay(0,0, "Calculando media...");

  // double meanFilteredVoltage = getOutputAverageVoltage(10, 500) - noiseVoltage;

  // lcd.clear();
  // updateLCDDisplay(0,0, "V prom = " + String(meanFilteredVoltage) + voltageUnits);
  // while (pushbuttonState() != 1) {}
}

// -------------------- //

void lcdDisplayInit() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void buttonAndLedInit() {
  pinMode(buttonPin1,INPUT);  // Botón Si
  pinMode(buttonPin2,INPUT);  // Botón No
  pinMode(buttonPin3,INPUT);  // Botón Reset

  pinMode(ledPin, OUTPUT);    // led
}

int pushbuttonState() {
  int pressedButton = -1;

  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);

  if (buttonState1 == 1) {
    pressedButton = 1;    // SI
  } else if (buttonState2 == 1) {
    pressedButton = 2;    // NO
  } else if (buttonState3 == 1) {
    pressedButton = 0;    // RESET
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
  lcd.setCursor(cell,row);
  lcd.print(displayString);
}

double readSensorInputVoltage() {
  const double inputVoltage = analogRead(A3);
  return inputVoltage;
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

void pressButtonToContinue(bool set) {
  String setValue = "";
  if (set) {
    setValue = "SI";
  } else {
    setValue = "NO";
  }
  updateLCDDisplay(0,3,setValue + " para continuar...");
}

int YesNoSelector() {
  int answer = 0;

  updateLCDDisplay(4,3,"SI");
  updateLCDDisplay(14,3,"NO");

  while ( pushbuttonState() < 1 ) {}

  if (pushbuttonState() == 1) {
    updateLCDDisplay(4,2,"__");
    answer = 1;
    delay(shortDelay);
  } else if (pushbuttonState() == 2) {
    updateLCDDisplay(14,2,"__");
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
  updateLCDDisplay(0,0,"Iniciando ajuste");
  updateLCDDisplay(0,1,"del Componente...");
  delay(interactionDisplayDuration);

  lcd.clear();
  updateLCDDisplay(0,0,"Apague el Sensor.");
  pressButtonToContinue(1);
  while (pushbuttonState() != 1) {}

  lcd.clear();
  updateLCDDisplay(0,0,"Calibrando ruido...");

  // Calibration
  noiseVoltage = getOutputAverageVoltage(calibrationSamples, calibrationSamplingDelay);

  lcd.clear();
  updateLCDDisplay(0,0,"Ruido calibrado.");
  updateLCDDisplay(0,1,"Ruido = " + String(noiseVoltage) + voltageUnits);

  delay(interactionDisplayDuration);

  lcd.clear();
  updateLCDDisplay(0,0,"Encienda el Sensor.");
  pressButtonToContinue(1);
  while (pushbuttonState() != 1) {}
}

void medicalConsultation() {
  lcd.clear();
  updateLCDDisplay(0,0,"Iniciando diagnosis");
  updateLCDDisplay(0,1,"medico...");
  delay(interactionDisplayDuration);

  int copia = 0;
  int dato = 0;

  int hasTos = 0;
  int hasCefalea = 0;
  int hasRinorrea = 0;

  lcd.clear();
  updateLCDDisplay(0,0,"Tiene Tos?:");
  hasTos = YesNoSelector();

  lcd.clear();
  updateLCDDisplay(0,0,"Tiene Cefalea?:");
  hasCefalea = YesNoSelector();

  lcd.clear();
  updateLCDDisplay(0,0,"Tiene Rinorrea?:");
  hasRinorrea = YesNoSelector();

  lcd.clear();
  updateLCDDisplay(0,0,"Respuestas:");
  updateLCDDisplay(0,1,"Tos:       " + String(hasTos));
  updateLCDDisplay(0,2,"Cefalea:   " + String(hasCefalea));
  updateLCDDisplay(0,3,"Rinorrea:  " + String(hasRinorrea));
  delay(interactionDisplayDuration);

  lcd.clear();
  copia = copia + 1;

  double allData[dataCaptureLoop] = {};
  while (dato < dataCaptureLoop) {
    dato = dato + 1;
    capturedDataStatus(dato, copia);
    updateLCDDisplay(0,3,"SI para capturar.");
    int fixedCapturedVoltage = 0;
    while (pushbuttonState() != 1) {
      int capturedOutputVoltage = readSensorInputVoltage() - noiseVoltage;
      fixedCapturedVoltage = capturedOutputVoltage;
      updateLCDDisplay(0,1,"Medicion: " + String(capturedOutputVoltage) + voltageUnits);
      delay(shortDelay);
    }
    allData[dato] = fixedCapturedVoltage;
    delay(shortDelay);
  }

  lcd.clear();

  while (pushbuttonState() != 1) {}
}