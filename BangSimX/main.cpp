#include <SFML/Graphics.hpp>
#include "Particle.h"
#include "HUD.h"
#include <vector>
#include <random>
#include <cmath>
#include <optional>

/**
 * @brief Resets the simulation state to the beginning.
 */
void resetUniverse(std::vector<Particle>& universe, sf::Clock& totalTime, float& G, sf::View& view, HUD& hud, Particle** target) {
    universe.clear();
    totalTime.restart();
    G = 5.0f;           // Reset gravitational constant
    hud.isEnding = false;
    *target = nullptr;  // Stop following particles

    // Reset view to default
    view.setCenter({ 640.f, 360.f });
    view.setSize({ 1280.f, 720.f });

    // Seeded random number generation
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(100.0f, 1180.0f), distY(100.0f, 620.0f);
    std::uniform_real_distribution<float> distSpeed(-4.0f, 4.0f), distMass(0.5f, 3.0f);

    // Spawn initial particles
    for (int i = 0; i < 5000; ++i) {
        universe.push_back(Particle({ distX(rng), distY(rng) }, { distSpeed(rng), distSpeed(rng) }, distMass(rng), sf::Color::White));
    }
}

int main() {
    // Create window and limit frame rate for consistency
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "BangSimX - Follow Mode");
    window.setFramerateLimit(500);

    HUD myHUD;
    float timeScale = 0.5f;     // Simulation speed multiplier
    bool isPaused = false;
    float G = 5.0;             // Gravitational constant
    float shakeIntensity = 0.0f;// Camera shake factor
    Particle* cameraTarget = nullptr; // Particle being followed

    std::vector<Particle> universe;
    sf::View worldView(sf::FloatRect({ 0.f, 0.f }, { 1280.f, 720.f }));
    sf::Clock clock;            // Frame delta clock
    float totalElapsed = 0.0f;  // Total simulated time

    resetUniverse(universe, clock, G, worldView, myHUD, &cameraTarget);

    while (window.isOpen()) {
        // --- EVENT HANDLING ---
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
                if (keyPressed->code == sf::Keyboard::Key::Space) isPaused = !isPaused;

                // Time control keys
                if (keyPressed->code == sf::Keyboard::Key::Num1) timeScale = 0.2f;
                if (keyPressed->code == sf::Keyboard::Key::Num2) timeScale = 1.0f;
                if (keyPressed->code == sf::Keyboard::Key::Num3) timeScale = 3.0f;

                if (keyPressed->code == sf::Keyboard::Key::R) {
                    resetUniverse(universe, clock, G, worldView, myHUD, &cameraTarget);
                    totalElapsed = 0.0f;
                }
            }
        }

        // --- PHYSICS UPDATE ---
        float realDt = clock.restart().asSeconds();
        if (!isPaused) {
            float dt = realDt * timeScale;
            totalElapsed += dt;

            // Gravity gets stronger after 15 seconds to force collapse
            if (totalElapsed > 60.0f) G = 30.0f;

            // N-squared collision and gravity logic
            for (size_t i = 0; i < universe.size(); ++i) {
                if (!universe[i].active) continue;

                // Auto-follow the first Black Hole that forms
                if (universe[i].isBlackHole && cameraTarget == nullptr) {
                    cameraTarget = &universe[i];
                }

                for (size_t j = i + 1; j < universe.size(); ++j) {
                    if (!universe[j].active) continue;

                    sf::Vector2f dir = universe[j].position - universe[i].position;
                    float distSq = dir.x * dir.x + dir.y * dir.y;
                    float dist = std::sqrt(distSq);
                    bool bothBH = universe[i].isBlackHole && universe[j].isBlackHole;

                    // --- COLLISION LOGIC ---
                    if (dist < (universe[i].shape.getRadius() + universe[j].shape.getRadius()) && !bothBH) {
                        // Absorption: Larger eats smaller
                        if (universe[i].mass >= universe[j].mass) { universe[i].grow(universe[j].mass); universe[j].active = false; }
                        else { universe[j].grow(universe[i].mass); universe[i].active = false; }
                    }
                    else if (bothBH && dist < 12.0f) {
                        // Black Hole Merger: The "End" of the sim
                        if (cameraTarget == &universe[j]) cameraTarget = &universe[i];
                        universe[i].grow(universe[j].mass);
                        universe[j].active = false;
                        shakeIntensity = 50.0f; // Big boom shake
                        myHUD.isEnding = true;
                    }
                    else {
                        // --- GRAVITY CALCULATION ---
                        // Softening prevents infinite force when particles are very close
                        float softening = bothBH ? 10.0f : 1000.0f;
                        float forceStrength = (G * universe[i].mass * universe[j].mass) / (distSq + softening);
                        sf::Vector2f force = (dir / dist) * forceStrength;

                        universe[i].applyForce(force * dt);
                        universe[j].applyForce(-force * dt);
                    }
                }
            }
            // Move particles based on calculated forces
            for (auto& p : universe) if (p.active) p.update(dt);
        }

        // --- CAMERA / VIEW MANAGEMENT ---
        sf::Vector2f targetCenter = { 640.f, 360.f };
        if (cameraTarget && cameraTarget->active) {
            targetCenter = cameraTarget->position;
        }

        // Apply Screen Shake if intensity is active
        if (shakeIntensity > 0.1f) {
            float ox = (rand() % 10 - 5) * (shakeIntensity / 10.0f);
            float oy = (rand() % 10 - 5) * (shakeIntensity / 10.0f);
            worldView.setCenter(targetCenter + sf::Vector2f(ox, oy));
            shakeIntensity *= 0.99f; // Decay shake over time
        }
        else {
            worldView.setCenter(targetCenter);
        }

        // --- RENDERING ---
        window.clear(sf::Color::Black);

        window.setView(worldView); // Draw particles in world space
        for (auto& p : universe) if (p.active) p.draw(window);

        // HUD is drawn last and handles its own View internally
        myHUD.update((int)(1.0f / (realDt > 0 ? realDt : 0.01f)), totalElapsed, timeScale, universe.size(), isPaused);
        myHUD.draw(window);

        window.display();
    }
    return 0;
}