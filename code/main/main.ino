#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <TimerEvent.h>
#include "base.hpp"
#include "Logger.hpp"
#include "OneWire.h"; 
#include <DallasTemperature.h>
#include "Save.hpp"

const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";
const char* myhostname = "Solarfreigabe10";
const char* title = "Gefrierschrank";
const unsigned int timerOnePeriod = 1000;
double tempMax;
double tempMin;
int relaisPin = 16;

ESP8266WebServer server(80);
base indexPage(&server);
Logger* logger = Logger::instance();
TimerEvent timerOne;
bool relStatus = false;
bool allow = false;
SaveEEprom storage;

OneWire oneWire(4); 
DallasTemperature sensors(&oneWire);

void handleSubmit() 
{
  logger->Debug(String("Value dynamicVariable2: ") + indexPage.Get_outSetTempMin());
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
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  timerOne.set(timerOnePeriod, timerOneFunc);
  indexPage.Set_headline(String(title) + " - " + myhostname);
  pinMode(relaisPin, OUTPUT);
}

void loop(void) {
  server.handleClient();
  timerOne.update();
}

void timerOneFunc()
{
  Serial.println("request Temperature...");
  sensors.requestTemperatures();
  double temp= sensors.getTempCByIndex(0) + storage.Get_calibrate();
  double treshold = storage.Get_treshold();
  Serial.println(String(indexPage.Get_outSetTempMax()) + "°C");
  Serial.println(String(indexPage.Get_outSetTempMin()) + "°C");
   
  String tempString(temp);
  double tempMax, tempMin;
  tempString = tempString.substring(tempString.length() - 2);
  indexPage.Set_temp(String(temp));  
  tempMax= storage.Get_tempMax();
  tempMin= storage.Get_tempMin();
  indexPage.Set_setTempMax(String(tempMax));
  indexPage.Set_setTempMin(String(tempMin));

  if (String("") != indexPage.Get_outSetTempMax())
  {
    storage.Set_tempMax(indexPage.Get_outSetTempMax().toDouble());
    indexPage.Set_outSetTempMax("");
  }
  if (String("") != indexPage.Get_outSetTempMin())
  {
    storage.Set_tempMin(indexPage.Get_outSetTempMin().toDouble());
    indexPage.Set_outSetTempMin("");
  }
  if (String("") != indexPage.Get_outTreshold())
  {
    Serial.println(indexPage.Get_outTreshold() + " double: " + indexPage.Get_outTreshold().toDouble());
    storage.Set_treshold(indexPage.Get_outTreshold().toDouble());
    indexPage.Set_outTreshold("");
  }
  if (String("") != indexPage.Get_outCalibrate())
  {
    storage.Set_calibrate(indexPage.Get_outCalibrate().toDouble());
    indexPage.Set_outCalibrate("");
  }
  

  if (!relStatus)
  {
    if ((temp < tempMin) && allow)
    {
      relStatus = true;
      digitalWrite(relaisPin, HIGH);
    }
    if ((temp > tempMax) && allow)
    {
      relStatus = true;
      digitalWrite(relaisPin, HIGH);
    }
  }
  else
  {
    if ((temp > (tempMin + treshold)) && (temp < (tempMax - treshold)))
    {
      Serial.println(String("Treshold: ") + treshold + "; temMin: " + tempMin + "; tempMax: " + tempMax + "; temp: " + temp);
      relStatus = false;
      digitalWrite(relaisPin, LOW);
    }
  }
  if (relStatus)
  {
    indexPage.Set_status("ON");
  }
  else
  {
    indexPage.Set_status("OFF");
  }

}
