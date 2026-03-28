#include "Particle.h"
#include <cmath>

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel, float m, sf::Color col)
    : position(pos), velocity(vel), mass(m) {
    // Initialize appearance
    shape.setRadius(1.5f);
    shape.setFillColor(col);
    shape.setOrigin({ 1.5f, 1.5f }); // Center origin for better rotation/scaling
}

void Particle::update(float dt) {
    if (spawnTimer > 0) spawnTimer -= dt;

    // Basic Euler integration
    position += velocity * dt;
    shape.setPosition(position);

    // Simulated "friction/drag" to keep the simulation from exploding too fast
    // Black holes have less drag to maintain dominance
    if (isBlackHole) {
        velocity *= 1.0f;
    }
    else {
        velocity *= 0.999999f;
    }
}

void Particle::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Particle::applyForce(sf::Vector2f force) {
    // New Velocity = Old Velocity + (Force / Mass)
    velocity += force / mass;
}

void Particle::grow(float otherMass) {
    mass += otherMass;

    if (!isBlackHole) {
        // Logarithmic scaling for radius so they don't get too huge too fast
        float newRadius = 1.5f + std::log10(mass + 1.0f) * 3.0f;
        shape.setRadius(newRadius);
        shape.setOrigin({ newRadius, newRadius });

        // Color shifts based on mass milestones
        if (mass > 40.0f)  shape.setFillColor(sf::Color::White);
        if (mass > 250.0f) shape.setFillColor(sf::Color(255, 165, 0)); // Orange star

        // Critical mass: Collapse into a Black Hole
        if (mass > 1200.0f) {
            isBlackHole = true;
            mass *= 2.0f; // Mass boost upon collapse
            shape.setFillColor(sf::Color::Black);
            shape.setOutlineThickness(2.0f);
            shape.setOutlineColor(sf::Color::White);
        }
    }
    else {
        // If already a black hole, keep fixed size but keep increasing mass internally
        shape.setRadius(12.0f);
        shape.setOrigin({ 12.0f, 12.0f });
    }
}