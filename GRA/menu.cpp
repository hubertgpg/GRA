#include "menu.h"
#include <iostream>
#include "Gwiazdozbior.h"

// konstruktor klasy Menu
// inicjalizuje elementy menu, w tym tytul i opcje, oraz wskaznik do obiektu gwiazdozbioru
Menu::Menu(float width, float height, Gwiazdozbior* gwiazdozbiorPtr)
    : selectedItemIndex(0), gwiazdozbior(gwiazdozbiorPtr) {
    // ladowanie czcionki dla menu
    if (!font.loadFromFile("MadimiOne-Regular.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    // ustawienia tytulu
    title.setFont(font);
    title.setString("PACMAN");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    title.setPosition(width / 2.0f, height / 6.0f);

    // lista opcji menu
    std::vector<std::u32string> menuItems = { U"Graj", U"Wyniki", U"Wyjœcie" }; // u¿ycie UTF-32
    for (size_t i = 0; i < menuItems.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        option.setString(sf::String::fromUtf32(menuItems[i].begin(), menuItems[i].end())); // konwersja z UTF-32 na sf::String
        option.setCharacterSize(30);
        option.setFillColor(i == selectedItemIndex ? sf::Color::Yellow : sf::Color::White);

        // ustawienie pozycji kazdej opcji menu
        sf::FloatRect optionBounds = option.getLocalBounds();
        option.setOrigin(optionBounds.width / 2.0f, optionBounds.height / 2.0f);
        option.setPosition(width / 2.0f, height / 2.0f + i * 50);

        // dodanie opcji do wektora
        options.push_back(option);
    }
}

// metoda rysujaca menu na ekranie
void Menu::draw(sf::RenderWindow& window) {
    gwiazdozbior->draw(); // rysowanie tla gwiazdozbioru
    window.draw(title);   // rysowanie tytulu
    for (const auto& option : options) {
        window.draw(option); // rysowanie kazdej opcji menu
    }
}

// metoda obslugujaca wejscie uzytkownika w menu
// obsluguje poruszanie sie po menu i wybor opcji
int Menu::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        // obsluga klawisza strzalki w gore
        if (event.key.code == sf::Keyboard::Up) {
            options[selectedItemIndex].setFillColor(sf::Color::White); // reset koloru poprzedniej opcji
            selectedItemIndex = (selectedItemIndex - 1 + options.size()) % options.size(); // przejscie do poprzedniej opcji
            options[selectedItemIndex].setFillColor(sf::Color::Yellow); // podswietlenie nowej opcji
        }
        // obsluga klawisza strzalki w dol
        else if (event.key.code == sf::Keyboard::Down) {
            options[selectedItemIndex].setFillColor(sf::Color::White); // reset koloru poprzedniej opcji
            selectedItemIndex = (selectedItemIndex + 1) % options.size(); // przejscie do nastepnej opcji
            options[selectedItemIndex].setFillColor(sf::Color::Yellow); // podswietlenie nowej opcji
        }
        // obsluga klawisza Enter
        else if (event.key.code == sf::Keyboard::Enter) {
            return selectedItemIndex; // zwraca indeks wybranej opcji
        }
    }
    return -1; // zwraca -1, jesli zadna opcja nie zostala wybrana
}
