#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <SFML/Graphics.hpp>
#include "MapData.h"
#include "Pacman.h"
#include "Blinky.h"
#include "Pinky.h"
#include "Inky.h"
#include "Clyde.h"
#include <vector>
#include "menu.h"
#include "Gwiazdozbior.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <locale>

// funkcja formatujaca czas na format mm:ss
std::string formatTime(float timeInSeconds) {
    int minutes = static_cast<int>(timeInSeconds) / 60;
    int seconds = static_cast<int>(timeInSeconds) % 60;

    std::ostringstream oss;
    oss << (minutes < 10 ? "0" : "") << minutes << ":"
        << (seconds < 10 ? "0" : "") << seconds;

    return oss.str();
}

// stale globalne
const float pacmanSpeed = 110.f; // predkosc pacmana
const int sidebarWidth = 200;    // szerokosc paska bocznego

// klasa reprezentujaca mape gry
class Map {
public:
    Map();
    void draw(sf::RenderWindow& window);

    const std::vector<sf::RectangleShape>& getPacmanWalls() const { return pacmanWalls; }
    const std::vector<sf::RectangleShape>& getGhostWalls() const { return ghostWalls; }
    std::vector<sf::CircleShape>& getDots() { return dots; }
    const sf::RectangleShape& getGhostRoomDoor() const { return ghostRoomDoor; }

private:
    std::vector<sf::RectangleShape> pacmanWalls; // sciany dla pacmana
    std::vector<sf::RectangleShape> ghostWalls;  // sciany dla duszkow
    std::vector<sf::CircleShape> dots;           // punkty do zebrania
    sf::RectangleShape ghostRoomDoor;           // drzwi do pokoju duszkow
};

// konstruktor mapy
Map::Map() {
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (mapData[y][x] == 1) {
                sf::RectangleShape wall(sf::Vector2f(tileSize * wallShrinkFactor, tileSize * wallShrinkFactor));
                wall.setFillColor(sf::Color::Blue);
                float offset = (tileSize - tileSize * wallShrinkFactor) / 2;
                wall.setPosition(x * tileSize + offset, y * tileSize + offset);
                pacmanWalls.push_back(wall);
                ghostWalls.push_back(wall);
            }
            else if (mapData[y][x] == 2) {
                sf::RectangleShape door(sf::Vector2f(tileSize * wallShrinkFactor, tileSize * wallShrinkFactor / 3));
                door.setFillColor(sf::Color::White);
                float offsetX = (tileSize - tileSize * wallShrinkFactor) / 2;
                float offsetY = (tileSize - tileSize * wallShrinkFactor / 3) / 2;
                door.setPosition(x * tileSize + offsetX, y * tileSize + offsetY);
                ghostRoomDoor = door;
                pacmanWalls.push_back(door);
            }
            else if (!(y >= 6 && y <= 8 && x >= 8 && x <= 10) && !(x == 9 && y == 13)) {
                sf::CircleShape dot(5.f);
                dot.setFillColor(sf::Color::White);
                dot.setPosition(x * tileSize + tileSize / 2 - dot.getRadius(), y * tileSize + tileSize / 2 - dot.getRadius());
                dots.push_back(dot);
            }
        }
    }
}

// rysowanie mapy
void Map::draw(sf::RenderWindow& window) {
    for (const auto& wall : pacmanWalls) {
        window.draw(wall);
    }
    window.draw(ghostRoomDoor);
    for (const auto& dot : dots) {
        window.draw(dot);
    }
}

// klasa gry
class Game {
public:
    Game();
    void run(); // glowna petla gry
    int getScore() const { return score; }


private:
    // elementy gry
    sf::RenderWindow window;
    Map map;
    Pacman pacman;
    Blinky blinky;
    Pinky pinky;
    Inky inky;
    Clyde clyde;

    // dane gry
    int score;
    float gameTime;
    int releaseThreshold;
    int ghostReleaseCounter;
    std::vector<bool> ghostInHouse;
    std::string gameState;
    bool isPaused;

    // elementy graficzne
    sf::Font font;
    sf::Text pacmanLabel;
    sf::Text scoreText;
    sf::Text timeText;
    sf::Text winText;
    sf::Text pacmanPositionText;
    sf::Text pauseText;

    // metody pomocnicze
    void update(float deltaTime);       // aktualizacja gry
    void render();                      // renderowanie gry
    void releaseNextGhost();            // uwolnienie kolejnego duszka

};

