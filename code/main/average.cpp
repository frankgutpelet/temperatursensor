#include "average.hpp"
#include <cmath>


average::average()
{

    index = 0;
    count = 0;
    sum = 0.0f;
}

void average::setValue(float value)
{
    this->index++;
    if (this->MAX_SIZE == index)
    {
      this->index = 0;
    }

    this->values[this->index] = value;
}

float average::getValue()
{
    float sum = 0;
    for (int i=0;i<this->MAX_SIZE;i++)
    {
      sum += this->values[i];
    }

    return sum / this->MAX_SIZE;
}
