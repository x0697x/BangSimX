#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Represents a single physical body in the simulation.
 */
class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass;
    sf::CircleShape shape;

    bool active = true;        // If false, particle is "eaten" and should be ignored
    bool isBlackHole = false;  // Special state for high-mass particles
    float spawnTimer = 0.5f;   // (Currently unused in physics, but present)

    Particle(sf::Vector2f pos, sf::Vector2f vel, float m, sf::Color col);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    /**
     * @brief Changes velocity based on Force = Mass * Acceleration
     */
    void applyForce(sf::Vector2f force);

    /**
     * @brief Consumes another particle, increasing mass and changing appearance.
     */
    void grow(float otherMass);
};