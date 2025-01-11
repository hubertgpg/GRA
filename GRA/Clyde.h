#pragma once
#ifndef CLYDE_H
#define CLYDE_H

#include "Ghost.h"

// klasa clyde dziedziczy po ghost
class Clyde : public Ghost {
public:
    // konstruktor clyde
    Clyde(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);

    // aktualizuje stan clyde (nadpisanie funkcji z klasy bazowej)
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls) override;

private:
    bool chasingPacman; // flaga czy clyde goni pacmana
    const sf::Vector2i scatterTarget = { 17, 13 }; // cel scatter dla clyde

    // logika kierunku ruchu clyde zale¿na od trybu (gonienie/scatter)
    void updateDirection() override;

    // oblicza odleg³oœæ clyde od pacmana
    float distanceToPacman() const;
};

#endif
