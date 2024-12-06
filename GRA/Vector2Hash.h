#pragma once
#ifndef VECTOR2HASH_H
#define VECTOR2HASH_H

#include <SFML/System.hpp>
#include <functional>

// rozszerzenie przestrzeni nazw std o specjalizacje szablonu hash dla sf::Vector2i
// hashowanie polega na przeksztalceniu danych (np. liczb, wektorow) na unikalna wartosc liczbow¹ (hash)
namespace std {
    template <>
    struct hash<sf::Vector2i> {
        // operator() oblicza wartosc hash dla obiektu sf::Vector2i
        size_t operator()(const sf::Vector2i& v) const noexcept {
            // oblicza hash dla wspolrzednej x
            size_t hashX = std::hash<int>()(v.x);

            // oblicza hash dla wspolrzednej y i przesuwa wynik o jeden bit w lewo
            // przesuniecie bitowe (<< 1) zmniejsza prawdopodobienstwo kolizji (tzn. sytuacji, gdy dwa rozne wektory maja ten sam hash)
            size_t hashY = std::hash<int>()(v.y) << 1;

            // laczy oba hashe przy uzyciu operacji XOR (^)
            // XOR sprawia, ze hash wynikowy jest bardziej unikalny, uwzgledniajac wartosci x i y
            return hashX ^ hashY;
        }
    };
}

#endif
