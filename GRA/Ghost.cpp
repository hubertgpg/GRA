#include "Ghost.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>

// konstruktor klasy ghost, ustawia poczatkowe wartosci i laduje teksture
Ghost::Ghost(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor, const std::string& textureFile, sf::Vector2f startPosition)
    : pacman(pacman), ghostRoomDoor(&ghostRoomDoor), speed(100.f), direction(0.f, 0.f), pendingDirection(0.f, 0.f) {
    texture.loadFromFile(textureFile);
    sprite.setTexture(texture);
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().y
    );
    sprite.setPosition(startPosition);
}

// aktualizuje stan duszka (pozycja, kierunek i kolizje)
void Ghost::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    updateDirection();

    if (pendingDirection != direction && canMoveInDirection(pendingDirection, deltaTime, walls)) {
        direction = pendingDirection;
        centerOnTile();
    }

    sf::Vector2f movement = direction * speed * deltaTime;
    sprite.move(movement);

    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-movement);
            direction = { 0.f, 0.f };
            break;
        }
    }
}

// rysuje duszka na ekranie
void Ghost::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// zwraca aktualna pozycje duszka
sf::Vector2f Ghost::getPosition() const {
    return sprite.getPosition();
}

// zwraca granice (bounding box) duszka
sf::FloatRect Ghost::getBounds() const {
    return sprite.getGlobalBounds();
}

// sprawdza, czy duszek moze ruszyc sie w wybranym kierunku
bool Ghost::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2f nextPosition = sprite.getPosition() + dir * static_cast<float>(tileSize);
    sprite.setPosition(nextPosition);

    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-dir * static_cast<float>(tileSize));
            return false;
        }
    }
    sprite.move(-dir * static_cast<float>(tileSize));
    return true;
}

// ustawia duszka na srodku kafelka
void Ghost::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

// sprawdza kolizje duszka z obiektem
bool Ghost::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false;
    return bounds.intersects(wall.getGlobalBounds());
}
