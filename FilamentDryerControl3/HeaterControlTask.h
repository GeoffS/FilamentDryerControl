#pragma once
#include <Arduino.h>
#include <GSSArduTasks.h>

#include "PinConfiguration.h"
#include "HeaterControl.h"
#include "PrintUtil.h"
#include "SharedVariables.h"
#include "Display.h"

class HeaterControlTask: public Task
{
public:
	HeaterControlTask(SharedVariables* sharedVar) :
			sv(sharedVar)
	{

	}

	void setup() override
	{

	}

	bool needSerial()
	{
#ifdef USE_SERIAL
		return true;
#else
		return false;
#endif
	}

	long loop(long currTime_ms) override
	{
		sv->nextOffTime_ms = 0;
		switch (sv->nextEventId)
		{
		case NO_ACTION:
			break;

		case HEATER_OFF:
			heaterOff();
			sv->nextEventId = START_INTERVAL;
			sv->nextEventTime_ms = sv->nextStartInterval_ms;
			break;

		case START_INTERVAL:
			sv->nextStartInterval_ms = currTime_ms + delay_ms;
			//processTemps();
			sv->nextOffTime_ms = processStartInterval(currTime_ms, sv->avgTemp_C);
			//display->displayCurrentOnTime(nextOffTime_ms);
			break;

		default:
			heaterOff();
			sv->stopped = true;
			return currTime_ms + 200;
		}
		return sv->nextEventTime_ms;
	}

private:
	SharedVariables* const sv;

	unsigned long processStartInterval(unsigned long now, float currTemp_C)
	{
		unsigned long nextOnTime_ms = calcOnTime(currTemp_C);

		if (nextOnTime_ms > 0)
		{
			// Turn the heater on and setup for the heater-off event:
			heaterOn();
			sv->nextEventId = HEATER_OFF;
			sv->nextEventTime_ms = now + nextOnTime_ms;
		}
		else
		{
			// No on-time.
			// Make sure the heater is off and setup for the next interrval start:
			heaterOff();
			sv->nextEventId = START_INTERVAL;
			sv->nextEventTime_ms = sv->nextStartInterval_ms;
		}

		return nextOnTime_ms;
	}

	unsigned long calcOnTime(float currTemp_C)
	{
		printVar_f("calcOnTime Temp", currTemp_C);
		//display->displayAvgTemperature(currTemp_C);

		if (currTemp_C < sv->startupTemp_C)
		{
			return startupOnTime_ms;
		}

		if (currTemp_C < sv->lowerSetPoint_C)
		{
			return lowOnTime_ms;
		}

		if (currTemp_C < sv->mediumLowSetPoint_C)
		{
			return equilibOnTime_ms;
		}

		if (currTemp_C < sv->upperSetPoint_C)
		{
			float delta_C = currTemp_C - sv->setPoint_C;

			sv->mediumLowEquilibOnTime_ms -= delta_C / 2
					* sv->mediumLowEquilibOnTimeDelta_ms;
			if (sv->mediumLowEquilibOnTime_ms < 500)
				sv->mediumLowEquilibOnTime_ms = 500;
			printVar_f("mediumLowEquilibOnTime_ms (hot)",
					sv->mediumLowEquilibOnTime_ms);

			return sv->mediumLowEquilibOnTime_ms;
		}

		// Above the upperSetPoint_C:
		//  1) Turn off the heater
		//  2) Reduce medmediumLowEquilibOnTime_ms by 500ms;
		float delta_C = sv->currTemp_C - sv->setPoint_C;
		sv->mediumLowEquilibOnTime_ms -= delta_C
				* sv->mediumLowEquilibOnTimeDelta_ms;
		if (sv->mediumLowEquilibOnTime_ms < 500)
			sv->mediumLowEquilibOnTime_ms = 500;
		printVar_f("mediumLowEquilibOnTime_ms (vhot)",
				sv->mediumLowEquilibOnTime_ms);
		return 0;
	}
};
