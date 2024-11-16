#include "Pathfinding.h"
#include "MapData.h"
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <functional>

namespace std {
    template <>
    struct hash<sf::Vector2i> {
        std::size_t operator()(const sf::Vector2i& v) const noexcept {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}

// Funkcja przekszta³caj¹ca wspó³rzêdne œwiata na wspó³rzêdne siatki
sf::Vector2i worldToGrid(const sf::Vector2f& pos) {
    return sf::Vector2i(
        static_cast<int>(pos.x / tileSize),
        static_cast<int>(pos.y / tileSize)
    );
}

// Nowa funkcja: gridToWorld, aby przekszta³ciæ wspó³rzêdne siatki na wspó³rzêdne œwiata
sf::Vector2f gridToWorld(const sf::Vector2i& gridPos) {
    return sf::Vector2f(
        gridPos.x * tileSize + tileSize / 2.f,
        gridPos.y * tileSize + tileSize / 2.f
    );
}

// Implementacja algorytmu BFS dla wyznaczenia najkrótszej œcie¿ki
std::vector<sf::Vector2i> bfs(const sf::Vector2i& start, const sf::Vector2i& goal) {
    std::queue<sf::Vector2i> frontier;
    std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom;

    frontier.push(start);
    cameFrom[start] = start;

    std::vector<sf::Vector2i> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.front();
        frontier.pop();

        if (current == goal) break;

        for (const auto& dir : directions) {
            sf::Vector2i next = current + dir;

            if (next.x >= 0 && next.x < mapWidth &&
                next.y >= 0 && next.y < mapHeight &&
                (mapData[next.y][next.x] == 0 || mapData[next.y][next.x] == 2) && // 0: droga, 2: punkt mo¿liwy do odwiedzenia
                cameFrom.find(next) == cameFrom.end()) {

                frontier.push(next);
                cameFrom[next] = current;
            }
        }
    }

    // Jeœli BFS nie znalaz³ drogi
    if (cameFrom.find(goal) == cameFrom.end()) {
        return {};
    }

    // Odtwarzanie œcie¿ki
    std::vector<sf::Vector2i> path;
    for (sf::Vector2i current = goal; current != start; current = cameFrom[current]) {
        path.push_back(current);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}
