#pragma once

#include "IndigoFrame.h"

class Packet : public IndigoFrame
{
private:
    uint32_t m_timeout_read = 0;


public:
    Packet();
    ~Packet();

    uint32_t Timeout() const;
    void     TimeoutSet(uint32_t timeout);

    bool     ReadBody(ISocket *socket);
    bool     Check() const;
};
