#pragma once

#include "ComPort.h"
#include "Transport/Interfaces/ISocket.h"
#include "Transport/Commands/Command.h"
#include "Transport/Commands/Packet.h"

#include "CommonLibEmPulse/IndigoTimer.h"
#include "CommonLibEmPulse/OutLogger.h"


class ComPortHandler : public LoggerPrint, public ISocketHandler, public ISocket
{
public:
    struct Timeouts : SocketTimeout
    {
        Timeouts(uint32_t cmd_id, uint32_t timeout_read, uint32_t timeout_write)
        {
            this->cmd_id        = cmd_id;
            this->timeout_read  = timeout_read;
            this->timeout_write = timeout_write;
        };

        uint32_t  cmd_id;
    };

private:
    enum constants
    {
        COM_PORT_SPEED = 2 * 1024,
        DEFAULT_WRITE = 100,
        DEFAULT_READ  = 10,
    };
    typedef std::vector<Timeouts> CmdTimeoutArray;


private:
    uint8_t       m_ewl_address    = 0;
    uint8_t       m_master_address = 0;
    uint8_t       m_operation_id   = 0;
    uint8_t       m_frame_number   = 0;
    uint8_t       m_tool_address   = 0;

    ComPort       m_com_port;

    IndigoTimer   m_timer_full;
    IndigoTimer   m_timer;
    
    uint32_t      m_com_port_speed = COM_PORT_SPEED;

    uint32_t      m_default_timeout_write = DEFAULT_WRITE;
    uint32_t      m_default_timeout_read = DEFAULT_READ;

    CmdTimeoutArray m_timeouts;


public:
    ComPortHandler();
    virtual  ~ComPortHandler();

    virtual SocketTimeout TimeoutsGet(uint32_t cmd_id) const override;
    virtual void   TimeoutsSet(uint32_t cmd_id, SocketTimeout &timeout) override;

    virtual bool   CommandSend(Command &command) override;
    virtual bool   PacketReceive(Packet &packet, const Command &command) override;

    virtual bool   WriteBytes(const uint8_t *data, uint32_t size) const override;
    virtual bool   ReadBytes(uint8_t *data, uint32_t size) override;
};