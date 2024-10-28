#include "main.h"
#include "_modbus.h"
#include "_SHTx.h"
#include "_PCF8574.h"
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
MODE_t Machine;


_PCF8574 Relay;
_PCF8574 IO;

hw_timer_t *timeIRQ;
// biến toàn cục.
float temp,hum, tempSet;
uint32_t timeTick = 0;
uint32_t timeLimit_DH1 = 0, timeLimit_DH2 = 2; // set bằng tool hoặc web.
bool Old_IO_1,Old_IO_2,Old_IO_3,Old_IO_4;   //biến trạng thái cũ nút nhấn.
bool IO1_State,IO2_State,IO3_State,IO4_State; // biến trạng thái nút nhấn.
void setup()   
{   

    Serial.begin(9600); 
    // khởi tạo modbus
    modbus.begin_modbus();
    // khởi tạo i2c.
    Relay.begin(RelayAdd, &Wire);
    IO.begin(IOAdd,&Wire);
    // khởi tạo nút trạng Auto hay Manual.
    IO.pinMode(IO_Machine, INPUT);
    // Khởi tạo 4 nút nhấn điều khiển.
    IO.pinMode(IO_1, INPUT);
    IO.pinMode(IO_2, INPUT);
    IO.pinMode(IO_3, INPUT);
    IO.pinMode(IO_4, INPUT);
    // khởi tạo relay.
    Relay.pinMode(Relay_1,OUTPUT);  // điều hòa 1.
    Relay.pinMode(Relay_2,OUTPUT);  // điều hòa 2.
    Relay.pinMode(Relay_3,OUTPUT);  // quạt 1.
    Relay.pinMode(Relay_4,OUTPUT);  // quạt 2.
    // set ngắt.
    setup_IQR(&timeIRQ, IQR_MANUAL);
    //
}

void loop() 
{
    State_Machine(); // láy trạng thái hoạt động
    readtemphum(1,&temp, &hum); // đọc giá trị nhiệt độ độ ẩm.
    timeTick = millis();
    if(Machine == AUTO)
    {
        tempSet = 28; // cái này sẽ phải set trong cấu hình tool hoặc web.
        if(temp >= tempSet)
        {
            // nhiệt độ đạt ngưỡng bật hết quạt với điều hòa.
            Relay.digitalwrite(Relay_1,HIGH); // bật điều hòa 1.
            Relay.digitalwrite(Relay_2,HIGH);
            Relay.digitalwrite(Relay_3,HIGH);
            Relay.digitalwrite(Relay_4,HIGH);
        }
        else //nếu không đạt ngưỡng.
        {
            if(Relay.digitalread(Relay_1)) // đh 1 đang bật.
            {
                timeTick = millis();
                if(timeLimit_DH1 < timeTick) //đạt limit.
                {
                    timeLimit_DH1 += timeLimit_DH1;

                }
            }
            else
            {
                bool _bool;
                if((timeLimit_DH1 > timeTick) & _bool)
                {
                    Relay.digitalwrite(Relay_1,HIGH);
                    Relay.digitalwrite(Relay_2,LOW);
                    _bool  =! _bool;
                }
            }
            // điều hòa 1.
            if (timeLimit_DH1 < timeTick ) // đủ thời gian thì tắt điều hòa 1. bật điều hòa 2.
            {
                Relay.digitalwrite(Relay_1,LOW); // tắt điều hòa 1.
                Relay.digitalwrite(Relay_2,HIGH); // bật điều hòa 2.
                timeTick = millis();
            }
            else
            {
                Relay.digitalwrite(Relay_1,HIGH);
            }
            if(timeLimit_DH2 > timeTick)
            {
                Relay.digitalwrite(Relay_1,HIGH); // tắt điều hòa 1.
                Relay.digitalwrite(Relay_2,LOW); // bật điều hòa 2.
                timeTick = millis();
            }
            
        }
    }
    else if(Machine == MANUAL)
    {
        // bật bằng mút nhấn.
        if(IO1_State)
        {
            Relay.digitalwrite(Relay_1,HIGH);
        }
        else
        {
            Relay.digitalwrite(Relay_1,LOW);
        }
        if(IO2_State)
        {
            Relay.digitalwrite(Relay_2,HIGH);
        }
        else
        {
            Relay.digitalwrite(Relay_2,LOW);
        }
        if(IO3_State)
        {
            Relay.digitalwrite(Relay_3,HIGH);
        }
        else
        {
            Relay.digitalwrite(Relay_3,LOW);
        }
        if(IO4_State)
        {
            Relay.digitalwrite(Relay_4,HIGH);
        }
        else
        {
            Relay.digitalwrite(Relay_4,LOW);
        }
    }
}


void State_Machine(void)
{
    bool Old_Machine;
    if(IO.digitalread(IO_Machine) && !Old_Machine)
    {
        Machine = AUTO; //tự động.
    }
    else
    {
        Machine = MANUAL;//thủ công
    }
    Old_Machine = IO.digitalread(IO_Machine);
}

void IQR_MANUAL() // đọc trạng thái nút nhấn bằng ngắt.
{
    if(Machine == MANUAL)
    {
        if(IO.digitalread(IO_1) && !Old_IO_1) // điều hòa 1.
        {
            IO1_State =! IO1_State;
        }
        Old_IO_1 = IO.digitalread(IO_1);

        if(IO.digitalread(IO_2) && !Old_IO_2) // điều hòa 2.
        {
            IO2_State =!IO2_State;
        }
        Old_IO_2 = IO.digitalread(IO_2);
        if(IO.digitalread(IO_3) && !Old_IO_3) // quạt 1.
        {
            IO3_State =! IO3_State;
        }
        Old_IO_3 = IO.digitalread(IO_3);
        if(IO.digitalread(IO_4) && !Old_IO_4) // quạt 2.
        {
            IO4_State =! IO4_State;
        }
        Old_IO_4 = IO.digitalread(IO_4);
    }
}

void setup_IQR(hw_timer_t **time,void (*NameIQR)())
{
    *time = timerBegin(0,80,true); // set timer 0 và 1MHz, đếm lên.
    timerAttachInterrupt(*time,NameIQR, true);
    timerAlarmWrite(*time, 100000, true); //100ms ngắt 1 lần.
    timerAlarmEnable(*time); // bật ngắt.  
}