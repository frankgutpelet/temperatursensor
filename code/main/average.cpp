#include "average.hpp"

average::average(uint8_t avgNo, float tol)
{
    size = avgNo;
    tolerance = tol;

    buffer = new float[size];
    index = 0;
    count = 0;
    sum = 0.0f;
}

void average::setValue(float value)
{
    // Wenn bereits Werte vorhanden sind → Toleranz prüfen
    if (count > 0)
    {
        float currentAvg = sum / count;
        if (abs(value - currentAvg) > tolerance)
        {
            return; // Wert ignorieren
        }
    }

    // Wenn Puffer voll → alten Wert abziehen
    if (count == size)
    {
        sum -= buffer[index];
    }
    else
    {
        count++;
    }

    buffer[index] = value;
    sum += value;

    index++;
    if (index >= size)
        index = 0;
}

float average::getValue() const
{
    if (count == 0)
        return 0.0f;

    return sum / count;
}
