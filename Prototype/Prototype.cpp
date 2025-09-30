#include "Prototype.h"
#include <iostream>

int main(){
    auto sword1 = std::make_unique<Sword> ("Excalibur", 45);
    auto shield1 = std::make_unique<Shield> ("Aymin", 30);
    auto potion1 = std::make_unique<Potion> ("Amortentia", 18);

    auto sword2 = sword1->clone();
    sword2->setAttributes("Shadowfang", 60);
    auto shield2 = shield1->clone();
    shield2->setAttributes("Iron Guard", 26);
    auto potion2 = potion1->clone();
    potion2->setAttributes("Eternity potion", 50);
    


    sword1->display();
    sword2->display();
    shield1->display();
    shield2->display();
    potion1->display();
    potion2->display();
}