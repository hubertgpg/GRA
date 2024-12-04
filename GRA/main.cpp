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
    if (!font.loadFromFile("ChainsawGeometric.ttf")) {
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
    winText.setString("Congratulations, you won!");
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
    pauseText.setString("PAUSE");
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

// zapisywanie wyniku do pliku
void saveScore(int score, const std::string& filename) {
    std::vector<int> scores;

    // odczytanie wynikow z pliku
    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        int currentScore;
        while (inputFile >> currentScore) {
            scores.push_back(currentScore);
        }
        inputFile.close();
    }

    // dodanie nowego wyniku
    scores.push_back(score);

    // ograniczenie liczby zapisanych wynikow do 15
    if (scores.size() > 15) {
        scores.clear();
        scores.push_back(score);
    }

    // zapisanie wynikow do pliku
    std::ofstream outputFile(filename, std::ios::trunc);
    if (outputFile.is_open()) {
        for (int s : scores) {
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

        // aktualizacja duszków
        if (!ghostInHouse[0]) pinky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[1]) inky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[2]) clyde.update(deltaTime, map.getGhostWalls());
        blinky.update(deltaTime, map.getGhostWalls());

        // aktualizacja tekstów
        scoreText.setString("Score: " + std::to_string(score));
        timeText.setString("Time: " + formatTime(gameTime));

        // zakoñczenie gry po zderzeniu pacmana z duszkiem
        if (pacman.getBounds().intersects(blinky.getBounds()) ||
            pacman.getBounds().intersects(pinky.getBounds()) ||
            pacman.getBounds().intersects(inky.getBounds()) ||
            pacman.getBounds().intersects(clyde.getBounds())) {
            gameState = "GAME_OVER";
            saveScore(score, "scores.txt");
        }

        // wygrana
        if (map.getDots().empty()) {
            gameState = "WIN";

            // zapis wyniku do pliku
            std::ofstream file("scores.txt", std::ios::app);
            if (file.is_open()) {
                file << score << "\n";
                file.close();
            }
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
        // t³o po zakoñczeniu gry
        sf::RectangleShape background(sf::Vector2f(mapWidth * tileSize + sidebarWidth, mapHeight * tileSize));
        background.setFillColor(sf::Color::Black);
        window.draw(background);

        // wyœwietlanie napisu wygranej
        if (gameState == "WIN") {
            window.draw(winText);
        }
        else if (gameState == "GAME_OVER") {
            // wyœwietlanie napisu przegranej
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setString("GAME OVER");
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
    if (!file.is_open()) { // sprawdzenie, czy plik zostal otwarty
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
    sf::Clock clock; // stoper do obliczania czasu

    // utworzenie obiektu gwiazdozbioru i menu
    Gwiazdozbior gwiazdozbior(200, window);
    Menu menu(800, 600, &gwiazdozbior);

    // wczytanie czcionki
    sf::Font font;
    if (!font.loadFromFile("ChainsawGeometric.ttf")) {
        std::cerr << "Blad ladowania czcionki" << std::endl;
        return -1; // zakoncz program w przypadku bledu
    }

    bool showInstructions = false; // flaga do kontrolowania wyswietlania instrukcji

    // tekst instrukcji
    sf::Text instructionsText;
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(18);
    instructionsText.setFillColor(sf::Color::White);
    instructionsText.setString("Witaj uzytkowniku w grze imitujacej gre PACMAN.\n\n"
        "Twoim zadaniem jako tytulowy PACMAN jest \n"
        "unikanie duszkow oraz zbieranie groszkow,\n"
        "ktore daja ci punkty (1 groszek - 10 punktow).\n\n"
        "W grze znajduja sie 4 duszki:\n"
        "- Blinky, ktory bedzie cie scigal\n"
        "- Pinky, ktory bedzie probowal zablokowac ci droge\n"
        "- Inky, ktory porusza sie calkowicie losowo\n"
        "- Clyde, ktory jest troche strachliwy\n\n"
        "Pacmanem steruje sie za pomoca strzalek.\n\n"
        "Gdy duszek cie zlapie gra sie konczy.\n"
        "W trakcie gry wciskajac klawisz ESC mozesz ja zatrzymac\n\n"
        "W Menu poruszamy sie strzalkami i wybieramy opcje\n"
        "wciskajac klawisz ENTER,\n\n"
        "tym samym klawiszem zamykamy gre w razie zwyciestwa\n"
        "lub porazki\n"
        "Powodzenia!!!");
    instructionsText.setPosition(50, 50);

    // glowna petla programu
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // zamkniecie okna
            }

            // obsluga nacisniecia klawiszy
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                showInstructions = !showInstructions; // przelaczanie wyswietlania instrukcji
            }

            // jesli instrukcje nie sa wyswietlane
            if (!showInstructions) {
                int action = menu.handleInput(event); // obsluga wejsciowego zdarzenia
                if (action != -1) {
                    if (action == 0) {
                        // rozpoczecie gry
                        Game game;
                        game.run();
                    }
                    else if (action == 1) {
                        // pokazanie wynikow
                        window.clear();
                        sf::Text scoresText;
                        scoresText.setFont(font);
                        scoresText.setCharacterSize(24);
                        scoresText.setFillColor(sf::Color::White);
                        scoresText.setString("Wyniki:\n\n" + loadScores("scores.txt"));
                        scoresText.setPosition(50, 50);

                        gwiazdozbior.draw(); // rysowanie gwiazdozbioru
                        window.draw(scoresText); // rysowanie wynikow
                        window.display();

                        // czekanie na nacisniecie klawisza przed powrotem do menu
                        while (true) {
                            sf::Event scoreEvent;
                            if (window.pollEvent(scoreEvent) && scoreEvent.type == sf::Event::KeyPressed) {
                                break; // po nacisnieciu dowolnego klawisza wychodzimy z okna wynikow
                            }
                        }
                    }
                    else if (action == 2) {
                        // zamkniecie okna
                        window.close();
                    }
                }
            }
        }

        // aktualizacja stanu gry
        float deltaTime = clock.restart().asSeconds();
        gwiazdozbior.update(deltaTime); // aktualizacja gwiazdozbioru

        window.clear(sf::Color::Black); // czyszczenie okna
        gwiazdozbior.draw(); // rysowanie gwiazdozbioru

        // rysowanie instrukcji, jeœli s¹ wlaczone
        if (showInstructions) {
            window.draw(instructionsText);
        }
        else {
            menu.draw(window); // rysowanie menu
        }

        window.display(); // wyswietlenie zmian na ekranie
    }

    return 0; // zakonczenie programu
}

// -----------------------------------------------------------------------------
// Tekstury u¿yte w tej grze s¹ darmowe i pochodz¹ z nastêpuj¹cego Ÿród³a:
// Autor: Pixelaholic
// Link: https://pixelaholic.itch.io/pac-man-game-art
// Licencja: Darmowe do u¿ytku osobistego i komercyjnego (zgodnie z warunkami na stronie).
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Czcionka u¿yta w tej grze ("ChainsawGeometric.ttf") jest darmowa i pochodzi z:
// Autor: Nick Curtis
// Link: https://www.1001fonts.com/chainsaw-geometric-font.html
// Licencja: Darmowe do u¿ytku osobistego i komercyjnego (zgodnie z warunkami na stronie).
// -----------------------------------------------------------------------------