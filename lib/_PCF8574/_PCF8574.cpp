#include "_PCF8574.h"

_PCF8574::_PCF8574()
{
}
void _PCF8574::begin(uint8_t i2cAdd, TwoWire *wire)
{
    delete(i2c_dev);

    i2c_dev = new Adafruit_I2CDevice(i2cAdd,wire);
    if(!i2c_dev->begin()) {while(1){}}// chekc lỗi ở đây
    return ;
}
bool _PCF8574::digitalwritebyte(uint8_t w)
{
    _writeBuf = w;
    return i2c_dev->write(&_writeBuf,1);
}
uint8_t _PCF8574::digitalreadbyte(uint8_t r)
{
    i2c_dev->read(&_readBuf,1);
    return _readBuf;
}
bool _PCF8574::digitalwrite(uint8_t _pinNum, bool val)
{
    if(val) // High
    {
        _writeBuf |= 1 << _pinNum;
    }
    else // Low
    {  
        _writeBuf &= ~(1 << _pinNum);
    }
    return i2c_dev->write(&_writeBuf,1);
}
bool _PCF8574::digitalread(uint8_t _pinNum)
{
    i2c_dev->read(&_readBuf, 1);
    return (_readBuf >> _pinNum) & 0x1;
}
bool _PCF8574::pinMode(uint8_t _pinNum, bool val)
{
    if(val == INPUT || val == INPUT_PULLUP)
    {
        _writeBuf |= 1 << _pinNum;
    }
    else
    {
        _writeBuf &= ~(1 << _pinNum);
    }
    return i2c_dev->write(&_writeBuf, 1);
}