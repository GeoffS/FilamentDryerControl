#pragma once
#include <Arduino.h>
#include <GSSArduTasks.h>

class TaskZero : Task
{
public:
    void setup() override
    {

    }

    long loop(long currTime_ms) override
    {

      return currTime_ms + 200;
    }
};
