#include <Arduino.h>
#include <gps.h>
GPS gps;
const char outputFormat[] =
    R"""(
lat: %lf
long: %lf
opMode: %c
HDOP: %f
PDOP: %f
#Sat: %d
Signal Strength: %f dBH
Fix Quality: %d
)""";
char string[256] ={0};
void setup()
{
  // put your setup code here, to run once:
  if (!gps.init(&Serial1, 9600))
  {
    Serial.println("GPS initialized");
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  double lat, lon;
  int numbSat, quality;
  char opMode;
  float HDOP, PDOP, sigStrength;
  /*
  while(Serial.available()==0)
  {
    Serial.println(Serial1.readStringUntil('\n'));
  }
  */
  if (gps.read_RMC(&lon, &lat, 1000))
  {
    Serial.println(F("RMC read fail"));
  }
  delay(100);
  if (gps.read_GSA(&opMode, &HDOP, &PDOP, 1000))
  {
    Serial.println(F("GSA read fail"));
  }
  delay(100);
  if (gps.read_GSV(&numbSat, &sigStrength, 1000))
  {
    Serial.println(F("GSV read fail"));
  }
  delay(100);
  if (gps.read_GGA(&quality, 1000))
  {
    Serial.println(F("GGA read fail"));
  }

  sprintf(string, outputFormat, lat,lon,opMode,HDOP,PDOP,numbSat,sigStrength,quality);
  Serial.printf(string);
  delay(1000);
}