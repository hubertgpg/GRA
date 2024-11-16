#include "Blinky.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>
#include <functional>

sf::Vector2f Blinky::getPosition() const {
    return sprite.getPosition();
}


Blinky::Blinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor)
    : pacman(pacman), ghostRoomDoor(&ghostRoomDoor), speed(100.f), direction(0.f, 0.f), pendingDirection(0.f, 0.f) {
    texture.loadFromFile("blinky.png");
    sprite.setTexture(texture);
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().y
    );
    sprite.setPosition(9 * tileSize, 6 * tileSize);
}

void Blinky::updateDirection() {
    sf::Vector2i blinkyGrid = worldToGrid(sprite.getPosition());
    sf::Vector2i pacmanGrid = worldToGrid(pacman.getPosition());

    std::vector<sf::Vector2i> path = bfs(blinkyGrid, pacmanGrid);

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - blinkyGrid;

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
}

void Blinky::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    updateDirection();

    if (pendingDirection != direction && canMoveInDirection(pendingDirection, deltaTime, walls)) {
        direction = pendingDirection;
        centerOnTile();
    }

    sf::Vector2f movement = direction * speed * deltaTime;
    sprite.move(movement);

    bool collided = false;
    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-movement);
            collided = true;
            break;
        }
    }

    if (collided) {
        direction = { 0.f, 0.f };
    }
}

void Blinky::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Blinky::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
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

void Blinky::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

bool Blinky::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false;
    return bounds.intersects(wall.getGlobalBounds());
}

sf::FloatRect Blinky::getBounds() const {
    return sprite.getGlobalBounds();
}
