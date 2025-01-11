#pragma once
#ifndef GHOST_H
#define GHOST_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"

// klasa reprezentujaca duszka w grze
class Ghost {
public:
    // konstruktor inicjalizujacy duszka
    Ghost(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor, const std::string& textureFile, sf::Vector2f startPosition);

    virtual ~Ghost() = default;

    // metoda aktualizujaca stan duszka (pozycja, kolizje, kierunek)
    virtual void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // metoda rysujaca duszka na ekranie
    void draw(sf::RenderWindow& window);

    // zwraca aktualna pozycje duszka
    sf::Vector2f getPosition() const;

    // zwraca bounding box duszka
    sf::FloatRect getBounds() const;

protected:
    const Pacman& pacman; // obiekt pacmana (do sledzenia pozycji)
    const sf::RectangleShape* ghostRoomDoor; // wskaznik na drzwi pokoju duchow
    sf::Texture texture; // tekstura duszka
    sf::Sprite sprite; // sprite duszka
    float speed; // predkosc duszka
    sf::Vector2f direction; // aktualny kierunek ruchu
    sf::Vector2f pendingDirection; // oczekiwany kierunek ruchu

    // metoda wirtualna do ustawiania kierunku ruchu (dla roznych typow duchow)
    virtual void updateDirection() = 0;

    // sprawdza, czy duszek moze poruszyc sie w wybranym kierunku
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // ustawia duszka na srodku kafelka
    void centerOnTile();

    // sprawdza kolizje duszka z obiektem
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);
};

#endif
