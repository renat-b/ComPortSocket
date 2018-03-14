#pragma once

#include "..\Common\MemPack.h"

class TransportDispatcher;

class Packet
{
protected:
    MemPack            m_body;
    uint8_t            m_id;
    uint32_t           m_timeout;


public:
    Packet();
    virtual ~Packet();

    bool               Init(const TransportDispatcher &handler);
    uint32_t           GetTimeout() const;
    uint8_t            ID() const;
    bool               Check() const;

    virtual void       Clear();
    virtual bool       Assign(const Packet &other);

    virtual bool       ReadHeader(TransportDispatcher &handler) = 0;
    virtual bool       ReadBody(TransportDispatcher &handler) = 0;

    virtual Packet    *Clone() const = 0;

    virtual uint8_t    CmdID() const = 0;
    virtual uint8_t    MessageID() const = 0;
    virtual uint16_t   Length() const = 0;

protected:
    virtual uint16_t   CalcCRC16Header() const = 0;
};

class PacketEWL : public Packet
{
private:

    #pragma pack(push, 1)
    struct HeaderEWL
    {
        uint8_t     own_address;
        uint8_t     command_id;
        uint8_t     message_id;
        uint8_t     reserve;          
        uint16_t    message_length;   // big endian
        uint8_t     tool_address;     
    };
    #pragma pack(pop)


private:
    HeaderEWL   m_header;

public:

    PacketEWL();
    virtual ~PacketEWL();

    virtual void       Clear();
    virtual bool       Assign(const Packet &other);

    virtual bool       ReadHeader(TransportDispatcher &handler) override;
    virtual bool       ReadBody(TransportDispatcher &handler) override;

    virtual uint8_t    CmdID()  const override;
    virtual uint8_t    MessageID()  const override;
    virtual uint16_t   Length() const override;

protected:
    virtual uint16_t   CalcCRC16Header() const override;          
};

class PacketAMI : public Packet
{
private:
    #pragma pack(push, 1)
    struct HeaderAMI
    {
        uint8_t     own_address;
        uint8_t     command_id;
        uint8_t     message_id;
        uint8_t     frame_number;
        uint16_t    message_length;    // big endian
    };
    #pragma pack(pop)

private:
    HeaderAMI   m_header;

public:

    PacketAMI();
    virtual ~PacketAMI();

    virtual void       Clear();
    virtual bool       Assign(const Packet &other);

    virtual bool       ReadHeader(TransportDispatcher &handler) override;
    virtual bool       ReadBody(TransportDispatcher &handler) override;

    virtual uint8_t    CmdID()  const override;
    virtual uint8_t    MessageID()  const override;
    virtual uint16_t   Length() const override;

protected:
    virtual uint16_t   CalcCRC16Header() const override;
};