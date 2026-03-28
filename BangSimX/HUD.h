#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Handles the Head-Up Display (UI) for the simulation.
 * Displays stats like FPS, time, and simulation speed.
 */
class HUD {
public:
    sf::Font font;           // The typeface used for the UI
    sf::Text text;           // The main stats text object
    sf::Text endText;        // The "THE END" message shown on collapse
    bool fontLoaded;         // Safety check to ensure font is ready
    bool isEnding = false;   // Flag to trigger the end-game UI state

    HUD();

    /**
     * @brief Refreshes the text string with current simulation data.
     */
    void update(int fps, float time, float speedMult, size_t particleCount, bool isPaused);

    /**
     * @brief Renders the HUD to the window, overriding the world view to stay fixed on screen.
     */
    void draw(sf::RenderWindow& window);
};