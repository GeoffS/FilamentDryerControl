#include <Arduino.h>

// Include the tasks framework:
#include <GSSArduTasks.h>

// Include the GSSUtil Button and Blinker classes:
#include <Button.h>
#include <Blinker.h>

// Include the thermocouple library:
#include "max6675.h"

// include the LCD library code:
#include <LiquidCrystal.h>

// Uncomment to print debugging info. to the serial monitor:
//#define USE_SERIAL;

#include "Display.h"
#include "PinConfiguration.h"
#include "TaskZero.h"
#include "HeaterControlTask.h"
#include "DisplayTask.h"
#include "CheckControlsTask.h"
#include "SharedVariables.h"

// Configure all the hardware:
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);

Blinker mediumBlink(750, 200);

Button stopStartButton = Button(stopStartButtonPin);
Button upButton = Button(upButtonPin, true);
Button downButton = Button(downButtonPin, true);
Button selectButton = Button(selectButtonPin, true);
Button backButton = Button(backButtonPin, true);

Display display(&lcd);

SharedVariables sv;

// Create all the tasks:
TaskZero task0(&sv);
HeaterControlTask heaterTask(&sv);
DisplayTask displayTask(&sv, &display);
CheckControlsTask checkControlsTask(&sv);

// Setup and run everything:
RUN_TASKS(&task0, &heaterTask, &checkControlsTask, &displayTask)
