#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Button Pins
#define WATER_BTN D5
#define FOOD_BTN D6
#define EMERGENCY_BTN D7

// Buzzer Pin
#define BUZZER D3

String lastMessage = "";

//---------------- LCD Function ----------------//
void showMessage(String msg)
{
  if (msg != lastMessage)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);
    lastMessage = msg;
  }
}

//---------------- Buzzer Functions ----------------//

// Need Water -> 2 short beeps
void waterBeep()
{
  for (int i = 0; i < 2; i++)
  {
    tone(BUZZER, 2000);
    delay(150);
    noTone(BUZZER);
    delay(150);
  }
}

// Need Food -> 3 short beeps
void foodBeep()
{
  for (int i = 0; i < 3; i++)
  {
    tone(BUZZER, 2000);
    delay(150);
    noTone(BUZZER);
    delay(150);
  }
}

// Emergency Tune
void emergencyBeep()
{
  while (digitalRead(EMERGENCY_BTN) == LOW)
  {
    // Short beep
    tone(BUZZER, 2500);
    delay(150);
    noTone(BUZZER);
    delay(150);

    // Long beep (2 seconds)
    tone(BUZZER, 1500);
    delay(2000);
    noTone(BUZZER);
    delay(150);

    // Short beep
    tone(BUZZER, 2500);
    delay(150);
    noTone(BUZZER);
    delay(150);

    // Long beep (2 seconds)
    tone(BUZZER, 1500);
    delay(2000);
    noTone(BUZZER);
    delay(300);
  }
}

//---------------- Setup ----------------//

void setup()
{
  lcd.init();
  lcd.backlight();

  pinMode(WATER_BTN, INPUT_PULLUP);
  pinMode(FOOD_BTN, INPUT_PULLUP);
  pinMode(EMERGENCY_BTN, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  showMessage("All is Fine");
}

//---------------- Loop ----------------//

void loop()
{
  if (digitalRead(WATER_BTN) == LOW)
  {
    showMessage("Need Water");
    waterBeep();

    while (digitalRead(WATER_BTN) == LOW)
      delay(10);
  }

  else if (digitalRead(FOOD_BTN) == LOW)
  {
    showMessage("Need Food");
    foodBeep();

    while (digitalRead(FOOD_BTN) == LOW)
      delay(10);
  }

  else if (digitalRead(EMERGENCY_BTN) == LOW)
  {
    showMessage("Emergency");
    emergencyBeep();

    while (digitalRead(EMERGENCY_BTN) == LOW)
      delay(10);
  }

  else
  {
    showMessage("All is Fine");
  }

  delay(50);
}