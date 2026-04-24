#pragma once
#include <SFML/Graphics.hpp>

class Particle {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass;
    sf::CircleShape shape;

    bool active = true;
    bool isBlackHole = false;
    float spawnTimer = 0.5f;

    Particle(sf::Vector2f pos, sf::Vector2f vel, float m, sf::Color col);

    void update(float dt);
    void draw(sf::RenderWindow& window);

    void applyForce(sf::Vector2f force);

    void grow(float otherMass);
};