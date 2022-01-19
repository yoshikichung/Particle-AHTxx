/******************************************************************************/
/*
  This is a Particle library for Aosong ASAIR AHT10/AHT15/AHT20/AHT21/AHT25/AM2301B/AM2311B
  Digital Humidity & Temperature Sensor

  Aosong ASAIR AHT1x/AHT2x features:
  - AHT1x +1.8v..+3.6v, AHT2x +2.2v..+5.5v
  - AHT1x 0.25uA..320uA, AHT2x 0.25uA..980uA
  - temperature range -40C..+85C
  - humidity range 0%..100%
  - typical accuracy T +-0.3C, RH +-2%
  - typical resolution T 0.01C, RH 0.024%
  - normal operating range T -20C..+60C, RH 10%..80%
  - maximum operating rage T -40C..+80C, RH 0%..100%
  - response time 8..30sec*
  - I2C bus speed 100KHz..400KHz, 10KHz recommended minimum
    * measurement with high frequency leads to heating
      of the sensor, measurements must be > 2 seconds
      apart to keep self-heating below 0.1C

  This device uses I2C bus to communicate, specials pins are required to interface
  Board:                                    SDA     SCL     Level
  Uno, Mini, Pro, ATmega168, ATmega328..... A4      A5      5v
  Mega2560................................. 20      21      5v
  Due, SAM3X8E............................. 20      21      3.3v
  Leonardo, Micro, ATmega32U4.............. 2       3       5v
*/
/******************************************************************************/
#include <Wire.h>
#include <AHTxx.h>

float ahtValue;  //to store T/RH result
AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR);  //sensor address, sensor type

/**************************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println();
  
  while (aht20.begin() != true) {
    Serial.println(F("AHT2x not connected or fail to load calibration coefficient"));  //(F()) save string to flash & keeps dynamic memory free
    delay(5000);
  }
  Serial.println(F("AHT20 OK"));
  //Wire.setClock(400000);  //experimental I2C speed! 400KHz, default 100KHz
}

/**************************************************************/
void loop() {
  /* DEMO - 1, every temperature or humidity call will read 6-bytes over I2C, total 12-bytes */
  Serial.println();
  Serial.println(F("DEMO 1: read 12-bytes"));

  ahtValue = aht20.readTemperature();  //read 6-bytes via I2C, takes 80 milliseconds
  Serial.print(F("Temperature...: "));
  if (ahtValue != AHTXX_ERROR) {  //AHTXX_ERROR = 255, library returns 255 if error occurs
    Serial.print(ahtValue);
    Serial.println(F(" +-0.3C"));
  } else {
    printStatus();  //print temperature command status
    if (aht20.softReset() == true)
      Serial.println(F("reset success"));  //as the last chance to make it alive
    else
      Serial.println(F("reset failed"));
  }
  delay(2000);  //measurement with high frequency leads to heating of the sensor, see NOTE

  ahtValue = aht20.readHumidity();  //read another 6-bytes via I2C, takes 80 milliseconds
  Serial.print(F("Humidity......: "));
  if (ahtValue != AHTXX_ERROR) {  //AHTXX_ERROR = 255, library returns 255 if error occurs
    Serial.print(ahtValue);
    Serial.println(F(" +-2%"));
  } else {
    printStatus();  //print humidity command status
  }
  delay(2000);  //measurement with high frequency leads to heating of the sensor, see NOTE

  /* DEMO - 2, temperature call will read 6-bytes via I2C, humidity will use same 6-bytes */
  Serial.println();
  Serial.println(F("DEMO 2: read 6-byte"));

  ahtValue = aht20.readTemperature();  //read 6-bytes via I2C, takes 80 milliseconds
  Serial.print(F("Temperature: "));
  if (ahtValue != AHTXX_ERROR) {  //AHTXX_ERROR = 255, library returns 255 if error occurs
    Serial.print(ahtValue);
    Serial.println(F(" +-0.3C"));
  } else {
    printStatus();  //print temperature command status
  }

  ahtValue = aht20.readHumidity(AHTXX_USE_READ_DATA);  //use 6-bytes from temperature reading, takes zero milliseconds!!!
  Serial.print(F("Humidity...: "));
  if (ahtValue != AHTXX_ERROR) {  //AHTXX_ERROR = 255, library returns 255 if error occurs
    Serial.print(ahtValue);
    Serial.println(F(" +-2%"));
  } else {
    printStatus();  //print temperature command status not humidity!!! RH measurement use same 6-bytes from T measurement
  }
  delay(10000);  //recomended polling frequency 8sec..30sec
}

/**************************************************************/
void printStatus() {
  switch (aht20.getStatus()) {
    case AHTXX_NO_ERROR:
      Serial.println(F("no error"));
      break;
    case AHTXX_BUSY_ERROR:
      Serial.println(F("sensor busy, increase polling time"));
      break;
    case AHTXX_ACK_ERROR:
      Serial.println(F("sensor didn't return ACK, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;
    case AHTXX_DATA_ERROR:
      Serial.println(F("received data smaller than expected, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;
    case AHTXX_CRC8_ERROR:
      Serial.println(F("computed CRC8 not match received CRC8, this feature supported only by AHT2x sensors"));
      break;
    default:
      Serial.println(F("unknown status"));    
      break;
  }
}