// konstruktor gry
Game::Game()
    : window(sf::VideoMode(mapWidth* tileSize + sidebarWidth, mapHeight* tileSize), "Pacman"),
    score(0),
    gameTime(0.f),
    releaseThreshold(250),
    ghostReleaseCounter(0),
    ghostInHouse{ true, true, true },
    blinky(pacman, map.getGhostRoomDoor()),
    pinky(pacman, map.getGhostRoomDoor()),
    inky(pacman, blinky, map.getGhostRoomDoor()),
    clyde(pacman, map.getGhostRoomDoor()),
    gameState("PLAYING"),
    isPaused(false) {

    // ladowanie czcionki
    if (!font.loadFromFile("MadimiOne-Regular.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    // konfiguracja napisu "PACMAN"
    pacmanLabel.setFont(font);
    pacmanLabel.setString("PACMAN");
    pacmanLabel.setCharacterSize(30);
    pacmanLabel.setFillColor(sf::Color::Yellow);
    sf::FloatRect pacmanBounds = pacmanLabel.getLocalBounds();
    pacmanLabel.setOrigin(pacmanBounds.left + pacmanBounds.width / 2.0f, pacmanBounds.top + pacmanBounds.height / 2.0f);
    pacmanLabel.setPosition(mapWidth * tileSize + sidebarWidth / 2.0f, 30);

    // konfiguracja napisu wyniku
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(mapWidth * tileSize + 10, 90);

    // konfiguracja napisu czasu
    timeText.setFont(font);
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(mapWidth * tileSize + 10, 120);

    // konfiguracja napisu "WYGRALES!"
    winText.setFont(font);
    winText.setString(L"GRATULACJE U¯YTKOWNIKU, WYGRA£EŒ");
    winText.setCharacterSize(50);
    winText.setFillColor(sf::Color::Yellow);

    // ustawienie srodkowania napisu "WYGRALES!"
    sf::FloatRect textBounds = winText.getLocalBounds();
    winText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    winText.setPosition((mapWidth * tileSize + sidebarWidth) / 2.0f, (mapHeight * tileSize) / 2.0f);

    // konfiguracja napisu o pozycji pacmana
    pacmanPositionText.setFont(font);
    pacmanPositionText.setCharacterSize(20);
    pacmanPositionText.setFillColor(sf::Color::White);
    pacmanPositionText.setPosition(mapWidth * tileSize + 10, 160);

    // konfiguracja napisu pauzy
    pauseText.setFont(font);
    pauseText.setString("Pauza");
    pauseText.setCharacterSize(50);
    pauseText.setFillColor(sf::Color::White);

    // ustawienie srodkowania napisu pauzy
    sf::FloatRect pauseBounds = pauseText.getLocalBounds();
    pauseText.setOrigin(pauseBounds.left + pauseBounds.width / 2.0f, pauseBounds.top + pauseBounds.height / 2.0f);
    pauseText.setPosition((mapWidth * tileSize + sidebarWidth) / 2.0f, (mapHeight * tileSize) / 2.0f);
    pauseText.setOutlineThickness(2);
    pauseText.setOutlineColor(sf::Color::Black);

}

// glowna petla gry
void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // zamkniecie okna
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // obsluga naciœniêcia klawiszy
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape && gameState == "PLAYING") {
                    isPaused = !isPaused;
                }

                // koniec gry po naciœniêciu ENTER w stanie wygranej lub przegranej
                if ((gameState == "WIN" || gameState == "GAME_OVER") && event.key.code == sf::Keyboard::Enter) {
                    window.close();
                }
            }
        }

        // logika gry, tylko gdy gra jest w trakcie
        if (!isPaused && gameState == "PLAYING") {
            pacman.handleInput(); // obsluga wejsciowych danych
            update(clock.restart().asSeconds()); // zaktualizowanie gry
        }
        else {
            clock.restart();
        }

        render(); // renderowanie widoku gry
    }
}

// funkcja do wczytania nazwy gracza
std::string getPlayerName(sf::RenderWindow& window, sf::Font& font) {
    sf::Text promptText(L"Podaj nazwe gracza: ", font, 24);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(50, 50);

    sf::Text inputText("", font, 24);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(50, 100);

    std::string playerName;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') {
                    // usun znak, jesli wcisnieto backspace
                    if (!playerName.empty()) {
                        playerName.pop_back();
                    }
                }
                else if (event.text.unicode == '\r') {
                    // potwierdz wprowadzenie przy enter
                    if (!playerName.empty()) {
                        return playerName;
                    }
                }
                else if (event.text.unicode < 128) {
                    playerName += static_cast<char>(event.text.unicode);
                }
            }
        }

        // aktualizacja tekstu wprowadzonego przez uzytkownika
        inputText.setString(playerName);

        // renderowanie promptu i inputu
        window.clear();
        window.draw(promptText);
        window.draw(inputText);
        window.display();
    }

    return playerName; // powrot, jesli okno zostalo zamkniete
}

