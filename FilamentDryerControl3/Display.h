#pragma once
#include <Arduino.h>
#include <LiquidCrystal.h>

class Display
{
private:
	LiquidCrystal* lcd;

public:
	Display(LiquidCrystal* sharedLcd)
	{
		lcd = sharedLcd;
	}

	void displayCurrentTemperature(float deg_C)
	{
		lcd->setCursor(0, 1);
		lcd->print(deg_C);
		lcd->setCursor(5, 1);
		lcd->print("C");
	}

	void displayAvgTemperature(float deg_C)
	{
		lcd->setCursor(0, 0);
		lcd->print(deg_C);
		lcd->setCursor(5, 0);
		lcd->print("C");
	}

	void displaySetPointTemperature(float setPoint_C)
	{
		lcd->setCursor(6, 0);
		lcd->print(" -> ");
		lcd->setCursor(10, 0);
		lcd->print(setPoint_C);
		lcd->setCursor(15, 0);
		lcd->print("C");
	}

	void displaySetPointWhileStopped(float setPoint_C)
	{
		lcd->setCursor(7, 0);
		lcd->print("Set");
		lcd->setCursor(10, 0);
		lcd->print(setPoint_C);
		lcd->setCursor(15, 0);
		lcd->print("C");
	}

	void displayCurrentOnTime(unsigned long onTime_ms)
	{
		int timeLcdOffset = 10;
		lcd->setCursor(timeLcdOffset, 1);
		lcd->print("    ms");
		if (onTime_ms <= 9)
			timeLcdOffset += 3; //= 7;
		else if (onTime_ms <= 99)
			timeLcdOffset += 2; // = 6;
		else if (onTime_ms <= 999)
			timeLcdOffset += 1; // = 5;
		lcd->setCursor(timeLcdOffset, 1);
		lcd->print(onTime_ms);
		lcd->setCursor(timeLcdOffset + 4, 1);
		lcd->print("ms");
	}
};
