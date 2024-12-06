#pragma once
#ifndef BLINKY_H
#define BLINKY_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"

// klasa reprezentujaca duszka Blinky
class Blinky {
public:
    // konstruktor inicjalizujacy duszka
    Blinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);

    // metoda aktualizujaca stan duszka
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // metoda rysujaca duszka na oknie
    void draw(sf::RenderWindow& window);

    // metoda zwracajaca pozycje duszka
    sf::Vector2f getPosition() const;

    // metoda zwracajaca granice (bounding box) duszka
    sf::FloatRect getBounds() const;

private:
    const Pacman& pacman; // odniesienie do obiektu Pac-Mana
    const sf::RectangleShape* ghostRoomDoor; // wskaznik na drzwi do pokoju duchow
    sf::Texture texture; // tekstura duszka
    sf::Sprite sprite; // sprite duszka
    float speed; // predkosc poruszania sie duszka
    sf::Vector2f direction; // aktualny kierunek ruchu
    sf::Vector2f pendingDirection; // oczekiwany kierunek ruchu

    // metoda aktualizujaca kierunek ruchu duszka
    void updateDirection();

    // sprawdzanie, czy duszek moze poruszyc sie w podanym kierunku
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // centrowanie duszka na srodku kafelka mapy
    void centerOnTile();

    // sprawdzanie kolizji z obiektem
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);
};

#endif
