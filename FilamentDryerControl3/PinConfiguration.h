#pragma once
#include <Arduino.h>

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

// Event types:
#define NO_ACTION 0
#define HEATER_OFF 1
#define START_INTERVAL 2

// Heater PWM period:
const int delay_ms = 5000;
