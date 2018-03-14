#pragma once

#include "Packet.h"


class Packets
{
private:
    typedef std::vector<Packet *> PacketArray;


private:
    PacketArray       m_packets;
    mutable uint32_t  m_pos;


public:
    Packets();
    ~Packets();

    void         Clear();

    bool         AddPacket(const Packet &packet);
    uint32_t     Count() const;
    
    Packet     *GetFirstPacket() const;
    Packet     *GetNextPacket()  const;
};