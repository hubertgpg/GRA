#include "Pinky.h"
#include "MapData.h"
#include "Pathfinding.h"

// konstruktor pinky, ustawia teksture i poczatkowa pozycje
Pinky::Pinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor)
    : Ghost(pacman, ghostRoomDoor, "pinky.png", { 9 * tileSize, 7 * tileSize }) {}

// metoda ustawiajaca kierunek ruchu pinky na podstawie pozycji pacmana
void Pinky::updateDirection() {
    sf::Vector2i pinkyGrid = worldToGrid(sprite.getPosition()); // aktualna pozycja pinky na siatce
    sf::Vector2i pacmanGrid = worldToGrid(pacman.getPosition()); // aktualna pozycja pacmana na siatce

    sf::Vector2f pacmanDirection = pacman.getDirection(); // kierunek ruchu pacmana
    sf::Vector2i pinkyTarget = pacmanGrid + sf::Vector2i(static_cast<int>(pacmanDirection.x * 4), static_cast<int>(pacmanDirection.y * 4));
    // cel pinky to pozycja przed pacmanem (4 kafelki w jego kierunku)

    std::vector<sf::Vector2i> path = bfs(pinkyGrid, pinkyTarget); // znajdowanie sciezki do celu

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1]; // kolejny krok na sciezce
        sf::Vector2i moveDirection = nextStep - pinkyGrid; // obliczanie kierunku ruchu
        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y); // ustawianie kierunku ruchu
    }
}
