#include "Blinky.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>
#include <functional>

// metoda zwracajaca aktualna pozycje duszka Blinky
sf::Vector2f Blinky::getPosition() const {
    return sprite.getPosition();
}

// konstruktor klasy Blinky
// ustawia poczatkowe wartosci, takie jak tekstura, pozycja i parametry ruchu
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

// funkcja aktualizujaca kierunek ruchu Blinkiego na podstawie pozycji Pac-Mana
void Blinky::updateDirection() {
    sf::Vector2i blinkyGrid = worldToGrid(sprite.getPosition());
    sf::Vector2i pacmanGrid = worldToGrid(pacman.getPosition());

    // obliczanie sciezki za pomoca algorytmu BFS
    std::vector<sf::Vector2i> path = bfs(blinkyGrid, pacmanGrid);

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - blinkyGrid;

        // ustawienie oczekiwanego kierunku ruchu
        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
}

// glowna funkcja aktualizujaca ruch i kolizje Blinkiego
void Blinky::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    updateDirection();

    // zmiana kierunku, jesli mozna wykonac ruch w oczekiwanym kierunku
    if (pendingDirection != direction && canMoveInDirection(pendingDirection, deltaTime, walls)) {
        direction = pendingDirection;
        centerOnTile();
    }

    // obliczenie wektora ruchu i przesuniecie duszka
    sf::Vector2f movement = direction * speed * deltaTime;
    sprite.move(movement);

    // sprawdzanie kolizji ze scianami
    bool collided = false;
    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-movement); // cofnij ruch po kolizji
            collided = true;
            break;
        }
    }

    // zatrzymanie ruchu w przypadku kolizji
    if (collided) {
        direction = { 0.f, 0.f };
    }
}

// metoda rysujaca duszka na ekranie
void Blinky::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// sprawdzanie, czy Blinky moze ruszyc w podanym kierunku
bool Blinky::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2f nextPosition = sprite.getPosition() + dir * static_cast<float>(tileSize);
    sprite.setPosition(nextPosition);

    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-dir * static_cast<float>(tileSize)); // przywrocenie pozycji
            return false;
        }
    }
    sprite.move(-dir * static_cast<float>(tileSize));
    return true;
}

// centrowanie duszka na srodku kafelka mapy
void Blinky::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

// sprawdzanie kolizji duszka z przeszkoda
bool Blinky::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false; // ignoruj drzwi do pokoju duchow
    return bounds.intersects(wall.getGlobalBounds());
}

// zwraca granice (bounding box) duszka
sf::FloatRect Blinky::getBounds() const {
    return sprite.getGlobalBounds();
}
