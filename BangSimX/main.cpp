#include <SFML/Graphics.hpp>
#include "Particle.h"
#include "HUD.h"
#include <vector>
#include <random>
#include <cmath>
#include <optional>

void resetUniverse(std::vector<Particle>& universe, sf::Clock& totalTime, float& G, sf::View& view) {
    universe.clear();
    totalTime.restart();
    G = 5.0f;
    view.setCenter({ 640.f, 360.f });
    view.setSize({ 1280.f, 720.f });

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(30.0f, 1250.0f), distY(30.0f, 690.0f);
    std::uniform_real_distribution<float> distSpeed(-6.0f, 6.0f), distMass(0.5f, 3.0f);

    for (int i = 0; i < 1000; ++i) {
        universe.push_back(Particle({ distX(rng), distY(rng) }, { distSpeed(rng), distSpeed(rng) }, distMass(rng), sf::Color(150, 180, 255, 160)));
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "BangSimX - Direct Fix");
    window.setFramerateLimit(60);

    HUD myHUD;
    float timeScale = 1.0f;
    float G = 5.0f;
    std::vector<Particle> universe;
    sf::View worldView(sf::FloatRect({ 0.f, 0.f }, { 1280.f, 720.f }));
    sf::Clock clock;
    sf::Clock totalTime;
    bool endingSequence = false;

    resetUniverse(universe, totalTime, G, worldView);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Num1) timeScale = 0.2f;
                if (keyPressed->code == sf::Keyboard::Key::Num2) timeScale = 1.0f;
                if (keyPressed->code == sf::Keyboard::Key::Num3) timeScale = 3.0f;
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    resetUniverse(universe, totalTime, G, worldView);
                    endingSequence = false;
                }
            }
        }

        float realDt = clock.restart().asSeconds();
        float dt = realDt * timeScale;
        if (dt > 0.05f) dt = 0.05f;

        float elapsed = totalTime.getElapsedTime().asSeconds();
        if (elapsed > 15.0f) G = 20.0f;

        sf::Vector2f singularityPos;
        float maxMass = 0;
        size_t activeCount = 0;

        for (size_t i = 0; i < universe.size(); ++i) {
            if (!universe[i].active) continue;
            activeCount++;
            if (universe[i].mass > maxMass) { maxMass = universe[i].mass; singularityPos = universe[i].position; }

            for (size_t j = i + 1; j < universe.size(); ++j) {
                if (!universe[j].active) continue;
                sf::Vector2f dir = universe[j].position - universe[i].position;
                float distSq = dir.x * dir.x + dir.y * dir.y;
                float dist = std::sqrt(distSq);
                float mergeDist = universe[i].shape.getRadius() + universe[j].shape.getRadius();

                if (dist < mergeDist) {
                    if (universe[i].mass >= universe[j].mass) { universe[i].grow(universe[j].mass); universe[j].active = false; }
                    else { universe[j].grow(universe[i].mass); universe[i].active = false; }
                }
                else {
                    float forceStrength = (G * universe[i].mass * universe[j].mass) / (distSq + 1000.0f);
                    sf::Vector2f force = (dir / dist) * forceStrength;
                    universe[i].applyForce(force * timeScale);
                    universe[j].applyForce(-force * timeScale);
                }
            }
        }

        if (maxMass > 1200.0f) endingSequence = true;
        if (endingSequence) {
            sf::Vector2f currentCenter = worldView.getCenter();
            worldView.setCenter(currentCenter + (singularityPos - currentCenter) * 0.01f);
            worldView.zoom(0.9999f);
        }

        // Use realDt for FPS so it's accurate even if the simulation is slowed down
        myHUD.update((int)(1.0f / (realDt > 0 ? realDt : 0.01f)), elapsed, timeScale, activeCount);

        window.clear(sf::Color(5, 5, 15));

        window.setView(worldView);
        for (auto& p : universe) if (p.active) { p.update(dt); p.draw(window); }

        // Draw the HUD last
        myHUD.draw(window);
        window.display();
    }
    return 0;
}