#include "Remote.h"
#include <iostream>

Remote::Remote(Device* dev) : device(dev) {}

void Remote::togglePower() {
    std::cout << "Remote: Toggling power..." << std::endl;
    if (device->isEnabled()) {
        device->disable();
    } else {
        device->enable();
    }
}

void Remote::volumeUp() {
    std::cout << "Remote: Volume up..." << std::endl;
    int currentVolume = device->getVolume();
    device->setVolume(currentVolume + 10);
}

void Remote::volumeDown() {
    std::cout << "Remote: Volume down..." << std::endl;
    int currentVolume = device->getVolume();
    device->setVolume(currentVolume - 10);
}

void Remote::channelUp() {
    std::cout << "Remote: Channel up..." << std::endl;
    int currentChannel = device->getChannel();
    device->setChannel(currentChannel + 1);
}

void Remote::channelDown() {
    std::cout << "Remote: Channel down..." << std::endl;
    int currentChannel = device->getChannel();
    device->setChannel(currentChannel - 1);
}

BasicRemote::BasicRemote(Device* dev) : Remote(dev) {
    std::cout << "Basic Remote initialized" << std::endl;
}

AdvancedRemote::AdvancedRemote(Device* dev) : Remote(dev) {
    std::cout << "Advanced Remote initialized" << std::endl;
}

void AdvancedRemote::mute() {
    std::cout << "Advanced Remote: Muting device..." << std::endl;
    device->setVolume(0);
}

void AdvancedRemote::setChannel(int channel) {
    std::cout << "Advanced Remote: Setting specific channel..." << std::endl;
    device->setChannel(channel);
}