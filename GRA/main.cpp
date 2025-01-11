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
#include <algorithm>

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
std::wstring getPlayerName(sf::RenderWindow& window, sf::Font& font) {
    sf::Text promptText(L"Podaj nazwê gracza: ", font, 24);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(50, 50);

    sf::Text inputText("", font, 24);
    inputText.setFillColor(sf::Color::White);
    inputText.setPosition(50, 100);

    std::wstring playerName;
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

struct Score {
    std::wstring playerName;
    int points;

    // serializacja wyniku do pliku
    void save(std::ofstream& outFile) const {
        size_t nameLength = playerName.size();
        outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        outFile.write(reinterpret_cast<const char*>(playerName.data()), nameLength * sizeof(wchar_t));
        outFile.write(reinterpret_cast<const char*>(&points), sizeof(points));
    }

    // deserializacja wyniku z pliku
    void load(std::ifstream& inFile) {
        size_t nameLength;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        playerName.resize(nameLength);
        inFile.read(reinterpret_cast<char*>(playerName.data()), nameLength * sizeof(wchar_t));
        inFile.read(reinterpret_cast<char*>(&points), sizeof(points));
    }
};

// funkcja do zapisywania wynikow z nazwa gracza
void saveScore(const std::wstring& playerName, int score, const std::string& filename) {
    std::vector<Score> scores;

    // odczytaj istniejace wyniki
    std::ifstream inputFile(filename, std::ios::binary);
    if (inputFile.is_open()) {
        while (!inputFile.eof()) {
            Score s;
            s.load(inputFile);
            if (inputFile) scores.push_back(s);
        }
        inputFile.close();
    }

    // dodaj nowy wynik
    scores.push_back({ playerName, score });

    // zapisz wszystkie wyniki do pliku
    std::ofstream outputFile(filename, std::ios::binary | std::ios::trunc);
    if (outputFile.is_open()) {
        for (const Score& s : scores) {
            s.save(outputFile);
        }
        outputFile.close();
    }
}

// funkcja do wczytywania wynikow z pliku
std::wstring loadScores(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return L"Brak dostêpnych wyników.";
    }

    // wczytaj wyniki
    std::vector<Score> scores;
    while (!file.eof()) {
        Score s;
        s.load(file);
        if (file) scores.push_back(s);
    }
    file.close();

    // posortuj wyniki malejaco po punktach
    std::sort(scores.begin(), scores.end(), [](const Score& a, const Score& b) {
        return a.points > b.points;
        });

    // przygotuj tekst do wyswietlenia (tylko 15 najlepszych wyników)
    std::wstringstream scoresStream;
    int position = 1;
    for (const auto& s : scores) {
        if (position > 15) break;
        scoresStream << position++ << L". " << s.playerName << L": " << s.points << L"\n";
    }

    return scoresStream.str();
}

class GameApp {
public:
    // konstruktor, ustawia okno gry, gwiazdozbior i menu, laduje czcionke
    GameApp()
        : window(sf::VideoMode(800, 600), "MENU"),
        gwiazdozbior(200, window),
        menu(800, 600, &gwiazdozbior),
        showScores(false) {
        if (!font.loadFromFile("MadimiOne-Regular.ttf")) {
            std::cerr << "Blad" << std::endl;
            throw std::runtime_error("Nie mozna wczytac czcionki");
        }
    }

    // uruchamia petle glowna gry
    void run() {
        while (window.isOpen()) {
            handleEvents(); // obs³uga zdarzeñ
            update();       // aktualizacja stanu gry
            render();       // rysowanie elementow na ekranie
        }
    }

private:
    sf::RenderWindow window; // okno gry
    sf::Clock clock;         // zegar do mierzenia czasu
    Gwiazdozbior gwiazdozbior; // obiekt gwiazdozbioru
    Menu menu;               // obiekt menu
    sf::Font font;           // czcionka do tekstu
    bool showScores;         // flaga do wyswietlania wynikow

    // metoda obslugujaca zdarzenia uzytkownika
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close(); // zamyka okno gry
            }

            if (!showScores) {
                int action = menu.handleInput(event); // sprawdza akcje w menu
                if (action != -1) {
                    if (action == 0) {
                        // zaczyna gre
                        std::wstring playerName = getPlayerName(window, font); // pobiera nazwe gracza
                        Game game; // tworzy gre
                        game.run(); // uruchamia gre
                        saveScore(playerName, game.getScore(), "scores.dat"); // zapisuje wynik
                    }
                    else if (action == 1) {
                        showScores = true; // wlacza widok wynikow
                    }
                    else if (action == 2) {
                        window.close(); // zamyka gre
                    }
                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                showScores = false; // wylacza widok wynikow
            }
        }
    }

    // metoda aktualizujaca stan gry
    void update() {
        float deltaTime = clock.restart().asSeconds(); // czas od ostatniej klatki
        gwiazdozbior.update(deltaTime); // aktualizuje gwiazdozbior
    }

    // metoda rysujaca elementy gry na ekranie
    void render() {
        window.clear(sf::Color::Black); // czysci ekran na czarno
        gwiazdozbior.draw(); // rysuje gwiazdozbior

        if (showScores) {
            // rysuje ekran wynikow
            sf::Text scoresText;
            scoresText.setFont(font);
            scoresText.setCharacterSize(24);
            scoresText.setFillColor(sf::Color::White);
            scoresText.setString(L"Najlepsi z najlepszych:\n\n" + loadScores("scores.dat"));
            scoresText.setPosition(50, 50);

            window.draw(scoresText);
        }
        else {
            // rysuje menu
            menu.draw(window);
        }

        window.display(); // wyswietla wszystko na ekranie
    }
};

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
        // tlo po zakonczeniu gry
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

int main() {
    GameApp app;
    app.run();

    return 0;
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