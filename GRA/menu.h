#pragma once
#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp>
#include <vector>

class Menu {
public:
    Menu(float width, float height);
    void draw(sf::RenderWindow& window);
    int handleInput(sf::Event& event);

private:
    sf::Font font;
    sf::Text title;
    std::vector<sf::Text> options;
    int selectedItemIndex;
};

#endif // MENU_H
