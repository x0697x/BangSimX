#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    sf::Font font;
    sf::Text text;
    bool fontLoaded;

    HUD();
    void update(int fps, float time, float speedMult, size_t particleCount);
    void draw(sf::RenderWindow& window);
};