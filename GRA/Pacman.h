#pragma once

#ifndef PACMAN_H
#define PACMAN_H

#include <SFML/Graphics.hpp>
#include <vector>

// klasa pacman - odpowiedzialna za obsluge ruchu, animacji i kolizji pacmana
class Pacman {
public:
    // konstruktor inicjalizujacy obiekt pacman
    Pacman();

    // metoda obslugujaca wejscie uzytkownika (klawiatura)
    void handleInput();

    // metoda aktualizujaca stan pacmana (ruch, kolizje, animacje)
    void update(float deltaTime, std::vector<sf::CircleShape>& dots, const std::vector<sf::RectangleShape>& walls, int& score);

    // metoda rysujaca pacmana w podanym oknie renderowania
    void draw(sf::RenderWindow& window);

    // metoda zwracajaca aktualna pozycje pacmana
    sf::Vector2f getPosition() const;

    // metoda centrujaca pacmana na srodku kafelka
    void centerOnTile();

    // metoda zwracajaca aktualny kierunek pacmana
    sf::Vector2f getDirection() const;

    // metoda zwracajaca granice pacmana (bounding box)
    sf::FloatRect getBounds() const;

private:
    sf::Sprite sprite;                  // obiekt reprezentujacy pacmana
    sf::Texture textures[4][3];         // tekstury animacji pacmana (dla kazdego kierunku i klatki)
    sf::Vector2f direction;             // aktualny kierunek ruchu pacmana
    sf::Vector2f pendingDirection;      // oczekujacy kierunek ruchu
    float speed;                        // predkosc pacmana
    int animationFrame;                 // aktualna klatka animacji
    float animationTimer;               // licznik czasu do zmiany klatki

    // metoda sprawdzajaca kolizje pacmana z obiektem sciany
    bool checkCollision(const sf::RectangleShape& wall);

    // metoda sprawdzajaca kolizje pacmana z punktem
    bool checkDotCollision(sf::CircleShape& dot);

    // metoda sprawdzajaca, czy pacman moze poruszac sie w podanym kierunku
    bool canMoveInDirection(const sf::Vector2f& dir, const std::vector<sf::RectangleShape>& walls);

    // metoda aktualizujaca animacje pacmana
    void updateAnimation(float deltaTime);

    // metoda wczytujaca tekstury animacji pacmana
    void loadTextures();

    // metoda zwracajaca indeks kierunku dla animacji
    int getDirectionIndex() const;
};

#endif // PACMAN_H
