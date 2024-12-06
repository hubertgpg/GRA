#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846 // definiowanie wartosci pi
#endif

// klasa reprezentujaca gwiazdozbior zlozony z gwiazd
class Gwiazdozbior {
private:
    // struktura przechowujaca dane pojedynczej gwiazdy
    struct Star {
        sf::ConvexShape shape;      // kszta³t gwiazdy
        sf::Vector2f velocity;      // predkosc gwiazdy
        float alpha;                // poziom przezroczystosci gwiazdy
    };

    std::vector<Star> stars;         // wektor przechowujacy wszystkie gwiazdy
    int count;                       // liczba gwiazd w gwiazdozbiorze
    sf::RenderWindow& window;        // odniesienie do okna, w ktorym rysowany jest gwiazdozbior
    std::random_device rd;           // generator losowych danych
    std::mt19937 gen;                // silnik generatora losowego

    // metoda tworzaca gwiazde na podstawie podanych parametrow
    sf::ConvexShape createStar(float radius, sf::Vector2f position, sf::Color color);

public:
    // konstruktor inicjalizujacy gwiazdozbior z okreslona liczba gwiazd i oknem
    Gwiazdozbior(int count, sf::RenderWindow& win);

    // metoda rysujaca gwiazdozbior w oknie
    void draw();

    // metoda aktualizujaca stan gwiazd (np. pozycje, animacje)
    void update(float deltaTime);
};
