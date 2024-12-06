#include "Pinky.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>
#include <functional>

// konstruktor klasy Pinky, inicjalizuje teksture, polozenie i parametry ducha
Pinky::Pinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor)
    : pacman(pacman), ghostRoomDoor(&ghostRoomDoor), speed(100.f), direction(0.f, 0.f), pendingDirection(0.f, 0.f) {
    texture.loadFromFile("pinky.png"); // wczytanie tekstury ducha
    sprite.setTexture(texture); // przypisanie tekstury do spirta
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().x, // skalowanie tekstury, aby pasowala do kafelkow
        (tileSize * wallShrinkFactor - 2.f) / texture.getSize().y
    );
    sprite.setPosition(9 * tileSize, 7 * tileSize); // poczatkowe polozenie ducha
}

// funkcja obliczajaca cel Pinky, bazujac na polozeniu i kierunku Pacmana
sf::Vector2i calculatePinkyTarget(const sf::Vector2i& pacmanGrid, const sf::Vector2f& pacmanDirection) {
    return pacmanGrid + sf::Vector2i(static_cast<int>(pacmanDirection.x * 4), static_cast<int>(pacmanDirection.y * 4));
}

// aktualizacja kierunku ruchu Pinky na podstawie sciezki BFS
void Pinky::updateDirection() {
    sf::Vector2i pinkyGrid = worldToGrid(sprite.getPosition()); // pozycja Pinky w siatce
    sf::Vector2i pacmanGrid = worldToGrid(pacman.getPosition()); // pozycja Pacmana w siatce

    sf::Vector2f pacmanDirection = pacman.getDirection(); // kierunek ruchu Pacmana
    sf::Vector2i pinkyTarget = calculatePinkyTarget(pacmanGrid, pacmanDirection); // cel Pinky

    std::vector<sf::Vector2i> path = bfs(pinkyGrid, pinkyTarget); // wyznaczanie sciezki BFS

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1]; // nastepny krok w sciezce
        sf::Vector2i moveDirection = nextStep - pinkyGrid; // kierunek ruchu do nastepnego kafelka

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y); // zapisanie kierunku do wykonania
    }
}

// aktualizacja stanu Pinky: kierunek, ruch i kolizje
void Pinky::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    updateDirection(); // aktualizacja kierunku na podstawie BFS

    if (pendingDirection != direction && canMoveInDirection(pendingDirection, deltaTime, walls)) {
        direction = pendingDirection; // zmiana kierunku
        centerOnTile(); // wycentrowanie na kafelku
    }

    sf::Vector2f movement = direction * speed * deltaTime; // obliczenie ruchu
    sprite.move(movement); // wykonanie ruchu

    // sprawdzenie kolizji z przeszkodami
    bool collided = false;
    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-movement); // cofniecie ruchu w przypadku kolizji
            collided = true;
            break;
        }
    }

    if (collided) {
        direction = { 0.f, 0.f }; // zatrzymanie ruchu w przypadku kolizji
    }
}

// rysowanie Pinky na oknie gry
void Pinky::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// sprawdzenie, czy Pinky moze poruszac sie w zadanym kierunku
bool Pinky::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2f nextPosition = sprite.getPosition() + dir * static_cast<float>(tileSize); // nastepna pozycja
    sprite.setPosition(nextPosition);
    for (const auto& wall : walls) {
        if (&wall != ghostRoomDoor && checkCollision(sprite.getGlobalBounds(), wall)) {
            sprite.move(-dir * static_cast<float>(tileSize)); // cofniecie w przypadku kolizji
            return false;
        }
    }
    sprite.move(-dir * static_cast<float>(tileSize)); // przywrocenie pozycji
    return true;
}

// wycentrowanie Pinky na kafelku
void Pinky::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

// sprawdzenie kolizji Pinky z przeszkoda
bool Pinky::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false; // ignoruje drzwi duchow
    return bounds.intersects(wall.getGlobalBounds()); // sprawdza kolizje z przeszkoda
}

// zwraca granice spirta Pinky
sf::FloatRect Pinky::getBounds() const {
    return sprite.getGlobalBounds();
}
