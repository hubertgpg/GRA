#pragma once
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Gwiazdozbior {
private:
    struct Star {
        sf::ConvexShape shape;
        sf::Vector2f velocity;
        float alpha;
    };

    std::vector<Star> stars;
    int count;
    sf::RenderWindow& window;
    std::random_device rd;
    std::mt19937 gen;

    sf::ConvexShape createStar(float radius, sf::Vector2f position, sf::Color color);

public:
    Gwiazdozbior(int count, sf::RenderWindow& win);
    void draw();
    void update(float deltaTime);
};
