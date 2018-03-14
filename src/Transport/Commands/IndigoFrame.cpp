#include "StdAfx.h"
#include "IndigoFrame.h"
#include "Transport/Interfaces/ISocket.h"

IndigoFrame::IndigoFrame()
{
}

IndigoFrame::~IndigoFrame()
{
}

uint8_t IndigoFrame::DestAddress() const
{
    return m_header.dest_address;
}

void IndigoFrame::DestAddressSet(uint8_t val)
{
    m_header.dest_address = val;
}

uint8_t IndigoFrame::SourceAddress() const
{
    return m_header.source_address;
}

void IndigoFrame::SourceAddressSet(uint8_t val)
{
    m_header.source_address = val;
}

uint8_t IndigoFrame::CmdID() const
{
    return m_header.cmd_id;
}

void IndigoFrame::CmdIDSet(uint8_t val)
{
    m_header.cmd_id = val;
}

uint8_t IndigoFrame::OperationID() const
{
    return m_header.operation_id;
}

void IndigoFrame::OperationIDSet(uint8_t val)
{
    m_header.operation_id = val;
}

uint8_t IndigoFrame::FrameNum() const
{
    return m_header.frame_number;
}

void IndigoFrame::FrameNumberSet(uint8_t val)
{
    m_header.frame_number = val;
}

bool IndigoFrame::HeaderRead(ISocket *socket)
{
    bool r = socket->ReadBytes((uint8_t *)&m_header, sizeof(m_header));
    return r;
}

bool IndigoFrame::HeaderWrite(ISocket *socket)
{
    bool r = socket->WriteBytes((uint8_t *)&m_header, sizeof(m_header));
    return r;
}

void IndigoFrame::Clear()
{
    memset(&m_header, 0, sizeof(m_header));
}

bool IndigoFrame::Check()
{
    return false;
}
