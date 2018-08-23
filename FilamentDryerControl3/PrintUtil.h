#pragma once

#include <Arduino.h>

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
