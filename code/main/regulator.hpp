class regulator
{
public:
	regulator(double tempMin, double tempMax, double tempMinForce, double tempMaxForce, double treshold);

	void setTempMin(double tempMin) { this->tempMin = tempMin; }
	void setTempMax(double tempMax) { this->tempMax = tempMax; }
	void setTempMinForce(double tempMinForce) { this->tempMinForce = tempMinForce; }
	void setTempMaxForce(double tempMaxForce) { this->tempMaxForce = tempMaxForce; }
	void setTreshold(double treshold) { this->treshold = treshold; }

	double getTempMin(void) { return this->tempMin; }
	double getTempMax(void) { return this->tempMax; }
	double getTempMinForce(void) { return this->tempMinForce; }
	double getTempMaxForce(void) { return this->tempMaxForce; }
	double getTreshold(void) { return this->treshold; }

	bool isOn(double temp, bool release);

private:
	double tempMin = 0;
	double tempMax = 0;
	double tempMinForce = 0;
	double tempMaxForce = 0;
	double treshold = 0.1;
	bool statusOn = false;

	bool isOnRel(double temp, double min, double max);
};
