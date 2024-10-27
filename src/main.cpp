#include "main.h"
#include "_modbus.h"
#include "_SHTx.h"
//#include "socket_tcp.h"

// const char* name = "Luong";
// const char* pass = "12345678";
// const char *serveripWF = "192.168.43.55";
// IPAddress ipsv (192,168,43,10);
// uint16_t server_port = 10000;
// String data = "WIFI send .........";
// String data_get;
// String data_eth_send = "Eth send.......";
// String data_eth_get;
// bool DHCP = true;

//socket_tcp _client;
//socket_tcp _server(port);

SoketTcp_WF _client;
// khởi tạo mode
ModeSock _mode = CLIENT;
_Modbus modbus;

uint8_t* rcv;

void setup()   
{   

    Serial.begin(9600); 
    modbus.begin_modbus();
    //Serial1.begin(9600,SERIAL_8N1, 16,17,false, 10UL); //do cấu hình chân :))
    //Serial2.begin(9600);
    /*
    Serial.println(_mode);
    _client.SetUp_Wifi(DHCP,name,pass);  
    setUp_Ethernet();  
    if(_mode == CLIENT)
    {
        _client.connect_SocketWF_CL(_mode,serveripWF, server_port);
     //   delay(5000);
        connect_Eth_CL(ipsv, server_port);
        //xClient.check_connected(_mode);
    }
    else  // mode server
    {
        //Server.connect_SocketWF_SV(_mode);
        //Server.check_connected(_mode);
        //Serial.println("Đang lắng nghe client.....");
    }
    */
   //Serial.println("Start....");
}

void loop() 
{
    
    //Server.check_connected(_mode);
    //Wifi_client_connect_Server();
    /*
    if (_mode == CLIENT)
    {
        Serial.println("..................WIFI...................");
        Serial.println(_client.send_data_string(_mode,data));
        Serial.println(_client.Get_data_string(_mode,data_get));

        Serial.println("..................ETH....................");
        Serial.println(send_data(data_eth_send));
        
        Serial.println(get_data(data_eth_get));

        //Serial.println(data_get);    
    }
    else
    { 
        //Serial.println(Server.send_data_string(_mode,data));
        //Serial.println(Server.Get_data_string(_mode,data_get));
       //Server.check_connected(_mode);
    }
    
    //Client.Get_data(_mode,data[1]);
    delay(10000);
    Serial.println(Ethernet.linkStatus());
    
    */
    // rcv = modbus.Send_requet(1,0x03,0x0000,0x0002);
    // if(rcv != nullptr)
    // {
    //     for (uint8_t i = 0; i < sizeof(rcv); i++)
    //     { 
    //         if (i > 2)
    //         {
    //             rcv[i] = i;
    //         }
    //         Serial.print(rcv[i], HEX);
    //         Serial.print(" ");
    //     }
    // }
    // delay(5000);
    // modbus.Send_requet(0x02,0x10,0x0000,rcv);
    
    float temp, hum;
    readtemphum(1, &temp, &hum);

    Serial.printf("nhiet do: %.2f", temp);
    Serial.println();
    Serial.printf("do am: %.2f", hum);
    Serial.println();
    delay(5000);
}