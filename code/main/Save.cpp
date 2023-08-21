#include <cstring>
#include "Save.hpp"
#include <ESP8266WiFi.h>

SaveEEprom::SaveEEprom()
{
  this->Read();
}

void SaveEEprom::Read()
{
  dataStruct storage;
  
  EEPROM.begin(sizeof(storage)); 
  EEPROM.get(0, storage);
  EEPROM.end();
  this->tempMin = storage.tempMin;
  this->tempMax = storage.tempMax;
  this->treshold = storage.treshold;
  this->calibrate = storage.calibrate;
  Serial.println(String("EEPROM Read 0:") + this->tempMin + " 8:" + this->tempMax);
}
void SaveEEprom::Save()
{
  dataStruct storage;
    
  Serial.println(String("EEPROM Save 0:") + this->tempMin + " 8:" + this->tempMax);
  storage.tempMin = this->tempMin;
  storage.tempMax = this->tempMax;
  storage.treshold = this->treshold;
  storage.calibrate = this->calibrate;
	EEPROM.begin(sizeof(storage)); 
	EEPROM.put(0, storage); 
	EEPROM.commit();
	EEPROM.end();
  this->Read();
}
void SaveEEprom::Set_tempMin(double tempMin)
{
  if (this->tempMin != tempMin)
  {
	  this->tempMin = tempMin;
    this->Save();
  }
}
void SaveEEprom::Set_tempMax(double tempMax)
{
	if (this->tempMax != tempMax)
  {
    this->tempMax = tempMax;
    this->Save();
  }
}
void SaveEEprom::Set_treshold(double treshold)
{
  if (this->treshold != treshold)
  {
    this->treshold = treshold;
    this->Save();
  }
}
void SaveEEprom::Set_calibrate(double calibrate)
{
  if (this->calibrate != calibrate)
  {
    this->calibrate = calibrate;
    this->Save();
  }
}
double SaveEEprom::Get_tempMin()
{
	return this->tempMin;
}
double SaveEEprom::Get_tempMax()
{
	return this->tempMax;
}
double SaveEEprom::Get_treshold()
{
  return this->treshold;
}
double SaveEEprom::Get_calibrate()
{
  return this->calibrate;
}
