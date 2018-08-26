#pragma once

#include <Arduino.h>
#include <GSSArduTasks.h>
#include "SharedVariables.h"
#include "HeaterControl.h"
#include "Display.h"

class DisplayTask: public Task
{
public:
	DisplayTask(SharedVariables* sharedVariables, Display* display) :
		sv(sharedVariables), disp(display)
	{
	}

	void setup() override
	{
	}

	long loop(long currTime_ms) override
	{

		disp->displayCurrentOnTime(sv->nextOffTime_ms);
		disp->displayAvgTemperature(sv->avgTemp_C);

		return currTime_ms + 100;
	}

private:
	SharedVariables* const sv;
	Display* const disp;
};





