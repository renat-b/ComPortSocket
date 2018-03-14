#include "StdAfx.h"
#include "Command.h"

Command::Command()
{
}

Command::~Command()
{
}


uint32_t Command::Timeout() const
{
    return m_timeout_write;
}

void Command::TimeoutSet(uint32_t timeout)
{
    m_timeout_write = timeout;
}

bool Command::SendBody(ISocket *socket)
{
    return false;
}
