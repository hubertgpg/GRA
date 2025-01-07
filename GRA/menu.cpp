#include "menu.h"
#include <iostream>
#include "Gwiazdozbior.h"

// konstruktor klasy Menu
Menu::Menu(float width, float height, Gwiazdozbior* gwiazdozbiorPtr)
    : selectedItemIndex(0), gwiazdozbior(gwiazdozbiorPtr) {
    // �adowanie czcionki dla menu
    if (!font.loadFromFile("MadimiOne-Regular.ttf")) {
        std::cerr << "B��d �adowania czcionki" << std::endl;
    }

    // ustawienia tytu�u
    title.setFont(font);
    title.setString("PACMAN");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    title.setPosition(width / 2.0f, height / 6.0f);

    // lista opcji menu
    std::vector<std::u32string> menuItems = { U"Graj", U"Wyniki", U"Wyj�cie" };
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
    instructionsText.setString(L"Witaj u�ytkowniku w grze imituj�cej gr� PACMAN.\n\n"
        L"Twoim zadaniem jako tytu�owy PACMAN jest \n"
        L"unikanie duszk�w oraz zbieranie groszk�w,\n"
        L"kt�re daj� ci punkty (1 groszek - 10 punkt�w).\n\n"
        L"W grze znajduj� si� 4 duszki:\n"
        L"- Blinky, kt�ry b�dzie ci� �ciga�\n"
        L"- Pinky, kt�ry b�dzie pr�bowa� zablokowa� ci drog�\n"
        L"- Inky, kt�ry porusza si� ca�kowicie losowo\n"
        L"- Clyde, kt�ry jest troch� strachliwy\n\n"
        L"Pacmanem steruje si� za pomoc� strza�ek.\n\n"
        L"Gdy duszek ci� z�apie gra si� ko�czy.\n"
        L"W trakcie gry, wciskaj�c klawisz ESC, mo�esz j� zatrzyma�.\n\n"
        L"W Menu poruszamy si� strza�kami i wybieramy opcje\n"
        L"wciskaj�c klawisz ENTER.\n\n"
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
            showInstructions = true; // wy�wietlanie instrukcji
            return -1;
        }
        else if (showInstructions && event.key.code == sf::Keyboard::Escape) {
            showInstructions = false; // zamkni�cie instrukcji
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