// funkcja do zapisywania wynikow z nazwa gracza
void saveScore(const std::string& playerName, int score, const std::string& filename) {
    std::vector<std::string> scores;

    // odczytanie wynikow z pliku
    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            scores.push_back(line);
        }
        inputFile.close();
    }

    // dodanie nowego wyniku
    scores.push_back(playerName + ": " + std::to_string(score));

    // ograniczenie liczby zapisanych wynikow do 15
    if (scores.size() > 15) {
        scores.erase(scores.begin(), scores.end() - 15);
    }

    // zapisanie wynikow do pliku
    std::ofstream outputFile(filename, std::ios::trunc);
    if (outputFile.is_open()) {
        for (const std::string& s : scores) {
            outputFile << s << "\n";
        }
        outputFile.close();
    }
}

// aktualizacja stanu gry
void Game::update(float deltaTime) {
    if (gameState == "PLAYING") {
        gameTime += deltaTime;
        pacman.update(deltaTime, map.getDots(), map.getPacmanWalls(), score);

        // aktualizacja pozycji pacmana
        sf::Vector2f pacmanPosition = pacman.getPosition();
        std::ostringstream positionStream;
        positionStream << "Pos: " << static_cast<int>(pacmanPosition.x)
            << ", " << static_cast<int>(pacmanPosition.y);
        pacmanPositionText.setString(positionStream.str());

        // uwolnienie nastepnego duszka
        if (score >= releaseThreshold) {
            releaseNextGhost();
        }

        // aktualizacja duszkow
        if (!ghostInHouse[0]) pinky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[1]) inky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[2]) clyde.update(deltaTime, map.getGhostWalls());
        blinky.update(deltaTime, map.getGhostWalls());

        // aktualizacja tekstów
        scoreText.setString("Punkty: " + std::to_string(score));
        timeText.setString("Czas: " + formatTime(gameTime));

        // zakonczenie gry po zderzeniu pacmana z duszkiem
        if (pacman.getBounds().intersects(blinky.getBounds()) ||
            pacman.getBounds().intersects(pinky.getBounds()) ||
            pacman.getBounds().intersects(inky.getBounds()) ||
            pacman.getBounds().intersects(clyde.getBounds())) {
            gameState = "GAME_OVER";
        }
        // wygrana
        if (map.getDots().empty()) {
            gameState = "WIN";
        }
    }
}
// uwolnienie kolejnego duszka
void Game::releaseNextGhost() {
    if (ghostReleaseCounter < ghostInHouse.size()) {
        ghostInHouse[ghostReleaseCounter] = false;
        ghostReleaseCounter++;
        releaseThreshold += 250;
    }
}
// renderowanie gry
void Game::render() {
    window.clear();

    if (gameState == "PLAYING" || isPaused) {
        map.draw(window); // rysowanie mapy
        pacman.draw(window); // rysowanie pacmana
        blinky.draw(window); // rysowanie blinkiego
        pinky.draw(window); // rysowanie pinkiego
        inky.draw(window); // rysowanie inky
        clyde.draw(window); // rysowanie clyde'a
        window.draw(pacmanLabel); // rysowanie napisu "PACMAN"
        window.draw(scoreText); // rysowanie napisu wyniku
        window.draw(timeText); // rysowanie napisu czasu
        window.draw(pacmanPositionText); // rysowanie napisu o pozycji pacmana

        // nakladka pauzy
        if (isPaused) {
            sf::RectangleShape overlay(sf::Vector2f(mapWidth * tileSize + sidebarWidth, mapHeight * tileSize));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);
            window.draw(pauseText);
        }
    }
    else {
        // t³o po zakonczeniu gry
        sf::RectangleShape background(sf::Vector2f(mapWidth * tileSize + sidebarWidth, mapHeight * tileSize));
        background.setFillColor(sf::Color::Black);
        window.draw(background);

        // wyswietlanie napisu wygranej
        if (gameState == "WIN") {
            window.draw(winText);
        }
        else if (gameState == "GAME_OVER") {
            // wyswietlanie napisu przegranej
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("KONIEC GRY");
            gameOverText.setCharacterSize(50);
            gameOverText.setFillColor(sf::Color::Red);
            sf::FloatRect textBounds = gameOverText.getLocalBounds();
            gameOverText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            gameOverText.setPosition((mapWidth * tileSize + sidebarWidth) / 2.0f, (mapHeight * tileSize) / 2.0f);
            window.draw(gameOverText);
        }
    }

    window.display();
}
// funkcja do wczytywania wynikow z pliku
std::string loadScores(const std::string& filename) {
    std::ifstream file(filename); // otwarcie pliku z wynikami
    if (!file.is_open()) { // sprawdzenie czy plik zostal otwarty
        return "Brak dostepnych wynikow."; // w przypadku braku pliku zwracamy komunikat
    }

    std::stringstream scoresStream; // strumien tekstowy do przechowywania wynikow
    std::string line;
    int position = 1; // numeracja wynikow
    while (std::getline(file, line)) { // wczytanie kazdego wiersza z pliku
        scoresStream << position++ << ". " << line << "\n"; // dodanie wyniku do strumienia
    }

    return scoresStream.str(); // zwrocenie wynikow w formie tekstu
}


