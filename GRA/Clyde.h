#pragma once
#ifndef CLYDE_H
#define CLYDE_H

#include <SFML/Graphics.hpp>
#include "Pacman.h"
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include "Vector2Hash.h"

// klasa reprezentujaca duszka Clyde w grze
class Clyde {
public:
    // konstruktor klasy Clyde, inicjalizuje duszka na podstawie pozycji Pac-Mana i drzwi pokoju duchow
    Clyde(const Pacman& pacman, const sf::RectangleShape& ghostRoomDoor);

    // metoda aktualizujaca stan duszka (ruch, kolizje, tryb)
    void update(float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // metoda rysujaca duszka na ekranie
    void draw(sf::RenderWindow& window);

    // metoda zwracajaca aktualna pozycje Clyde
    sf::Vector2f getPosition() const;

    // metoda zwracajaca granice (bounding box) Clyde
    sf::Rect<float> getBounds() const;

private:
    const Pacman& pacman; // referencja do Pac-Mana
    const sf::RectangleShape* ghostRoomDoor; // wskaznik na drzwi pokoju duchow
    sf::Texture texture; // tekstura duszka
    sf::Sprite sprite; // sprite duszka
    float speed; // predkosc poruszania
    sf::Vector2f direction; // aktualny kierunek ruchu
    sf::Vector2f pendingDirection; // kierunek, ktory Clyde zamierza obrac

    bool chasingPacman; // flaga wskazujaca, czy Clyde goni Pac-Mana
    const sf::Vector2i scatterTarget = { 17, 13 }; // cel scatter dla Clyde

    // metoda aktualizujaca kierunek ruchu Clyde w zaleznosci od trybu (gonienie/scatter)
    void updateDirection();

    // metoda sprawdzajaca, czy Clyde moze ruszyc w podanym kierunku
    bool canMoveInDirection(const sf::Vector2f& dir, float deltaTime, const std::vector<sf::RectangleShape>& walls);

    // metoda centrowania Clyde na srodku kafelka mapy
    void centerOnTile();

    // metoda sprawdzajaca kolizje Clyde z przeszkoda
    bool checkCollision(const sf::FloatRect& bounds, const sf::RectangleShape& wall);

    // metoda obliczajaca odleglosc Clyde od Pac-Mana
    float distanceToPacman() const;
};

#endif
