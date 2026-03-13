#pragma once

#include <stdint.h>

class average
{
public:
    average();

    void setValue(float value);
    float getValue();

private:
    static constexpr uint8_t MAX_SIZE = 20;

    float values[MAX_SIZE];  

    uint8_t index;
    uint8_t count;

    float sum;
    float tolerance;
};
