#include "gps.h"

////    Public Functions    ////

/**
 * @brief Initilize a GPS connection at the selected port and baudrate.
 * 
 * @param serial a pointer to the serial port to be used
 * @param baud the initial baudrate for the GPS connection
 * @return uint8_t Always zero
 */
uint8_t GPS::init(HardwareSerial *serial, uint32_t baud)
{
    gpsSerial = serial;         // store serial port pointer
    gpsBaud   = baud;           // keep a copy of the current baudrate

    gpsSerial->begin(gpsBaud);  // start connection

    /*
    
    {
        char command[] = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\x0d\x0a";
        for(int i = 0; i < 51; i++)
            gpsSerial->write(command[i]); // RMC only

        delay( 100 );
    }
    */

    // {
    //     char command[] = "$PMTK251,19200*22\x0d\x0a";
    //     for(int i = 0; i < 19; i++)
    //         gpsSerial->write(command[i]); // baud rate

    //     gpsSerial->flush();
    //     delay( 100 );
    //     gpsSerial->end();
    //     gpsSerial->begin( 19200 );
    // }

    // {
    //     char command[] = "$PMTK220,200*2C\x0d\x0a";
    //     for(int i = 0; i < 17; i++)
    //         gpsSerial->write(command[i]); // baud rate
    //     delay( 100 );
    // }

    return 0;
}


/**
 * @brief Read current location from and RMC message.
 *        Uses signed decimal degrees format.
 * 
 * @param lon pointer to store longitude
 * @param lat pointer to store latitude 
 * @param maxTime maximum time to wait for a correct reading in milliseconds. Default is 1000 ms = 1 s
 * @return uint8_t Zero on success
 */
uint8_t GPS::readLocation(double *lon, double *lat, u_int32_t maxTime)
{
    String rmcSentence[15], rawMessage;
    uint16_t pos   = 0, strPos = 0;
    uint32_t start = millis();              // store current time

    while(millis() - start <= maxTime)      // break on timeout
    {
        if(gpsSerial->find("RMC"))                                  // RMC sentence found
        {
            rawMessage = gpsSerial->readStringUntil('\n');          // save RMC message

            for(unsigned int i = 0; i < rawMessage.length(); i++)   // iterate over the message to parse
            {
                if(i == rawMessage.length() - 1)
                {
                    rmcSentence[pos] = rawMessage.substring(strPos, i);
                    break;
                }
                if(rawMessage.substring(i, i+1) == ",")             // info is saved like CSV
                {
                    rmcSentence[pos] = rawMessage.substring(strPos, i);
                    strPos           = i + 1;
                    pos++;
                }
            }

            // {
            //     Serial.println(rawMessage);

            //     for (int i = 0; i < 15; i++)
            //     {
            //         Serial.println(rmcSentence[i]);
            //     }
                
            // }

            if(rmcSentence[2] != "A")
            {
                return 1;
            }

            {
                int intTemp;
                float floatTemp;

                // read and convert latitude from degree-minute to decimal form
                floatTemp = rmcSentence[3].toFloat();
                intTemp   = (int) floatTemp;
                intTemp  -= intTemp % 100;
                *lat      = floatTemp - intTemp;
                intTemp  /= 100;
                *lat      = (*lat / 60) + intTemp;

                if(rmcSentence[4] == "S")
                    *lat *= -1;


                // read and convert longitude from degree-minute to decimal form
                floatTemp = rmcSentence[5].toFloat();
                intTemp   = (int) floatTemp;
                intTemp  -= intTemp % 100;
                *lon      = floatTemp - intTemp;
                intTemp  /= 100;
                *lon      = (*lon / 60) + intTemp;

                if(rmcSentence[6] == "W")
                    *lon *= -1;
            }

            return 0;
        }
    }

    return 1;   // only reached on timeout
}