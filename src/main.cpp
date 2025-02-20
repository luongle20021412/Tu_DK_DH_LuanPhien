#include "main.h"
#include "_modbus.h"
#include "_SHTx.h"
#include "_PCF8574.h"
#include "LiquidCrystal_I2C.h"

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

// khởi tạo modbus.
_Modbus modbus;
MODE_t Machine = AUTO;

// Khởi tạo IO qua I2C.
_PCF8574 Relay;
_PCF8574 IO;

// khởi tạo LCD.
LiquidCrystal_I2C LCD(LCD_Add, COL_LCD, ROW_LCD);

// trạng thái set.
SetTime_t modeSetTime = NO_SET_TIME;
SetUp_t Setup = NO_SET_UP;
SetTemp_t SetTemp = NO_SET_TEMP;

hw_timer_t *timeIRQ;
//id 
uint8_t _id = 1;
// biến toàn cục.
float temp,hum, tempSetMax, tempSetMin;
int tempIMPALE = 0; // biến này dùng để điều chỉnh nhiệt độ.
uint32_t timeTickDH1 = 0, timeTickDH2 = 0 ,timeTickTemp = 0, timeTick = 0;
uint8_t timeset1 = 1, timeset2 = 1, timeset3, timeset4, countSet = 0; // biến của chế độ setuo. set bằng tool hoặc web | set theo giờ.
uint32_t timeLimit_DH1, timeLimit_DH2; 
bool Old_IO_1,Old_IO_2,Old_IO_3,Old_IO_4;   //biến trạng thái cũ nút nhấn.
bool IO1_State,IO2_State,IO3_State,IO4_State; // biến trạng thái nút nhấn.
bool Starttick1, Starttick2, StateSet; // biến đếm thời gian bắt đầu.
volatile bool RL1_State,RL2_State,RL3_State,RL4_State; \
bool displayDelete; // biến xóa mà hình.
static uint8_t countSetup = 0, countSetTime = 0, countSetTemp = 0; // biến đếm các lựa chọn của trạng thái setup.
void setup()   
{   

    Serial.begin(9600); 
    // khởi tạo modbus
    modbus.begin_modbus();
    // khởi tạo i2c.
    Relay.begin(RelayAdd, &Wire);
    IO.begin(IOAdd,&Wire);
    // khởi tạo LCD với i2c.
    LCD.init();
    LCD.backlight();
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
    // cacl
    timeLimit_DH1 = cacl_Timetick(timeset1);
    timeLimit_DH2 = cacl_Timetick(timeset2);
    //Machine = MANUAL;
}

