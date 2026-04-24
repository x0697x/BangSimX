#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
    sf::Font font;
    sf::Text text;
    sf::Text endText;
    bool fontLoaded;
    bool isEnding = false;

    HUD();

    void update(int fps, float time, float speedMult, size_t particleCount, bool isPaused);

    void draw(sf::RenderWindow& window);
};