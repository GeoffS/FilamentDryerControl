#pragma once

class ControlVariables
{
public:
	volatile float setPoint_C = 50;
	volatile float hysterisis_C = 4;
	volatile float startupTemp_C = setPoint_C - 20;

	volatile float upperSetPoint_C = setPoint_C + hysterisis_C / 2;
	volatile float mediumLowSetPoint_C = setPoint_C - hysterisis_C / 2;
	volatile float lowerSetPoint_C = setPoint_C - hysterisis_C;
};
