#include <EEPROM.h>
#include <LiquidCrystal.h>

// pin componenti
const byte photoPin = A0;
const byte pinLedYellow = 3;
const byte pinLedRed = 5;
const byte pinLedGreen = 6;
const byte pinBuzzer = 11;

// variabili dati
byte AL;
byte RT = 20;
byte RC = 1;
char mode = 'C';
boolean DS = true;
byte OM = 3;
byte LL = 30; // soglia accensione
byte UL = 70; // soglia max
byte greenLedValue; //intensita led pinLedGreen OM-3
byte redLedValue; //intensita led pinLedRed OM-3

// variabili di comodo
String string;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
unsigned long interval = RT * 100;
boolean ld = true;

//indirizzi EEPROM
const byte adressLL = 0;
const byte adressUL = 1;
const byte adressOM = 2;
const byte adressRT = 3;
const byte adressRC = 4;
const byte adressDS = 5;

// pin lcd
const byte db4 = 10;
const byte db5 = 9;
const byte db6 = 8;
const byte db7 = 7;

const byte en = 12;
const byte rs = 13;
LiquidCrystal lcd(rs,en,db4,db5,db6,db7);

void setup() {
  pinMode(photoPin, INPUT);
  pinMode(pinLedYellow, OUTPUT);
  pinMode(pinLedRed, OUTPUT);
  pinMode(pinLedGreen, OUTPUT);
  readFromEEPROM();
  Serial.begin(9600);
  attachInterrupt(0, readToggle, RISING);
  lcd.begin(16,2);
}

void loop() {

  // LEGGO A INTERVALLI RT
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) > interval) {
    previousMillis = currentMillis;
    AL = readPhoto();
    if (mode == 'D') {
      displayData(); // debug print
    }
    updateLCD();
  }

   
  // modifico stati in base alle variabili
  if (mode == 'A' || mode == 'D') {

    if(mode == 'A')
      digitalWrite(pinLedYellow, LOW);
    else{
      unsigned long currentMillis = millis();
      if((currentMillis - previousMillis2) > interval){
        previousMillis2 = currentMillis;
        if(ld)
          analogWrite(pinLedYellow, 128);
        else
          analogWrite(pinLedYellow, 0);
        ld = !ld;  
      }
    }

    // codice Analisi o Debug
    switch (OM) {
      case 1:
        // CASO OM 1
        if (AL < LL) {
          digitalWrite(pinLedRed, HIGH);
          digitalWrite(pinLedGreen, LOW);
        } else if (AL > UL) {
          digitalWrite(pinLedRed, LOW);
          digitalWrite(pinLedGreen, HIGH);
        } else {
          digitalWrite(pinLedRed, HIGH);
          digitalWrite(pinLedGreen, HIGH);
        }
        break;

      case 2:
        // CASO OM 2
        if (AL < LL) {
          digitalWrite(pinLedRed, HIGH);
          digitalWrite(pinLedGreen, LOW);
        } else if (AL > UL) {
          digitalWrite(pinLedRed, LOW);
          digitalWrite(pinLedGreen, HIGH);
        } else {
          digitalWrite(pinLedRed, LOW);
          digitalWrite(pinLedGreen, LOW);
        }
        break;

      case 3:
        // CASO OM 3
        if (AL < LL) {
          digitalWrite(pinLedRed, HIGH);
          digitalWrite(pinLedGreen, LOW);
        } else if (AL > UL) {
          digitalWrite(pinLedRed, LOW);
          digitalWrite(pinLedGreen, HIGH);
        } else {
          // calcolo differenza peRCentuALe
          greenLedValue = (byte)(((float)(AL - LL) / (float)(UL - LL)) * 255);
          redLedValue = (byte)(((float)(UL - AL) / (float)(UL - LL)) * 255);
          analogWrite(pinLedRed, redLedValue);
          analogWrite(pinLedGreen, greenLedValue);
        }
        break;
    }
  } else {
    // Codice Configurazione
    digitalWrite(pinLedRed, LOW);
    digitalWrite(pinLedGreen, LOW);
    digitalWrite(pinLedYellow, HIGH);

    string = Serial.readString();
    String cmd, par;
    cmd = string.substring(0, 2);
    par = string.substring(3, string.length());
    if (cmd == "LL") {
      if (par.toInt() < 100 && par.toInt() > 0 && par.toInt() < UL) {
        LL = par.toInt();
        successChange();
      } else {
        failureChange();
      }
    } else if (cmd == "UL") {
      if (par.toInt() < 100 && par.toInt() > 0 && par.toInt() > LL) {
        UL = par.toInt();
        successChange();
      } else {
        failureChange();
      }
    } else if (cmd == "OM") {
      if (par.toInt() == 1 || par.toInt() == 2 || par.toInt() == 3) {
        OM = par.toInt();
        successChange();
      } else {
        failureChange();
      }
    } else if (cmd == "DS") {
      if (par.toInt() == 1) {
        DS = true;
        successChange();
      } else if (par.toInt() == 0) {
        DS = false;
        successChange();
      } else {
        failureChange();
      }
    } else if (cmd == "RC") {
      if (par.toInt() < 6 && par.toInt() > 0) {
        RC = par.toInt();
        successChange();
      } else {
        failureChange();
      }
    } else if (cmd == "RT") {
      if (par.toInt() < 101 && par.toInt() > 0) {
        RT = par.toInt();
        interval = RT * 100;
        successChange();
      } else {
        failureChange();
      }
    }
    // UPDATE EEPROM
    updateEEPROM();
  }
}

