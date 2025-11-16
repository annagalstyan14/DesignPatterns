#ifndef DEVICE_H
#define DEVICE_H

class Device {
public:
    virtual ~Device() = default;
    
    virtual bool isEnabled() const = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual int getVolume() const = 0;
    virtual void setVolume(int volume) = 0;
    virtual int getChannel() const = 0;
    virtual void setChannel(int channel) = 0;
};

class TV : public Device {
private:
    bool enabled;
    int volume;
    int channel;
    
public:
    TV();
    
    bool isEnabled() const override;
    void enable() override;
    void disable() override;
    int getVolume() const override;
    void setVolume(int volume) override;
    int getChannel() const override;
    void setChannel(int channel) override;
};

class Radio : public Device {
private:
    bool enabled;
    int volume;
    int station;
    
public:
    Radio();
    
    bool isEnabled() const override;
    void enable() override;
    void disable() override;
    int getVolume() const override;
    void setVolume(int volume) override;
    int getChannel() const override;
    void setChannel(int channel) override;
};

class Soundbar : public Device {
private:
    bool enabled;
    int volume;
    int bassLevel;
    
public:
    Soundbar();
    
    bool isEnabled() const override;
    void enable() override;
    void disable() override;
    int getVolume() const override;
    void setVolume(int volume) override;
    int getChannel() const override;
    void setChannel(int channel) override;
};

#endif