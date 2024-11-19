#include "Gwiazdozbior.h"

Gwiazdozbior::Gwiazdozbior(int count, sf::RenderWindow& win)
    : count(count), window(win), gen(rd()) {

    std::uniform_real_distribution<float> distR(5.0f, 15.0f);
    std::uniform_int_distribution<int> distX(0, win.getSize().x);
    std::uniform_int_distribution<int> distY(0, win.getSize().y);
    std::uniform_real_distribution<float> distV(-50.0f, 50.0f);
    std::uniform_int_distribution<int> distRGB(128, 255);

    for (int i = 0; i < count; ++i) {
        Star star;
        float radius = distR(gen);
        sf::Vector2f position(distX(gen), distY(gen));
        sf::Color color(distRGB(gen), distRGB(gen), distRGB(gen), 255);

        star.shape = createStar(radius, position, color);
        star.velocity = sf::Vector2f(distV(gen), distV(gen));
        star.alpha = 255.0f;

        stars.push_back(star);
    }
}

sf::ConvexShape Gwiazdozbior::createStar(float radius, sf::Vector2f position, sf::Color color) {
    sf::ConvexShape star;
    const int points = 10;
    star.setPointCount(points);

    float innerRadius = radius * 0.5f;
    for (int i = 0; i < points; ++i) {
        float angle = i * (2 * M_PI / points);
        float r = (i % 2 == 0) ? radius : innerRadius;
        star.setPoint(i, sf::Vector2f(r * std::cos(angle), r * std::sin(angle)));
    }

    star.setPosition(position);
    star.setFillColor(color);
    return star;
}

void Gwiazdozbior::update(float deltaTime) {
    std::uniform_real_distribution<float> distR(5.0f, 15.0f);
    std::uniform_int_distribution<int> distX(0, window.getSize().x);
    std::uniform_int_distribution<int> distY(0, window.getSize().y);
    std::uniform_real_distribution<float> distV(-50.0f, 50.0f);
    std::uniform_int_distribution<int> distRGB(128, 255);

    for (auto& star : stars) {
        sf::Vector2f pos = star.shape.getPosition();
        pos += star.velocity * deltaTime;

        if (pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y) {
            if (star.alpha > 0) {
                star.alpha -= 150.0f * deltaTime;
                if (star.alpha < 0) star.alpha = 0;
            }
            else {
                pos = sf::Vector2f(distX(gen), distY(gen));
                float radius = distR(gen);
                sf::Color color(distRGB(gen), distRGB(gen), distRGB(gen), 255);

                star.shape = createStar(radius, pos, color);
                star.velocity = sf::Vector2f(distV(gen), distV(gen));
                star.alpha = 255.0f;
            }
        }

        star.shape.setPosition(pos);
        sf::Color color = star.shape.getFillColor();
        color.a = static_cast<sf::Uint8>(star.alpha);
        star.shape.setFillColor(color);
    }
}

void Gwiazdozbior::draw() {
    for (const auto& star : stars) {
        if (star.alpha > 0) {
            window.draw(star.shape);
        }
    }
}
