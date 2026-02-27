#pragma once
#include <Arduino.h>

class average
{
public:
    average(uint8_t avgNo, float tolerance);

    void setValue(float value);
    float getValue() const;

private:
    float* buffer;
    uint8_t size;
    uint8_t index;
    uint8_t count;

    float tolerance;
    float sum;
};