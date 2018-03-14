#pragma once

#include "IndigoFrame.h"

class Command : public IndigoFrame
{
private:
    uint32_t m_timeout_write = 0;

public:
    Command();
    ~Command();

    uint32_t Timeout() const;
    void     TimeoutSet(uint32_t timeout);

    bool     SendBody(ISocket *socket);
};
