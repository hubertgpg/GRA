#include "menu.h"
#include <iostream>
#include "Gwiazdozbior.h"

// konstruktor klasy Menu
Menu::Menu(float width, float height, Gwiazdozbior* gwiazdozbiorPtr)
    : selectedItemIndex(0), gwiazdozbior(gwiazdozbiorPtr) {
    // ³adowanie czcionki dla menu
    if (!font.loadFromFile("MadimiOne-Regular.ttf")) {
        std::cerr << "B³¹d ³adowania czcionki" << std::endl;
    }

    // ustawienia tytu³u
    title.setFont(font);
    title.setString("PACMAN");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    title.setPosition(width / 2.0f, height / 6.0f);

    // lista opcji menu
    std::vector<std::u32string> menuItems = { U"Graj", U"Wyniki", U"Wyjœcie" };
    for (size_t i = 0; i < menuItems.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        option.setString(sf::String::fromUtf32(menuItems[i].begin(), menuItems[i].end()));
        option.setCharacterSize(30);
        option.setFillColor(i == selectedItemIndex ? sf::Color::Yellow : sf::Color::White);

        sf::FloatRect optionBounds = option.getLocalBounds();
        option.setOrigin(optionBounds.width / 2.0f, optionBounds.height / 2.0f);
        option.setPosition(width / 2.0f, height / 2.0f + i * 50);

        options.push_back(option);
    }

    // tekst instrukcji
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(18);
    instructionsText.setFillColor(sf::Color::White);
    instructionsText.setString(L"Witaj u¿ytkowniku w grze imituj¹cej grê PACMAN.\n\n"
        L"Twoim zadaniem jako tytu³owy PACMAN jest \n"
        L"unikanie duszków oraz zbieranie groszków,\n"
        L"które daj¹ ci punkty (1 groszek - 10 punktów).\n\n"
        L"W grze znajduj¹ siê 4 duszki:\n"
        L"- Blinky, który bêdzie ciê œciga³\n"
        L"- Pinky, który bêdzie próbowa³ zablokowaæ ci drogê\n"
        L"- Inky, który porusza siê ca³kowicie losowo\n"
        L"- Clyde, który jest trochê strachliwy\n\n"
        L"Pacmanem steruje siê za pomoc¹ strza³ek.\n\n"
        L"Gdy duszek ciê z³apie gra siê koñczy.\n"
        L"W trakcie gry, wciskaj¹c klawisz ESC, mo¿esz j¹ zatrzymaæ.\n\n"
        L"W Menu poruszamy siê strza³kami i wybieramy opcje\n"
        L"wciskaj¹c klawisz ENTER.\n\n"
        L"Powodzenia!!!");
    instructionsText.setPosition(50, 50);
}

void Menu::draw(sf::RenderWindow& window) {
    if (showInstructions) {
        window.draw(instructionsText); // rysowanie tylko instrukcji
    }
    else {
        gwiazdozbior->draw();
        window.draw(title);

        for (const auto& option : options) {
            window.draw(option);
        }
    }
}

int Menu::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F1) {
            showInstructions = true; // wyœwietlanie instrukcji
            return -1;
        }
        else if (showInstructions && event.key.code == sf::Keyboard::Escape) {
            showInstructions = false; // zamkniêcie instrukcji
            return -1;
        }
        else if (!showInstructions) {
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
    }
    return -1;
}
