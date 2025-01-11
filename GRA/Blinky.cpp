#include "Blinky.h"
#include "MapData.h"
#include "Pathfinding.h"

// konstruktor blinkiego
// ustawia poczatkowa pozycje i teksture blinkiego
Blinky::Blinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor)
    : Ghost(pacman, ghostRoomDoor, "blinky.png", { 9 * tileSize, 6 * tileSize }) {}

// logika ruchu blinkiego
// oblicza kierunek, w ktorym powinien sie ruszyc blinky, aby podazac za pacmanem
void Blinky::updateDirection() {
    sf::Vector2i blinkyGrid = worldToGrid(sprite.getPosition());
    sf::Vector2i pacmanGrid = worldToGrid(pacman.getPosition());

    // szuka sciezki od blinkiego do pacmana
    std::vector<sf::Vector2i> path = bfs(blinkyGrid, pacmanGrid);

    if (path.size() > 1) {
        sf::Vector2i nextStep = path[1];
        sf::Vector2i moveDirection = nextStep - blinkyGrid;
        pendingDirection = sf::Vector2f(moveDirection.x, moveDirection.y);
    }
}
