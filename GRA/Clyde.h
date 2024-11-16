#ifndef CLYDE_H
#define CLYDE_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"

class Clyde {
public:
    Clyde(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    sf::Rect<float> getBounds() const;  

private:
    const Pacman& pacman;
    const sf::RectangleShape* ghostRoomDoor;
    sf::Texture texture;
    sf::Sprite sprite;
    float speed;
    sf::Vector2f direction;
    sf::Vector2f pendingDirection;

    bool chasingPacman; 
    const sf::Vector2i scatterTarget = { 17, 13 };

    void updateDirection();
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);
    void centerOnTile();
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);
    float distanceToPacman() const;
};

#endif
