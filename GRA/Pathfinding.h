#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

// funkcja przeksztalcajaca wspolrzedne swiata na wspolrzedne siatki (indeksy kafelkow)
sf::Vector2i worldToGrid(const sf::Vector2f& pos);

// funkcja przeksztalcajaca wspolrzedne siatki na wspolrzedne swiata (srodek kafelka)
sf::Vector2f gridToWorld(const sf::Vector2i& gridPos);

// implementacja algorytmu bfs (breadth-first search) do wyznaczania najkrotszej sciezki
std::vector<sf::Vector2i> bfs(const sf::Vector2i& start, const sf::Vector2i& goal);
