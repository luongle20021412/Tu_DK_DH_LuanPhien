#ifndef _socket_tcp_h_
#define _socket_tcp_h_

#include "Wifi.h"
#include "CLient.h"
#include "Server.h"
#include "Arduino.h"

#define port 10000
class socket_tcp : public WiFiClient
{
    public:
        socket_tcp();
	    ~socket_tcp();

        socket_tcp& xClient(Client& xClient)
        {
            this->_client = & xClient;
            return *this;
        };

        void begin_client(void); // bắt đầu client ( gửi kết nối đến với các client).
        void begin_server(void); // bắt đầu server( lắng nghe các client đến). 
    
        bool check_connected();
        bool reconnect();
        // đọc ghi.

        String Send_data(String data);
        String Get_data(String data);

        //biến
        Client* _client;
        WiFiServer _server(port);

        uint32_t time_check;
};
#endif