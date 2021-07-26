#include <Arduino.h>
#include <gps.h>
GPS gps;

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
  int numbSat,quality;
  char opMode;
  float HDOP, PDOP, sigStrength;
  /*
  while(Serial.available()==0)
  {
    Serial.println(Serial1.readStringUntil('\n'));
  }
  */
  if (!gps.read_RMC(&lon, &lat, 1000))
  {
    Serial.println(lat);
    Serial.println(lon);
  }
  delay(1000);
  Serial.println("************* GSA ***************");

  if (!gps.read_GSA(&opMode, &HDOP, &PDOP, 1000))
  {
    Serial.println(opMode);
    Serial.println(HDOP);
    Serial.println(PDOP);
  }
  delay(500);

  Serial.println("************ GSV ****************");
  if (!gps.read_GSV(&numbSat, &sigStrength, 1000))
  {
    Serial.println(numbSat);
    Serial.println(sigStrength);
  }
    delay(1000);

  Serial.println("************ GGA: ****************");
  
    if (!gps.read_GGA(&quality,1000))
  {
    Serial.println(quality);
  }
    delay(1000);

  Serial.println("************* RMC: ***************");
    delay(2000);

  
}