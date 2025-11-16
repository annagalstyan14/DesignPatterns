#include "Device.h"
#include <iostream>

TV::TV() : enabled(false), volume(50), channel(1) {}

bool TV::isEnabled() const {
    return enabled;
}

void TV::enable() {
    enabled = true;
    std::cout << "TV is now ON" << std::endl;
}

void TV::disable() {
    enabled = false;
    std::cout << "TV is now OFF" << std::endl;
}

int TV::getVolume() const {
    return volume;
}

void TV::setVolume(int vol) {
    volume = vol;
    std::cout << "TV volume set to: " << volume << std::endl;
}

int TV::getChannel() const {
    return channel;
}

void TV::setChannel(int ch) {
    channel = ch;
    std::cout << "TV channel set to: " << channel << std::endl;
}

Radio::Radio() : enabled(false), volume(30), station(101) {}

bool Radio::isEnabled() const {
    return enabled;
}

void Radio::enable() {
    enabled = true;
    std::cout << "Radio is now ON" << std::endl;
}

void Radio::disable() {
    enabled = false;
    std::cout << "Radio is now OFF" << std::endl;
}

int Radio::getVolume() const {
    return volume;
}

void Radio::setVolume(int vol) {
    volume = vol;
    std::cout << "Radio volume set to: " << volume << std::endl;
}

int Radio::getChannel() const {
    return station;
}

void Radio::setChannel(int st) {
    station = st;
    std::cout << "Radio station set to: " << station << " FM" << std::endl;
}

Soundbar::Soundbar() : enabled(false), volume(40), bassLevel(5) {}

bool Soundbar::isEnabled() const {
    return enabled;
}

void Soundbar::enable() {
    enabled = true;
    std::cout << "Soundbar is now ON" << std::endl;
}

void Soundbar::disable() {
    enabled = false;
    std::cout << "Soundbar is now OFF" << std::endl;
}

int Soundbar::getVolume() const {
    return volume;
}

void Soundbar::setVolume(int vol) {
    volume = vol;
    std::cout << "Soundbar volume set to: " << volume << std::endl;
}

int Soundbar::getChannel() const {
    return bassLevel;
}

void Soundbar::setChannel(int bass) {
    bassLevel = bass;
    std::cout << "Soundbar bass level set to: " << bassLevel << std::endl;
}