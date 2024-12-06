#pragma once
#ifndef INKY_H
#define INKY_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include "Blinky.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"

// klasa reprezentujaca duszka Inky'ego
class Inky {
public:
    // konstruktor - inicjalizuje Inky'ego z odniesieniem do Pacmana, Blinky'ego i drzwi pokoju duchow
    Inky(const Pacman& pacman, const Blinky& blinky, const sf::RectangleShape& ghostRoomDoor);

    // metoda aktualizujaca ruch i stan Inky'ego
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // metoda rysujaca Inky'ego w oknie gry
    void draw(sf::RenderWindow& window);

    // zwraca aktualna pozycje Inky'ego
    sf::Vector2f getPosition() const;

    // zwraca aktualne granice Inky'ego
    sf::FloatRect getBounds() const;

private:
    const Pacman& pacman;                    // odniesienie do obiektu Pacmana
    const Blinky& blinky;                    // odniesienie do obiektu Blinky'ego
    const sf::RectangleShape* ghostRoomDoor; // wskaznik na drzwi pokoju duchow
    sf::Texture texture;                     // tekstura Inky'ego
    sf::Sprite sprite;                       // sprite reprezentujacy Inky'ego
    float speed;                             // predkosc ruchu Inky'ego
    sf::Vector2f direction;                  // obecny kierunek ruchu
    sf::Vector2f pendingDirection;           // oczekujacy kierunek ruchu
    sf::Vector2i targetTile;                 // kafelek celu

    // aktualizuje kierunek ruchu Inky'ego
    void updateDirection(const std::vector<sf::RectangleShape>& walls);

    // wybiera losowy kafelek, ktory nie jest sciana
    sf::Vector2i getRandomWalkableTile(const std::vector<sf::RectangleShape>& walls);

    // sprawdza, czy Inky moze poruszac sie w danym kierunku, unikajac kolizji
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // centruje Inky'ego na srodku kafelka
    void centerOnTile();

    // sprawdza, czy granice Inky'ego koliduja z podanym obiektem
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);
};

#endif
