#pragma once
#include "Ghost.h"

// klasa blinky dziedziczy po klasie Ghost
class Blinky : public Ghost {
public:
    // konstruktor klasy blinky
    Blinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);

protected:
    // funkcja do obliczania kierunku ruchu blinkiego
    void updateDirection() override; // specyficzna logika kierunku
};
