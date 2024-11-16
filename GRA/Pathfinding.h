#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

sf::Vector2i worldToGrid(const sf::Vector2f& pos);
sf::Vector2f gridToWorld(const sf::Vector2i& gridPos); // Nowa funkcja
std::vector<sf::Vector2i> bfs(const sf::Vector2i& start, const sf::Vector2i& goal);
