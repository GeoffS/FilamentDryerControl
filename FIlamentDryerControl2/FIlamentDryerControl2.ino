#define FDC_VERSION "2.0.0"

// Stringify macro expansion...
#define xstr(s) str(s)
#define str(s) #s

#define HARDWARE_INCLUDE xstr(HARDWARE.h)
#define SSM_INCLUDE xstr(SSM_VERSION.h)

#include <Button.h>
#include <Blinker.h>
// Include the thermocouple library:
#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;

const int RELAY_PIN = 5;
const int LED_PIN = 13;

// include the LCD library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 6, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BOOL2HIGHLOW ?HIGH:LOW

int stopStartButtonPin = A1;
int upButtonPin = A3;
int downButtonPin = A0;
int selectButtonPin = A4;
int backButtonPin = A2;

Blinker mediumBlink(750, 200);

Button stopStartButton = Button(stopStartButtonPin);

Button upButton = Button(upButtonPin, true);
Button downButton = Button(downButtonPin, true);
Button selectButton = Button(selectButtonPin, true);
Button backButton = Button(backButtonPin, true);

ULONG zeroTime_ms;

void setup()
{
//  pinMode(RELAY_PIN, OUTPUT);
//  pinMode(LED_PIN, OUTPUT);
//
//  digitalWrite(RELAY_PIN, LOW);
//  digitalWrite(LED_PIN, LOW);

  //MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0); lcd.print("Filament Dryer");
  lcd.setCursor(0, 1); lcd.print(FDC_VERSION);
  delay(500);
  lcd.setCursor(0, 0); lcd.print("              ");
  lcd.setCursor(0, 1); lcd.print("              ");

  zeroTime_ms = millis();
}

bool stopped = true;

void loop()
{
  unsigned long now = millis();

  //mediumBlink.updateLedOnFlag(&now);

  // Check the buttons:
  stopStartButton.checkButtonState();
  upButton.checkButtonState();
  downButton.checkButtonState();
  selectButton.checkButtonState();
  backButton.checkButtonState();

  lcd.setCursor(0, 0); lcd.print(now);

  showButton("St", stopStartButton, 0);
  showButton("Up", upButton, 3);
  showButton("Dn", downButton, 7);
  showButton("Sl", selectButton, 10);
  showButton("Bk", backButton, 13);
}

void showButton(String id, Button button, int offset)
{
  if (button.isPressed()) { lcd.setCursor(offset, 1); lcd.print(  id); }
  else                    { lcd.setCursor(offset, 1); lcd.print("  "); }
}


