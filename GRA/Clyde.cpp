#include "Clyde.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>
#include <iostream> 

sf::Vector2f Clyde::getPosition() const {
    return sprite.getPosition();
}

Clyde::Clyde(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor)
    : pacman(pacman), ghostRoomDoor(&ghostRoomDoor), speed(100.f), direction(0.f, 0.f), pendingDirection(0.f, 0.f), chasingPacman(true) {
    texture.loadFromFile("clyde.png");
    sprite.setTexture(texture);
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().y
    );
    sprite.setPosition(10 * tileSize, 7 * tileSize); 
}

void Clyde::updateDirection() {
    sf::Vector2i clydeGrid = worldToGrid(sprite.getPosition());
    sf::Vector2i targetGrid = chasingPacman ? worldToGrid(pacman.getPosition()) : scatterTarget;

    std::cout << "Clyde position: " << clydeGrid.x << ", " << clydeGrid.y
        << " Target: " << targetGrid.x << ", " << targetGrid.y << std::endl;

    if (!chasingPacman && clydeGrid == scatterTarget) {
        chasingPacman = true;
        std::cout << "Clyde reached scatter target. Resuming Pacman chase." << std::endl;
        return;
    }

    std::vector<sf::Vector2i> path = bfs(clydeGrid, targetGrid);

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - clydeGrid;

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
        std::cout << "Clyde next step: " << nextStep.x << ", " << nextStep.y
            << " Direction: " << pendingDirection.x << ", " << pendingDirection.y << std::endl;
    }
    else {
        std::cout << "Path is empty. Clyde stands still." << std::endl;
        pendingDirection = { 0.f, 0.f };
    }
}

void Clyde::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    if (chasingPacman && distanceToPacman() < 6.f * tileSize) {
        chasingPacman = false;
        std::cout << "Clyde is too close to Pacman. Switching to scatter mode." << std::endl;
    }

    updateDirection();

    if (pendingDirection != direction && canMoveInDirection(pendingDirection, deltaTime, walls)) {
        direction = pendingDirection;
        centerOnTile();
        std::cout << "Direction updated: " << direction.x << ", " << direction.y << std::endl;
    }

    sf::Vector2f movement = direction * speed * deltaTime;
    sprite.move(movement);

    bool collided = false;
    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-movement);
            collided = true;
            std::cout << "Collision detected. Clyde repositioned." << std::endl;
            break;
        }
    }

    if (collided) {
        direction = { 0.f, 0.f };
    }
}

void Clyde::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Clyde::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2f nextPosition = sprite.getPosition() + dir * static_cast<float>(tileSize);
    sprite.setPosition(nextPosition);
    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            std::cout << "Wall detected at direction: " << dir.x << ", " << dir.y << std::endl;
            sprite.move(-dir * static_cast<float>(tileSize));
            return false;
        }
    }
    sprite.move(-dir * static_cast<float>(tileSize));
    return true;
}

void Clyde::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

bool Clyde::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false;
    return bounds.intersects(wall.getGlobalBounds());
}

float Clyde::distanceToPacman() const {
    sf::Vector2f pacmanPos = pacman.getPosition();
    sf::Vector2f clydePos = sprite.getPosition();
    return std::sqrt(std::pow(pacmanPos.x - clydePos.x, 2) + std::pow(pacmanPos.y - clydePos.y, 2));
}

sf::Rect<float> Clyde::getBounds() const {
    return sprite.getGlobalBounds();
}