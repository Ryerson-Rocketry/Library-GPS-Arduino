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
    gpsSerial = serial; // store serial port pointer
    gpsBaud = baud;     // keep a copy of the current baudrate

    gpsSerial->begin(gpsBaud); // start connection

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
 * @brief Read current GGA message, returns GPS fix quality and type
 * 
 * @param quality pointer to storefix auality/type
 * @param maxTime maximum time to wait for a correct reading in milliseconds. Default is 1000 ms = 1 s
 * @return uint8_t Zero on success
 */
uint8_t GPS::read_GGA (int *quality, u_int32_t maxTime)
{
    String ggaSentence[16];
    uint32_t start = millis(); // store current time

    while (millis() - start <= maxTime) // break on timeout
    {
        if (gpsSerial->find("GGA")) // RMC sentence found
        {

            readMSG(ggaSentence);
          
            *quality = ggaSentence[6].toInt(); 
            // if no fix return 1:
            if (ggaSentence[6]== '0')
            {
                return 1;
            }

            return 0;
        }
    }

    return 1; // only reached on timeout
}

/**
 * @brief Read current GSA message, returns GPS precision and mode types 
 * 
 * @param HDOP pointer to store horizontal dilution precision
 * @param PDOP pointer to store position dilution precision
 * @param opMode pointer to store operation mode (set auto or manual for 2d/3d operation)
 * @param maxTime maximum time to wait for a correct reading in milliseconds. Default is 1000 ms = 1 s
 * @return uint8_t Zero on success
 */
uint8_t GPS::read_GSA (char *opMode, float *HDOP, float *PDOP, u_int32_t maxTime)
{
    String gsaSentence[20];
    uint32_t start = millis(); // store current time

    while (millis() - start <= maxTime) // break on timeout
    {
        if (gpsSerial->find("GSA")) // RMC sentence found
        {

            readMSG(gsaSentence);
          

            *opMode = (char) &gsaSentence[1];
            *PDOP = gsaSentence[15].toFloat();
            
            *HDOP = gsaSentence[16].toFloat();

            return 0;
        }
    }

    return 1; // only reached on timeout
}


/**
 * @brief Read current GSV message, returns GPS #sat and signal strength
 * 
 * @param numbSat pointer to store #sat
 * @param sigStrength pointer to store signal strength(in dBH)

 * @param maxTime maximum time to wait for a correct reading in milliseconds. Default is 1000 ms = 1 s
 * @return uint8_t Zero on success
 */
uint8_t GPS::read_GSV (int *numbSat, float *sigStrength, u_int32_t maxTime)
{
    String gsvSentence[16];
    uint32_t start = millis(); // store current time

    while (millis() - start <= maxTime) // break on timeout
    {
        if (gpsSerial->find("GSV")) // RMC sentence found
        {

            readMSG(gsvSentence);
           *numbSat = gsvSentence[3].toInt();
           *sigStrength = gsvSentence[7].toFloat();


            return 0;
        }
    }

    return 1; // only reached on timeout
}


/**
 * @brief Read current RMC message, returns GPS location 
 *        Uses signed decimal degrees format.
 * 
 * @param lon pointer to store longitude
 * @param lat pointer to store latitude 
 * @param maxTime maximum time to wait for a correct reading in milliseconds. Default is 1000 ms = 1 s
 * @return uint8_t Zero on success
 */
uint8_t GPS::read_RMC(double *lon, double *lat, u_int32_t maxTime)
{
    String rmcSentence[15];
    uint32_t start = millis(); // store current time

    while (millis() - start <= maxTime) // break on timeout
    {
        if (gpsSerial->find("RMC")) // RMC sentence found
        {

            readMSG(rmcSentence);
            if (rmcSentence[2] != "A")
            {
                return 1;
            }

            {
                int intTemp;
                float floatTemp;

                // read and convert latitude from degree-minute to decimal form
                floatTemp = rmcSentence[3].toFloat();
                intTemp = (int)floatTemp;
                intTemp -= intTemp % 100;
                *lat = floatTemp - intTemp;
                intTemp /= 100;
                *lat = (*lat / 60) + intTemp;

                if (rmcSentence[4] == "S")
                    *lat *= -1;

                // read and convert longitude from degree-minute to decimal form
                floatTemp = rmcSentence[5].toFloat();
                intTemp = (int)floatTemp;
                intTemp -= intTemp % 100;
                *lon = floatTemp - intTemp;
                intTemp /= 100;
                *lon = (*lon / 60) + intTemp;

                if (rmcSentence[6] == "W")
                    *lon *= -1;
            }

            return 0;
        }
    }

    return 1; // only reached on timeout
}



/**
 * @brief Read current NMEA msg
 * 
 * @param Sentence pointer to store msg 
 * @return uint8_t Zero on success
 * */

uint8_t GPS::readMSG(String *Sentence)
{
    String rawMessage;
    uint16_t pos = 0, strPos = 0;

    rawMessage = gpsSerial->readStringUntil('\n'); // save RMC message

    for (unsigned int i = 0; i < rawMessage.length(); i++) // iterate over the message to parse
    {
        if (i == rawMessage.length() - 1)
        {
            Sentence[pos] = rawMessage.substring(strPos, i);
            break;
        }
        if (rawMessage.substring(i, i + 1) == ",") // info is saved like CSV
        {
            Sentence[pos] = rawMessage.substring(strPos, i);
            strPos = i + 1;
            pos++;
        }
    }
    return 0;
}