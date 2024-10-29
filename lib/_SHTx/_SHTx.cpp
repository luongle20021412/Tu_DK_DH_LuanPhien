#include "_SHTx.h"

_Modbus Modbus;

void setup_TempHum(uint8_t operating,int _Norm)
{
    // ghi vào eeproom.

}
void readtemphum(uint8_t id, float *temp, float *hum,int _Norm )
{
    uint8_t *Rcv;
    uint16_t Rawtemp, Rawhum;
    float x;
    //Serial.println("xxxxxxxxxxxxxxxxxxxxxxx");
    Rcv = Modbus.Send_requet(id, 0x04,0x0000,2); // lấy dữ liệu nhiệt độ và độ ẩm.
    //Serial.println("............");
    if(Rcv == nullptr)
    {
        //Serial.println("Đã nullptr");
        return;
    }
    Rawtemp = ((Rcv[0] << 8 )| Rcv[1]);   // lấy giá trị nhiệt độ.
    Rawhum = ((Rcv[2] << 8 )| Rcv[3]);    // láy giá trị độ ẩm.
    // biến đổi giá trị thô thành giá tị độ C.
    if(Rawtemp < 1000) x = 0.1; // độ phân giải 0.1
    else x= 0.01;               // độ phân giải 0.01
    *temp = (float)Rawtemp * x + _Norm;
    *hum = (float)Rawhum* x ;
    Serial.printf("Temx: %.2f\n", *temp);
    //Serial.printf("humx: %.2f\n", *hum);
    if(_Norm != 0)
    {
        *temp = *temp + _Norm;
    }
    // Serial.print(Rawtemp, HEX);
    // Serial.print(" ");
    // Serial.print(Rawhum, HEX);
    // Serial.println();
    // Serial.printf("Tem: %.2f\n", temp);
    // Serial.printf("hum: %.2f\n", hum);
}                    