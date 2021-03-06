#define FDC_VERSION "2.1.2"

//#define USE_SERIAL

#include <Button.h>
#include <Blinker.h>
// Include the thermocouple library:
#include "max6675.h"

// include the LCD library code:
#include <LiquidCrystal.h>

const int d7        =  2; // LCD
const int d6        =  3; // LCD
const int d5        =  4; // LCD
const int RELAY_PIN =  5; // Oven Control
const int d4        =  6; // LCD
const int ktcSO     =  8; // MAX6675 TC boards
const int ktcCS     =  9; // MAX6675 TC boards
const int ktcCLK    = 10; // MAX6675 TC boards
const int en        = 11; // LCD
const int rs        = 12; // LCD
const int LED_PIN   = 13; // Oven status LED

const int downButtonPin = A0;
const int stopStartButtonPin = A1;
const int backButtonPin = A2;
const int upButtonPin = A3;
const int selectButtonPin = A4;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

#define BOOL2HIGHLOW ?HIGH:LOW

Blinker mediumBlink(750, 200);

Button stopStartButton = Button(stopStartButtonPin);
Button upButton        = Button(upButtonPin, true);
Button downButton      = Button(downButtonPin, true);
Button selectButton    = Button(selectButtonPin, true);
Button backButton      = Button(backButtonPin, true);

ULONG zeroTime_ms;

const int delay_ms = 5000;

float setPoint_C = 50;
float hysterisis_C = 4;
float startupTemp_C = setPoint_C - 20;

float upperSetPoint_C = setPoint_C + hysterisis_C / 2;
float mediumLowSetPoint_C = setPoint_C - hysterisis_C / 2;
float lowerSetPoint_C = setPoint_C - hysterisis_C;

float startupDutyCycle = 1.0;
float lowDutyCycle = 0.6;
float equlibDutyCycle = 0.4;
float mediumLowEqulibDutyCycle = 0.3;

unsigned long startupOnTime_ms = delay_ms * startupDutyCycle;
unsigned long equilibOnTime_ms = delay_ms * equlibDutyCycle;
unsigned long lowOnTime_ms = delay_ms * lowDutyCycle;
unsigned long mediumLowEquilibOnTime_ms = delay_ms * mediumLowEqulibDutyCycle;

unsigned long mediumLowEquilibOnTimeDelta_ms = 5;

bool stopped = true;

// Event types:
#define NO_ACTION 0
#define HEATER_OFF 1
#define START_INTERVAL 2

unsigned long nextEventTime_ms;
unsigned long nextStartInterval_ms;
int nextEventId = NO_ACTION;

#define MAX_LAST_TEMPS 10
float lastTemps_C[MAX_LAST_TEMPS];
int lastTempsCount = 0;
float avgTemp_C = -999;
float lastTempsFull = false;

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

#ifdef USE_SERIAL
  Serial.begin(9600);
#endif

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.clear();
  // Print a message to the LCD.
  lcd.setCursor(0, 0); lcd.print("Filament Dryer");
  lcd.setCursor(0, 1); lcd.print(FDC_VERSION);
  delay(1000);
  lcd.clear();
  heaterOff();
  updateControlVariables();
  displaySetPointWhileStopped(setPoint_C);

  zeroTime_ms = millis();
}

unsigned long nextTempReading_ms = 0;
float currTemp_C = 99.00;

