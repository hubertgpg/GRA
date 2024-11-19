#pragma once
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Gwiazdozbior.h"



class Menu {
public:
    Menu(float width, float height, Gwiazdozbior* gwiazdozbiorPtr);
    void draw(sf::RenderWindow& window);
    int handleInput(sf::Event& event);


private:
    sf::Font font;
    sf::Text title;
    std::vector<sf::Text> options;
    int selectedItemIndex;
    Gwiazdozbior* gwiazdozbior;
};

#endif 
