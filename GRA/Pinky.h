#pragma once
#include "Ghost.h"

// klasa reprezentujaca ducha pinky
class Pinky : public Ghost {
public:
    // konstruktor pinky, przyjmuje obiekt pacmana i drzwi do pokoju duchow
    Pinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);

protected:
    // metoda ustawiajaca kierunek ruchu pinky (logika specyficzna dla pinky)
    void updateDirection() override;
};
