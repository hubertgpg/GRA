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



std::string formatTime(float timeInSeconds) {
    int minutes = static_cast<int>(timeInSeconds) / 60;
    int seconds = static_cast<int>(timeInSeconds) % 60;

    
    std::ostringstream oss;
    oss << (minutes < 10 ? "0" : "") << minutes << ":"
        << (seconds < 10 ? "0" : "") << seconds;

    return oss.str();
}

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
    float gameTime;
    int releaseThreshold;
    int ghostReleaseCounter;
    std::vector<bool> ghostInHouse;
    std::string gameState;
    bool isPaused;
    sf::Font font;
    sf::Text pacmanLabel;
    sf::Text scoreText;
    sf::Text timeText;
    sf::Text winText;
    sf::Text pacmanPositionText;
    sf::Text pauseText;

    void update(float deltaTime);
    void render();
    void releaseNextGhost();
};

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
    isPaused(false) 
{

    if (!font.loadFromFile("ChainsawGeometric.ttf")) {
        std::cerr << "Error loading font" << std::endl;
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
    scoreText.setPosition(mapWidth * tileSize + 10, 90);

    
    timeText.setFont(font);
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(mapWidth * tileSize + 10, 120); 

    winText.setFont(font);
    winText.setString("Congratulations, you won!");
    winText.setCharacterSize(50);
    winText.setFillColor(sf::Color::Yellow);

    sf::FloatRect textBounds = winText.getLocalBounds();
    winText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    winText.setPosition((mapWidth * tileSize + sidebarWidth) / 2.0f, (mapHeight * tileSize) / 2.0f);

    pacmanPositionText.setFont(font);
    pacmanPositionText.setCharacterSize(20);
    pacmanPositionText.setFillColor(sf::Color::White);
    pacmanPositionText.setPosition(mapWidth * tileSize + 10, 160); 

    pauseText.setFont(font);
    pauseText.setString("PAUSE");
    pauseText.setCharacterSize(50);
    pauseText.setFillColor(sf::Color::White);
    sf::FloatRect pauseBounds = pauseText.getLocalBounds();
    pauseText.setOrigin(pauseBounds.left + pauseBounds.width / 2.0f, pauseBounds.top + pauseBounds.height / 2.0f);
    pauseText.setPosition((mapWidth * tileSize + sidebarWidth) / 2.0f, (mapHeight * tileSize) / 2.0f);
    pauseText.setOutlineThickness(2);
    pauseText.setOutlineColor(sf::Color::Black);

}



void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape && gameState == "PLAYING") {
                    isPaused = !isPaused; 
                }

                if ((gameState == "WIN" || gameState == "GAME_OVER") && event.key.code == sf::Keyboard::Enter) {
                    window.close(); 
                }
            }
        }

        if (!isPaused && gameState == "PLAYING") {
            pacman.handleInput();
            update(clock.restart().asSeconds());
        }
        else {
            
            clock.restart();
        }

        render();
    }
}

void saveScore(int score, const std::string& filename) {
    std::vector<int> scores;

    
    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        int currentScore;
        while (inputFile >> currentScore) {
            scores.push_back(currentScore);
        }
        inputFile.close();
    }

    
    scores.push_back(score);

    
    if (scores.size() > 15) {
        scores.clear(); 
        scores.push_back(score); 
    }

    
    std::ofstream outputFile(filename, std::ios::trunc); 
    if (outputFile.is_open()) {
        for (int s : scores) {
            outputFile << s << "\n";
        }
        outputFile.close();
    }
}

void Game::update(float deltaTime) {
    if (gameState == "PLAYING") {
        gameTime += deltaTime;
        pacman.update(deltaTime, map.getDots(), map.getPacmanWalls(), score);

        sf::Vector2f pacmanPosition = pacman.getPosition();
        std::ostringstream positionStream;
        positionStream << "Pos: " << static_cast<int>(pacmanPosition.x)
            << ", " << static_cast<int>(pacmanPosition.y);
        pacmanPositionText.setString(positionStream.str());

        if (score >= releaseThreshold) {
            releaseNextGhost();
        }

        if (!ghostInHouse[0]) pinky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[1]) inky.update(deltaTime, map.getGhostWalls());
        if (!ghostInHouse[2]) clyde.update(deltaTime, map.getGhostWalls());
        blinky.update(deltaTime, map.getGhostWalls());

        scoreText.setString("Score: " + std::to_string(score));
        timeText.setString("Time: " + formatTime(gameTime));

        if (pacman.getBounds().intersects(blinky.getBounds()) ||
            pacman.getBounds().intersects(pinky.getBounds()) ||
            pacman.getBounds().intersects(inky.getBounds()) ||
            pacman.getBounds().intersects(clyde.getBounds())) {
            gameState = "GAME_OVER";
            saveScore(score, "scores.txt");
        }

        if (map.getDots().empty()) {
            gameState = "WIN";

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

    if (gameState == "PLAYING" || isPaused) {
        
        map.draw(window);
        pacman.draw(window);
        blinky.draw(window);
        pinky.draw(window);
        inky.draw(window);
        clyde.draw(window);
        window.draw(pacmanLabel);
        window.draw(scoreText);
        window.draw(timeText);
        window.draw(pacmanPositionText);

        if (isPaused) {
            
            sf::RectangleShape overlay(sf::Vector2f(mapWidth * tileSize + sidebarWidth, mapHeight * tileSize));
            overlay.setFillColor(sf::Color(0, 0, 0, 150)); 
            window.draw(overlay);
            window.draw(pauseText);
        }
    }
    else {
        
        sf::RectangleShape background(sf::Vector2f(mapWidth * tileSize + sidebarWidth, mapHeight * tileSize));
        background.setFillColor(sf::Color::Black);
        window.draw(background);

        if (gameState == "WIN") {
            
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
    int position = 1; 
    while (std::getline(file, line)) {
        scoresStream << position++ << ". " << line << "\n";
    }

    return scoresStream.str();
}



int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Pacman Menu");
    sf::Clock clock;

    Gwiazdozbior gwiazdozbior(200, window); 
    Menu menu(800, 600, &gwiazdozbior);     

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
                    
                    Game game;
                    game.run();
                }
                else if (action == 1) {
                    
                    window.clear();
                    sf::Text scoresText;
                    scoresText.setFont(font);
                    scoresText.setCharacterSize(24);
                    scoresText.setFillColor(sf::Color::White);
                    scoresText.setString("Scores:\n\n" + loadScores("scores.txt"));
                    scoresText.setPosition(50, 50);

                    gwiazdozbior.draw(); 
                    window.draw(scoresText);
                    window.display();

                    
                    while (true) {
                        sf::Event scoreEvent;
                        if (window.pollEvent(scoreEvent) && scoreEvent.type == sf::Event::KeyPressed) {
                            break;
                        }
                    }
                }
                else if (action == 2) {
                   
                    window.close();
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        gwiazdozbior.update(deltaTime); 

        window.clear(sf::Color::Black);
        gwiazdozbior.draw(); 
        menu.draw(window);
        window.display();
    }

    return 0;
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