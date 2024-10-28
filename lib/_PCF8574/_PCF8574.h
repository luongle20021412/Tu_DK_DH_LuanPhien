#ifndef _PCF8574_H_
#define _PCF8574_H_

#include "Adafruit_BusIO_Register.h"
#include "Adafruit_I2CDevice.h"

class _PCF8574 
{
    private:
        uint8_t _readBuf = 0xFF, _writeBuf = 0xFF;
        Adafruit_I2CDevice *i2c_dev;
    public:
        _PCF8574();
        void begin(uint8_t i2cAdd, TwoWire *wire);
        
        bool digitalwritebyte(uint8_t w);
        uint8_t digitalreadbyte(uint8_t r);

        bool pinMode(uint8_t _pinNum, bool val);
        bool digitalwrite(uint8_t _pinNum, bool val);
        bool digitalread(uint8_t _pinNum);

};
#endif