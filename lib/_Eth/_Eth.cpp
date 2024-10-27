#include "_Eth.h"

//config ip tĩnh.
IPAddress _ip = (192,168,4,1);
IPAddress _gw = (192,168,4,1);
IPAddress _dns = (8,8,8,8);
IPAddress _snm = (255,255,255,0);

EthernetClient _CLient;
EthernetServer _Server(port);
void setUp_Eth(Mode _mode, bool DHCP)
{
    Ethernet.init(CS);   
    if(_mode == SERVER)
    {
        //getMAC(); lấy địa chỉ mac từ eeprom. chưa thực hiện
        byte _MAC[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
        
    }
    else
    {

    }
    if(DHCP) // ip động.
    {

    }
}
void getMAC(void)
{
   // byte _MAC = 
}
