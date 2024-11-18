#include "menu.h"
#include <iostream>

Menu::Menu(float width, float height) : selectedItemIndex(0) {
    if (!font.loadFromFile("ChainsawGeometric.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    // Title
    title.setFont(font);
    title.setString("PACMAN");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    title.setPosition(width / 2.0f, height / 6.0f);

    // Menu options
    std::vector<std::string> menuItems = { "Play", "Scores", "Exit" };
    for (size_t i = 0; i < menuItems.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        option.setString(menuItems[i]);
        option.setCharacterSize(30);
        option.setFillColor(i == selectedItemIndex ? sf::Color::Yellow : sf::Color::White);

        sf::FloatRect optionBounds = option.getLocalBounds();
        option.setOrigin(optionBounds.width / 2.0f, optionBounds.height / 2.0f);
        option.setPosition(width / 2.0f, height / 2.0f + i * 50);

        options.push_back(option);
    }
}

void Menu::draw(sf::RenderWindow& window) {
    window.draw(title);
    for (const auto& option : options) {
        window.draw(option);
    }
}

int Menu::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            options[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex = (selectedItemIndex - 1 + options.size()) % options.size();
            options[selectedItemIndex].setFillColor(sf::Color::Yellow);
        }
        else if (event.key.code == sf::Keyboard::Down) {
            options[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex = (selectedItemIndex + 1) % options.size();
            options[selectedItemIndex].setFillColor(sf::Color::Yellow);
        }
        else if (event.key.code == sf::Keyboard::Enter) {
            return selectedItemIndex;
        }
    }
    return -1; // No action taken
}