int main() {
    // utworzenie okna gry
    sf::RenderWindow window(sf::VideoMode(800, 600), "Menu gry Pacman");
    sf::Clock clock; // Stoper do obliczania czasu

    //utworzenie obiektu gwiazdozbioru i menu
    Gwiazdozbior gwiazdozbior(200, window);
    Menu menu(800, 600, &gwiazdozbior);

    // wczytanie czcionki
    sf::Font font;
    if (!font.loadFromFile("MadimiOne-Regular.ttf")) {
        std::cerr << "B³¹d ³adowania czcionki" << std::endl;
        return -1; // zakoncz program w przypadku bledu
    }

    bool showInstructions = false; // flaga do kontrolowania wyswietlania instrukcji
    bool showScores = false;       // flaga do kontrolowania wyœwietlania wynikow

    // tekst instrukcji
    sf::Text instructionsText;
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
        L"Tym samym klawiszem zamykamy grê w razie zwyciêstwa\n"
        L"lub pora¿ki.\n\n"
        L"Powodzenia!!!");
    instructionsText.setPosition(50, 50);
    // glowna petla programu
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // oblsuga klawisza pomocy
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::F1) {
                    showInstructions = !showInstructions; // przelaczenie wyswietlania instrukcji
                }
                else if (showScores && event.key.code == sf::Keyboard::Escape) {
                    // powrot do menu
                    showScores = false;
                }
            }
            // jesli instrukcje i wyniki nie sa wyswietlane
            if (!showInstructions && !showScores) {
                int action = menu.handleInput(event); // oblsuga zdarzenia wejsciowego
                if (action != -1) {
                    if (action == 0) {
                        // wyswietlanie okna do wprowadzenia nazwy gracza
                        std::string playerName = getPlayerName(window, font);

                        // rozpczecieie gry
                        Game game;
                        game.run();
                        // po zakonczeniu gry zapisanie wyniku
                        saveScore(playerName, game.getScore(), "scores.txt");
                    }
                    else if (action == 1) {
                        // pokazanie wyników
                        showScores = true;
                    }
                    else if (action == 2) {
                        window.close();
                    }
                }
            }
        }

        // aktualizacja stanu gry
        float deltaTime = clock.restart().asSeconds();
        gwiazdozbior.update(deltaTime);

        window.clear(sf::Color::Black); // czyszczenie okna
        gwiazdozbior.draw();            // rysowanie gwiazdozbioru

        if (showInstructions) {
            // rysowanie instrukcji
            window.draw(instructionsText);
        }
        else if (showScores) {
            // rysowanie wynikow
            sf::Text scoresText;
            scoresText.setFont(font);
            scoresText.setCharacterSize(24);
            scoresText.setFillColor(sf::Color::White);
            scoresText.setString("Ostatnie wyniki:\n\n" + loadScores("scores.txt"));
            scoresText.setPosition(50, 50);

            window.draw(scoresText);
        }
        else {
            menu.draw(window); // rysowanie menu
        }

        window.display(); // wyswietlanie zmian na ekranie
    }

    return 0; // zakonczenie programu
}





// Serdeczne podziêkowania dla twórców tekstur i czcionki.
// -----------------------------------------------------------------------------
// Tekstury u¿yte w tej grze s¹ darmowe i pochodz¹ z nastêpuj¹cego Ÿród³a:
// Autor: Pixelaholic
// Link: https://pixelaholic.itch.io/pac-man-game-art
// Licencja: Darmowe do u¿ytku osobistego i komercyjnego (zgodnie z warunkami na stronie).
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Czcionka u¿yta w tej grze ("MadimiOne-Regular.ttf") jest darmowa i pochodzi z:
// Autor: Taurai Valerie Mtake, Mirko Velimiroviæ
// Link: https://fonts.google.com/specimen/Madimi+One
// Licencja: SIL OFL 1.1.
// -----------------------------------------------------------------------------