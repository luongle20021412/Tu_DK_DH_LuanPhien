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
MODE_t Machine = AUTO;


_PCF8574 Relay;
_PCF8574 IO;

hw_timer_t *timeIRQ;
// biến toàn cục.
float temp,hum, tempSetMax, tempSetMin;
uint32_t timeTickDH1 = 0, timeTickDH2 = 0 ,timeTickTemp = 0;
uint32_t timeLimit_DH1 = 20000, timeLimit_DH2 = 20000; // set bằng tool hoặc web.
bool Old_IO_1,Old_IO_2,Old_IO_3,Old_IO_4;   //biến trạng thái cũ nút nhấn.
bool IO1_State,IO2_State,IO3_State,IO4_State; // biến trạng thái nút nhấn.
bool Starttick1, Starttick2;
volatile bool RL1_State,RL2_State,RL3_State,RL4_State; 

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
    // khởi tạo relay(MỨC THẤP).
    Relay.pinMode(Relay_1,OUTPUT);  // điều hòa 1.
    Relay.pinMode(Relay_2,OUTPUT);  // điều hòa 2.
    Relay.pinMode(Relay_3,OUTPUT);  // quạt 1.
    Relay.pinMode(Relay_4,OUTPUT);  // quạt 2.
    //Serial.println("Đã qua setup");
}

void loop() 
{
    State_Machine(); // láy trạng thái hoạt động
     if((millis() - timeTickTemp) > 1000) // 5;s đọc nhiệt độ 1 lần.
    {  
        //Serial.println("Đã vào temp");
        readtemphum(1,&temp,&hum);
        Serial.printf("temp:  %.1f\n", temp);
        timeTickTemp = millis();
    }
    if(Machine == AUTO)
    {
       // Serial.println("Đã vào auto.");
        // đọc trạng thái điều hòa.
        RL1_State = Relay.digitalread(Relay_1);
        RL2_State = Relay.digitalread(Relay_2);
        RL3_State = Relay.digitalread(Relay_3);
        RL4_State = Relay.digitalread(Relay_4);
        // Serial.println(RL1_State);
        // Serial.println(RL2_State);
        // Serial.println(RL3_State);
        // Serial.println(RL4_State);    
        //Serial.println("Đã vào manual");
        tempSetMax = 28; // cái này sẽ phải lấy từ trong eppr, tool hoặc web cấu hình mà ghi vào eppr.
        tempSetMin = 25;
        if(temp >= tempSetMax)
        {
            // Serial.println("Đã vào ngưỡng trên");
            // Serial.printf("Tem: %.1f\n", temp);
            // Serial.printf("Tem: %.1f\n", tempSetMax);
            // Serial.println("Đã vào ngưỡng");
            // nhiệt độ đạt ngưỡng bật hết quạt với điều hòa.
            Relay.digitalwrite(Relay_1,HIGH); // bật điều hòa 1.
            Relay.digitalwrite(Relay_2,HIGH);
            Relay.digitalwrite(Relay_3,HIGH);
            Relay.digitalwrite(Relay_4,HIGH);
        }
        else if(tempSetMin < temp && temp < tempSetMax) // trong khoảng ngưỡng
        {
            // tắt quạt.
            Relay.digitalwrite(Relay_3,LOW);
            Relay.digitalwrite(Relay_4,LOW);
            //khởi đầu
            if(RL1_State == 1 && RL2_State == 1) // bằng 1 thì sẽ tắt.
            {
                RL1_State = 0;
            }
            //Serial.println("Đã vào ngưỡng trung bình");
            // dùng timetick
            if(!RL1_State && !Starttick1) // đh 1 đang bật.
            {
                timeTickDH1 = millis(); // time bắt đầu bật điều hòa 1.
                Starttick1 = true;
                //Serial.println("timetickDH1: ");
                //Serial.println(timeTickDH1);
            }
            else if(!RL2_State && !Starttick2) // điều hòa 2.
            {
                timeTickDH2 = millis();
                Starttick2 = true;
                //Serial.print("timetickDH2: ");
                //Serial.println(timeTickDH2);
            }
            // điều hòa 1.
            if(!RL1_State)
            {
                //Serial.println("ĐH 1 đang bật");
                if(uint32_t(millis() - (timeTickDH1)) > timeLimit_DH1) //đạt limit time.(timetic)
                {  
                   // Serial.print("milis: ");
                    //Serial.println(millis());
                    //Serial.print("limit1: ");
                    //Serial.println(uint32_t(millis() - (timeTickDH1)));
                    //Serial.println("đã đạt giới hạn thời gian đh1");
                    Relay.digitalwrite(Relay_1,LOW);    // tắt điều hòa 1.
                    Relay.digitalwrite(Relay_2,HIGH);    // bật điều hòa 2
                    Starttick1 = false;
                    Starttick2 = false;    
                }
                else
                {
                    Relay.digitalwrite(Relay_1,HIGH);   // bật 1
                    Relay.digitalwrite(Relay_2,LOW);    // tắt 2.
                    Starttick2 = true;                  // điều hòa 1 bật thì kh bao h được set giá trị thời gian đh bắt đầu mở.
                }
            }
            // điều hòa 2.
            else if(!RL2_State)
            {
                //Serial.println("ĐH 2 đang bật");
                if((uint32_t(millis() - (timeTickDH2)) > timeLimit_DH2)) //đạt limit time.(timetic)
                {
                    // Serial.print("milis: ");
                    // Serial.println(millis());
                    // Serial.print("limit2: ");
                    // Serial.println(uint32_t(millis() - (timeTickDH2)));
                    // Serial.println("đã đạt giới hạn thời gian đh2");
                    Relay.digitalwrite(Relay_2,LOW);    // tắt điều hòa 2.
                    Relay.digitalwrite(Relay_1,HIGH);    // bật điều hòa 1.
                    Starttick1 = false;
                    Starttick2 = false;           // bật lai đếm lần tiếp theo.
                }
                else
                {
                    Relay.digitalwrite(Relay_2,HIGH);   // bật 2
                    Relay.digitalwrite(Relay_1,LOW);    // tắt 1.
                    Starttick1 = true;
                }
            }
        }
        else if (temp < tempSetMin) // dưỡi ngưỡng min.
        {
            //bật 2 relay 3 và 4 để điều khiển quạt.
            Relay.digitalwrite(Relay_1,LOW);   // bật 1
            Relay.digitalwrite(Relay_2,LOW);
            Relay.digitalwrite(Relay_3,HIGH);
            Relay.digitalwrite(Relay_4,HIGH);
            
        }
    }
    else if(Machine == MANUAL)
    {
        //Serial.println("Đã vào manual");
        // lấy trạng thái.
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
        // bật bằng mút nhấn.(nhấn 1 lần là bật nhấn 2 lần là tắt)
        Relay.digitalwrite(Relay_1,IO1_State ? HIGH: LOW);
        Relay.digitalwrite(Relay_2,IO2_State ? HIGH: LOW);
        Relay.digitalwrite(Relay_3,IO3_State ? HIGH: LOW);
        Relay.digitalwrite(Relay_4,IO4_State ? HIGH: LOW);
    }
}

void State_Machine(void)
{
    bool Old_Machine;
    if(IO.digitalread(IO_Machine) && !Old_Machine)
    {
        Machine = MANUAL; //thủ công. 
    }
    else
    {
        Machine = AUTO; //tự động.
    }
    Old_Machine = IO.digitalread(IO_Machine);
}
void setup_Timelimit(void)
{

}