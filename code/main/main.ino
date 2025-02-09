#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <TimerEvent.h>
#include "base.hpp"
#include "Logger.hpp"
#include "OneWire.h"
#include <DallasTemperature.h>
#include "Save.hpp"
#include "regulator.hpp"

const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";
const char* myhostname = "WasserbettSabrina";

const char* title = "Temperatursteuerung";
const unsigned int timerOnePeriod = 1000;
const unsigned int relOffBreakSeconds = 2; //2 minutes
unsigned int relOffBreakCounter = 0;
double tempMax;
double tempMin;
double tempMaxForce;
double tempMinForce;
double lastTemp = 0.0;
int relaisPin = 16; //D0 @ nodeMCU
regulator tempRegulator(0,0,0,0,0);
String fwVersion = "Version 1.2";
ESP8266WebServer server(80);
base indexPage(&server);
Logger* logger = Logger::instance();
TimerEvent timerOne;
bool relStatus = false;
bool allow = false;
SaveEEprom storage;

enum e_mode {
  ON,
  OFF,
  AUTO
};

e_mode switchMode = AUTO;

OneWire oneWire(4); //D2 @ nodeMCU
DallasTemperature sensors(&oneWire);

void handleSubmit() 
{
  String newMode = indexPage.Get_mode();
  
  if (String("") != indexPage.Get_outSetTempMax())
  {
    double tempMax = indexPage.Get_outSetTempMax().toDouble();
    storage.Set_tempMax(tempMax);
    indexPage.Set_outSetTempMax("");
    tempRegulator.setTempMax(tempMax);
  }
  else if (String("") != indexPage.Get_outSetTempMin())
  {
    double tempMin = indexPage.Get_outSetTempMin().toDouble();
    storage.Set_tempMin(tempMin);
    indexPage.Set_outSetTempMin("");
    tempRegulator.setTempMin(tempMin);
  }
  else if (String("") != indexPage.Get_outSetTempMaxForce())
  {
    double tempMaxForce = indexPage.Get_outSetTempMaxForce().toDouble();
    storage.Set_tempMaxForce(tempMaxForce);
    indexPage.Set_outSetTempMax("");
    tempRegulator.setTempMaxForce(tempMaxForce);
  }
  else if (String("") != indexPage.Get_outSetTempMinForce())
  {
    double tempMinForce = indexPage.Get_outSetTempMinForce().toDouble();
    storage.Set_tempMinForce(tempMinForce);
    indexPage.Set_outSetTempMin("");
    tempRegulator.setTempMinForce(tempMinForce);
  }
  else if (String("") != indexPage.Get_outTreshold())
  {
    double treshold = indexPage.Get_outTreshold().toDouble();
    Serial.println(indexPage.Get_outTreshold() + " double: " + treshold);
    storage.Set_treshold(treshold);
    tempRegulator.setTreshold(treshold);
  }
  else if (String("") != indexPage.Get_outCalibrate())
  {
    storage.Set_calibrate(indexPage.Get_outCalibrate().toDouble());
    indexPage.Set_outCalibrate("");
  }
  else if (String("OFF") == newMode)
  {
    indexPage.Set_mode("AUTO");
    switchMode = AUTO;
  }
  else if (String("AUTO") == newMode)
  {
    indexPage.Set_mode("ON");
    switchMode = ON;
  }
  else if (String("ON") == newMode)
  {
    indexPage.Set_mode("OFF");
    switchMode = OFF;
  }
  Render();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void Render()
{
  indexPage.Set_headline(String(title) + " - " + myhostname);
  indexPage.Render();
}

void Release()
{
  String cmd = server.arg("cmnd");
  Serial.println(cmd);
  if (String("Power on") == cmd)
  {
    indexPage.Set_allow("On");
    allow = true;
  }
  else if (String("Power off") == cmd)
  {
    indexPage.Set_allow("Off");
    allow = false;
    relStatus = false;
  }
  else
  {
    server.send(404, "text/plain", String("Bad Command: ") + cmd);
    return;
  }
  server.send(200, "text/plain", cmd);
}
void sendTemp()
{
  server.send(200, "text/plain", indexPage.Get_temp());
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(myhostname);
  WiFi.begin(ssid, password);
  sensors.begin();


  server.on("/", Render);
  server.on("/cm", Release);
  server.on("/temp", sendTemp);
  indexPage.SetCallback_submit(handleSubmit); 
  indexPage.Set_mode("AUTO");
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  timerOne.set(timerOnePeriod, timerOneFunc);
  indexPage.Set_headline(String(title) + " - " + myhostname);
  indexPage.Set_version(fwVersion);
  pinMode(relaisPin, OUTPUT);

  tempRegulator.setTempMin(storage.Get_tempMin());
  tempRegulator.setTempMax(storage.Get_tempMax());
  tempRegulator.setTempMinForce(storage.Get_tempMinForce());
  tempRegulator.setTempMaxForce(storage.Get_tempMaxForce());
  tempRegulator.setTreshold(storage.Get_treshold());
}

void loop(void) {
  server.handleClient();
  timerOne.update();
}

void timerOneFunc()
{
  double temp = 0.0;
  double treshold = 0.0;
  double tempMin = 0.0;
  double tempMax = 0.0;
  String tempString = "";
  
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0) + storage.Get_calibrate();
  if (-50.0 < temp)
  {
    lastTemp = temp;
  }


  treshold = storage.Get_treshold();
  //Serial.println(String(indexPage.Get_outSetTempMax()) + "°C");
  //Serial.println(String(indexPage.Get_outSetTempMin()) + "°C");
   
  tempString = String(lastTemp);
  //tempString = tempString.substring(tempString.length() - 2);
  indexPage.Set_temp(tempString);  
  tempMax= storage.Get_tempMax();
  tempMin= storage.Get_tempMin();
  tempMaxForce= storage.Get_tempMaxForce();
  tempMinForce= storage.Get_tempMinForce();
  indexPage.Set_setTempMax(String(tempMax));
  indexPage.Set_setTempMin(String(tempMin));
  indexPage.Set_setTempMaxForce(String(tempMaxForce));
  indexPage.Set_setTempMinForce(String(tempMinForce));
  indexPage.Set_calibrate(String(storage.Get_calibrate()));
  indexPage.Set_treshold(String(storage.Get_treshold()));

  if (OFF == switchMode)
  {
    relStatus = false;
  }
  else if (ON == switchMode)
  {
    relStatus = true;
  }
  else
  {
    relStatus = tempRegulator.isOn(lastTemp, allow);
  }
  
  Serial.println(String("tempMax:") + tempRegulator.getTempMax() + "°C; tempMin:" + tempRegulator.getTempMin() + "°C; tempMinForce:" + tempRegulator.getTempMinForce() + "; tempMaxForce:" + tempRegulator.getTempMaxForce() + "°C" + "; treshold:" + tempRegulator.getTreshold());
  
  if (relStatus)
  {
    if (relOffBreakSeconds > relOffBreakCounter)
    {
      indexPage.Set_status(String("WAIT ") + (relOffBreakSeconds - relOffBreakCounter) + " sec");
      relOffBreakCounter++;
    }
    else
    {
      indexPage.Set_status("ON");
      digitalWrite(relaisPin, HIGH);
    }
  }
  else
  {
    relOffBreakCounter = 0;
    indexPage.Set_status("OFF");
    digitalWrite(relaisPin, LOW);
  }
}
