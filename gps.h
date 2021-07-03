#ifndef __RRC_GPS__
#define __RRC_GPS__


////    Includes    ////
#include <Arduino.h>


////    Defines    ////


////    GPS class    ////
class GPS
{
    public:
        GPS() {};
        ~GPS() {};

        uint8_t init             (HardwareSerial *serial, uint32_t baud);

        uint8_t readLocation     (double *lon, double *lat, uint32_t maxTime = 1000);
    private:
        HardwareSerial  *gpsSerial;
        uint32_t         gpsBaud;
};

#endif
