#pragma once
#ifndef PINKY_H
#define PINKY_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"



class Pinky {
public:
    Pinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;


private:
    const Pacman& pacman;
    const sf::RectangleShape* ghostRoomDoor;
    sf::Texture texture;
    sf::Sprite sprite;
    float speed;
    sf::Vector2f direction;
    sf::Vector2f pendingDirection;

    void updateDirection();
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);
    void centerOnTile();
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);
};

#endif
