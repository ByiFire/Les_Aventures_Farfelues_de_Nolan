#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>

enum class GameState { Menu, Playing, Ending };

class Player {
public:
    Player(sf::Vector2f size, float speed);
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);
    sf::FloatRect getGlobalBounds() const;
private:
    sf::RectangleShape shape_;
    float speed_;
};

struct TriggerZone {
    sf::FloatRect bounds;
    std::string targetArea;
    std::string eventText;
    bool used = false; // <-- pour savoir si le trigger a déjà été activé
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
    bool checkCollision(const sf::FloatRect& playerBounds, const sf::FloatRect& zoneBounds);

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
    std::optional<sf::Text> eventText_;
    bool gameFinished_;

    // Fade transition
    bool fading_ = false;
    bool fadeIn_ = false;
    float fadeAlpha_ = 0.f;
    float fadeSpeed_ = 600.f;
    std::string nextArea_;

    // Porte (rectangle visible)
    sf::RectangleShape doorRect_;
    sf::RectangleShape doorRueRect_;
};
