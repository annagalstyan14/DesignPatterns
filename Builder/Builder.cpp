#include "Builder.h"
#include <iostream>

int main() {
    WoodHouseBuilder woodHouseBuilder;
    StoneHouseBuilder stoneHouseBuilder;
    Director director1(woodHouseBuilder);
    Director director2(stoneHouseBuilder);

    director1.buildHouse("luxury");
    House* house1 = woodHouseBuilder.getHouse();
    std::cout << "House 1 is built: " << house1->window << "\n" << house1->door << "\n" << house1->walls << "\n" << house1->roof;
    if (!house1->pool.empty()) std::cout << "\n" << house1->pool;
    if (!house1->garden.empty()) std::cout << "\n" << house1->garden;
    if (!house1->garage.empty()) std::cout << "\n" << house1->garage;
    std::cout << std::endl;

    director2.buildHouse("simple");
    House* house2 = stoneHouseBuilder.getHouse();
    std::cout << "House 2 is built: " << house2->window << "\n" << house2->door << "\n" << house2->walls << "\n" << house2->roof;
    if (!house2->pool.empty()) std::cout << "\n" << house2->pool;
    if (!house2->garden.empty()) std::cout << "\n" << house2->garden;
    if (!house2->garage.empty()) std::cout << "\n" << house2->garage;
    std::cout << std::endl;

    return 0;
}