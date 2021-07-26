#ifndef __RRC_GPS__
#define __RRC_GPS__

////    Includes    ////
#include <Arduino.h>

////    Defines    ////

////    GPS class    ////
class GPS
{
public:
    GPS(){};
    ~GPS(){};

    uint8_t init(HardwareSerial *serial, uint32_t baud);

    uint8_t read_RMC(double *lon, double *lat, uint32_t maxTime = 1000);
    uint8_t readMSG(String *Sentence);
    uint8_t read_GSA (char *opMode, float *HDOP, float *PDOP, u_int32_t maxTime=1000);
    uint8_t read_GSV (int *numbSat, float *sigStrength, u_int32_t maxTime=1000);
    uint8_t read_GGA (int *quality, u_int32_t maxTime=1000);

    
private:
    HardwareSerial *gpsSerial;
    uint32_t gpsBaud;
};

#endif
