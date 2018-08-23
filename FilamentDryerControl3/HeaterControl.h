#pragma once

#include <Arduino.h>

#include "PinConfiguration.h"

static void heaterOn()
{
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  //lcd.setCursor(6, 1); lcd.print(" ON");
}

static void heaterOff()
{
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  //lcd.setCursor(6, 1); lcd.print("OFF");
}
