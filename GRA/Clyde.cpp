#include "Clyde.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>

// metoda zwracajaca aktualna pozycje duszka Clyde
sf::Vector2f Clyde::getPosition() const {
    return sprite.getPosition();
}

// konstruktor klasy Clyde
// ustawia poczatkowe wartosci, takie jak tekstura, pozycja, predkosc i tryb goniacy/scatter
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

// metoda aktualizujaca kierunek ruchu Clyde w zaleznosci od trybu (gonienie/scatter)
void Clyde::updateDirection() {
    sf::Vector2i clydeGrid = worldToGrid(sprite.getPosition());
    sf::Vector2i targetGrid = chasingPacman ? worldToGrid(pacman.getPosition()) : scatterTarget;

    // sprawdzanie, czy Clyde osiagnal cel w trybie scatter
    if (!chasingPacman && clydeGrid == scatterTarget) {
        chasingPacman = true;
        return;
    }

    // obliczanie sciezki za pomoca algorytmu BFS
    std::vector<sf::Vector2i> path = bfs(clydeGrid, targetGrid);

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - clydeGrid;

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
    else {
        pendingDirection = { 0.f, 0.f }; // brak ruchu, gdy sciezka nie istnieje
    }
}

// metoda aktualizujaca stan Clyde, w tym jego tryb, kierunek i kolizje
void Clyde::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    float distance = distanceToPacman();

    // zmiana trybu na scatter, gdy Clyde jest blisko Pac-Mana
    if (chasingPacman && distance < 6.f * tileSize) {
        chasingPacman = false;
    }

    // powrot do gonienia, gdy Clyde jest wystarczajaco daleko od Pac-Mana
    if (!chasingPacman && distance > 8.f * tileSize) {
        chasingPacman = true;
    }

    updateDirection();

    // sprawdzanie, czy mozna zmienic kierunek na oczekiwany
    if (pendingDirection != direction && canMoveInDirection(pendingDirection, deltaTime, walls)) {
        direction = pendingDirection;
        centerOnTile();
    }

    // przesuniecie Clyde w kierunku zgodnym z predkoscia
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

// metoda rysujaca Clyde na ekranie
void Clyde::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// sprawdzanie, czy Clyde moze ruszyc w podanym kierunku
bool Clyde::canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls) {
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

// metoda centrowania Clyde na srodku kafelka mapy
void Clyde::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

// metoda sprawdzajaca kolizje Clyde z przeszkoda
bool Clyde::checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall) {
    if (&wall == ghostRoomDoor) return false; // ignoruj kolizje z drzwiami do pokoju duchow
    return bounds.intersects(wall.getGlobalBounds());
}

// metoda obliczajaca odleglosc Clyde od Pac-Mana
float Clyde::distanceToPacman() const {
    sf::Vector2f pacmanPos = pacman.getPosition();
    sf::Vector2f clydePos = sprite.getPosition();
    return std::sqrt(std::pow(pacmanPos.x - clydePos.x, 2) + std::pow(pacmanPos.y - clydePos.y, 2));
}

// metoda zwracajaca granice (bounding box) Clyde
sf::Rect<float> Clyde::getBounds() const {
    return sprite.getGlobalBounds();
}
