#include "Pinky.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>
#include <functional>

Pinky::Pinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor)
    : pacman(pacman), ghostRoomDoor(&ghostRoomDoor), speed(100.f), direction(0.f, 0.f), pendingDirection(0.f, 0.f) {
    texture.loadFromFile("pinky.png");
    sprite.setTexture(texture);
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().y
    );
    sprite.setPosition(9 * tileSize, 7 * tileSize);
}

sf::Vector2i calculatePinkyTarget(const sf::Vector2i& pacmanGrid, const sf::Vector2f& pacmanDirection) {
    return pacmanGrid + sf::Vector2i(static_cast<int>(pacmanDirection.x * 4), static_cast<int>(pacmanDirection.y * 4));
}

void Pinky::updateDirection() {
    sf::Vector2i pinkyGrid = worldToGrid(sprite.getPosition());
    sf::Vector2i pacmanGrid = worldToGrid(pacman.getPosition());

    sf::Vector2f pacmanDirection = pacman.getDirection();
    sf::Vector2i pinkyTarget = calculatePinkyTarget(pacmanGrid, pacmanDirection);

    std::vector<sf::Vector2i> path = bfs(pinkyGrid, pinkyTarget);

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - pinkyGrid;

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
}

void Pinky::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
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

void Pinky::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Pinky::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
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

void Pinky::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

bool Pinky::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false;
    return bounds.intersects(wall.getGlobalBounds());
}

sf::FloatRect Pinky::getBounds() const {
    return sprite.getGlobalBounds();
}
