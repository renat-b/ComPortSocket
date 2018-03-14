#include "stdafx.h"
#include "TransportDispatcher.h"

TransportDispatcher::TransportDispatcher() : m_ewl_address(0), m_master_address(0), m_operation_id(0), m_frame_number(0), m_tool_address(0), m_com_port_speed(COM_PORT_SPEED), m_default_timeout_write(DEFAULT_WRITE), m_default_timeout_read(DEFAULT_READ)
{
}

TransportDispatcher::~TransportDispatcher()
{
}

uint8_t TransportDispatcher::GetOperationID() const
{
    uint8_t r = 0;
    
    return r;
    /*
    if (m_tool)
    {
        r = m_tool->GetOperationID();
        return r;
    }
    return 0;
    */
}

void TransportDispatcher::SetDefaultTimeouts(uint32_t default_timeout_read, uint32_t default_timeout_write)
{
    m_default_timeout_read  = default_timeout_read;
    m_default_timeout_write = default_timeout_write;
}

void TransportDispatcher::SetCmdTimeout(uint32_t cmd_id, uint32_t timeout_read, uint32_t timeout_write)
{
    for (uint32_t i = 0; i < m_timeouts.size(); i++)
    {
        CmdPortHandlerTimeout &cmd_timing = m_timeouts[i];

        if (cmd_timing.cmd_id == cmd_id)
        {
            cmd_timing.timeout_read  = timeout_read;
            cmd_timing.timeout_write = timeout_write;
            return;
        }
    }

    CmdPortHandlerTimeout cmd_timing = { cmd_id, timeout_read, timeout_write };
    m_timeouts.push_back(cmd_timing);
}

TransportDispatcher::CmdPortHandlerTimeout TransportDispatcher::GetCmdTimeout(uint32_t cmd_id) const
{
    CmdPortHandlerTimeout default_timeout = { cmd_id, m_default_timeout_read, m_default_timeout_write };

    for (uint32_t i = 0; i < m_timeouts.size(); i++)
    {
        const CmdPortHandlerTimeout &timeout = m_timeouts[i];

        if (timeout.cmd_id == cmd_id)
            return timeout;
    }

    return default_timeout;
}

bool TransportDispatcher::SendCommand(const Command &command)
{
    bool  r;
    DWORD wait_ms;

    r = m_com_port.ClearPort();
    if (!r)
    {
        PrintLog(LogCmdError, "Error clear COM port [%d]", m_com_port.LastOS());
        return false;
    }

    r = command.Send(m_com_port);
    if (!r)
    {
        PrintLog(LogCmdError, "Error write COM port [%d]", m_com_port.LastOS());
        return false;
    }
    
    // рассчитаем время ожидания записи в com порт, если менее 100мс, берем 100мс
    wait_ms = (command.GetSize() * 1000) / m_com_port_speed;
    if (wait_ms < command.GetTimeout())
        wait_ms = command.GetTimeout();

    m_timer.Wait(wait_ms);
    return r;
}

bool TransportDispatcher::ReadPacket(Packet &packet, const Command &command)
{
    // рассчитаем колво мс в течении которого можно получать данные
    uint32_t delay_readed_ms = packet.GetTimeout();

    // запускаем таймер
    m_timer.Start(delay_readed_ms);

    PrintLog(LogCmdOK, "Read packet, cmd id: 0x%x, msg id: %d", command.CmdID(), command.MsgID());
    // читаем заголовок пакета
    if (!packet.ReadHeader(*this))
    {
        PrintLog(LogCmdError, "Failed read header packet, cmd id: 0x%x, msg id: %d", command.CmdID(), command.MsgID());
        return false;
    }

    // проверим корректность заголовка
    if (packet.CmdID() != command.CmdID())
    {
        PrintLog(LogCmdError, "Wrong cmd id source: 0x%x, dest: 0x%x, msg id: %d", command.CmdID(), packet.CmdID(), command.MsgID());
        return false;
    }

    // читаем тело сообщения и crc16
    if (!packet.ReadBody(*this))
    {
        PrintLog(LogCmdError, "Failed read body packet, cmd id: 0x%x, msg id: %d", command.CmdID(), command.MsgID());
        return false;
    }

    // проверим корректность пакета
    if (!packet.Check())
    {
        PrintLog(LogCmdError, "Incorrect packet, cmd id: 0x%x, msg id: %x", command.CmdID(), command.MsgID());
        return false;
    }

    return true;
}

bool TransportDispatcher::ReadData(uint8_t *buf, uint32_t size)
{
    bool      r;
    uint32_t  read = size;
    uint32_t  remaind = read;

    do
    {
        r = m_com_port.Read(buf, read);
        if (!r)
        {
            PrintLog(LogCmdError, "Error read packet, all bytes: %d, remaind bytes: %d, last OS error [%d]", size, remaind, m_com_port.LastOS());
            return false;
        }

        remaind -= read;
        buf     += read;

        // прочитали весь пакет, выходим
        if (remaind == 0)
            break;

        read = remaind;
        m_timer.Wait(5);
    } while (!m_timer.IsExpired()); // проверяем, истекло время?

    // данные прочитали, все хорошо 
    if (remaind == 0)
        return true;

    // данные прочитали не до конца 
    PrintLog(LogCmdError, "Packet processing time expired, count bytes to read: %d, count remaining bytes: %d", size, remaind);
    return false;
}