void loop()
{
  unsigned long now = millis();

  if (now >= nextTempReading_ms)
  {
    currTemp_C = ktc.readCelsius();
    //printVar_f("currTemp_C", currTemp_C);
    displayCurrentTemperature(currTemp_C);
    //    if (currTemp_C == NAN)
    //    {
    //      stopped = true;
    //      return;
    //    }
    lastTemps_C[lastTempsCount] = currTemp_C;
    if (!lastTempsFull && lastTempsCount >= (MAX_LAST_TEMPS - 1)) lastTempsFull = true;
    lastTempsCount = (lastTempsCount + 1) % MAX_LAST_TEMPS;

    nextTempReading_ms += 500;

    if (!lastTempsFull) return;
  }

  processTemps();

  // Check the start-stop button:
  stopStartButton.checkButtonState();

  if (stopped)
  {
    if (stopStartButton.wasClicked())
    {
      stopped = false;
      lcd.clear();
      displayCurrentTemperature(currTemp_C);
      displaySetPointTemperature(setPoint_C);
      stopStartButton.resetClicked();
      nextEventTime_ms = now;
      nextEventId = START_INTERVAL;
      return;
    }

    // Check the other buttons only when we're stopped:
    upButton.checkButtonState();
    downButton.checkButtonState();
    selectButton.checkButtonState();
    backButton.checkButtonState();

    if (upButton.wasClicked())
    {
      setPoint_C += 5.0;
      updateControlVariables();
      displaySetPointWhileStopped(setPoint_C);
    }
    else if (downButton.wasClicked())
    {
      setPoint_C -= 5.0;
      updateControlVariables();
      displaySetPointWhileStopped(setPoint_C);
    }


    return;
  }

  if (stopStartButton.wasClicked())
  {
    stopped = true;
    lcd.clear();
    heaterOff();
    displayCurrentTemperature(currTemp_C);
    displaySetPointWhileStopped(setPoint_C);
  }

  if (now >= nextEventTime_ms)
  {
    unsigned long nextOffTime_ms = 0;
    switch (nextEventId)
    {
      case NO_ACTION:
        break;

      case HEATER_OFF:
        heaterOff();
        nextEventId = START_INTERVAL;
        nextEventTime_ms = nextStartInterval_ms;
        break;

      case START_INTERVAL:
        nextStartInterval_ms = now + delay_ms;
        //processTemps();
        nextOffTime_ms = processStartInterval(now, avgTemp_C);
        displayCurrentOnTime(nextOffTime_ms);
        break;

      default:
        heaterOff();
        stopped = true;
    }
  }
}

void processTemps()
{
  avgTemp_C = 0.0;
  for (int i = 0; i < MAX_LAST_TEMPS; i++) avgTemp_C += lastTemps_C[i];
  avgTemp_C /= MAX_LAST_TEMPS;
  //displayAvgTemperature(avgTemp_C);
  //lastTempsCount = 0;
}

unsigned long processStartInterval(unsigned long now, float currTemp_C)
{
  unsigned long nextOnTime_ms = calcOnTime(currTemp_C);

  if (nextOnTime_ms > 0)
  {
    // Turn the heater on and setup for the heater-off event:
    heaterOn();
    nextEventId = HEATER_OFF;
    nextEventTime_ms = now + nextOnTime_ms;
  }
  else
  {
    // No on-time.
    // Make sure the heater is off and setup for the next interrval start:
    heaterOff();
    nextEventId = START_INTERVAL;
    nextEventTime_ms = nextStartInterval_ms;
  }

  return nextOnTime_ms;
}

unsigned long calcOnTime(float currTemp_C)
{
  printVar_f("calcOnTime Temp", currTemp_C);
  displayAvgTemperature(currTemp_C);

  if (currTemp_C < startupTemp_C)
  {
    return startupOnTime_ms;
  }

  if (currTemp_C < lowerSetPoint_C)
  {
    return lowOnTime_ms;
  }

  if (currTemp_C < mediumLowSetPoint_C)
  {
    return equilibOnTime_ms;
  }

  if (currTemp_C < upperSetPoint_C)
  {
    float delta_C = currTemp_C - setPoint_C;

    mediumLowEquilibOnTime_ms -= delta_C / 2 * mediumLowEquilibOnTimeDelta_ms;
    if (mediumLowEquilibOnTime_ms < 500) mediumLowEquilibOnTime_ms = 500;
    printVar_f("mediumLowEquilibOnTime_ms (hot)", mediumLowEquilibOnTime_ms);

    return mediumLowEquilibOnTime_ms;
  }

  // Above the upperSetPoint_C:
  //  1) Turn off the heater
  //  2) Reduce medmediumLowEquilibOnTime_ms by 500ms;
  float delta_C = currTemp_C - setPoint_C;
  mediumLowEquilibOnTime_ms -= delta_C * mediumLowEquilibOnTimeDelta_ms;
  if (mediumLowEquilibOnTime_ms < 500) mediumLowEquilibOnTime_ms = 500;
  printVar_f("mediumLowEquilibOnTime_ms (vhot)", mediumLowEquilibOnTime_ms);
  return 0;
}

