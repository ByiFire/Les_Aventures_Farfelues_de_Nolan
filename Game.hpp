#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <algorithm>

enum class GameState { Menu, Playing, Ending };

class Player {
public:
    Player(sf::Vector2f size, float speed);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getGlobalBounds() const;
    void setPosition(sf::Vector2f pos);
private:
    sf::RectangleShape shape_;
    float speed_;
};

struct TriggerZone {
    sf::FloatRect bounds;
    std::string currentArea;
    std::string targetArea;
    bool used = false;
};

class Game {
public:
    Game();
    void run();
private:
    void processEvents();
    void update(sf::Time dt);
    void render();
    void setupMenu();
    void setupTriggers();
    void setupFurniture();
    bool checkCollision(const sf::FloatRect& playerBounds, const sf::FloatRect& zoneBounds);
    sf::Vector2f calculateSpawnPosition(const std::string& newArea);
    void checkFurnitureCollision();

    sf::RenderWindow window_;
    GameState state_;
    sf::Clock clock_;

    // Menu
    sf::Font font_;
    std::optional<sf::Text> titleText_;
    sf::RectangleShape playButtonRect_;
    std::optional<sf::Text> playButtonText_;

    // Player
    Player player_;

    // Zones trigger
    std::vector<TriggerZone> triggers_;
    std::string currentArea_;
    std::string nextArea_;
    sf::Vector2f nextSpawnPos_;
    std::optional<sf::Text> eventText_;

    bool gameFinished_;

    // Fade transition
    bool fading_;
    bool fadeIn_;
    float fadeAlpha_;
    float fadeSpeed_;

    // Portes et chemins (rectangles visibles)
    sf::RectangleShape doorRect_;
    sf::RectangleShape doorRueRect_;
    sf::RectangleShape pathRect_;
    sf::RectangleShape pathManifRect_;
    sf::RectangleShape trainRect_;
    sf::RectangleShape trainDoorRect_;
    sf::RectangleShape trainExitDoorRect_;

    // Meubles maison
    std::vector<sf::RectangleShape> furniture_;
};
