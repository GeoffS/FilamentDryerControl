#pragma once
#include <Arduino.h>
#include <GSSArduTasks.h>
#include "SharedVariables.h"
#include "HeaterControl.h"

class TaskZero: public Task
{
public:
	TaskZero(SharedVariables* sharedVariables) : sv(sharedVariables)
	{
	}

	void setup() override
	{
	}

	long loop(long currTime_ms) override
	{
		// Redundant check.
		// If we're stopped the heater should be off.
		// Make it so...
		if (sv->stopped) heaterOff();

		return 0; // Doesn't matter...
	}

private:
	SharedVariables* const sv;

};
