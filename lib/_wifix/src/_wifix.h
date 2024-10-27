//_wifix.h

#ifndef __wifix_h_
#define __wifix_h_

#include "WiFi.h"

#define PORT 80

enum ModeSock
{
    CLIENT = 0,
    SERVER 
};
class SoketTcp_WF : public WiFiClient, public WiFiServer
{
    private:
        uint16_t port;
        WiFiServer _server;
        WiFiClient _client;
    public:
        SoketTcp_WF();
        ~SoketTcp_WF();
        
        SoketTcp_WF(uint16_t _port) : port(_port), _server(_port), _client() {} 
        // socket
        void connect_SocketWF_CL(ModeSock _mode,const char* serverIP, uint16_t serverPORT);
        void connect_SocketWF_SV(ModeSock _mode);

        bool check_connected(ModeSock _mode);
        String send_data_string(ModeSock _mode,String data);
        String Get_data_string(ModeSock _mode, String data);
        
        void SetUp_Wifi(bool enDHCP,const char* nameWF, const char* pwWF,IPAddress ip = (192,168,1,1),IPAddress gw = (192, 168, 1, 1),
                        IPAddress dns = (8, 8, 8, 8),IPAddress snm = (255, 255, 255, 0));

        void Setup_Access_Wifi(void);
        void seting_wifi(void);

        void acceptsv(void);
        // biên
        
};

#endif
