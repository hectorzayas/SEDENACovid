#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD Display Setup
char lcdArray1[] = "";
char lcdArray2[] = "";
char lcdArray3[] = "";
char lcdArray4[] = "";
LiquidCrystal_I2C lcd(0x27, 20, 4);


void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  presentacion();
  // lcd.print(lcdArray1);
  // lcd.setCursor(0, 1);

  pinMode(2,INPUT); //botón Si
  pinMode(3,INPUT); //botón No
  pinMode(4,INPUT); //botón Reinicio
}

void loop() {
  // put your main code here, to run repeatedly:

}

void presentacion(){
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("SECRETARIA DE LA");
  lcd.setCursor(2,2);
  lcd.print("DEFENSA NACIONAL");          
  delay(3000);//8000
  lcd.clear();
  lcd.setCursor(2,1);
  lcd.print("ESCUELA MILITAR");
  lcd.setCursor(4,2);
  lcd.print("DE MEDICINA");          
  delay(3000);//5000
  lcd.clear();
}