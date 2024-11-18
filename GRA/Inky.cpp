#include "Inky.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>
#include <random>

// Funkcja do losowania liczb ca³kowitych w zakresie
int randomInt(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

sf::Vector2f Inky::getPosition() const {
    return sprite.getPosition();
}

Inky::Inky(const Pacman& pacman, const Blinky& blinky, const sf::RectangleShape& ghostRoomDoor)
    : pacman(pacman), blinky(blinky), ghostRoomDoor(&ghostRoomDoor), speed(100.f), direction(0.f, 0.f), pendingDirection(0.f, 0.f) {
    texture.loadFromFile("inky.png");
    sprite.setTexture(texture);
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().y
    );
    sprite.setPosition(8 * tileSize, 7 * tileSize);
    targetTile = worldToGrid(sprite.getPosition()); // Pocz¹tkowy cel to aktualna pozycja
}

void Inky::updateDirection(const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2i inkyGrid = worldToGrid(sprite.getPosition());

    // Jeœli Inky osi¹gn¹³ cel, losuje nowe wspó³rzêdne celu
    if (inkyGrid == targetTile) {
        targetTile = getRandomWalkableTile(walls);
    }

    // ZnajdŸ œcie¿kê do celu
    std::vector<sf::Vector2i> path = bfs(inkyGrid, targetTile);

    if (!path.empty() && path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - inkyGrid;

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
}

sf::Vector2i Inky::getRandomWalkableTile(const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2i randomTile;
    bool valid = false;

    // Losuj dopóki nie znajdziesz pola, które nie koliduje z murem
    do {
        randomTile = { randomInt(0, mapWidth - 1), randomInt(0, mapHeight - 1) };
        sf::Vector2f worldPos = gridToWorld(randomTile);

        // SprawdŸ kolizjê z murami
        valid = true;
        for (const auto& wall : walls) {
            if (wall.getGlobalBounds().contains(worldPos)) {
                valid = false;
                break;
            }
        }
    } while (!valid);

    return randomTile;
}

void Inky::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    updateDirection(walls);

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

void Inky::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Inky::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
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

void Inky::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

bool Inky::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false;
    return bounds.intersects(wall.getGlobalBounds());
}

sf::FloatRect Inky::getBounds() const {
    return sprite.getGlobalBounds();
}
