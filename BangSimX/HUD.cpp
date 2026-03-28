#include "HUD.h"
#include <iostream>

HUD::HUD() : fontLoaded(false), text(font) {
    // Attempting to load directly from the root project folder
    if (font.openFromFile("type.ttf")) {
        fontLoaded = true;
        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Cyan);
        text.setPosition({ 20.f, 20.f });
        std::cout << "HUD: Font loaded successfully from project root." << std::endl;
    }
    else {
        // If this fails, the update() and draw() functions will skip silently instead of crashing
        std::cerr << "HUD Error: Could not find type.ttf in the project folder!" << std::endl;
    }
}

void HUD::update(int fps, float time, float speedMult, size_t particleCount) {
    if (!fontLoaded) return; // Prevent crashes if font is missing

    std::string speedStr = (speedMult < 0.5f) ? "Slow" : (speedMult > 1.5f) ? "Fast" : "Normal";

    text.setString(
        "FPS: " + std::to_string(fps) +
        "\nTime: " + std::to_string((int)time) + "s" +
        "\nSpeed: " + speedStr +
        "\nParticles: " + std::to_string(particleCount) +
        "\nPress 'R' to Reset"
    );
}

void HUD::draw(sf::RenderWindow& window) {
    if (!fontLoaded) return; // Skip drawing if font failed to load

    sf::View oldView = window.getView();
    // Overlay view for the UI
    window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, sf::Vector2f(window.getSize()))));
    window.draw(text);
    window.setView(oldView);
}