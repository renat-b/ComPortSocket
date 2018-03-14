#include "StdAfx.h"
#include "ComPort.h"

ComPort::ComPort() : m_last_error(0), m_handle(NULL)
{
}

ComPort::~ComPort()
{
}

bool ComPort::Read(uint8_t *data, uint32_t &size)
{
    DWORD readed = 0, read;

    read = size;
    BOOL r = ReadFile(m_handle, data, read, &readed, NULL);
    if (r == FALSE)
    {
        m_last_error = GetLastError();
        return false;
    }

    size = readed;
    return true;
}

bool ComPort::Write(const uint8_t *data, uint32_t size) const
{
    BOOL  r;
    DWORD writed = 0, write;

    if (!data)
        return false;
    if (size <= 0)
        return false;
    
    write = size;
    r = WriteFile(m_handle, data, write, &writed, NULL);
    if (r == FALSE)
    {
        m_last_error = GetLastError();
        return false;
    }

    if (writed != write)
        return false;

    return true;
}

bool ComPort::ClearPort()
{
    if (!PurgeComm(m_handle, PURGE_TXCLEAR | PURGE_RXCLEAR))
        return false;

    return true;
}

void ComPort::SetHandle(HANDLE handle)
{
    m_handle = handle;
}

HANDLE ComPort::GetHandle() const
{
    return m_handle;
}

DWORD ComPort::LastOS() const
{
    return m_last_error;
}
