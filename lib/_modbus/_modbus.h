#ifndef _modbus_h_
#define _modbus_h_

#include "Arduino.h"

enum Serial_t
{
    NoSerial = 0,
    SERIAL1,
    SERIAL2
}; // cài đặt trên web hoặc tool.
enum Mode_t
{
    SLAVE = 1,
    MASTER 
};

#define FC03 0x03   //  đọc dữ liệu.
#define FC04 0x04    // 
#define FC06 0x06
#define FC16 0x10
// size send
#define MAX_REGISTOR 1000 // max thanh ghi  

 #define SIZE_RESPONE 200 
 #define SIZE_CMD_SEND 50
 #define SIZE_RCV_FC03 12
 #define SIZE_RCV_FC06 12
 #define SIZE_RCV_FC16 200


//#define REG          0x0000
//#define numREG       0x0001
class _Modbus 
{
    private:
    public: 
        void begin_modbus(void);
        uint8_t Listens(void) ; // có thể đọc và ghi dữ liệu.
        uint8_t* Send_requet(uint8_t id,uint8_t xFunc = 0x03, uint16_t xREG = 0x0000,uint8_t _numREG = 0x0001);
        void  Send_requet(uint8_t id,uint8_t xFunc = 0x03, uint16_t xREG = 0x0000,uint8_t * Data = nullptr);
        uint16_t _crc16(uint8_t arr[], uint8_t length);
        
        //bien
        uint8_t Mode_channel = Serial1;
        //Biến
        uint8_t	id; // ID thiết bị	
        uint8_t	mode; // Master or slave
        uint8_t Rcv[SIZE_CMD_SEND]; // Mã truyền đi khi ở chế độ master
        uint8_t Data_Slave[SIZE_RESPONE]; 
        uint8_t Data_Master[SIZE_RESPONE]; // Mã phản hồi khi ở chế độ Slave
        uint8_t Data_Respons[SIZE_RESPONE];
        uint16_t REG_Modbus[MAX_REGISTOR]; // Mảng mã nhận đc ( mảng mã dữ liệu).
};

#endif