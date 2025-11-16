#ifndef REMOTE_H
#define REMOTE_H

#include "Device.h"

class Remote {
protected:
    Device* device;
    
public:
    Remote(Device* dev);
    virtual ~Remote() = default;
    
    virtual void togglePower();
    virtual void volumeUp();
    virtual void volumeDown();
    virtual void channelUp();
    virtual void channelDown();
};

class BasicRemote : public Remote {
public:
    BasicRemote(Device* dev);
};

class AdvancedRemote : public Remote {
public:
    AdvancedRemote(Device* dev);
    
    void mute();
    void setChannel(int channel);
};

#endif