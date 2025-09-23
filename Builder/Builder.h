#ifndef BUILDER
#define BUILDER
#include <string>
#include <iostream>

class House {
public:
    std::string window;
    std::string door;
    std::string walls;
    std::string roof;
    std::string pool;
    std::string garden;
    std::string garage;

    void setWindow(const std::string& window) { this->window = window; }
    void setDoor(const std::string& door) { this->door = door; }
    void setPool(const std::string& pool) { this->pool = pool; }
    void setGarden(const std::string& garden) { this->garden = garden; }
    void setGarage(const std::string& garage) { this->garage = garage; }
    void setWalls(const std::string& walls) { this->walls = walls; }
    void setRoof(const std::string& roof) { this->roof = roof; }
};

class HouseBuilder {
public:
    virtual void reset() = 0;
    virtual void buildWindow() = 0;
    virtual void buildDoor() = 0;
    virtual void buildWalls() = 0;
    virtual void buildRoof() = 0;
    virtual void buildPool() = 0;
    virtual void buildGarden() = 0;
    virtual void buildGarage() = 0;
    virtual House* getHouse() = 0;
    virtual ~HouseBuilder() = default; // Virtual destructor for proper cleanup
};

class WoodHouseBuilder : public HouseBuilder {
public:
    void reset() override {
        product = House();
    }

    void buildWindow() override {
        product.setWindow("Wooden Window");
    }

    void buildDoor() override {
        product.setDoor("Wooden Door");
    }

    void buildWalls() override {
        product.setWalls("Wooden Walls");
    }

    void buildRoof() override {
        product.setRoof("Wooden Roof");
    }

    void buildPool() override {
        product.setPool("Pool for a Wooden House");
    }

    void buildGarden() override {
        product.setGarden("Garden for a Wooden House");
    }

    void buildGarage() override {
        product.setGarage("Garage for a Wooden House");
    }

    House* getHouse() override {
        return &product;
    }

private:
    House product;
};

class StoneHouseBuilder : public HouseBuilder {
public:
    void reset() override {
        product = House();
    }

    void buildWindow() override {
        product.setWindow("Stone Window");
    }

    void buildDoor() override {
        product.setDoor("Stone Door");
    }

    void buildWalls() override {
        product.setWalls("Stone Walls");
    }

    void buildRoof() override {
        product.setRoof("Stone Roof");
    }

    void buildPool() override {
        product.setPool("Pool for a Stone House");
    }

    void buildGarden() override {
        product.setGarden("Garden for a Stone House");
    }

    void buildGarage() override {
        product.setGarage("Garage for a Stone House");
    }

    House* getHouse() override {
        return &product;
    }

private:
    House product;
};

class Director {
public:
    Director(HouseBuilder& housebuilder) : housebuilder(housebuilder) {}

    void changeBuilder(HouseBuilder& newBuilder) {
        housebuilder = newBuilder;
    }

    void buildHouse(const std::string& type) {
        housebuilder.reset();
        if (type == "simple") {
            housebuilder.buildWindow();
            housebuilder.buildDoor();
            housebuilder.buildWalls();
            housebuilder.buildRoof();
        } else if (type == "luxury") {
            housebuilder.buildWindow();
            housebuilder.buildDoor();
            housebuilder.buildWalls();
            housebuilder.buildRoof();
            housebuilder.buildPool();
            housebuilder.buildGarden();
            housebuilder.buildGarage();
        } else {
            std::cerr << "Unknown type of House\n";
        }
    }

private:
    HouseBuilder& housebuilder;
};

#endif
