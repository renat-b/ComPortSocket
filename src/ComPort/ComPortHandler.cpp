#include "stdafx.h"
#include "ComPortHandler.h"

ComPortHandler::ComPortHandler()
{
}

ComPortHandler::~ComPortHandler()
{
}

SocketTimeout ComPortHandler::TimeoutsGet(uint32_t cmd_id) const
{
    Timeouts timeout(cmd_id, m_default_timeout_read, m_default_timeout_write);
    for (uint32_t i = 0; i < m_timeouts.size(); i++)
    {
        const Timeouts &timeout = m_timeouts[i];

        if (timeout.cmd_id == cmd_id)
            return timeout;
    }
    return timeout;
}

void ComPortHandler::TimeoutsSet(uint32_t cmd_id, SocketTimeout &timeout)
{
    for (uint32_t i = 0; i < m_timeouts.size(); i++)
    {
        Timeouts &cmd_timeout = m_timeouts[i];
        if (cmd_timeout.cmd_id == cmd_id)
        {
            cmd_timeout.timeout_read  = timeout.timeout_read;
            cmd_timeout.timeout_write = timeout.timeout_write;
            return;
        }
    }

    Timeouts cmd_timeout(cmd_id, timeout.timeout_read, timeout.timeout_write);
    m_timeouts.push_back(cmd_timeout);
}

bool ComPortHandler::CommandSend(Command &command)
{
    bool r = m_com_port.ClearPort();
    if (!r)
    {
        PrintLog(LogCmdError, "Error clear COM port [%d]", m_com_port.LastErrorOS());
        return false;
    }

    r = command.SendBody(this);
    if (!r)
    {
        PrintLog(LogCmdError, "Error write COM port [%d]", m_com_port.LastErrorOS());
        return false;
    }
    
    uint32_t wait_ms = command.Timeout();
    m_timer.Wait(wait_ms);
    return true;
}

bool ComPortHandler::PacketReceive(Packet &packet, const Command &command)
{
    // рассчитаем колво мс в течении которого можно получать данные
    uint32_t delay_readed_ms = packet.Timeout();

    // запускаем таймер
    m_timer.Start(delay_readed_ms);

    PrintLog(LogCmdOK, "Read packet, cmd id: 0x%x, msg id: %d", command.CmdID(), 0);
    // читаем заголовок пакета
    if (!packet.HeaderRead(this))
    {
        PrintLog(LogCmdError, "Failed read header packet, cmd id: 0x%x, msg id: %d", command.CmdID(), 0);
        return false;
    }

    // проверим корректность заголовка
    if (packet.CmdID() != command.CmdID())
    {
        PrintLog(LogCmdError, "Wrong cmd id source: 0x%x, dest: 0x%x, msg id: %d", command.CmdID(), packet.CmdID(), 0);
        return false;
    }

    // читаем тело сообщения и crc16
    if (!packet.ReadBody(this))
    {
        PrintLog(LogCmdError, "Failed read body packet, cmd id: 0x%x, msg id: %d", command.CmdID(), 0);
        return false;
    }

    // проверим корректность пакета
    if (!packet.Check())
    {
        PrintLog(LogCmdError, "Incorrect packet, cmd id: 0x%x, msg id: %x", command.CmdID(), 0);
        return false;
    }
    return true;
}

bool ComPortHandler::WriteBytes(const uint8_t *data, uint32_t size) const
{
    bool r = m_com_port.Write(data, size);
    return r;
}

bool ComPortHandler::ReadBytes(uint8_t *data, uint32_t size)
{
    bool      r;
    uint32_t  read = size;
    uint32_t  remaind = read;

    do
    {
        r = m_com_port.Read(data, read);
        if (!r)
        {
            PrintLog(LogCmdError, "Error read packet, all bytes: %d, remaind bytes: %d, last OS error [%d]", size, remaind, m_com_port.LastErrorOS());
            return false;
        }

        remaind  -= read;
        data     += read;

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