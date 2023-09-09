#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 4, en = 5, d4 = 6, d5 = 7, d6 = 8, d7 = 9;
    //88pa     2,      9,      10,     11,     12,     13
    //8x2      4,      6,      11,     12,     13,     14
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);  //rs en d4 d5 d6 d7

#define MOTOR_OUTPUT 3
#define BUTTONS A0

const int waitTimeLoc = 0;
const int moveTimeLoc = 1;

const int btnNONE = 0;
const int btnUP = 1;
const int btnDOWN = 2;
const int btnLEFT = 3;
const int btnRIGHT = 4;
const int btnSELECT = 5;

const String version = "V1.03";
const String date = "08-08-23";

int keyPressed;
int screenId = 0;
int prevScreen = 0;
int waitTime = 0;
double moveTime = 0.0;

int waitTimeMemory;
double moveTimeMemory;

bool paramsWritten = false;

bool motorMovement = false;

bool ready = false;
short readyMillis = 200;
unsigned long previousMillisButton = 0, previousMillisMotor = 0;

byte memorySymbol[] = {
  0x15, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x15, 0x00
};

byte checkMarkSymbol[] = {
  0x00, 0x01, 0x01, 0x12, 0x0A, 0x04, 0x00, 0x00
};

void unready() {
  ready = false;
}

void nextPage() {
  ++screenId;
}

void prevPage() {
  --screenId;
}

int readAnalogButton() {
  int button = analogRead(BUTTONS);

  if (button < 50) return btnDOWN;
  if (button < 555) return btnRIGHT;
  if (button < 720) return btnUP;
  if (button < 800) return btnLEFT;
  return btnNONE;  // when all others fail, return this.
}

void motorState() {
  unsigned long currentMillis = millis();

  unsigned long moveMillis = moveTime * 1000, waitMillis = waitTime * 1000;
  if (motorMovement == false && currentMillis - previousMillisMotor >= waitMillis) {
    motorMovement = true;
    previousMillisMotor = currentMillis;
  }

  if (motorMovement == true && currentMillis - previousMillisMotor >= moveMillis) {
    motorMovement = false;
    previousMillisMotor = currentMillis;
  }
}

bool paramsChanged() {
  if (waitTimeMemory != waitTime || moveTimeMemory != moveTime)
    return true;
  else
    return false;
}

void saveParams() {
  Serial.print("Zapisuje stop ");
  Serial.println(waitTime);
  Serial.print("Zapisuje ruch ");
  Serial.println(moveTime);
  EEPROM.write(waitTimeLoc, waitTime);
  EEPROM.put(moveTimeLoc, moveTime);
  waitTimeMemory = waitTime;
  moveTimeMemory = moveTime;
  paramsWritten = true;
}

void lcdDisplayPage(unsigned int screenId) {
  switch (screenId) {
    case 0:
      {
        // lcd.setCursor(0, 0);
        // lcd.print(keyPressed);
        lcd.setCursor(2, 0);
        if (motorMovement) {
          lcd.print("RUCH");
        } else {
          lcd.print("STOP");
        }
        lcd.setCursor(3, 1);
        lcd.write(char(0));
        lcd.setCursor(4, 1);
        if (paramsChanged()) {
          lcd.print("!");
          if (keyPressed == btnUP) {
            saveParams();
          }
        } else {
          lcd.write(char(1));
        }

        break;
      }
    case 1:
      {
        lcd.setCursor(0, 0);
        lcd.print("Przerwa");
        lcd.setCursor(0, 1);
        lcd.print(String(waitTime) + "s  ");

        switch (keyPressed) {
          case btnDOWN:
            {
              if (ready)
                --waitTime;
              unready();
              break;
            }
          case btnUP:
            {
              if (ready)
                ++waitTime;
              unready();
              break;
            }
        }
        break;
      }
    case 2:
      {
        lcd.setCursor(0, 0);
        lcd.print("Ruch    ");
        lcd.setCursor(0, 1);
        lcd.print(moveTime);
        lcd.setCursor(4, 1);
        lcd.print("s  ");

        switch (keyPressed) {
          case btnDOWN:
            {
              if (ready)
                moveTime -= 0.1;
              unready();
              break;
            }
          case btnUP:
            {
              if (ready)
                moveTime += 0.1;
              unready();
              break;
            }
        }
        break;
      }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTONS, INPUT);
  pinMode(MOTOR_OUTPUT, OUTPUT);

  //EEPROM.put(moveTimeLoc, 0.5);

  lcd.begin(8, 2);
  lcd.createChar(0, memorySymbol);
  lcd.createChar(1, checkMarkSymbol);

  waitTime = EEPROM.read(waitTimeLoc);
  EEPROM.get(moveTimeLoc, moveTime);
  
  waitTimeMemory = EEPROM.read(waitTimeLoc);
  EEPROM.get(moveTimeLoc, moveTimeMemory);

  lcd.setCursor(0, 0);
  lcd.print(version);
  lcd.setCursor(0, 1);
  lcd.print(date);

  delay(2000);
  lcd.clear();
  lcd.home();
}

void loop() {
  keyPressed = readAnalogButton();

  unsigned long currentMillis = millis();

  moveTime = constrain(moveTime, 0.5, 1.5);
  waitTime = constrain(waitTime, 0, 10);
  screenId = constrain(screenId, 0, 2);

  switch (keyPressed) {
    case btnRIGHT:
      {
        if (ready)
          nextPage();
        unready();
        break;
      }
    case btnLEFT:
      {
        if (ready)
          prevPage();
        unready();
        break;
      }
  }

  if (!ready && keyPressed && currentMillis - previousMillisButton >= readyMillis) {
    ready = true;
    previousMillisButton = currentMillis;
  }

  lcdDisplayPage(screenId);
  if (screenId != prevScreen) {
    lcd.clear();
    prevScreen = screenId;
  }

  motorState();

  if (waitTime == 0)
  {
    motorMovement = true;
  }

  if (motorMovement == true) {
    digitalWrite(MOTOR_OUTPUT, HIGH);
  } else {
    digitalWrite(MOTOR_OUTPUT, LOW);
  }
}