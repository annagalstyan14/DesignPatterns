#include "Device.h"
#include "Remote.h"
#include <iostream>

int main() {

    Device* tv = new TV();
    Device* radio = new Radio();
    Device* soundbar = new Soundbar();
    
    std::cout << "\n=== Test 1: Basic Remote with TV ===" << std::endl;
    Remote* basicRemoteForTV = new BasicRemote(tv);
    basicRemoteForTV->togglePower();
    basicRemoteForTV->volumeUp();
    basicRemoteForTV->channelUp();
    basicRemoteForTV->togglePower();
    
    std::cout << "\n=== Test 2: Advanced Remote with Radio ===" << std::endl;
    AdvancedRemote* advancedRemoteForRadio = new AdvancedRemote(radio);
    advancedRemoteForRadio->togglePower();
    advancedRemoteForRadio->volumeUp();
    advancedRemoteForRadio->volumeUp();
    advancedRemoteForRadio->setChannel(105);  // Set to 105 FM
    advancedRemoteForRadio->mute();  // Advanced feature
    advancedRemoteForRadio->togglePower();
    
    std::cout << "\n=== Test 3: Basic Remote with Soundbar ===" << std::endl;
    Remote* basicRemoteForSoundbar = new BasicRemote(soundbar);
    basicRemoteForSoundbar->togglePower();
    basicRemoteForSoundbar->volumeUp();
    basicRemoteForSoundbar->volumeUp();
    basicRemoteForSoundbar->channelUp();
    basicRemoteForSoundbar->togglePower();
    
    std::cout << "\n=== Test 4: Advanced Remote with TV ===" << std::endl;
    AdvancedRemote* advancedRemoteForTV = new AdvancedRemote(tv);
    advancedRemoteForTV->togglePower();
    advancedRemoteForTV->setChannel(7); 
    advancedRemoteForTV->volumeUp();
    advancedRemoteForTV->mute();
    advancedRemoteForTV->togglePower();
    
    delete basicRemoteForTV;
    delete advancedRemoteForRadio;
    delete basicRemoteForSoundbar;
    delete advancedRemoteForTV;
    delete tv;
    delete radio;
    delete soundbar;
    
    return 0;
}