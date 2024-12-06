#include "Pacman.h"
#include "MapData.h"
#include <cmath>
#include <string>

// stala predkosc poruszania pacmana
const float pacmanSpeed = 200.f;

// konstruktor klasy pacman, inicjalizuje pola obiektu
Pacman::Pacman() : direction(0.f, 0.f), pendingDirection(0.f, 0.f), speed(pacmanSpeed), animationFrame(0), animationTimer(0.f) {
    loadTextures(); // wczytuje tekstury dla animacji pacmana
    sprite.setTexture(textures[1][0]); // ustawia poczatkowa teksture
    sprite.setScale(
        (tileSize * wallShrinkFactor - 2.f) / textures[0][0].getSize().x,
        (tileSize * wallShrinkFactor - 2.f) / textures[0][0].getSize().y
    );
    // ustawia poczatkowa pozycje pacmana
    sprite.setPosition(
        9 * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2,
        13 * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2
    );
}

// metoda wczytujaca tekstury animacji pacmana
void Pacman::loadTextures() {
    std::string directions[] = { "up", "down", "left", "right" };
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::string filename = "pacman_" + directions[i] + "_" + std::to_string(j) + ".png";
            textures[i][j].loadFromFile(filename); // wczytuje plik tekstury
        }
    }
}

// metoda centrujaca pacmana na srodku kafelka
void Pacman::centerOnTile() {
    sf::Vector2f currentPos = sprite.getPosition();
    float centeredX = std::round(currentPos.x / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().width / 2;
    float centeredY = std::round(currentPos.y / tileSize) * tileSize + tileSize / 2 - sprite.getGlobalBounds().height / 2;
    sprite.setPosition(centeredX, centeredY);
}

// metoda obslugujaca wejscie uzytkownika (klawiatura)
void Pacman::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        pendingDirection = { 0.f, -1.f };
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        pendingDirection = { 0.f, 1.f };
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        pendingDirection = { -1.f, 0.f };
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        pendingDirection = { 1.f, 0.f };
    }
}

// metoda aktualizujaca stan pacmana (ruch, kolizje, animacje)
void Pacman::update(float deltaTime, std::vector<sf::CircleShape>& dots, const std::vector<sf::RectangleShape>& walls, int& score) {
    // sprawdza i zmienia kierunek ruchu, jesli jest to mozliwe
    if (pendingDirection == -direction && canMoveInDirection(pendingDirection, walls)) {
        direction = pendingDirection;
    }
    else if (pendingDirection != direction && canMoveInDirection(pendingDirection, walls)) {
        direction = pendingDirection;
        centerOnTile();
    }

    // aktualizuje pozycje pacmana
    sf::Vector2f movement = direction * speed * deltaTime;
    sprite.move(movement);

    // sprawdza kolizje z scianami
    bool collided = false;
    for (const auto& wall : walls) {
        if (checkCollision(wall)) {
            sprite.move(-movement); // cofa ruch w przypadku kolizji
            collided = true;
            break;
        }
    }

    if (collided) {
        direction = { 0.f, 0.f }; // zatrzymuje pacmana po kolizji
    }

    // sprawdza kolizje z punktami (dots) i zwieksza wynik
    for (auto it = dots.begin(); it != dots.end();) {
        if (checkDotCollision(*it)) {
            it = dots.erase(it); // usuwa punkt po zebraniu
            score += 10;         // zwieksza wynik
        }
        else {
            ++it;
        }
    }

    updateAnimation(deltaTime); // aktualizuje animacje
}

// metoda aktualizujaca animacje pacmana
void Pacman::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer >= 0.1f) { // zmiana klatki co 0.1 sekundy
        animationTimer = 0.f;
        animationFrame = (animationFrame + 1) % 3; // zmiana klatki animacji
        sprite.setTexture(textures[getDirectionIndex()][animationFrame]);
    }
}

// metoda zwracajaca indeks kierunku dla animacji
int Pacman::getDirectionIndex() const {
    if (direction.x > 0) return 3; // prawo
    if (direction.x < 0) return 2; // lewo
    if (direction.y > 0) return 1; // dol
    return 0; // gora
}

// metoda sprawdzajaca, czy pacman moze poruszyc sie w podanym kierunku
bool Pacman::canMoveInDirection(const sf::Vector2f& dir, const std::vector<sf::RectangleShape>& walls) {
    sf::Vector2f nextPosition = sprite.getPosition() + dir * static_cast<float>(tileSize);
    sprite.setPosition(nextPosition);
    for (const auto& wall : walls) {
        if (checkCollision(wall)) {
            sprite.move(-dir * static_cast<float>(tileSize));
            return false;
        }
    }
    sprite.move(-dir * static_cast<float>(tileSize));
    return true;
}

// metoda sprawdzajaca kolizje pacmana z obiektem sciany
bool Pacman::checkCollision(const sf::RectangleShape& wall) {
    return sprite.getGlobalBounds().intersects(wall.getGlobalBounds());
}

// metoda sprawdzajaca kolizje pacmana z punktem
bool Pacman::checkDotCollision(sf::CircleShape& dot) {
    return sprite.getGlobalBounds().intersects(dot.getGlobalBounds());
}

// metoda zwracajaca aktualna pozycje pacmana
sf::Vector2f Pacman::getPosition() const {
    return sprite.getPosition();
}

// metoda rysujaca pacmana w oknie
void Pacman::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

// metoda zwracajaca aktualny kierunek pacmana
sf::Vector2f Pacman::getDirection() const {
    return direction;
}

// metoda zwracajaca granice (bounding box) pacmana
sf::FloatRect Pacman::getBounds() const {
    return sprite.getGlobalBounds();
}
