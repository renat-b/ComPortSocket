#pragma once

#include "Command.h"
#include "Packet.h"
#include "ComPort.h"
#include "..\Common\IndigoTimer.h"
#include "..\Common\OutLogger.h"

class TransportDispatcher : public LoggerPrint
{
    friend class CommandEWL;
    friend class CommandAMI;
    friend class PacketEWL;
    friend class PacketAMI;

public:
    struct CmdPortHandlerTimeout
    {
        uint32_t  cmd_id;
        uint32_t  timeout_read;
        uint32_t  timeout_write;
    };

private:
    enum constants
    {
        COM_PORT_SPEED = 2 * 1024,
        DEFAULT_WRITE = 100,
        DEFAULT_READ  = 10,
    };
    typedef std::vector<CmdPortHandlerTimeout> CmdTimeoutArray;


private:
    uint8_t       m_ewl_address;
    uint8_t       m_master_address;
    uint8_t       m_operation_id;
    uint8_t       m_frame_number;
    uint8_t       m_tool_address;

    ComPort       m_com_port;

    IndigoTimer   m_timer_full;
    IndigoTimer   m_timer;
    
    uint32_t      m_com_port_speed;

    uint32_t      m_default_timeout_write;
    uint32_t      m_default_timeout_read;

    CmdTimeoutArray m_timeouts;


public:
    TransportDispatcher();
    virtual  ~TransportDispatcher();

    uint8_t    GetOperationID() const;

    void       SetDefaultTimeouts(uint32_t default_timeout_read, uint32_t default_timeout_write);
    void       SetCmdTimeout(uint32_t cmd_id, uint32_t timeout_read, uint32_t timeout_write);
    CmdPortHandlerTimeout  GetCmdTimeout(uint32_t cmd_id) const;
    bool       SendCommand(const Command &command);
    bool       ReadPacket(Packet &packet, const Command &command);


protected:
    bool       ReadData(uint8_t *data, uint32_t size);
};