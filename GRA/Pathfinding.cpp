#include "Pathfinding.h"
#include "MapData.h"
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <functional>

// specjalizacja szablonu hash dla sf::Vector2i, umozliwia uzycie tego typu jako klucza w std::unordered_map
namespace std {
    template <>
    struct hash<sf::Vector2i> {
        std::size_t operator()(const sf::Vector2i& v) const noexcept {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1); // unikalny hash dla kazdej pary (x, y)
        }
    };
}

// funkcja przeksztalcajaca wspolrzedne swiata na wspolrzedne siatki (indeksy kafelkow)
sf::Vector2i worldToGrid(const sf::Vector2f& pos) {
    return sf::Vector2i(
        static_cast<int>(pos.x / tileSize), // dzieli wspolrzedna x przez rozmiar kafelka
        static_cast<int>(pos.y / tileSize)  // dzieli wspolrzedna y przez rozmiar kafelka
    );
}

// funkcja przeksztalcajaca wspolrzedne siatki na wspolrzedne swiata
sf::Vector2f gridToWorld(const sf::Vector2i& gridPos) {
    return sf::Vector2f(
        gridPos.x * tileSize + tileSize / 2.f, // srodek kafelka na osi x
        gridPos.y * tileSize + tileSize / 2.f  // srodek kafelka na osi y
    );
}

// implementacja algorytmu BFS (breadth-first search) do wyznaczania najkrotszej sciezki
std::vector<sf::Vector2i> bfs(const sf::Vector2i& start, const sf::Vector2i& goal) {
    std::queue<sf::Vector2i> frontier;               // kolejka przechowujaca wierzcholki do odwiedzenia
    std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom; // mapa przechowujaca odwiedzone wierzcholki i ich rodzicow

    frontier.push(start); // dodanie punktu startowego do kolejki
    cameFrom[start] = start; // oznaczenie punktu startowego jako odwiedzonego

    // mozliwe kierunki ruchu: prawo, lewo, dol, gora
    std::vector<sf::Vector2i> directions = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    while (!frontier.empty()) {
        sf::Vector2i current = frontier.front(); // pobiera pierwszy element z kolejki
        frontier.pop();

        if (current == goal) break; // jesli osiagnieto cel, przerywa petle

        for (const auto& dir : directions) {
            sf::Vector2i next = current + dir; // oblicza nowa pozycje na siatce

            // sprawdza, czy nowa pozycja jest w granicach mapy i czy mozna ja odwiedzic
            if (next.x >= 0 && next.x < mapWidth &&
                next.y >= 0 && next.y < mapHeight &&
                (mapData[next.y][next.x] == 0 || mapData[next.y][next.x] == 2) && // 0: droga, 2: punkt mozliwy do odwiedzenia
                cameFrom.find(next) == cameFrom.end()) { // sprawdza, czy wierzcholek nie zostal jeszcze odwiedzony

                frontier.push(next); // dodaje nowy wierzcholek do kolejki
                cameFrom[next] = current; // zapisuje skad przyszlismy do tego wierzcholka
            }
        }
    }

    // sprawdza, czy algorytm znalazl droge do celu
    if (cameFrom.find(goal) == cameFrom.end()) {
        return {}; // zwraca pusta sciezke, jesli nie znaleziono drogi
    }

    // odtwarza sciezke od celu do startu
    std::vector<sf::Vector2i> path;
    for (sf::Vector2i current = goal; current != start; current = cameFrom[current]) {
        path.push_back(current);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end()); // odwraca sciezke, aby zaczynala sie od startu
    return path;
}
