#pragma once
#ifndef VECTOR2HASH_H
#define VECTOR2HASH_H

#include <SFML/System.hpp>
#include <functional>


namespace std {
    template <>
    struct hash<sf::Vector2i> {
        size_t operator()(const sf::Vector2i& v) const noexcept {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1);
        }
    };
}

#endif