void updateControlVariables()
{
  upperSetPoint_C = setPoint_C + hysterisis_C / 2;
  mediumLowSetPoint_C = setPoint_C - hysterisis_C / 2;
  lowerSetPoint_C = setPoint_C - hysterisis_C;
  startupTemp_C = setPoint_C - 20;

  startupDutyCycle = 1.0;
  lowDutyCycle = 0.6;
  equlibDutyCycle = 0.4;
  mediumLowEqulibDutyCycle = 0.3;

  startupOnTime_ms = delay_ms * startupDutyCycle;
  equilibOnTime_ms = delay_ms * equlibDutyCycle;
  lowOnTime_ms = delay_ms * lowDutyCycle;
  mediumLowEquilibOnTime_ms = delay_ms * mediumLowEqulibDutyCycle;
  printVar_f("mediumLowEquilibOnTime_ms (update)", mediumLowEquilibOnTime_ms);

  printControlVariables();
}

void heaterOn()
{
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  //lcd.setCursor(6, 1); lcd.print(" ON");
}

void heaterOff()
{
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  //lcd.setCursor(6, 1); lcd.print("OFF");
}

void displayCurrentTemperature(float deg_C)
{
  lcd.setCursor(0, 1); lcd.print(deg_C);
  lcd.setCursor(5, 1); lcd.print("C");
}

void displayAvgTemperature(float deg_C)
{
  lcd.setCursor(0, 0); lcd.print(deg_C);
  lcd.setCursor(5, 0); lcd.print("C");
}

void displaySetPointTemperature(float setPoint_C)
{
  lcd.setCursor(6, 0); lcd.print(" -> ");
  lcd.setCursor(10, 0); lcd.print(setPoint_C);
  lcd.setCursor(15, 0); lcd.print("C");
}

void displaySetPointWhileStopped(float setPoint_C)
{
  lcd.setCursor( 7, 0); lcd.print("Set");
  lcd.setCursor(10, 0); lcd.print(setPoint_C);
  lcd.setCursor(15, 0); lcd.print("C");
}

void displayCurrentOnTime(unsigned long onTime_ms)
{
  int timeLcdOffset = 10;
  lcd.setCursor(timeLcdOffset, 1); lcd.print("    ms");
  if (onTime_ms <= 9) timeLcdOffset += 3; //= 7;
  else if (onTime_ms <= 99) timeLcdOffset += 2; // = 6;
  else if (onTime_ms <= 999) timeLcdOffset += 1; // = 5;
  lcd.setCursor(timeLcdOffset, 1); lcd.print(onTime_ms);
  lcd.setCursor(timeLcdOffset + 4, 1); lcd.print("ms");
}

void printVar_ul(String name, unsigned long value)
{
#ifdef USE_SERIAL
  Serial.print(name + " ");
  Serial.println(value);
#endif
}

void printVar_f(String name, float value)
{
#ifdef USE_SERIAL
  Serial.print(name + " ");
  Serial.println(value);
#endif
}

void printVar_b(String name, bool value)
{
#ifdef USE_SERIAL
  Serial.print(name + " ");
  Serial.println(value ? "true" : "false");
#endif
}

void printControlVariables()
{
#ifdef USE_SERIAL
  printVar_f("startupTemp_C", startupTemp_C);
  printVar_f("lowerSetPoint_C", lowerSetPoint_C);
  printVar_f("mediumLowSetPoint_C", mediumLowSetPoint_C);
  printVar_f("setPoint_C", setPoint_C);
  printVar_f("upperSetPoint_C", upperSetPoint_C);

  printVar_f("startupOnTime_ms", startupOnTime_ms);
  printVar_f("lowOnTime_ms", lowOnTime_ms);
  printVar_f("mediumLowEquilibOnTime_ms", mediumLowEquilibOnTime_ms);
  printVar_f("equilibOnTime_ms", equilibOnTime_ms);
#endif
}

