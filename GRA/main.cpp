#include <SFML/Graphics.hpp>
#include "MapData.h"
#include "Pacman.h"
#include "Blinky.h"
#include "Pinky.h"
#include "Inky.h"
#include "Clyde.h"
#include <vector>
#include "menu.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>


const float pacmanSpeed = 110.f;
const int sidebarWidth = 200;

class Map {
public:
    Map();
    void draw(sf::RenderWindow& window);

    const std::vector<sf::RectangleShape>& getPacmanWalls() const { return pacmanWalls; }
    const std::vector<sf::RectangleShape>& getGhostWalls() const { return ghostWalls; }
    std::vector<sf::CircleShape>& getDots() { return dots; }
    const sf::RectangleShape& getGhostRoomDoor() const { return ghostRoomDoor; }

private:
    std::vector<sf::RectangleShape> pacmanWalls;
    std::vector<sf::RectangleShape> ghostWalls;
    std::vector<sf::CircleShape> dots;
    sf::RectangleShape ghostRoomDoor;
};

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

void Map::draw(sf::RenderWindow& window) {
    for (const auto& wall : pacmanWalls) {
        window.draw(wall);
    }
    window.draw(ghostRoomDoor);
    for (const auto& dot : dots) {
        window.draw(dot);
    }
}

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    Map map;
    Pacman pacman;
    Blinky blinky;
    Pinky pinky;
    Inky inky;
    Clyde clyde;
    int score;
    int releaseThreshold;
    int ghostReleaseCounter;
    std::vector<bool> ghostInHouse;
    std::string gameState;
    sf::Font font;
    sf::Text pacmanLabel;
    sf::Text scoreText;
    sf::Text winText;

    void update(float deltaTime);
    void render();
    void releaseNextGhost();
};

Game::Game()
    : window(sf::VideoMode(mapWidth* tileSize + sidebarWidth, mapHeight* tileSize), "Pacman"),
    score(0),
    releaseThreshold(250),
    ghostReleaseCounter(0),
    ghostInHouse{ true, true, true },
    blinky(pacman, map.getGhostRoomDoor()),
    pinky(pacman, map.getGhostRoomDoor()),
    inky(pacman, blinky, map.getGhostRoomDoor()),
    clyde(pacman, map.getGhostRoomDoor()),
    gameState("PLAYING") {

    if (!font.loadFromFile("ChainsawGeometric.ttf")) {
   
    }

    pacmanLabel.setFont(font);
    pacmanLabel.setString("PACMAN");
    pacmanLabel.setCharacterSize(30);
    pacmanLabel.setFillColor(sf::Color::Yellow);
    sf::FloatRect pacmanBounds = pacmanLabel.getLocalBounds();
    pacmanLabel.setOrigin(pacmanBounds.left + pacmanBounds.width / 2.0f, pacmanBounds.top + pacmanBounds.height / 2.0f);
    pacmanLabel.setPosition(mapWidth * tileSize + sidebarWidth / 2.0f, 30);

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(mapWidth * tileSize + 10, 70);

    winText.setFont(font);
    winText.setString("Congratulations, you won!");
    winText.setCharacterSize(50);
    winText.setFillColor(sf::Color::Yellow);

    sf::FloatRect textBounds = winText.getLocalBounds();
    winText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    winText.setPosition((mapWidth * tileSize + sidebarWidth) / 2.0f, (mapHeight * tileSize) / 2.0f);
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (gameState == "PLAYING") {
            pacman.handleInput();
        }
        update(clock.restart().asSeconds());
        render();
    }
}

void Game::update(float deltaTime) {
    if (gameState == "PLAYING") {
        pacman.update(deltaTime, map.getDots(), map.getPacmanWalls(), score);

        if (score >= releaseThreshold) {
            releaseNextGhost();
        }

        if (!ghostInHouse[0]) pinky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[1]) inky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[2]) clyde.update(deltaTime, map.getGhostWalls());
        blinky.update(deltaTime, map.getGhostWalls());

        scoreText.setString("Score: " + std::to_string(score));

        if (pacman.getBounds().intersects(blinky.getBounds()) ||
            pacman.getBounds().intersects(pinky.getBounds()) ||
            pacman.getBounds().intersects(inky.getBounds()) ||
            pacman.getBounds().intersects(clyde.getBounds())) {
            gameState = "GAME_OVER";

            // Zapisz wynik do pliku
            std::ofstream file("scores.txt", std::ios::app);
            if (file.is_open()) {
                file << score << "\n";
                file.close();
            }
        }

        if (map.getDots().empty()) {
            gameState = "WIN";

            // Zapisz wynik do pliku
            std::ofstream file("scores.txt", std::ios::app);
            if (file.is_open()) {
                file << score << "\n";
                file.close();
            }
        }
    }
}


void Game::releaseNextGhost() {
    if (ghostReleaseCounter < ghostInHouse.size()) {
        ghostInHouse[ghostReleaseCounter] = false;
        ghostReleaseCounter++;
        releaseThreshold += 250;
    }
}

void Game::render() {
    window.clear();

    if (gameState == "PLAYING") {
        map.draw(window);
        pacman.draw(window);
        blinky.draw(window);
        pinky.draw(window);
        inky.draw(window);
        clyde.draw(window);
        window.draw(pacmanLabel);
        window.draw(scoreText);
    }
    else if (gameState == "WIN") {
        window.draw(winText);
    }
    else if (gameState == "GAME_OVER") {
        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setString("Game Over!");
        gameOverText.setCharacterSize(50);
        gameOverText.setFillColor(sf::Color::Red);
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        gameOverText.setPosition((mapWidth * tileSize + sidebarWidth) / 2.0f, (mapHeight * tileSize) / 2.0f);
        window.draw(gameOverText);
    }

    window.display();
}




std::string loadScores(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "No scores available.";
    }

    std::stringstream scoresStream;
    std::string line;
    int position = 1; // Pozycja wyniku na liœcie
    while (std::getline(file, line)) {
        scoresStream << position++ << ". " << line << "\n";
    }

    return scoresStream.str();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pacman Menu");
    Menu menu(800, 600);

    sf::Font font;
    if (!font.loadFromFile("ChainsawGeometric.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return -1;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            int action = menu.handleInput(event);
            if (action != -1) {
                if (action == 0) {
                    // Start the game
                    Game game;
                    game.run();
                }
                else if (action == 1) {
                    // Display scores
                    window.clear();
                    sf::Text scoresText;
                    scoresText.setFont(font);
                    scoresText.setCharacterSize(24);
                    scoresText.setFillColor(sf::Color::White);
                    scoresText.setString("Scores:\n\n" + loadScores("scores.txt"));
                    scoresText.setPosition(50, 50);

                    window.draw(scoresText);
                    window.display();

                    // Oczekuj na dowolne zdarzenie zamykaj¹ce podgl¹d wyników
                    while (true) {
                        sf::Event scoreEvent;
                        if (window.pollEvent(scoreEvent) && scoreEvent.type == sf::Event::KeyPressed) {
                            break;
                        }
                    }
                }
                else if (action == 2) {
                    // Exit
                    window.close();
                }
            }
        }

        window.clear();
        menu.draw(window);
        window.display();
    }

    return 0;
}
