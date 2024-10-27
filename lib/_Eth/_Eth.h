#ifndef __Eth_h_
#define __Eth_H_

#include "Ethernet.h"
#include "Server.h"
#include "Arduino.h"
#include "stdint.h"

#define port    10000
#define CS      5
enum Mode
{
    CLIENT = 0,
    SERVER
};

void setUp_Eth(Mode _mode, bool DHCP = false); // mode clien hoặc server.

void getMAC(void);
#endif