/*  
 *   FUNZIONI
*/

int readPhoto() {
  int tmp;
  int somma = 0;
  for (int i = 0; i < RC; i++) {
    tmp = analogRead(photoPin);
    somma += tmp;
  }
  return map(somma/RC, 0, 1023, 0, 100);
}

void readToggle() {
  if (mode == 'C' && DS)
    mode = 'D';
  else if (mode == 'C' && !DS)
    mode = 'A';
  else
    mode = 'C';
}

void successChange() {
  for (byte i = 0; i < 3; i++) {
    digitalWrite(pinLedGreen, HIGH);
    analogWrite(pinBuzzer, 59999);
    delay(100);
    digitalWrite(pinLedGreen, LOW);
    analogWrite(pinBuzzer, 0);
    delay(100);
  }
}

void failureChange() {
  analogWrite(pinBuzzer, 1000);
  for (byte i = 0; i < 3; i++) {
    digitalWrite(pinLedRed, HIGH);
    delay(100);
    digitalWrite(pinLedRed, LOW);
    delay(100);
  }
  analogWrite(pinBuzzer, 0);
}

void updateEEPROM() {
  EEPROM.update(adressLL, LL);
  EEPROM.update(adressUL, UL);
  EEPROM.update(adressOM, OM);
  EEPROM.update(adressRT, RT);
  EEPROM.update(adressRC, RC);
  EEPROM.update(adressDS, DS);
}

void readFromEEPROM() {
  LL=EEPROM.read(adressLL);
  UL=EEPROM.read(adressUL);
  OM=EEPROM.read(adressOM);
  RT=EEPROM.read(adressRT);
  RC=EEPROM.read(adressRC);
  DS=EEPROM.read(adressDS);
}

void displayData() {
  Serial.print("OM:  ");
  Serial.println(OM);
  Serial.print("LL:  ");
  Serial.println(LL);
  Serial.print("UL:  ");
  Serial.println(UL);
  Serial.println("------");
  Serial.print("AL:  ");
  Serial.println(AL);
  if (OM == 3 && AL > LL && AL  < UL) {
    Serial.print("Red value: ");
    Serial.println(redLedValue);
    Serial.print("Green value: ");
    Serial.println(greenLedValue);
  }
  Serial.println("------");
  Serial.print("RC:  ");
  Serial.println(RC);
  Serial.print("RT:  ");
  Serial.println(RT);

  Serial.println("_________________________________");
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LL:");
  lcd.print(LL);
  lcd.print(" R:");
  lcd.print(RT);
  lcd.print(" OM:");
  lcd.print(OM);
  lcd.setCursor(0,1);
  lcd.print("UL:");
  lcd.print(UL);
  lcd.print(" AL:");
  lcd.print(AL);
  lcd.print(" |");
  lcd.print(mode);
  lcd.print("|");
}

