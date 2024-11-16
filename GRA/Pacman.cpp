#include "Pacman.h"
#include "MapData.h"
#include <cmath>
#include <string>

const float pacmanSpeed = 200.f;

Pacman::Pacman() : direction(0.f, 0.f), pendingDirection(0.f, 0.f), speed(pacmanSpeed), animationFrame(0), animationTimer(0.f) {
    loadTextures();
    sprite.setTexture(textures[1][0]);
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / textures[0][0].getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / textures[0][0].getSize().y
    );
    sprite.setPosition(9 * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2,
        13 * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2);
}

void Pacman::loadTextures() {
    std::string directions[] = { "up", "down", "left", "right" };
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::string filename = "pacman_" + directions[i] + "_" + std::to_string(j) + ".png";
            textures[i][j].loadFromFile(filename);
        }
    }
}

void Pacman::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

void Pacman::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        pendingDirection = { 0.f, -1.f };
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        pendingDirection = { 0.f, 1.f };
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        pendingDirection = { -1.f, 0.f };
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        pendingDirection = { 1.f, 0.f };
    }
}

void Pacman::update(float deltaTime, std::vector<sf::CircleShape>& dots, const std::vector<sf::RectangleShape>& walls, int& score) {
    if (pendingDirection == -direction && canMoveInDirection(pendingDirection, walls)) {
        direction = pendingDirection;
    }
    else if (pendingDirection != direction && canMoveInDirection(pendingDirection, walls)) {
        direction = pendingDirection;
        centerOnTile();
    }

    sf::Vector2f movement = direction * speed * deltaTime;
    sprite.move(movement);

    bool collided = false;
    for (const auto& wall : walls) {
        if (checkCollision(wall)) {
            sprite.move(-movement);
            collided = true;
            break;
        }
    }

    if (collided) {
        direction = { 0.f, 0.f };
    }

    for (auto it = dots.begin(); it != dots.end();) {
        if (checkDotCollision(*it)) {
            it = dots.erase(it);
            score += 10;
        }
        else {
            ++it;
        }
    }

    updateAnimation(deltaTime);
}

void Pacman::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer >= 0.1f) {
        animationTimer = 0.f;
        animationFrame = (animationFrame + 1) % 3;
        sprite.setTexture(textures[getDirectionIndex()][animationFrame]);
    }
}

int Pacman::getDirectionIndex() const {
    if (direction.x > 0) return 3;
    if (direction.x < 0) return 2;
    if (direction.y > 0) return 1;
    return 0;
}

bool Pacman::canMoveInDirection(const sf::Vector2f& dir, const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2f nextPosition = sprite.getPosition() + dir * static_cast<float>(tileSize);
    sprite.setPosition(nextPosition);
    for (const auto& wall : walls) {
        if (checkCollision(wall)) {
            sprite.move(-dir * static_cast<float>(tileSize));
            return false;
        }
    }
    sprite.move(-dir * static_cast<float>(tileSize));
    return true;
}

bool Pacman::checkCollision(const sf::RectangleShape& wall) {
    return sprite.getGlobalBounds().intersects(wall.getGlobalBounds());
}

bool Pacman::checkDotCollision(sf::CircleShape& dot) {
    return sprite.getGlobalBounds().intersects(dot.getGlobalBounds());
}

sf::Vector2f Pacman::getPosition() const {
    return sprite.getPosition();
}

void Pacman::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::Vector2f Pacman::getDirection() const {
    return direction;
}

sf::FloatRect Pacman::getBounds() const {
    return sprite.getGlobalBounds();
}
