#pragma once
#include <SFML/Graphics.hpp>

// Classe abstraite pour toutes les entités du jeu
class IEntity {
public:
    virtual ~IEntity() = default;

    // Méthodes virtuelles pures (= obligatoires pour les classes filles)
    virtual void update(sf::Time dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getGlobalBounds() const = 0;

    // Méthodes virtuelles simples
    virtual void setPosition(sf::Vector2f pos) = 0;
    virtual sf::Vector2f getPosition() const = 0;
};