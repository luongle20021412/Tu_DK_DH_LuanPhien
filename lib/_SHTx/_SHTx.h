#ifndef _SHTx_H_
#define _SHTx_H_

#include "_modbus.h"

void setup_TempHum(uint8_t operating,int _Norm);

void readtemphum(uint8_t id, float *temp, float *hum, int _Norm = 0);

#endif