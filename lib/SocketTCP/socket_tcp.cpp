#include "socket_tcp.h"

IPAddress ip, gw, dns, snm;
IPAddress ipserver(192,168,43,250);


void socket_tcp::begin_client(void)
{
     // ipserrver và port lấy từ eeproom.
     uint8_t counttime = 0;
     if(!_client->connect(ipserver,port)) // nếu kết nối không thành công thì tự động kết nối thêm 3 lần, nếu kh kết nối với server được thì bỏ qua.
     {
          while(counttime < 4)
          {
               if(_client->connect(ipserver,port))
               {
                    break;
               }
               if (counttime == 3)
               {
                    break;
               }
               counttime ++;
               delay(1000);
          }
     }

}
void socket_tcp::begin_server(void)
{
     if(port != 0)  // port khác 0.
     {
          _server.begin();
     }
}
bool socket_tcp::check_connected()
{
     if(_client->connected())
     {
          return true;
     }
     else
     {
          begin_client(); // kết nối lại client.
          return false;
     }
}
String socket_tcp::Send_data(String data)
{
     String cmd = "";
     if(!check_connected()) // nếu kh có kết nối thì thoát khỏi gửi dữ liệu.
     {
          cmd = "Thất bại"; 
          return (cmd);
     }
     _client->println(data); // gửi chuỗi.
}
String socket_tcp :: Get_data(String data)
{
     String cmd ="";
     _client =&(_server.available());
     if(!check_connected()) // nếu kh có kết nối thì thoát khỏi gửi dữ liệu.
     {
          cmd = "Thất bại"; 
          return (cmd);
     }
     while ((_client->available() > 0))
     {
          data = _client->readString();
     }
     return data;

}