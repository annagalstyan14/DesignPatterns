#ifndef PLAYER_H
#define PLAYER_H
#include <string>

class Player {
private:
    std::string name;
    int x, y;

public:
    Player(const std::string& name) : name(name), x(0), y(0) {}
    
    void move(int dx, int dy);
    void jump(int height);
    void attack(const std::string& target);
    
    int getX() const { return x; }
    int getY() const { return y; }
    void setY(int newY) { y = newY; }
    std::string getName() const { return name; }
};

#endif