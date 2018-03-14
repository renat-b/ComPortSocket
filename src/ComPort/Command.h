#pragma once

#include "..\Common\MemPack.h"

class TransportDispatcher;
class ComPort;

class Command
{
protected:
    MemPack    m_body;

    uint32_t   m_timeout; 
    uint8_t    m_message_id;


public:
    Command();
    virtual ~Command();

    virtual uint32_t   MsgID() const = 0;
    virtual uint32_t   CmdID() const = 0; 
    virtual void       Clear();
    virtual bool       Send(ComPort &port) const;


public:
    uint32_t           GetTimeout() const;
    const uint8_t     *GetData() const;
    uint32_t           GetSize() const; 
};


class CommandEWL : public Command
{
private:
    #pragma pack(push, 1)
    struct DataCommandEwlHeader
    {
        uint8_t    ewl_address;
        uint8_t    master_address;
        uint8_t    cmd_id;
        uint8_t    message_id; 
        uint8_t    frame_number;
        uint16_t   message_length;    // big endian
        uint8_t    tool_address;
    };
    #pragma pack(pop)

private:
    DataCommandEwlHeader    m_header;   

public:

    CommandEWL();
    virtual ~CommandEWL();

public:
    virtual uint32_t   MsgID() const override;
    virtual uint32_t   CmdID() const override; 
    virtual void       Clear() override;

private:
    bool         CmdCreate(uint8_t cmd, const TransportDispatcher &handler);
    bool         CreateHeader(uint8_t cmd, const TransportDispatcher &handler, uint32_t body_size);
    void         CreateCRC16();
};


class CommandAMI : public Command
{
private:
    #pragma pack(push, 1)
    struct DataCommandAmiHeader
    {
        uint8_t    tool_address;
        uint8_t    master_address;
        uint8_t    cmd_id;
        uint8_t    message_id;
        uint8_t    frame_number;
        uint16_t   message_length;    // big endian
    };
    #pragma pack(pop)


private:
    DataCommandAmiHeader    m_header;

public:
    CommandAMI();
    virtual ~CommandAMI();

public:
    virtual uint32_t   MsgID() const override;
    virtual uint32_t   CmdID() const override;
    virtual void       Clear() override;

private:
    bool         CmdCreate(uint8_t cmd, const TransportDispatcher &handler);
    bool         CreateHeader(uint8_t cmd, const TransportDispatcher &handler, uint32_t body_size);
    void         CreateCRC16();
};