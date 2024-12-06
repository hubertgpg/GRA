#pragma once

#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <SFML/Graphics.hpp>
#include <vector>

// rozmiar pojedynczego kafelka w pikselach
const int tileSize = 40;

// wymiary mapy w kafelkach
const int mapWidth = 19;
const int mapHeight = 15;

// wspolczynnik zmniejszenia wymiarow sciany
const float wallShrinkFactor = 0.95f;

// dane mapy - 2D wektor reprezentujacy uklad planszy
// 0 - puste pole, 1 - sciana, 2 - drzwi do pokoju duszkow
const std::vector<std::vector<int>> mapData = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1},
    {1,0,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,0,1},
    {1,0,1,0,0,0,0,1,0,1,0,1,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,2,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
    {1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1},
    {1,1,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,1},
    {1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

#endif
