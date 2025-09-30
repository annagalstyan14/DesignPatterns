#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <memory>
#include <iostream>
#include <string>

class GameItem {
    public:
    virtual ~GameItem() = default;
    virtual std::unique_ptr<GameItem> clone() const = 0;
    virtual void display() const =0;
    virtual void setAttributes (const std::string& name, int value) = 0;
};

class Sword : public GameItem {
    public:
    Sword(std::string n, int d) : name(n), damage(d) {}
    void setAttributes (const std::string& n, int d) override {name = n; damage = d;}
    void display() const override {
        std::cout << "Sword: " << name << "\n" << " Damage: " << damage << std::endl;
    }
    std::unique_ptr<GameItem> clone() const override {
        return std::make_unique<Sword> (*this);
    }
    private:
    std::string name;
    int damage;
};

class Shield : public GameItem {
    public:
    Shield(std::string n, int def) : name(n), defense(def) {}
    void setAttributes (const std::string& n, int def) override {name = n; defense = def;}
    void display() const override {
        std::cout << "Shield: " << name <<  "\n" << " Defense: " << defense << std::endl;
    }
    std::unique_ptr<GameItem> clone() const override {
        return std::make_unique<Shield> (*this);
    }
    private:
    std::string name;
    int defense;
};

class Potion : public GameItem {
    public:
    Potion(std::string n, int hp) : name(n), heal(hp) {}
    void setAttributes (const std::string& n, int hp) override {name = n; heal = hp;}
    void display() const override {
        std::cout << "Potion: " << name << "\n" << " Heal: " << heal << std::endl;
    }
    std::unique_ptr<GameItem> clone() const override {
        return std::make_unique<Potion> (*this);
    }
    private:
    std::string name;
    int heal;
};
#endif
