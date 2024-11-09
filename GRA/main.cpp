#include <SFML/Graphics.hpp>
#include "MapData.h" // Import mapy
#include "Pacman.h" // Import klasy Pacman
#include "Blinky.h" // Import klasy Blinky
#include <vector>

const float pacmanSpeed = 110.f;
const int sidebarWidth = 200;

class Map {
public:
    Map();
    void draw(sf::RenderWindow& window);

    const std::vector<sf::RectangleShape>& getWalls() const { return walls; }
    std::vector<sf::CircleShape>& getDots() { return dots; }

private:
    std::vector<sf::RectangleShape> walls;
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
                walls.push_back(wall);
            }
            else if (!(y >= 6 && y <= 8 && x >= 8 && x <= 10) && !(x == 9 && y == 13)) {
                sf::CircleShape dot(5.f);
                dot.setFillColor(sf::Color::White);
                dot.setPosition(x * tileSize + tileSize / 2 - dot.getRadius(), y * tileSize + tileSize / 2 - dot.getRadius());
                dots.push_back(dot);
            }
        }
    }

    ghostRoomDoor.setSize(sf::Vector2f(tileSize * wallShrinkFactor, tileSize * wallShrinkFactor / 3));
    ghostRoomDoor.setFillColor(sf::Color::White);
    float offsetX = (tileSize - tileSize * wallShrinkFactor) / 2;
    float offsetY = (tileSize - tileSize * wallShrinkFactor / 3) / 2;
    ghostRoomDoor.setPosition(9 * tileSize + offsetX, 6 * tileSize + offsetY);
    walls.push_back(ghostRoomDoor);
}

void Map::draw(sf::RenderWindow& window) {
    for (const auto& wall : walls) {
        window.draw(wall);
    }
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
    Blinky blinky; // Dodajemy Blinkiego
    int score;
    sf::Font font;
    sf::Text scoreText;

    void update(float deltaTime);
    void render();
};

Game::Game()
    : window(sf::VideoMode(mapWidth* tileSize + sidebarWidth, mapHeight* tileSize), "Pacman"),
    score(0), blinky(pacman) { // Inicjalizacja Blinkiego z referencj¹ do pacmana

    font.loadFromFile("ChainsawGeometric.ttf");
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(mapWidth * tileSize + 10, 10);
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

        pacman.handleInput();
        update(clock.restart().asSeconds());
        render();
    }
}

void Game::update(float deltaTime) {
    pacman.update(deltaTime, map.getDots(), map.getWalls(), score);
    blinky.update(deltaTime); // Aktualizacja pozycji Blinkiego
    scoreText.setString("Score: " + std::to_string(score));
}

void Game::render() {
    window.clear();
    map.draw(window);
    pacman.draw(window);
    blinky.draw(window); // Rysowanie Blinkiego
    window.draw(scoreText);
    window.display();
}

int main() {
    Game game;
    game.run();
    return 0;
}
