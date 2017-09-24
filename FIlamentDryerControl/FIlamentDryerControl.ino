const int RELAY_PIN = 5;
const int LED_PIN = 13;
const int delay_ms = 5000;

#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;

float setPoint_C = 70;
float hysterisis_C = 4;

float upperSetPoint_C = setPoint_C + hysterisis_C / 2;
float mediumLowSetPoint_C = setPoint_C - hysterisis_C / 2;
float lowerSetPoint_C = setPoint_C - hysterisis_C;
float startupTemp_C = 33;

float startupDutyCycle = 1.0;
float lowDutyCycle = 0.6;
float equlibDutyCycle = 0.4;
float mediumLowEqulibDutyCycle = 0.3;

unsigned long startupOnTime_ms = delay_ms * startupDutyCycle;
unsigned long equilibOnTime_ms = delay_ms * equlibDutyCycle;
unsigned long lowOnTime_ms = delay_ms * lowDutyCycle;
unsigned long mediumLowEquilibOnTime_ms = delay_ms * mediumLowEqulibDutyCycle;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

void setup()
{
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  printVarf("startupTemp_C", startupTemp_C);
  printVarf("lowerSetPoint_C", lowerSetPoint_C);
  printVarf("mediumLowSetPoint_C", mediumLowSetPoint_C);
  printVarf("setPoint_C", setPoint_C);
  printVarf("upperSetPoint_C", upperSetPoint_C);

  printVar("startupOnTime_ms", startupOnTime_ms);
  printVar("lowOnTime_ms", lowOnTime_ms);
  printVar("mediumLowEquilibOnTime_ms", mediumLowEquilibOnTime_ms);
  printVar("equilibOnTime_ms", equilibOnTime_ms);

  delay(500);
}

void loop()
{
  float deg_C = ktc.readCelsius();
  Serial.print(deg_C);

  if (deg_C < startupTemp_C)
  {
    heatFor(startupOnTime_ms);
    return;
  }

  if (deg_C < lowerSetPoint_C)
  {
    heatFor(lowOnTime_ms);
    return;
  }

  if (deg_C < mediumLowSetPoint_C)
  {
    heatFor(equilibOnTime_ms);
    return;
  }

  if (deg_C < upperSetPoint_C)
  {
    heatFor(mediumLowEquilibOnTime_ms);
    return;
  }

  // Above the upperSetPoint_C -> turn off the heater:
  heaterOff();
  Serial.println(",0");
  delay(delay_ms);
  return;
}

void heatFor(unsigned long onTime_ms)
{
  Serial.print(",");
  Serial.println(onTime_ms);
  if (onTime_ms > 0)
  {
    heaterOn();
    delay(onTime_ms);
  }
  float offTime_ms = delay_ms - onTime_ms;
  if (offTime_ms > 0)
  {
    heaterOff();
    delay(delay_ms - onTime_ms);
  }
  return;
}

void heaterOn()
{
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
}

void heaterOff()
{
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void printVar(String name, unsigned long value)
{
  Serial.print(name + " ");
  Serial.println(value);
}

void printVarf(String name, float value)
{
  Serial.print(name + " ");
  Serial.println(value);
}

