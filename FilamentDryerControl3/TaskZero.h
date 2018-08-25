#pragma once
#include <Arduino.h>
#include <GSSArduTasks.h>

class TaskZero : public Task
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
