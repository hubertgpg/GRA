#include "blinky.h"
#include "MapData.h"
#include <cmath>

Blinky::Blinky(const Pacman& pacman) : pacman(pacman), speed(80.f) {
    texture.loadFromFile("blinky.png"); // Upewnij siê, ¿e plik istnieje
    sprite.setTexture(texture);
    sprite.setPosition(9 * tileSize, 8 * tileSize); // Pocz¹tkowa pozycja Blinkiego
}

sf::Vector2f Blinky::getDirectionTowardsPacman() const {
    sf::Vector2f direction = pacman.getPosition() - sprite.getPosition();
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    return length != 0 ? direction / length : sf::Vector2f(0.f, 0.f); // Normalizacja wektora
}

void Blinky::update(float deltaTime) {
    sf::Vector2f direction = getDirectionTowardsPacman();
    sprite.move(direction * speed * deltaTime);
}

void Blinky::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
