#pragma once

class SharedVariables
{
public:
	volatile int nextEventId = NO_ACTION;
	volatile bool stopped = true;
	unsigned long nextEventTime_ms;
	unsigned long nextStartInterval_ms;

	volatile float setPoint_C = 50;
	volatile float hysterisis_C = 4;
	volatile float startupTemp_C = setPoint_C - 20;

	volatile float upperSetPoint_C = setPoint_C + hysterisis_C / 2;
	volatile float mediumLowSetPoint_C = setPoint_C - hysterisis_C / 2;
	volatile float lowerSetPoint_C = setPoint_C - hysterisis_C;

#define MAX_LAST_TEMPS 10
	volatile float lastTemps_C[MAX_LAST_TEMPS];
	volatile int lastTempsCount = 0;
	volatile float avgTemp_C = -999;
	volatile float lastTempsFull = false;

	volatile unsigned long startupOnTime_ms = delay_ms * startupDutyCycle;
	volatile unsigned long equilibOnTime_ms = delay_ms * equlibDutyCycle;
	volatile unsigned long lowOnTime_ms = delay_ms * lowDutyCycle;
	volatile unsigned long mediumLowEquilibOnTime_ms = delay_ms * mediumLowEqulibDutyCycle;

	volatile unsigned long mediumLowEquilibOnTimeDelta_ms = 5;

	volatile float currTemp_C = 99.00;
};
