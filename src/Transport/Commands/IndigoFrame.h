#pragma once

#include "CommonLibEmPulse/MemPack.h"

class ISocket;

class IndigoFrame
{
protected:
    struct FrameHeader
    {
        uint8_t     dest_address;     // destination address
        uint8_t     source_address;   // source address
        uint8_t     cmd_id;           // commmand code
        uint8_t     operation_id;     // operation code
        uint8_t     frame_number;     // frame number
        uint16_t    length;           // data length
        uint8_t     crc8;             // header crc8
        uint8_t     marker;           // protocol marker
        uint16_t    ser_num;          // serial number
        uint32_t    pack_num;         // pack number
        uint8_t     status;           // status
    };


protected:
    FrameHeader  m_header;
    MemPack      m_body;


public:
    IndigoFrame();
    ~IndigoFrame();
    
    uint8_t DestAddress() const;
    void    DestAddressSet(uint8_t val);

    uint8_t SourceAddress() const;       
    void    SourceAddressSet(uint8_t val); 

    uint8_t CmdID() const;       
    void    CmdIDSet(uint8_t val);       

    uint8_t OperationID() const; 
    void    OperationIDSet(uint8_t val); 

    uint8_t FrameNum() const; 
    void    FrameNumberSet(uint8_t val); 


    bool    HeaderRead(ISocket *socket);
    bool    HeaderWrite(ISocket *socket);

    
private:
    void    Clear();
    bool    Check();
};