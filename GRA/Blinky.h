#pragma once
#ifndef BLINKY_H
#define BLINKY_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"

class Blinky {
public:
    Blinky(const Pacman& pacman);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

private:
    const Pacman& pacman; // Referencja do Pacmana, by znaæ jego pozycjê
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

    sf::Vector2f getDirectionTowardsPacman() const;
};

#endif // BLINKY_H
