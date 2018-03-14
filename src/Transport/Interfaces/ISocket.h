#pragma once

#include "Transport/Commands/Command.h"
#include "Transport/Commands/Packet.h"

struct SocketTimeout
{
    uint32_t  timeout_read;
    uint32_t  timeout_write;
};

class ISocket
{
public:
    virtual bool WriteBytes(const uint8_t *data, uint32_t size) const = 0;
    virtual bool ReadBytes(uint8_t *data, uint32_t size) = 0;
};


class ISocketHandler
{
public:
    virtual SocketTimeout TimeoutsGet(uint32_t cmd_id) const = 0;
    virtual void TimeoutsSet(uint32_t cmd_id, SocketTimeout &timeout) = 0;

    virtual bool CommandSend(Command &command) = 0;
    virtual bool PacketReceive(Packet &packet, const Command &command) = 0;
};