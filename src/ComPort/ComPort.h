#pragma once

class ComPort
{
private:
    HANDLE         m_handle;
    mutable DWORD  m_last_error;

public:
    ComPort();
    ~ComPort();

    bool     Read(uint8_t *data, uint32_t &size);
    bool     Write(const uint8_t *data, uint32_t size) const;

    bool     ClearPort();

    void     SetHandle(HANDLE handle);
    HANDLE   GetHandle() const;

    DWORD    LastOS() const;
};
