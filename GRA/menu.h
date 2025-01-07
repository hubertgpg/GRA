#pragma once

#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Gwiazdozbior.h"

// klasa menu - odpowiedzialna za wyswietlanie i obsluge menu
class Menu {
public:
    // konstruktor klasy Menu, inicjalizuje menu na podstawie rozmiaru okna i wskaznika na Gwiazdozbior
    Menu(float width, float height, Gwiazdozbior* gwiazdozbiorPtr);

    // metoda rysujaca menu na podanym oknie renderowania
    void draw(sf::RenderWindow& window);

    // metoda obslugujaca zdarzenia, zwraca indeks wybranej opcji
    int handleInput(sf::Event& event);

private:
    sf::Font font;                      // czcionka uzywana w menu
    sf::Text title;                     // tytul menu
    std::vector<sf::Text> options;      // opcje menu
    int selectedItemIndex;              // indeks aktualnie wybranej opcji
    Gwiazdozbior* gwiazdozbior;         // wskaznik na Gwiazdozbior, do ktorego menu odnosi sie

    sf::Text instructionsText;          // tekst z instrukcjami gry
    bool showInstructions = false;      // flaga kontrolujaca wyswietlanie instrukcji
};

#endif
