#include "Gwiazdozbior.h"

// konstruktor klasy Gwiazdozbior, inicjalizuje gwiazdy w losowych pozycjach i z losowymi parametrami
Gwiazdozbior::Gwiazdozbior(int count, sf::RenderWindow& win)
    : count(count), window(win), gen(rd()) {

    // rozklady losowe dla parametrów gwiazd
    std::uniform_real_distribution<float> distR(5.0f, 15.0f); // promien gwiazd
    std::uniform_int_distribution<int> distX(0, win.getSize().x); // pozycja x
    std::uniform_int_distribution<int> distY(0, win.getSize().y); // pozycja y
    std::uniform_real_distribution<float> distV(-50.0f, 50.0f); // predkosc
    std::uniform_int_distribution<int> distRGB(128, 255); // kolory RGB

    // generowanie gwiazd
    for (int i = 0; i < count; ++i) {
        Star star;
        float radius = distR(gen); // losowy promien
        sf::Vector2f position(distX(gen), distY(gen)); // losowa pozycja
        sf::Color color(distRGB(gen), distRGB(gen), distRGB(gen), 255); // losowy kolor

        // tworzenie ksztaltu gwiazdy
        star.shape = createStar(radius, position, color);
        star.velocity = sf::Vector2f(distV(gen), distV(gen)); // losowa predkosc
        star.alpha = 255.0f; // pelna przezroczystosc

        stars.push_back(star);
    }
}

// metoda tworzy gwiazde o zadanych parametrach
sf::ConvexShape Gwiazdozbior::createStar(float radius, sf::Vector2f position, sf::Color color) {
    sf::ConvexShape star;
    const int points = 10; // liczba wierzcholkow gwiazdy
    star.setPointCount(points);

    float innerRadius = radius * 0.5f; // promien wewnetrzny gwiazdy
    for (int i = 0; i < points; ++i) {
        float angle = i * (2 * M_PI / points); // kat dla kazdego wierzcholka
        float r = (i % 2 == 0) ? radius : innerRadius; // naprzemiennie promien zewnetrzny i wewnetrzny
        star.setPoint(i, sf::Vector2f(r * std::cos(angle), r * std::sin(angle)));
    }

    star.setPosition(position); // ustawienie pozycji
    star.setFillColor(color); // ustawienie koloru
    return star;
}

// metoda aktualizujaca pozycje i parametry gwiazd
void Gwiazdozbior::update(float deltaTime) {
    // rozklady losowe dla regeneracji gwiazd
    std::uniform_real_distribution<float> distR(5.0f, 15.0f);
    std::uniform_int_distribution<int> distX(0, window.getSize().x);
    std::uniform_int_distribution<int> distY(0, window.getSize().y);
    std::uniform_real_distribution<float> distV(-50.0f, 50.0f);
    std::uniform_int_distribution<int> distRGB(128, 255);

    for (auto& star : stars) {
        sf::Vector2f pos = star.shape.getPosition();
        pos += star.velocity * deltaTime; // aktualizacja pozycji na podstawie predkosci

        // sprawdzanie, czy gwiazda wyszla poza ekran
        if (pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y) {
            // zmniejszanie przezroczystosci gwiazdy
            if (star.alpha > 0) {
                star.alpha -= 150.0f * deltaTime;
                if (star.alpha < 0) star.alpha = 0; // minimalna przezroczystosc
            }
            // regeneracja gwiazdy, gdy zniknie
            else {
                pos = sf::Vector2f(distX(gen), distY(gen)); // nowa pozycja
                float radius = distR(gen); // nowy promien
                sf::Color color(distRGB(gen), distRGB(gen), distRGB(gen), 255); // nowy kolor

                star.shape = createStar(radius, pos, color); // odtworzenie ksztaltu gwiazdy
                star.velocity = sf::Vector2f(distV(gen), distV(gen)); // nowa predkosc
                star.alpha = 255.0f; // pelna przezroczystosc
            }
        }

        // aktualizacja pozycji i przezroczystosci gwiazdy
        star.shape.setPosition(pos);
        sf::Color color = star.shape.getFillColor();
        color.a = static_cast<sf::Uint8>(star.alpha); // ustawienie przezroczystosci
        star.shape.setFillColor(color);
    }
}

// metoda rysujaca wszystkie gwiazdy na ekranie
void Gwiazdozbior::draw() {
    for (const auto& star : stars) {
        if (star.alpha > 0) { // rysowanie tylko widocznych gwiazd
            window.draw(star.shape);
        }
    }
}
