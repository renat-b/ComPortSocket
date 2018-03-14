#include "StdAfx.h"
#include "Command.h"
#include "TransportDispatcher.h"
#include "..\Common\ConvertData.h"
#include "..\Common\crc16.h"

Command::Command() : m_timeout(0), m_message_id(0)
{
}

Command::~Command()
{
}

uint32_t Command::GetTimeout() const
{
    return m_timeout;
}

void Command::Clear()
{
    m_body.Clear();
}

bool Command::Send(ComPort &port) const
{
    bool r = port.Write(m_body.GetData(), m_body.GetSize());
    return r;
}

const uint8_t *Command::GetData() const
{
    const uint8_t *data;

    data = (const uint8_t *)m_body.GetData();
    return data;
}

uint32_t Command::GetSize() const
{
    uint32_t size;

    size = m_body.GetSize();
    return size;
}

CommandEWL::CommandEWL() 
{
}

CommandEWL::~CommandEWL()
{

}

uint32_t CommandEWL::MsgID() const
{
    return m_message_id;
}

uint32_t CommandEWL::CmdID() const
{    
    return m_header.cmd_id;
}

void CommandEWL::Clear()
{
    memset(&m_header, 0, sizeof(m_header));
    Command::Clear();
}

bool CommandEWL::CmdCreate(uint8_t cmd, const TransportDispatcher &handler)
{
    Clear();

    bool r = CreateHeader(cmd, handler, 0);
    if (r)
        CreateCRC16();

    return r;
}

bool CommandEWL::CreateHeader(uint8_t cmd, const TransportDispatcher &handler, uint32_t body_size)
{
    m_header.ewl_address = handler.m_ewl_address;
    m_header.master_address = handler.m_master_address;
    m_header.cmd_id = cmd;

    m_message_id = handler.GetOperationID();
    m_header.message_id = m_message_id;
    m_header.frame_number = handler.m_frame_number;

    m_header.message_length = body_size + sizeof(uint8_t);
    Big2LittelEndian(&m_header.message_length, sizeof(m_header.message_length));

    m_header.tool_address = handler.m_tool_address;

    m_body.Add(&m_header, sizeof(m_header));
    m_timeout = handler.GetCmdTimeout(cmd).timeout_write;
    return true;
}

void  CommandEWL::CreateCRC16()
{
    // расчитаем crc для заголовка
    uint8_t *data = (uint8_t *)&m_header;
    uint32_t size = sizeof(m_header);

    data += sizeof(uint8_t);
    size -= sizeof(uint8_t);

    uint16_t crc = get_crc_16(0, data, size);
    // расчитаем crc для тела сообщения
    data = m_body.GetData() + sizeof(m_header);
    size = m_body.GetSize() - sizeof(m_header);

    crc = get_crc_16(crc, data, size);
    // добавим полученное crc в тело сообщения
    m_body.AddInt16(crc);
}


CommandAMI::CommandAMI()
{
    memset(&m_header, 0, sizeof(m_header));
}

CommandAMI::~CommandAMI()
{

}

uint32_t CommandAMI::MsgID() const
{
    return m_header.message_id;
}

uint32_t CommandAMI::CmdID() const
{
    return m_header.cmd_id;
}

void CommandAMI::Clear()
{
    memset(&m_header, 0, sizeof(m_header));
    Command::Clear();
}

bool CommandAMI::CmdCreate(uint8_t cmd, const TransportDispatcher &handler)
{
    Clear();

    bool r = CreateHeader(cmd, handler, 0);
    if (r)
        CreateCRC16();
    return r;
}

bool CommandAMI::CreateHeader(uint8_t cmd, const TransportDispatcher &handler, uint32_t body_size)
{
    // заполним поля заголовка пакета
    m_header.tool_address   = handler.m_tool_address;
    m_header.master_address = handler.m_master_address;
    m_header.cmd_id = cmd;

    m_message_id          = handler.GetOperationID();
    m_header.message_id   = m_message_id;
    m_header.frame_number = handler.m_frame_number;

    m_header.message_length = body_size;
    Big2LittelEndian(&m_header.message_length, sizeof(m_header.message_length));

    // выставим время ожидания
    m_timeout = handler.GetCmdTimeout(cmd).timeout_write;
    // добавим в тело заголовок сообщения
    m_body.Add(&m_header, sizeof(m_header));
    return true;
}

void CommandAMI::CreateCRC16()
{
    // расчитаем crc16 для заголовка
    uint8_t *data = (uint8_t *)&m_header;
    uint32_t size = sizeof(m_header);

    data += sizeof(uint8_t);
    size -= sizeof(uint8_t);

    uint16_t crc = get_crc_16(0, data, size);
    
    // расчитаем crc16 для тела сообщения
    data = m_body.GetData() - sizeof(m_header);
    size = m_body.GetSize() - sizeof(m_header);

    crc = get_crc_16(crc, data, size);
    // добавим расчитанное crc в тело сообщения
    m_body.AddInt16(crc);
}