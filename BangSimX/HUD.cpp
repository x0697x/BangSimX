#include "HUD.h"
#include <iostream>
#include <sstream>
#include <iomanip>

HUD::HUD() : fontLoaded(false), text(font), endText(font) {
    // Attempt to load the font file from the resources folder
    if (font.openFromFile("Resources/type.ttf")) {
        fontLoaded = true;

        // Setup stats text (Top-left)
        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
        text.setPosition({ 20.f, 20.f });

        // Setup "The End" text (Screen Center)
        endText.setFont(font);
        endText.setCharacterSize(80);
        endText.setFillColor(sf::Color::Red);
        endText.setString("THE END");

        // Center the origin of the endText for perfect middle-alignment
        sf::FloatRect textBounds = endText.getLocalBounds();
        endText.setOrigin({
            textBounds.position.x + textBounds.size.x / 2.0f,
            textBounds.position.y + textBounds.size.y / 2.0f
            });
    }
}

float appVersion = 1.0f;

void HUD::update(int fps, float time, float speedMult, size_t particleCount, bool isPaused) {
    if (!fontLoaded) return;

    // Convert speed multiplier to a user-friendly label
    std::string speedStr = isPaused ? "PAUSED" : (speedMult < 0.5f ? "Slow" : (speedMult > 1.5f ? "Fast" : "Normal"));

    // Format version string
    std::stringstream ss;
    ss << appVersion;
    std::string versionStr = ss.str();

    // Compile the final display string
    text.setString(
        "FPS: " + std::to_string(fps) +
        "\nTime: " + std::to_string((int)time) + "s" +
        "\nSpeed: " + speedStr +
        "\nParticles: " + std::to_string(particleCount) +
        "\n[Space] Pause | [1-3] Speed | [R] Reset" +
        "\n[Esc] Exit"
        "\n\nVer. " + versionStr +
        "\nCreated by x0697x"
    );
}

void HUD::draw(sf::RenderWindow& window) {
    if (!fontLoaded) return;

    // Temporarily switch to a static UI view so the HUD doesn't move with the camera
    sf::View oldView = window.getView();
    window.setView(sf::View(sf::FloatRect({ 0.f, 0.f }, sf::Vector2f(window.getSize()))));

    window.draw(text);

    // Draw the end-game message if two black holes merged
    if (isEnding) {
        endText.setPosition(sf::Vector2f(window.getSize()) / 2.0f);
        window.draw(endText);
    }

    // Restore the world view for particle rendering
    window.setView(oldView);
}