#pragma once

#ifndef _main_h_
#define _main_h_

 /**************include**************/
//#include "WiFi.h"
#include "SPI.h"
#include "Ethernet.h"
#include "Client.h"
#include "Server.h"
#include "EEPROM.h"
#include "_wifix.h"
 /**************define***************/

#define SCK     18
#define MISO    19
#define MOSI    23
#define CS      5
#define port    10000
// define relay.
#define RelayAdd    0x21
#define Relay_1     0
#define Relay_2     1
#define Relay_3     2
#define Relay_4     3   
// define IO.
#define IOAdd       0x23
#define IO_1        0
#define IO_2        1
#define IO_3        2
#define IO_4        3
#define IO_5        4
#define IO_6        5
#define IO_7        6
#define IO_8        7
// octet giá trị nhận từ web hoặc tool.
typedef struct 
{
   uint8_t  first_octet = 192;
   uint8_t  second_octet = 168;
   uint8_t  third_octet = 43;
   uint8_t  fourth_octet = 250;
}Octet_t;
// số lượng
typedef enum
{
    ONE,
    TWO,
    THREE,
    FOUR
}Quantity_t;
// mode 
typedef enum
{
    AUTO,
    MANUAL
}MODE_t;



/*************Set Up Quantity**************/

uint8_t Quantity_Ari_Conditioning = 0;
uint8_t Quantity_Fan = 0;

/******************config Ethernet***************/ 
Octet_t eth_ip;
Octet_t eth_dns;
Octet_t eth_gateway;
Octet_t eth_subnet;

// địa chỉ mac cho ethernet
byte mac[]=  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// địa chỉ tĩnh cho ethernet.    
IPAddress Eth_ip(eth_ip.first_octet, eth_ip.second_octet, eth_ip.third_octet, eth_ip.fourth_octet);
IPAddress Eth_dns(eth_dns.first_octet, eth_dns.second_octet, eth_dns.third_octet, eth_dns.fourth_octet);
IPAddress Eth_gateway(eth_gateway.first_octet, eth_gateway.second_octet, eth_gateway.third_octet, eth_gateway.fourth_octet);
IPAddress Eth_subnet(eth_subnet.first_octet, eth_subnet.second_octet, eth_subnet.third_octet, eth_subnet.fourth_octet);
EthernetClient _client_eth;
    #if client_or_server ==  CLIENT
        void setUp_Ethernet(void)
        {
            // pin CS 
            Ethernet.init(CS);
            //Ethernet.begin(mac);
            Ethernet.begin(mac, Eth_ip);
            Serial.println(Ethernet.localIP());
        }
        void connect_Eth_CL(IPAddress serverip, uint16_t _port)
        {
            
            if(_client_eth.connect(serverip, _port))
            {
                Serial.println("Kết nối ETH tới server thành công.");
                _client_eth.println("Client Eth đã kết nối.");
            }
            else
            {
                Serial.println("Kết nối tới server thất bại.");
            }
        }
        String get_data(String data)
        {
            while (_client_eth.connected())
            {
                if(_client_eth.available())
                {
                    data = _client_eth.readString();
                    _client_eth.println("Clien ETH đã nhận được dữ liệu từ server");
                    _client_eth.stop();
                }
            }
            return data;
        }
        String send_data(String data)
        {
            String cmd ="";
            if(_client_eth.connected())
            {
                _client_eth.println(data);
                cmd = "Client Eth đã gửi tới server";
                return (cmd);
            }
        }
    #else  


    #endif
#endif