void loop() 
{
    State_Machine(); // láy trạng thái hoạt động
    Dislplay_LCD(); // hiển thị màn hình.
    if((millis() - timeTickTemp) > 1000) // 5;s đọc nhiệt độ 1 lần.
    {  
        //Serial.println("Đã vào temp");
        readtemphum(_id,&temp,&hum, tempIMPALE);
        //Serial.printf("temp:  %.1f\n", temp);
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
        setup_Button_();
        // Serial.println(RL1_State);
        // Serial.println(RL2_State);
        // Serial.println(RL3_State);
        // Serial.println(RL4_State);    
        //Serial.println("Đã vào manual");
        //tempSetMax = 28; // cái này sẽ phải lấy từ trong eppr, tool hoặc web cấu hình mà ghi vào eppr.
        //tempSetMin = 25;
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
            if(RL1_State == 1 && RL2_State == 1) // bằng 1 thì sẽ tắt | nếu cả 2 RL đang tắt thì sẽ bật RL1
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
        //temp = 0; 
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
        Serial.println(IO.digitalread(IO_1));
        // bật bằng mút nhấn.(nhấn 1 lần là bật nhấn 2 lần là tắt)
        Relay.digitalwrite(Relay_1,IO1_State ? HIGH: LOW);
        Relay.digitalwrite(Relay_2,IO2_State ? HIGH: LOW);
        Relay.digitalwrite(Relay_3,IO3_State ? HIGH: LOW);
        Relay.digitalwrite(Relay_4,IO4_State ? HIGH: LOW);
    }
}
bool stateMachine;
bool Old_Machine;
uint32_t cacl_Timetick(uint8_t value)
{
    uint32_t timeLimit;
    timeLimit = value * 1000 * 60 * 60; // đổi thười gian h ra tick.
    return timeLimit;
}
void State_Machine(void)
{
    
    if(IO.digitalread(IO_Machine) && !Old_Machine) // công tắc xoay
    {
        stateMachine =! stateMachine;
        IO1_State = !RL1_State;
        IO2_State = !RL2_State;
        IO3_State = !RL3_State;
        IO4_State = !RL4_State;
    }
    Old_Machine = IO.digitalread(IO_Machine);

    if(stateMachine) Machine = MANUAL;
    else Machine = AUTO; //tự động.
    //Serial.println(stateMachine);
    
}
void setup_Button_(void)
{
    static bool oke_setup, oke_settime, oke_settemp,oke_choose, block_setup, block_settime, block_settemp;
    static int TempSet = 0; // biến tăng giảm giá trị nhiệt độ nếu muốn điều chỉnh cho phù hợp.
    // vào chế độ cài đặt.
    if(IO.digitalread(IO_1) && countSet != 10) // mode set | countset = 10 sẽ block
    {
        //Serial.println("da vao");
        if((millis() - timeTick) > 000) // 1s
        {  
            //Serial.println("da vao 1s");
            countSet ++;
            displayDelete = true;
            Serial.println(countSet);
            if(countSet == 10)
            {
                Setup = NO_SET_UP;
                countSetup = 0;
                countSetTime = 0;
                countSetTemp = 0;
                
            }
            timeTick = millis();
            
        }
    }
    else // nếu không đủ thời gian thì thoát.
    {
        if(countSet < 10 || countSet > 10) countSet = 0;
    }
/*******************************************************************************/
    if(countSet == 10)
    {   
        /*********************************nút số 1*********************************/
        if(IO.digitalread(IO_1) && !Old_IO_1)
        {
            Serial.println("Đã nhán nút số 1 để chọn .");
            displayDelete = true;
            if(countSetup != 0 && block_setup == false) oke_setup = true, block_setup = true;
            if(countSetTime != 0 && countSetup == 1 && block_settime == false) oke_settime = true, block_settime = true;
            if(countSetTemp != 0 && countSetup == 2 && block_settemp == false) oke_settemp = true, block_settemp = true;

            if(modeSetTime != NO_SET_TIME || SetTemp != NO_SET_TEMP) oke_choose = true;
            // Serial.print("oke setup: "); Serial.println(oke_setup);
            // Serial.print("oke settime: "); Serial.println(oke_settime);
            // Serial.print("oke settemp: "); Serial.println(oke_settemp);

            // Serial.print(" setup: "); Serial.println(Setup);
            // Serial.print(" settime: "); Serial.println(modeSetTime);
            // Serial.print(" settemp: "); Serial.println(SetTemp);

        }
        Old_IO_1 = IO.digitalread(IO_1);
        /*********************************nút số 2*********************************/
        // nếu nhấn nút 2 thì quay lại.
        if(IO.digitalread(IO_2) && !Old_IO_2)
        {
            //Serial.println("Đã nhán nút số 2 quay lại.");
            displayDelete = true;
            if(Setup == NO_SET_UP) 
            {
                Serial.println("thoát ở khỏi setup");
                Setup = NO_SET_UP; 
                modeSetTime = NO_SET_TIME;
                SetTemp = NO_SET_TEMP;
                countSet = 0; 
                oke_setup = false;
                oke_settime = false;
                oke_settemp = false;
                block_setup = false;
            }
            else if((Setup == SET_TIME && modeSetTime == NO_SET_TIME ) || (Setup == SET_TEMP && SetTemp == NO_SET_TEMP))
            { 
                Serial.println("thoát ở chọn RL");
                //countSetTime = 0;
                countSetup = 0; 
                oke_setup = false;
                block_setup = false;
                Setup = NO_SET_UP;
            }
            else if(modeSetTime != NO_SET_TIME)
            {
                Serial.println("thoát ở chỉnh time");
                countSetTime = 0;
                oke_settime = false;
                block_settime = false;
                modeSetTime = NO_SET_TIME;
            }
            else if(SetTemp != NO_SET_TEMP) 
            { 
                Serial.println("thoát ở chỉnh temp");
                countSetTemp = 0; 
                oke_settemp = false;
                block_settemp = false;
                SetTemp = NO_SET_TEMP;
            }
            // Serial.print("a: "); Serial.println(countSet);
            // Serial.print("b: "); Serial.println(countSetTime);
            // Serial.print("c: "); Serial.println(countSetTemp);
            // Serial.print("MOdesettime "); Serial.println(modeSetTime);
            // Serial.print("set temp "); Serial.println(SetTemp);
            // Serial.print("set up "); Serial.println(Setup);
        }
        Old_IO_2 = IO.digitalread(IO_2);
        
        /*********************************nút số 3*********************************/
        // tăng giá trị.
        if(IO.digitalread(IO_3) && !Old_IO_3)
        {
            Serial.println("Đã nhán nút số 3 ++.");
            StateSet =! StateSet;
            if(modeSetTime == NO_SET_TIME && SetTemp == NO_SET_TEMP && block_setup == false) countSetup++;    // set up.
            else if(Setup == SET_TIME && SetTemp == NO_SET_TEMP && oke_setup == true && block_settime == false) countSetTime++;        // set time
            else if(Setup == SET_TEMP && modeSetTime == NO_SET_TIME && oke_setup == true && block_settemp == false) countSetTemp++;      // set temp.

            if(countSetup >= 3 || countSetup == 0) countSetup = 1;
            else if((countSetTime >= 6 || countSetTime == 0) && oke_setup == true) countSetTime = 1;
            else if((countSetTemp >= 5 || countSetTemp == 0) && oke_setup == true) countSetTemp = 1;

            // tăng giá trị của các RL, SET TIME, TEMP.
            if(modeSetTime == SET_RL1){timeset1++; if(timeset1 > 12) timeset1 = 0;}
            else if(modeSetTime == SET_RL2){timeset2++; if(timeset2 > 12) timeset2 = 0;}
            else if(modeSetTime == SET_RL3){timeset3++; if(timeset3 > 12) timeset3 = 0;}
            else if(modeSetTime == SET_RL4){timeset4++; if(timeset4 > 12) timeset4 = 0;}
            
            // giá trị Temp
            if(SetTemp != NO_SET_TEMP)
            {
                TempSet++; 
                if(TempSet > 10 && SetTemp == xIMMPALE) TempSet = 0;
                Serial.println("Đã công temp.");
            }  // tăng nhiệt độ.

            // Serial.print("count SetUp: "); Serial.println(countSetup);
            // Serial.print("count SetTIME: "); Serial.println(countSetTime);
            // Serial.print("count SetTEMP: "); Serial.println(countSetTemp);

            // Serial.print("oke setup: "); Serial.println(oke_setup);
            // Serial.print("oke settime: "); Serial.println(oke_settime);
            // Serial.print("oke settemp: "); Serial.println(oke_settemp);

            // Serial.print(" setup: "); Serial.println(Setup);
            // Serial.print(" settime: "); Serial.println(modeSetTime);
            // Serial.print(" settemp: "); Serial.println(SetTemp);
        }
        Old_IO_3 = IO.digitalread(IO_3);
        /*********************************nút số 4*********************************/
        // giảm giá trị.
        if(IO.digitalread(IO_4) && !Old_IO_4)
        {
            Serial.println("Đã nhán nút số 4 --.");
            StateSet =! StateSet;
            if(modeSetTime == NO_SET_TIME && SetTemp == NO_SET_TEMP && block_setup == false) countSetup--;    // set up.
            else if(Setup == SET_TIME && SetTemp == NO_SET_TEMP && oke_setup == true && block_settime == false) countSetTime--;        // set time
            else if(Setup == SET_TEMP && modeSetTime == NO_SET_TIME && oke_setup == true && block_settemp == false) countSetTemp--;      // set temp.
            
            if(countSetup >= 3 || countSetup == 0) countSetup = 2;
            else if((countSetTime >= 6 || countSetTime == 0) && oke_setup == true) countSetTime = 5;
            else if((countSetTemp >= 5 || countSetTemp == 0) && oke_setup == true) countSetTemp = 4; 

            // giảm giá trị của các RL, SET TIME, TEMP.
            if(modeSetTime == SET_RL1){timeset1--; if(timeset1 > 12) timeset1 = 12;}
            else if(modeSetTime == SET_RL2){timeset2--; if(timeset2 > 12) timeset2 = 12;}
            else if(modeSetTime == SET_RL3){timeset3--; if(timeset3 > 12) timeset3 = 12;}
            else if(modeSetTime == SET_RL4){timeset4--; if(timeset4 > 12) timeset4 = 12;}
            
            // giá trị Temp
            if(SetTemp != NO_SET_TEMP)
            {
                Serial.println("Đã trừ temp.");
                TempSet--; 
                if((TempSet > 10 || TempSet > -10) && SetTemp == tempIMPALE) TempSet = 0;
                
            }  // giảm nhiệt độ.
            Serial.print("SetUp: "); Serial.println(countSetup);
            Serial.print("SetTIME: "); Serial.println(countSetTime);
            Serial.print("SetTEMP: "); Serial.println(countSetTemp);
        }
        Old_IO_4 = IO.digitalread(IO_4);

        /******************************Trạng thái*********************************/
        if(oke_setup)
        {   
            switch(countSetup)
            {
                case 0: {Setup = NO_SET_UP;break;}
                case 1: {Setup = SET_TIME;break;}
                case 2: {Setup = SET_TEMP;break;}
                default: break;
            }
            //Serial.print("ModeSetUP: "); Serial.println(Setup);
            //oke_setup =! oke_setup;
            if(oke_settime)
            {
                switch(countSetTime)
                {
                    case 0: {modeSetTime = NO_SET_TIME;break;}
                    case 1: {modeSetTime = SET_RL1;break;}
                    case 2: {modeSetTime = SET_RL2;break;}
                    case 3: {modeSetTime = SET_RL3;break;}
                    case 4: {modeSetTime = SET_RL4;break;}
                    default: break;
                }
               // Serial.print("ModeSetTime: "); Serial.println(modeSetTime);
            // oke_settime =! oke_settime;
            }
            else if(oke_settemp)
            {
                switch (countSetTemp)
                {
                    case 0:{SetTemp = NO_SET_TEMP; break;}
                    case 1:{SetTemp = xTEMP_MAX; break;}
                    case 2:{SetTemp = xTEMP_MIN; break;}
                    case 3:{SetTemp = xTEMP_LIMIT; break;}
                    case 4:{SetTemp = xIMMPALE; break;}
                    default: break;
                }
               // Serial.print("ModeSetTEMP: "); Serial.println(SetTemp);
                //oke_settemp =! oke_settemp;
            }
        }
        /************************************************************************************/
        if(Setup == SET_TIME)
        {
            if(modeSetTime == SET_RL1)
            {
                //Serial.println("RL1");
                timeLimit_DH1 = cacl_Timetick(timeset1);
            }
            else if(modeSetTime == SET_RL2)
            {
                //if(timeset2 * 1000 * 60 * 60 > timeLimit_DH2) timeTickDH2 = ;
                timeLimit_DH1 = cacl_Timetick(timeset2);
            }
            else if(modeSetTime == SET_RL3)
            {
                timeLimit_DH1 = cacl_Timetick(timeset3);
            }
            else if(modeSetTime == SET_RL4)
            {
                timeLimit_DH1 = cacl_Timetick(timeset4);
            }
        }
        // chọn set temp.
        else if(Setup == SET_TEMP) // set temp
        {
            if(SetTemp == xTEMP_MAX) tempSetMax = TempSet;
            else if(SetTemp == xTEMP_MIN) tempSetMin = TempSet;
            else if(SetTemp == xTEMP_LIMIT);
            else if(SetTemp == xIMMPALE) tempIMPALE = TempSet;
        }

        
    }
}
void Dislplay_LCD(void)
{
    static bool _bool;
    // hiển thị chế độ setup
    if(countSet == 10)
    {
        if(millis() - timeTick > 500)
        {
            _bool =! _bool;
            timeTick = millis();
            //Serial.println(_bool);
        }
        if(Setup == NO_SET_UP && modeSetTime == NO_SET_TIME && SetTemp == NO_SET_TEMP)
        {
            if(displayDelete){LCD.clear(); displayDelete = false;}
            LCD.setCursor(0,0); // cột 0 hàng 0;
            LCD.print("MODE SET UP");
            LCD.setCursor(0,1); // cột 0 hàng 1;
            LCD.print("SET TIME");
            if(_bool && countSetup == 1) LCD.setCursor(0,1), LCD.print("        ");
            LCD.setCursor(0,2); // cột 0 hàng 2;
            LCD.print("SET TEMP");
            if(_bool && countSetup == 2) LCD.setCursor(0,2), LCD.print("        ");
        }
        else if(Setup == SET_TIME)
        {
            if(modeSetTime != NO_SET_TIME)
            {
                if(displayDelete){LCD.clear(); displayDelete = false;}
                if(modeSetTime == SET_RL1)
                {
                    LCD.setCursor(0,0); // cột 0 hàng 0;
                    LCD.print("RELAY 1");
                    LCD.setCursor(0,1); // cột 0 hàng 0;
                    LCD.print("TIME:");
                    LCD.setCursor(6,1);
                    LCD.print(timeset1);
                    if(_bool && modeSetTime == SET_RL1) LCD.setCursor(6,1), LCD.print("   ");
                }
                else if(modeSetTime == SET_RL2)
                {
                    LCD.setCursor(0,0); // cột 0 hàng 0;
                    LCD.print("RELAY 2");
                    LCD.setCursor(0,1); // cột 0 hàng 0;
                    LCD.print("TIME:");
                    LCD.setCursor(6,1);
                    LCD.print(timeset2);
                    if(_bool && modeSetTime == SET_RL2) LCD.setCursor(6,1), LCD.print("   ");
                }
                else if(modeSetTime == SET_RL3)
                {
                    LCD.setCursor(0,0); // cột 0 hàng 0;
                    LCD.print("RELAY 3");
                    LCD.setCursor(0,1); // cột 0 hàng 0;
                    LCD.print("TIME:");
                    LCD.setCursor(6,1);
                    LCD.print(timeset3);
                    if(_bool) LCD.setCursor(6,1), LCD.print("   ");
                }
                else if(modeSetTime == SET_RL4)
                {
                    LCD.setCursor(0,0); // cột 0 hàng 0;
                    LCD.print("RELAY 4");
                    LCD.setCursor(0,1); // cột 0 hàng 0;
                    LCD.print("TIME:");
                    LCD.setCursor(6,1);
                    LCD.print(timeset4);
                    if(_bool) LCD.setCursor(6,1), LCD.print("   ");
                }
            }
            else
            {
                if(displayDelete){LCD.clear(); displayDelete = false;}
                LCD.setCursor(0,0); // cột 0 hàng 0;
                LCD.print("SET TIME:");
                LCD.setCursor(10,0); // cột 0 hàng 0;
                LCD.print("RELAY 1");
                if(_bool && countSetTime == 1) LCD.setCursor(10,0), LCD.print("       ");
                LCD.setCursor(10,1); // cột 0 hàng 0;
                LCD.print("RELAY 2");
                if(_bool && countSetTime == 2) LCD.setCursor(10,1), LCD.print("       ");
                LCD.setCursor(10,2); // cột 0 hàng 0;
                LCD.print("RELAY 3");
                if(_bool && countSetTime == 3)LCD.setCursor(10,2), LCD.print("       ");
                LCD.setCursor(10,3); // cột 0 hàng 0;
                LCD.print("RELAY 4");
                if(_bool && countSetTime == 4)LCD.setCursor(10,3), LCD.print("       ");
            }
        }
        else if(Setup == SET_TEMP)
        {
            if(SetTemp != NO_SET_TEMP)
            {
                if(displayDelete){LCD.clear(); displayDelete = false;}
                if(SetTemp == xTEMP_MAX)
                {
                    LCD.setCursor(0,0); // cột 0 hàng 0;
                    LCD.print("TEM MAX");
                    LCD.setCursor(0,1); // cột 0 hàng 0;
                    LCD.print("TEMP:");
                    LCD.setCursor(6,1);
                    LCD.print((uint8_t)tempSetMax);
                    if(_bool) LCD.setCursor(6,1), LCD.print("   ");
                }
                else if(SetTemp == xTEMP_MIN)
                {
                    LCD.setCursor(0,0); // cột 0 hàng 0;
                    LCD.print("TEM MAX");
                    LCD.setCursor(0,1); // cột 0 hàng 0;
                    LCD.print("TEMP:");
                    LCD.setCursor(6,1);
                    LCD.print((uint8_t)tempSetMin);
                    if(_bool) LCD.setCursor(6,1), LCD.print("   ");
                }
                else if(SetTemp == xIMMPALE)
                {
                    LCD.setCursor(0,0); // cột 0 hàng 0;
                    LCD.print("TEM MAX");
                    LCD.setCursor(0,1); // cột 0 hàng 0;
                    LCD.print("TEMP:");
                    LCD.setCursor(6,1);
                    LCD.print((uint8_t)tempIMPALE);
                    if(_bool) LCD.setCursor(6,1), LCD.print("   ");
                }
            }
            else
            {
                if(displayDelete){LCD.clear(); displayDelete = false;}
                LCD.setCursor(0,0); // cột 0 hàng 0;
                LCD.print("SET TEMP:");
                LCD.setCursor(9,0); // cột 0 hàng 0;
                LCD.print("TEMP MAX");
                if(_bool && countSetTemp == 1) LCD.setCursor(9,0), LCD.print("        ");
                LCD.setCursor(9,1); // cột 0 hàng 0;
                LCD.print("TEMP MIN");
                if(_bool && countSetTemp == 2) LCD.setCursor(9,1), LCD.print("        ");
                LCD.setCursor(9,2); // cột 0 hàng 0;
                LCD.print("TEMP LIMIT");
                if(_bool && countSetTemp == 3) LCD.setCursor(9,2), LCD.print("          ");
                LCD.setCursor(9,3); // cột 0 hàng 0;
                LCD.print("TEMP IMPALE");
                if(_bool && countSetTemp == 4) LCD.setCursor(9,3), LCD.print("            ");
            }
        }
    }
    else
    {
        if(displayDelete){LCD.clear(); displayDelete = false;}
        LCD.setCursor(9,0); // cột 0 hàng 0;
        LCD.print("VHB");
        // hiển thị trạng thái relay.
        LCD.setCursor(0,1); // cột 0 hàng 1;
        LCD.print("[RELAY]:");
        // Thời gian hoạt động relay.
        LCD.setCursor(0,2); // cột 0 hàng 2;
        LCD.print("[TIMESET]:");
        LCD.setCursor(11,2); // cột 0 hàng 2;
        LCD.print(timeset1);
        LCD.setCursor(13,2); // cột 0 hàng 2;
        LCD.print(timeset2);
        // hiển thị trạng thái hoạt động. 
        LCD.setCursor(0,3); // cột 0 hàng 3;
        LCD.print("[MODE]:");
        if(Machine == AUTO || Machine == MANUAL)
        {
            if(Machine == AUTO)
            {
                LCD.setCursor(10,3);
                LCD.print("AUTO  ");
                // đọc trạng thái relay.
                LCD.setCursor(10,1); // relay 1.
                LCD.print(!RL1_State);
                LCD.setCursor(12,1); // relay 2.
                LCD.print(!RL2_State);
                LCD.setCursor(14,1); // relay 3.
                LCD.print(!RL3_State);
                LCD.setCursor(16,1); // relay 4.
                LCD.print(!RL4_State);
            }
            else
            {
                LCD.setCursor(10,3);
                LCD.print("MANUAL");
                // đọc trạng thái nút nhấn.
                LCD.setCursor(10,1); // relay 1.
                LCD.print(IO1_State);
                LCD.setCursor(12,1); // relay 2.
                LCD.print(IO2_State);
                LCD.setCursor(14,1); // relay 3.
                LCD.print(IO3_State);
                LCD.setCursor(16,1); // relay 4.
                LCD.print(IO4_State);
            }
        }
    }
}