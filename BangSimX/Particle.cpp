#include "Particle.h"
#include <cmath>

Particle::Particle(sf::Vector2f pos, sf::Vector2f vel, float m, sf::Color col)
    : position(pos), velocity(vel), mass(m) {
    shape.setRadius(1.5f);
    shape.setFillColor(col);
    shape.setOrigin({ 1.5f, 1.5f });
}

void Particle::update(float dt) {
    if (spawnTimer > 0) spawnTimer -= dt;
    position += velocity * dt;
    shape.setPosition(position);
    velocity *= 0.997f;
}

void Particle::draw(sf::RenderWindow& window) { window.draw(shape); }

void Particle::applyForce(sf::Vector2f force) { velocity += force / mass; }

void Particle::grow(float otherMass) {
    mass += otherMass;
    if (!isBlackHole) {
        float newRadius = 1.5f + std::log10(mass + 1.0f) * 3.5f;
        shape.setRadius(newRadius);
        shape.setOrigin({ newRadius, newRadius });
        if (mass > 40.0f)  shape.setFillColor(sf::Color(255, 240, 150));
        if (mass > 250.0f) shape.setFillColor(sf::Color(255, 100, 50));
        if (mass > 1200.0f) {
            isBlackHole = true;
            shape.setFillColor(sf::Color::Black);
            shape.setOutlineThickness(2.0f);
            shape.setOutlineColor(sf::Color::White);
        }
    }
    else {
        shape.setRadius(12.0f);
        shape.setOrigin({ 12.0f, 12.0f });
    }
}