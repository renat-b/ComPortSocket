#include "StdAfx.h"
#include "Packet.h"

Packet::Packet()
{
}

Packet::~Packet()
{
}


uint32_t Packet::Timeout() const
{
    return m_timeout_read;
}

void Packet::TimeoutSet(uint32_t timeout)
{
    m_timeout_read = timeout;
}

bool Packet::ReadBody(ISocket *socket)
{
    return false;
}

bool Packet::Check() const
{
    return false;
}

