#include <SFML/Graphics.hpp>
#include "Particle.h"
#include "HUD.h"
#include <vector>
#include <random>
#include <cmath>
#include <optional>

void resetUniverse(std::vector<Particle>& universe, sf::Clock& totalTime, float& G, sf::View& view, HUD& hud, Particle** target) {
    universe.clear();
    totalTime.restart();
    G = 5.0f;
    hud.isEnding = false;
    *target = nullptr; // Reset camera target
    view.setCenter({ 640.f, 360.f });
    view.setSize({ 1280.f, 720.f });

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(100.0f, 1180.0f), distY(100.0f, 620.0f);
    std::uniform_real_distribution<float> distSpeed(-4.0f, 4.0f), distMass(0.5f, 3.0f);

    for (int i = 0; i < 5000; ++i) {
        universe.push_back(Particle({ distX(rng), distY(rng) }, { distSpeed(rng), distSpeed(rng) }, distMass(rng), sf::Color::White));
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "BangSimX - Follow Mode");
    window.setFramerateLimit(60);

    HUD myHUD;
    float timeScale = 1.0f;
    bool isPaused = false;
    float G = 5.0f;
    float shakeIntensity = 0.0f;
    Particle* cameraTarget = nullptr;

    std::vector<Particle> universe;
    sf::View worldView(sf::FloatRect({ 0.f, 0.f }, { 1280.f, 720.f }));
    sf::Clock clock;
    float totalElapsed = 0.0f;

    resetUniverse(universe, clock, G, worldView, myHUD, &cameraTarget);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) window.close();
                if (keyPressed->code == sf::Keyboard::Key::Space) isPaused = !isPaused;

                // SPEED INPUTS FIXED HERE
                if (keyPressed->code == sf::Keyboard::Key::Num1) timeScale = 0.2f;
                if (keyPressed->code == sf::Keyboard::Key::Num2) timeScale = 1.0f;
                if (keyPressed->code == sf::Keyboard::Key::Num3) timeScale = 3.0f;

                if (keyPressed->code == sf::Keyboard::Key::R) {
                    resetUniverse(universe, clock, G, worldView, myHUD, &cameraTarget);
                    totalElapsed = 0.0f;
                }
            }
        }

        float realDt = clock.restart().asSeconds();
        if (!isPaused) {
            float dt = realDt * timeScale;
            totalElapsed += dt;
            if (totalElapsed > 15.0f) G = 30.0f;

            for (size_t i = 0; i < universe.size(); ++i) {
                if (!universe[i].active) continue;

                // Track the first black hole created
                if (universe[i].isBlackHole && cameraTarget == nullptr) {
                    cameraTarget = &universe[i];
                }

                for (size_t j = i + 1; j < universe.size(); ++j) {
                    if (!universe[j].active) continue;

                    sf::Vector2f dir = universe[j].position - universe[i].position;
                    float distSq = dir.x * dir.x + dir.y * dir.y;
                    float dist = std::sqrt(distSq);
                    bool bothBH = universe[i].isBlackHole && universe[j].isBlackHole;

                    if (dist < (universe[i].shape.getRadius() + universe[j].shape.getRadius()) && !bothBH) {
                        if (universe[i].mass >= universe[j].mass) { universe[i].grow(universe[j].mass); universe[j].active = false; }
                        else { universe[j].grow(universe[i].mass); universe[i].active = false; }
                    }
                    else if (bothBH && dist < 12.0f) {
                        if (cameraTarget == &universe[j]) cameraTarget = &universe[i];
                        universe[i].grow(universe[j].mass);
                        universe[j].active = false;
                        shakeIntensity = 50.0f;
                        myHUD.isEnding = true;
                    }
                    else {
                        float softening = bothBH ? 10.0f : 1000.0f;
                        float forceStrength = (G * universe[i].mass * universe[j].mass) / (distSq + softening);
                        sf::Vector2f force = (dir / dist) * forceStrength;
                        universe[i].applyForce(force * dt); // Applied with dt (includes timeScale)
                        universe[j].applyForce(-force * dt);
                    }
                }
            }
            for (auto& p : universe) if (p.active) p.update(dt);
        }

        // CAMERA FOLLOW & SHAKE LOGIC
        sf::Vector2f targetCenter = { 640.f, 360.f };
        if (cameraTarget && cameraTarget->active) {
            targetCenter = cameraTarget->position;
        }

        if (shakeIntensity > 0.1f) {
            float ox = (rand() % 10 - 5) * (shakeIntensity / 10.0f);
            float oy = (rand() % 10 - 5) * (shakeIntensity / 10.0f);
            worldView.setCenter(targetCenter + sf::Vector2f(ox, oy));
            shakeIntensity *= 0.95f;
        }
        else {
            worldView.setCenter(targetCenter);
        }

        window.clear(sf::Color::Black);
        window.setView(worldView);
        for (auto& p : universe) if (p.active) p.draw(window);
        myHUD.update((int)(1.0f / (realDt > 0 ? realDt : 0.01f)), totalElapsed, timeScale, universe.size(), isPaused);
        myHUD.draw(window);
        window.display();
    }
    return 0;
}