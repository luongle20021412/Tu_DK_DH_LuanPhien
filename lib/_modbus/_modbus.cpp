#include "_modbus.h"
Serial_t xSerial = SERIAL1;
Mode_t xMode = MASTER;

void _Modbus :: begin_modbus(void) // khởi tạo trong setup.
{
    // sẽ phải đọc cấu hình từ bộ nhớ. hoặc socket tcp.
    if(xSerial == SERIAL1)
    {
        Serial1.begin(9600,SERIAL_8N1, 16,17,false, 10UL);
    }
    else if(xSerial == SERIAL2)
    {
        Serial2.begin(9600,SERIAL_8N1, 32,33,false, 10UL);
    }

}
// gửi yêu cầu dành cho master FC10.
void _Modbus ::Send_requet(uint8_t id,uint8_t xFunc, uint16_t xREG, uint8_t *Data)
{
    // [1,2,3,4,5,6,7,8]        [1,2] -> 1 byte. 5
    uint8_t _DataSize = sizeof(Data);
    uint8_t _numREG;

    if (_DataSize % 2 == 0) _numREG = _DataSize / 2;
    else                    _numREG = (_DataSize / 2) + 1;

    uint8_t xREGHigh = (uint8_t)(xREG / 256);
    uint8_t xREGLow = (uint8_t)(xREG % 256);

    uint8_t _numREGHingh = _numREG * 256;
    uint8_t _numREGLow = _numREG;
    uint8_t lenth = 7 +_DataSize;
    
    uint8_t Send[lenth] = {
                        id,                     //id slave
                        xFunc,                  // function code
                        xREGHigh,               // địa chỉ thanh ghi bắt đầu(byte cao)
                        xREGLow,                // (byte thấp)
                        _numREGHingh,           // số lượng thanh ghi cần đọc (byte cao) // 
                        _numREGLow,             // (byte thấp).
                        _DataSize               // số byte dữ liệu cần ghi.
                        };
    // láy dữ liệu cần ghi.
    for(uint8_t i = 0; i < _DataSize; i ++)
    {
        Send[7 + i] = Data[i];
    }
    // in
    for(uint8_t i = 0; i < lenth; i ++)
    {
        Serial.print(Send[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");
            // crc
            Send[lenth - 2] =  _crc16(Send,lenth - 2) % 256;      // byte thấp.
            Send[lenth - 1] =  _crc16(Send,lenth - 2) / 256;      // byte cao.
            Serial.print(_crc16(Send,lenth - 2), HEX);
            Serial.print(" ");

    if(xMode == MASTER)
    {
        Serial.println("đã vào master...");
        if (xSerial == SERIAL1)
        {
            // gửi yêu cầu phản hồi 
            Serial.println("đã vào Serial...");
            Serial1.write(Send, lenth);
            Serial1.flush();
            for(uint8_t i = 0; i < lenth; i ++)
            {
                Serial.print(Send[i], HEX);
                Serial.print(" ");
            }
            Serial.println("");
        }
        else if(xSerial == SERIAL2)
        {
            Serial.println("đã vào Serial...");
            Serial2.write(Send, lenth);
            Serial2.flush();
        }
    }
}
// gửi yêu cầu dành cho master FC03 FC04 FC06.
uint8_t* _Modbus ::Send_requet(uint8_t id,uint8_t xFunc, uint16_t xREG,uint8_t _numREG)
{   
    /*nếu sử dụng fc 06 thì biến _numREG sẽ thành biến để ghi giá trị vào thanh ghi.*/
    // biến địa chỉ bắt đầu thanh ghi.
    uint8_t xREGHigh = (uint8_t)(xREG / 256);
    uint8_t xREGLow = (uint8_t)(xREG % 256);
    // biến số lượng thanh ghi.
    uint8_t _numREGHingh = 0x00;
    uint8_t _numREGLow = _numREG;

    uint8_t Send[8] = {
                        id,                     //id slave
                        xFunc,                  // function code
                        xREGHigh,               // địa chỉ thanh ghi bắt đầu(byte cao)
                        xREGLow,                // (byte thấp)
                        _numREGHingh,           // số lượng thanh ghi cần đọc (byte cao) // có thể thay thế thành dữ liệu để gửi đi FC06.
                        _numREGLow,             // (byte thấp)
                        };
            Send[6] = _crc16(Send, 6) % 256;               // crc (byte thấp)
            Send[7] = _crc16(Send, 6) / 256;
    // tạo cấu trúc dữ liệu để gửi yêu cầu.
    if(xMode == MASTER)
    {
        //Serial.println("đã vào master...");
        if (xSerial == SERIAL1)
        {   // gửi yêu cầu phản hồi 
            //Serial.println("đã vào Serial...");
            if(Send[1] == 0x03 || Send[1] == 0x04 || Send[1] == 0x06)
            {
                //Serial.println("đã gửi yêu cầu 03 || 04...");
                Serial1.write(Send, 8);
                Serial1.flush();
                // for (int i = 0; i < 8; i++)
                // {
                //     Serial.print(Send[i], HEX);
                //     Serial.print(" ");
                // }
                // Serial.println();
            }
            delay(100);
            // nhận yêu cầu phản hồi.
            if(Serial1.available() > 0) // có phản hồi lại
            {
                //Serial.println("đã nhận yêu cầu phản hồi...");
                uint8_t lenght =  5 + _numREG * 2;
                // Serial.println(lenght);
                if(Send[1] == 0x03 || Send[1] == 0x04 ) Serial1.readBytes(Rcv,lenght); // só byte phản hồi tăng theo số lượng thanh ghi muốn đọc.
                if(Send[1] == 0x06)                     Serial1.readBytes(Rcv, 8);  // mặc định là phản hồi 8 byte.
                
                // for (int i = 0; i < lenght; i++)
                // {
                //     Serial.print(Rcv[i], HEX);
                //     Serial.print(" ");
                // }
            }
            else
            {
                //Serial.println("chưa có phản hồi...");
                return nullptr;
            }  
        }
        else if(xSerial == SERIAL2)
        {
            if(Send[1] == 0x03 || Send[1] == 0x04 || Send[1] == 0x06)
            {
                Serial2.write(Send, 8);
                Serial2.flush();
            } 
            delay(10);
            // nhận yêu cầu phản hồi.
            if(Serial2.available() > 0) // có phản hồi lại
            {
                uint8_t lenght =  5 + _numREG * 2;
                if(Send[1] == 0x03 || Send[1] == 0x04 ) Serial2.readBytes(Rcv,lenght); // só byte phản hồi tăng theo số lượng thanh ghi muốn đọc.
                if(Send[1] == 0x06)                     Serial2.readBytes(Rcv, 8);  // mặc định là phản hồi 8 byte.
                // for(uint8_t i = 0; i < _numREG * 2; i++)
                // {
                //     Serial.print(Rcv[i], HEX);
                //     Serial.print("");
                // }
                // Serial.println();
            }   
        }
        // check crc
        // FC 0x03 0x04.
        if(Send[1] == 0x03 || Send[1] == 0x04)
        {
            //Serial.println("đã vào check crc đc phản hồi...");
            uint8_t lenght = (_numREG * 2) + 5;
            // Serial.println(".............");
            // Serial.println(_numREG, HEX);
            // Serial.println(lenght, HEX);
            // Serial.println( _crc16(Rcv, lenght - 2), HEX);
            // Serial.println(Rcv[lenght - 2], HEX);
            // Serial.println(Rcv[lenght - 1], HEX);
            Data_Master[_numREG];
            if(Rcv[lenght - 2] == _crc16(Rcv, lenght - 2) % 256 && Rcv[lenght - 1] == _crc16(Rcv, lenght - 2) / 256)
            {
                //Serial.println("đã vào lấy dữ liệu...");
                // lấy dữ liệu phản hồi.
                for(uint8_t i = 0; i < _numREG * 2; i++)
                {
                    Data_Master[i] = (Rcv[3 + i]); // lấy dữ liệu của từng thanh ghi. (byte cao)
                    //Serial.print(Data_Master[i], HEX);
                    //Serial.print("");
                }
                // Serial.println();
                return Data_Master;
            }
            return nullptr;
        }
        //FC06
        else if( Send[1] == 0x06)
        {
            Data_Master[8];
            if(Rcv[6] == _crc16(Rcv, 6) % 256 && Rcv[7] == _crc16(Rcv, 6) / 256)
            {
                for (uint8_t i = 0; i < 8; i++)
                {
                    Data_Master[i] = Rcv[i];
                }
                // Serial.println("Đã ghi thành công...");
                return Data_Master;
            }
        }    
        return nullptr;
    }
    return nullptr;
}
// dành cho slave.
uint8_t _Modbus :: Listens(void) 
{
    String str;
    uint8_t _read;
    uint8_t lenght;
    if(xMode == SLAVE)
    {
        if(xSerial == SERIAL1)
        {   
            if(Serial1.available() > 0) // có phản hồi lại
            {
                str = Serial1.readString();
                Serial.println("đã nhận yêu cầu phản hồi...");
                for (uint8_t i = 0; i < str.length(); i++) 
                {
                    Rcv[i] = str[i];
                    Serial.print(Rcv[i]);
                    Serial.print(" ");
                }
                lenght = sizeof(Rcv);
                _read = SERIAL1;
            }
        }
        else if(xSerial == SERIAL2)
        {
            if(Serial2.available() > 0) // có phản hồi lại
            {
                str = Serial2.readString();
                Serial.println("đã nhận yêu cầu phản hồi...");
                for (uint8_t i = 0; i < str.length(); i++) 
                {
                    Rcv[i] = str[i];
                    Serial.print(Rcv[i]);
                    Serial.print(" ");
                }
                lenght = sizeof(Rcv);
                _read = SERIAL2;
            }
        }
        if(_read != 0)
        {
            //check crc
            if(Rcv[0] = id && Rcv[1] == 0x03)
            {
                Serial.println("đã vào check crc đc phản hồi...");
                if(Rcv[lenght - 2] == _crc16(Rcv, lenght - 2) % 256 && Rcv[lenght - 1] == _crc16(Rcv, lenght - 2) / 256)
                {
                    //tạo cấu trúc phản hồi.
                    Data_Respons[0] = Rcv[0];               // id slave.
                    Data_Respons[1] = Rcv[1];               // funccode.
                    Data_Respons[2] = Rcv[5] * 2;           // tổng số byte dữ liệu số lượng thanh ghi * 2.
                    uint8_t lenght = 3 + Data_Respons[2];
                    for(uint8_t i = 0; i < Data_Respons[2]; i++)
                    {
                        Data_Respons[3 + i * 2] = REG_Modbus[i] / 256;      // ghi từng byte vào mảng phản hồi.( byte cao).
                        Data_Respons[4 + i * 2] = REG_Modbus[i] % 256;      // byte thấp.
                    }
                    // crc.
                    Data_Respons[lenght + 1] = _crc16(Data_Respons,lenght) % 256; // byte thấp. 
                    Data_Respons[lenght + 2] = _crc16(Data_Respons,lenght) / 256; // byte cao.
                    // gửi phản hồi.
                    for(uint8_t i = 0; i < sizeof(Data_Respons); i ++)
                    {
                        if(xSerial == SERIAL1)          Serial1.write(Data_Respons[i]);
                        else if(xSerial == SERIAL2)     Serial2.write(Data_Respons[i]);
                    }
                }
            }
            //FC06
            else if(Rcv[0] = id && Rcv[1] == 0x06)
            {  
                Serial.println("đã vào check crc đc phản hồi...");
                if(Rcv[lenght - 2] == _crc16(Rcv, lenght - 2) % 256 && Rcv[lenght - 1] == _crc16(Rcv, lenght - 2) / 256)
                {
                    // ghi vào thanh ghi.
                    uint16_t xReg = Rcv[2] * 256 + Rcv[3];      //lấy địa chỉ thanh ghi.
                    REG_Modbus[xReg] = Rcv[4] * 256 + Rcv[5];   // ghi vào địa chỉ thanh ghi modbus.
                    // phản hồi.
                    for (uint8_t i = 0; i < lenght; i++)
                    {
                        if(xSerial == SERIAL1)          Serial1.write(Rcv[i]);
                        else if(xSerial == SERIAL2)     Serial2.write(Rcv[i]);
                    }
                    
                }
            }
            else if(Rcv[0] = id && Rcv[1] == 0x10)
            {
                Serial.println("đã vào check crc đc phản hồi...");
                if(Rcv[lenght - 2] == _crc16(Rcv, lenght - 2) % 256 && Rcv[lenght - 1] == _crc16(Rcv, lenght - 2) / 256)
                {
                    // ghi vào thanh ghi.
                    uint16_t xReg = Rcv[2] * 256 + Rcv[3];      //lấy địa chỉ thanh ghi.
                    uint8_t _DataSize = Rcv[6];    // lấy số lượng thanh ghi.
                    for( uint8_t i = 0; i < _DataSize / 2; i ++)
                    {
                        REG_Modbus[xReg] = Rcv[6 + _DataSize] * 256 + Rcv[7 + _DataSize];   // ghi vào địa chỉ thanh ghi modbus.
                    }
                    // phản hồi.
                    for( uint8_t i = 0; i < 6; i++)
                    {
                        Data_Respons[i] = Rcv[i]; 
                    }
                    Data_Respons[6] = _crc16(Data_Respons, 6) % 256;
                    Data_Respons[7] = _crc16(Data_Respons, 6) / 256;

                    for( uint8_t i = 0; i < sizeof(Data_Respons); i++)
                    {
                        if(xSerial == SERIAL1)          Serial1.write(Data_Respons[i]);
                        else if(xSerial == SERIAL2)     Serial2.write(Data_Respons[i]);
                    }
                }
            }
        }
    }
}
uint16_t _Modbus::_crc16(uint8_t arr[], uint8_t length)
{
    uint16_t _crc = 0xFFFF;//giá trị crc ban đầu.
    
    for (int i = 0; i < length; i++)
    {
        _crc ^= arr[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if(_crc & 0x0001)
            {
                _crc >>= 1;
                _crc ^=0xA001;
            }
            else
            {
                _crc >>= 1;
            }
        }
    }
    return (_crc);
}
