#include "Clyde.h"
#include "MapData.h"
#include "Pathfinding.h"
#include <cmath>

// konstruktor clyde
Clyde::Clyde(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor)
    : Ghost(pacman, ghostRoomDoor, "clyde.png", { 10 * tileSize, 7 * tileSize }), chasingPacman(true) {
    // na pocz¹tku clyde goni pacmana
}

// aktualizuje kierunek clyde
void Clyde::updateDirection() {
    sf::Vector2i clydeGrid = worldToGrid(sprite.getPosition());
    sf::Vector2i targetGrid = chasingPacman ? worldToGrid(pacman.getPosition()) : scatterTarget;

    // zmienia tryb na chasing, gdy osi¹gnie cel scatter
    if (!chasingPacman && clydeGrid == scatterTarget) {
        chasingPacman = true;
        return;
    }

    // oblicza œcie¿kê do celu
    std::vector<sf::Vector2i> path = bfs(clydeGrid, targetGrid);

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - clydeGrid;

        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
    else {
        pendingDirection = { 0.f, 0.f }; // brak ruchu gdy nie ma œcie¿ki
    }
}

// aktualizuje tryb clyde i kierunek ruchu
void Clyde::update(float deltaTime, const std::vector<sf::RectangleShape>& walls) {
    float distance = distanceToPacman();

    // zmienia tryb na scatter, gdy clyde jest blisko pacmana
    if (chasingPacman && distance < 6.f * tileSize) {
        chasingPacman = false;
    }

    // wraca do trybu gonienia, gdy jest wystarczaj¹co daleko
    if (!chasingPacman && distance > 8.f * tileSize) {
        chasingPacman = true;
    }

    updateDirection(); // aktualizuje kierunek
    Ghost::update(deltaTime, walls); // u¿ywa metody z klasy bazowej do ruchu
}

// oblicza odleg³oœæ clyde od pacmana
float Clyde::distanceToPacman() const {
    sf::Vector2f pacmanPos = pacman.getPosition();
    sf::Vector2f clydePos = sprite.getPosition();
    return std::sqrt(std::pow(pacmanPos.x - clydePos.x, 2) + std::pow(pacmanPos.y - clydePos.y, 2));
}
