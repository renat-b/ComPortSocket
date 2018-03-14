#pragma once

#include "Packets.h"
#include "TransportDispatcher.h"

class Task
{
protected:
    TransportDispatcher  m_transport;

public:
    Task();
    ~Task();

    bool  Run();

protected:
    bool  Prepare();
    bool  GetResult();
};

class TaskGetEwlSize : public Task
{
public:
    TaskGetEwlSize();
    ~TaskGetEwlSize();
    
    uint32_t  Count() const;
};

class TaskGetEwlData : public Task
{
public:
    TaskGetEwlData();
    ~TaskGetEwlData();

    Packets &Packets() const;
};
