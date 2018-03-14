#include "StdAfx.h"
#include "Packet.h"
#include "TransportDispatcher.h"
#include "..\Common\ConvertData.h"
#include "..\Common\crc16.h"

Packet::Packet() : m_id(0), m_timeout(0)
{
}


Packet::~Packet()
{
}

bool Packet::Init(const TransportDispatcher &handler)
{
    Clear(); 

    m_timeout = handler.GetCmdTimeout(m_id).timeout_read;
    return true;
}

uint32_t Packet::GetTimeout() const
{
    return m_timeout;
}

void Packet::Clear()
{   
    m_body.Clear();
}

bool Packet::Assign(const Packet &other)
{
    bool r = m_body.Assign(other.m_body);

    if (r)
    {
        m_id  = other.m_id; 
        m_timeout = other.m_timeout;
    }
    return r;
}

uint8_t Packet::ID() const
{
    return m_id;
}

bool Packet::Check() const 
{
    if ((Length() + sizeof(uint16_t)) > m_body.GetSize())
        return false;

    uint16_t crc16_calc = CalcCRC16Header(); 
    // продолжим расчет crc для тела сообщения, за вычетом crc16 в конце тела сообщения
    uint8_t *data = m_body.GetData();
    uint32_t len = m_body.GetSize() - sizeof(uint16_t);
    crc16_calc = get_crc_16(crc16_calc, data, len); 
    
    // найдем место где расположнеия crc16 в теле сообщения     
    data = m_body.GetData() + m_body.GetSize() - sizeof(uint16_t);
    uint16_t crc16_body = *(uint16_t *)(data);
    
    // crc не совпали, ошибка
    if (crc16_calc != crc16_body)
        return false;


    data = m_body.GetData();
    len  = m_body.GetSize() - sizeof(uint16_t);
    return true;
}


PacketEWL::PacketEWL()
{
    memset(&m_header, 0, sizeof(m_header));
}

PacketEWL::~PacketEWL()
{
}

void PacketEWL::Clear()
{
    memset(&m_header, 0, sizeof(m_header));
    Packet::Clear();
}

bool PacketEWL::Assign(const Packet &other)
{
    if (other.ID() != ID())
        return false;

    m_header = ((const PacketEWL &) other).m_header;

    bool r = Packet::Assign(other);
    return r;
}

bool PacketEWL::ReadHeader(TransportDispatcher &handler)
{
    bool r = handler.ReadData((uint8_t *)&m_header, sizeof(m_header));
    return r;
}

bool PacketEWL::ReadBody(TransportDispatcher &handler)
{
    uint8_t  *data;
    uint32_t  len = Length() + sizeof(uint16_t);
    
    if (!m_body.Resize(len))
        return false;

    data = m_body.GetData();
    bool r = handler.ReadData(data, len);
    if (r)
        m_body.SetSize(len);
    return r;
}

uint8_t PacketEWL::CmdID() const
{
    return m_header.command_id;
}

uint8_t PacketEWL::MessageID() const
{
    return m_header.message_id;
}

uint16_t PacketEWL::Length() const
{
    uint16_t len = m_header.message_length;
    Big2LittelEndian(&len, sizeof(len));
    len -= sizeof(uint8_t);
    return len;
}

uint16_t PacketEWL::CalcCRC16Header() const
{
    uint16_t crc16 = get_crc_16(0, (uint8_t *)&m_header, sizeof(m_header)); 
    return crc16;
}

PacketAMI::PacketAMI()
{
    memset(&m_header, 0, sizeof(m_header));
}

PacketAMI::~PacketAMI()
{
    memset(&m_header, 0, sizeof(m_header));
}

void  PacketAMI::Clear()
{
    memset(&m_header, 0, sizeof(m_header));
    Packet::Clear();
}

bool PacketAMI::Assign(const Packet &other)
{
    if (ID() != other.ID())
        return false;

    m_header = ((const PacketAMI &)other).m_header;

    bool r = Packet::Assign(other);
    return r;
}

bool PacketAMI::ReadHeader(TransportDispatcher &handler)
{
    bool r = handler.ReadData((uint8_t *)&m_header, sizeof(m_header));
    return r;
}

bool PacketAMI::ReadBody(TransportDispatcher &handler)
{
    uint8_t  *data;
    uint32_t  len = Length() + sizeof(uint16_t);
    
    if (!m_body.Resize(len))
        return false;

    data = m_body.GetData();
    bool r = handler.ReadData(data, len);
    if (r)
        m_body.SetSize(len);
    return r;
}

uint8_t PacketAMI::CmdID() const
{
    return m_header.command_id;
}

uint8_t PacketAMI::MessageID() const
{
    return m_header.message_id;
}

uint16_t PacketAMI::Length() const
{
    uint16_t len = m_header.message_length;
    Big2LittelEndian(&len, sizeof(len));
    return len;
}

uint16_t PacketAMI::CalcCRC16Header() const
{
    uint16_t crc16 = get_crc_16(0, (uint8_t *)&m_header, sizeof(m_header)); 
    return crc16;
}