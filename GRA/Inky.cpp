#include "Inky.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>
#include <random>

// funkcja generujaca losowa liczbe calkowita w zakresie [min, max]
int randomInt(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// zwraca aktualna pozycje Inky'ego
sf::Vector2f Inky::getPosition() const {
    return sprite.getPosition();
}

// konstruktor Inky'ego - inicjalizuje teksture, pozycje startowa oraz cel
Inky::Inky(const Pacman& pacman, const Blinky& blinky, const sf::RectangleShape& ghostRoomDoor)
    : pacman(pacman), blinky(blinky), ghostRoomDoor(&ghostRoomDoor), speed(100.f), direction(0.f, 0.f), pendingDirection(0.f, 0.f) {
    texture.loadFromFile("inky.png");
    sprite.setTexture(texture);
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().y
    );
    sprite.setPosition(8 * tileSize, 7 * tileSize);
    targetTile = worldToGrid(sprite.getPosition());
}

// aktualizuje kierunek ruchu Inky'ego na podstawie scian i celu
void Inky::updateDirection(const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2i inkyGrid = worldToGrid(sprite.getPosition());

    // jesli Inky osiagnal cel, wybierz nowy losowy cel
    if (inkyGrid == targetTile) {
        targetTile = getRandomWalkableTile(walls);
    }

    // znajdz sciezke do celu i ustaw nastepny krok
    std::vector<sf::Vector2i> path = bfs(inkyGrid, targetTile);

    if (!path.empty() && path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - inkyGrid;

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
}

// wybiera losowy dostepny kafelek na mapie, unikajac scian
sf::Vector2i Inky::getRandomWalkableTile(const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2i randomTile;
    bool valid = false;

    do {
        randomTile = { randomInt(0, mapWidth - 1), randomInt(0, mapHeight - 1) };
        sf::Vector2f worldPos = gridToWorld(randomTile);

        // sprawdzenie, czy losowy kafelek nie zawiera sie w scianie
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

// aktualizuje pozycje i ruch Inky'ego oraz sprawdza kolizje ze scianami
void Inky::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    updateDirection(walls);

    // zmiana kierunku ruchu, jesli mozliwe
    if (pendingDirection != direction && canMoveInDirection(pendingDirection, deltaTime, walls)) {
        direction = pendingDirection;
        centerOnTile();
    }

    // przesuniecie duszka
    sf::Vector2f movement = direction * speed * deltaTime;
    sprite.move(movement);

    // sprawdzenie kolizji ze scianami
    bool collided = false;
    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-movement);
            collided = true;
            break;
        }
    }

    // zatrzymanie duszka w razie kolizji
    if (collided) {
        direction = { 0.f, 0.f };
    }
}

// rysuje Inky'ego w oknie gry
void Inky::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// sprawdza, czy mozna poruszyc sie w okreslonym kierunku, unikajac scian
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

// centruje Inky'ego na srodku kafelka
void Inky::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

// sprawdza kolizje Inky'ego z podanym obiektem
bool Inky::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false;
    return bounds.intersects(wall.getGlobalBounds());
}

// zwraca aktualne granice Inky'ego jako prostokat
sf::FloatRect Inky::getBounds() const {
    return sprite.getGlobalBounds();
}
