#pragma once
#include <Arduino.h>
#include <GSSArduTasks.h>
#include "SharedVariables.h"

class CheckControlsTask: public Task
{
public:
	CheckControlsTask(SharedVariables* sharedVariables) : sv(sharedVariables)
	{
	}

	void setup() override
	{
	}

	long loop(long currTime_ms) override
	{


		return currTime_ms + 100;
	}

private:
	SharedVariables* const sv;

};





