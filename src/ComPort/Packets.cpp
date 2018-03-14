#include "StdAfx.h"
#include "Packets.h"


Packets::Packets() : m_pos(0)
{
}


Packets::~Packets()
{
    Clear();
}


void Packets::Clear()
{
    for (uint32_t i = 0; i < m_packets.size(); i++)
    {
        delete m_packets[i];
        m_packets[i] = nullptr;
    }
    m_packets.clear();
}


bool Packets::AddPacket(const Packet &packet)
{
    Packet *dst = packet.Clone();

    if (!dst)
        return false;
    
    bool r = dst->Assign(packet);
    if (r)
        m_packets.push_back(dst);
    else
    {
        delete dst;
	    dst = nullptr;
    }
    return r;
}


uint32_t Packets::Count() const
{
    uint32_t count = m_packets.size();
    return count;
}


Packet *Packets::GetFirstPacket() const
{
    m_pos = 0;
    if (m_pos >= m_packets.size())
        return nullptr; 

    Packet *packet = m_packets[m_pos];
    m_pos++;
    return packet;
}


Packet *Packets::GetNextPacket() const
{
    if (m_pos >= m_packets.size())
        return nullptr;

    Packet *packet = m_packets[m_pos];
    m_pos++;
    return packet;
}
