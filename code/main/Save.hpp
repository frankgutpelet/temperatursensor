#include <ESP_EEPROM.h>

class SaveEEprom
{
  typedef struct
  {
    double tempMin;
    double tempMax;
    double tempMinForce;
    double tempMaxForce;
    double treshold;
    double calibrate;
  }dataStruct;
  
	public:
    SaveEEprom();
	  void Save();
    void Read();
    void Set_tempMin(double tempMin);
	  void Set_tempMax(double tempMax);
   void Set_tempMinForce(double tempMinForce);
   void Set_tempMaxForce(double tempMaxForce);
    void Set_treshold(double treshold);
    void Set_calibrate(double calibrate);
	  double Get_tempMin();
	  double Get_tempMax();
   double Get_tempMinForce();
   double Get_tempMaxForce();
    double Get_treshold();
    double Get_calibrate();
	
	private:
	  double tempMin;
	  double tempMax;
   double tempMinForce;
   double tempMaxForce;
    double treshold;
    double calibrate;
};
