#include "_SHTx.h"

_Modbus Modbus;

void readtemphum(uint8_t id, float *temp, float *hum)
{
    uint8_t *Rcv;
    uint16_t Rawtemp, Rawhum;

    Rcv = Modbus.Send_requet(id, 0x04,0x0000,2); // lấy dữ liệu nhiệt độ và độ ẩm.
    if(Rcv == nullptr)
    {
        return;
    }
    Rawtemp = ((Rcv[0] << 8 )| Rcv[1]);   // lấy giá trị nhiệt độ.
    Rawhum = ((Rcv[2] << 8 )| Rcv[3]);    // láy giá trị độ ẩm.
    // biến đổi giá trị thô thành giá tị độ C.
    *temp = (float)Rawtemp * 0.1;
    *hum = (float)Rawhum* 0.1;
    // Serial.print(Rawtemp, HEX);
    // Serial.print(" ");
    // Serial.print(Rawhum, HEX);
    // Serial.println();
    // Serial.printf("Tem: %.2f\n", temp);
    // Serial.printf("hum: %.2f\n", hum);
}                    