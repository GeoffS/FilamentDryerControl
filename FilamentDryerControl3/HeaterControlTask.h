#pragma once
#include <Arduino.h>
#include <GSSArduTasks.h>

#include "PinConfiguration.h"
#include "HeaterControl.h"
#include "PrintUtil.h"
#include "SharedVariables.h"

class HeaterControlTask: Task
{
public:
	HeaterControlTask(
			int* const sharedNextEventId,
			bool* sharedStopped,
			unsigned long* const sharedNextEventTime_ms,
			unsigned long* const sharedNextStartInterval_ms,
			SharedVariables* const sharedVar,
			Display* const sharedLcd) :
				nextEventId(sharedNextEventId),
				stopped(sharedStopped),
				nextEventTime_ms(sharedNextEventTime_ms),
				nextStartInterval_ms(sharedNextStartInterval_ms),
				sv(sharedVar),
				display(sharedLcd)
	{

	}

	void setup() override
	{

	}

	long loop(long currTime_ms) override
	{
		unsigned long nextOffTime_ms = 0;
		switch (*nextEventId)
		{
		case NO_ACTION:
			break;

		case HEATER_OFF:
			heaterOff();
			*nextEventId = START_INTERVAL;
			*nextEventTime_ms = *nextStartInterval_ms;
			break;

		case START_INTERVAL:
			*nextStartInterval_ms = currTime_ms + delay_ms;
			//processTemps();
			nextOffTime_ms = processStartInterval(currTime_ms, sv->avgTemp_C);
			display->displayCurrentOnTime(nextOffTime_ms);
			break;

		default:
			heaterOff();
			*stopped = true;
			return currTime_ms + 200;
		}
	}

private:
	volatile int* const nextEventId;
	volatile bool* const stopped;
	volatile unsigned long* const nextEventTime_ms;
	volatile unsigned long* const nextStartInterval_ms;
	SharedVariables* const sv;
	Display* const display;

	unsigned long processStartInterval(unsigned long now, float currTemp_C)
	{
		unsigned long nextOnTime_ms = calcOnTime(currTemp_C);

		if (nextOnTime_ms > 0)
		{
			// Turn the heater on and setup for the heater-off event:
			heaterOn();
			*nextEventId = HEATER_OFF;
			*nextEventTime_ms = now + nextOnTime_ms;
		}
		else
		{
			// No on-time.
			// Make sure the heater is off and setup for the next interrval start:
			heaterOff();
			*nextEventId = START_INTERVAL;
			*nextEventTime_ms = *nextStartInterval_ms;
		}

		return nextOnTime_ms;
	}

	unsigned long calcOnTime(float currTemp_C)
	{
		printVar_f("calcOnTime Temp", currTemp_C);
		display->displayAvgTemperature(currTemp_C);

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
		sv->mediumLowEquilibOnTime_ms -= delta_C * sv->mediumLowEquilibOnTimeDelta_ms;
		if (sv->mediumLowEquilibOnTime_ms < 500)
			sv->mediumLowEquilibOnTime_ms = 500;
		printVar_f("mediumLowEquilibOnTime_ms (vhot)",
				sv->mediumLowEquilibOnTime_ms);
		return 0;
	}
};
