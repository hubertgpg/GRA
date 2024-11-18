#ifndef INKY_H
#define INKY_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include "Blinky.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"

class Inky {
public:
    Inky(const Pacman& pacman, const Blinky& blinky, const sf::RectangleShape& ghostRoomDoor);
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

private:
    const Pacman& pacman;
    const Blinky& blinky;
    const sf::RectangleShape* ghostRoomDoor;
    sf::Texture texture;
    sf::Sprite sprite;
    float speed;
    sf::Vector2f direction;
    sf::Vector2f pendingDirection;
    sf::Vector2i targetTile;

    void updateDirection(const std::vector<sf::RectangleShape>& walls);
    sf::Vector2i getRandomWalkableTile(const std::vector<sf::RectangleShape>& walls);
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);
    void centerOnTile();
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);
};

#endif
