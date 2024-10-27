#include "_wifix.h"

bool Mode_Wifi  = true;     // Mode_wifi = 0 -> phát wifi, Mode_Wifi = 1 thu wifi.

//const char *nameWF_point ="WIFI OF ESP";
//const char *pwWF_point ="12345678";
        
//server
const char* host = "192.168.43.2"; // địa chỉ của laptop
const uint16_t port = PORT;

SoketTcp_WF::SoketTcp_WF() 
{
}

SoketTcp_WF::~SoketTcp_WF() 
{
}

void SoketTcp_WF::SetUp_Wifi(bool enDHCP,const char* nameWF, const char* pwWF,IPAddress ip,IPAddress gw, IPAddress dns, IPAddress snm)
{
    if(enDHCP == false)
    {
        Serial.println("set IP tĩnh:");
        WiFi.config(ip,gw, snm, dns); // cấu hình ip tĩnh.
    }
    //WiFi.softAP(nameWF,pwWF);
    WiFi.begin(nameWF,pwWF);
    
    while (WiFi.status() != WL_CONNECTED) // chờ kết nối wifi.
    {
        delay(1000); 
        Serial.print(".");
    }
    
    Serial.print("IP WIFI: ");
    Serial.println(WiFi.localIP());  // lấy ip wifi.
    Serial.println("đã kết nối wifi!!!");
}
/*
void SoketTcp_WF::Setup_Access_Wifi(ModeSock mode )
{
    if(mode == CLIENT)
    {
        // name pass wifi mode phát
        if(enDHCP == false)
        {
            Serial.println("set IP tĩnh:");
            WiFi.softAPConfig(ip,gw, snm, dns); // cấu hình ip tĩnh.
        }
        WiFi.softAP(nameWF_point,pwWF_point); // phát wifi.
        Serial.println(WiFi.softAPIP()); // lấy ip wifi.
    }
    else // mode server
    {

    }
}

void SoketTcp_WF::seting_wifi(void)
{

    if(Mode_Wifi)
    {
        SetUp_Wifi();
    }
    else
    {
        Setup_Access_Wifi();
    }
}
*/    
void SoketTcp_WF::connect_SocketWF_CL(ModeSock _mode,const char* serverIP, uint16_t serverPORT)
{
    if(_mode == CLIENT)
    {           
       if(_client.connect(serverIP,serverPORT))
       {
            _client.println("Client WiFi đã kết nối!!!");
            Serial.println("Kết nối thành công với server!!!");
       }
       else
       {
            Serial.println("Kết nối với server thất bại!!!");
       }
    }
    
}
void SoketTcp_WF::connect_SocketWF_SV(ModeSock _mode)
{
    if(_mode == SERVER)
    {
        _server.begin(); // lắng nghe client.
        _server.println("Đã kết nối đến server!!!");
    }
    
}

bool SoketTcp_WF::check_connected(ModeSock _mode)
{
    if(_mode == CLIENT)
    {
        if(_client.connected())
        {
            Serial.println("client vẫn còn kết nối!!!");
            return true;
        }
        else
        {
            Serial.println("client đã ngắt kết nối!!!");
            return false;
        }
    }
    else // mode server
    {
        _client = _server.available();
        if(_client) // 
        {
            _client.println("hello client!!");
            Serial.println("đã có client kết nối!!!");
            Serial.println(_client.remoteIP());
            return true;
        }
        else
        {
            Serial.println("chưa có client kết nối!!!");
            return false;
        }
        
    }
}
String SoketTcp_WF::send_data_string(ModeSock _mode,String  data)// gửi dữ liệu 1 biến 
{ 
    String done = "Đã gửi thành công";
    if(!check_connected(_mode))
    {
        data ="Thoát khỏi send";
        return data;
    }
    if(_mode == CLIENT)
    {
        Serial.println("Đang gửi dữ liệu tới server......");
        _client.println(data); // ghi dữ liệu
        //cSerial.println("Done");
        _client.println("client Đã gửi dữ liệu đến !!!");
        //_client.stop();
    }
    else// mode server.
    {
        connect_SocketWF_SV(_mode);
        Serial.println("Đang gửi dữ liệu tới client......");
        _client.println(data);
        _client.println("Server Đã gửi dữ liệu đến Client WIFI!!!");                           
    }
    
    return done;  
}
String SoketTcp_WF::Get_data_string(ModeSock _mode,String data) // ghi dữ liệu vào 1 biến
{
    //char buffer[50];
     if(!check_connected(_mode))
    {
       // Serial.println("Thoát khỏi get");
        data = "thoat khoi get";
        return data;
    }
    if(_mode == CLIENT)
    {
 //         connect_SocketWF_CL(_mode,) kết nối với server.
        while(_client.available() > 0)
        {
            Serial.println("Đang lấy dữ liệu từ server.......");
            data =_client.readString(); // đọc dữ liệu
            Serial.print(data);
            _client.println("Client WIFI đã nhận được dữ liệu!!!");
        }
        delay(100);
        _client.stop();
        return data;
    }
    else // mode server
    {
        connect_SocketWF_SV(_mode);
        Serial.println("Đang lấy dữ liệu từ client.......");
        data = _client.readString();
        _client.println("Server đã nhận được dữ liệu!!!");
        return data;
    }
}


void SoketTcp_WF::acceptsv()
{
    _server.accept();
} 
