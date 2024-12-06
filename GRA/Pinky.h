#pragma once
#ifndef PINKY_H
#define PINKY_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"

// klasa pinky reprezentuje ducha sledzacego gracza w grze
class Pinky {
public:
    // konstruktor pinky, inicjalizuje pozycje i ustawienia
    Pinky(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);

    // aktualizuje logike ruchu pinky
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // rysuje pinky na oknie
    void draw(sf::RenderWindow& window);

    // zwraca granice (bounding box) pinky dla kolizji
    sf::FloatRect getBounds() const;

private:
    const Pacman& pacman; // odniesienie do obiektu pacman dla sledzenia jego pozycji
    const sf::RectangleShape* ghostRoomDoor; // wskaznik na drzwi pokoju duchow
    sf::Texture texture; // tekstura pinky
    sf::Sprite sprite; // sprite reprezentujacy pinky
    float speed; // predkosc ruchu pinky
    sf::Vector2f direction; // aktualny kierunek ruchu pinky
    sf::Vector2f pendingDirection; // kierunek oczekujacy na zmiane

    // aktualizuje kierunek na podstawie algorytmu pathfinding
    void updateDirection();

    // sprawdza czy mozna sie poruszyc w danym kierunku
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // centruje pinky na srodku kafelka
    void centerOnTile();

    // sprawdza kolizje pinky z przeszkodami
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);
};

#endif
