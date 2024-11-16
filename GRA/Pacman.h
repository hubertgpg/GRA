#pragma once
#ifndef PACMAN_H
#define PACMAN_H

#include <SFML/Graphics.hpp>
#include <vector>


class Pacman {
public:
    Pacman();
    void handleInput();
    void update(float deltaTime, std::vector<sf::CircleShape>& dots, const std::vector<sf::RectangleShape>& walls, int& score);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const;
    void centerOnTile();

    sf::Vector2f getDirection() const;
    sf::FloatRect getBounds() const;

private:
    sf::Sprite sprite;
    sf::Texture textures[4][3];
    sf::Vector2f direction;
    sf::Vector2f pendingDirection;
    float speed;
    int animationFrame;
    float animationTimer;

    bool checkCollision(const sf::RectangleShape& wall);
    bool checkDotCollision(sf::CircleShape& dot);
    bool canMoveInDirection(const sf::Vector2f& dir, const std::vector<sf::RectangleShape>& walls);
    void updateAnimation(float deltaTime);
    void loadTextures();
    int getDirectionIndex() const;
};

#endif
