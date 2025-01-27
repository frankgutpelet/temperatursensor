#include "regulator.hpp"
#include "Arduino.h"

regulator::regulator(double tempMin, double tempMax, double tempMinForce, double tempMaxForce, double treshold)
{
	this->tempMin = tempMin;
	this->tempMax = tempMax;
	this->tempMinForce = tempMinForce;
	this->tempMaxForce = tempMaxForce;
	this->treshold = treshold;
}

bool regulator::isOn(double temp, bool release)
{
	if (release)
	{
    Serial.println("Release");
		return this->isOnRel(temp, this->tempMin, this->tempMax);
	}
	else
	{
    Serial.println("!Release");
		return this->isOnRel(temp, this->tempMinForce, this->tempMaxForce);
	}

}

bool regulator::isOnRel(double temp, double min, double max)
{
	if (!this->statusOn)
	{
		if (temp > max)
		{
      Serial.println("temp > max");
			this->statusOn = true;
		}
    else if (temp < min)
    {
      Serial.println("temp < min");
      this->statusOn = true;
    }
		else
		{
			this->statusOn = false;
		}
	}
	else
	{
		if (temp <= (min + treshold))
		{
      Serial.println("temp <= (min + treshold)");
			this->statusOn = true;
		}
    else if (temp >= (max - treshold))
    {
      Serial.println("temp >= (max - treshold");
      this->statusOn = true;
    }
		else
		{
			this->statusOn = false;
		}
	}
	return this->statusOn;
}
