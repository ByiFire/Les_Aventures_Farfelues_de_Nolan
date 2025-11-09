#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <algorithm>
#include <memory>
#include <variant>
#include "IEntity.hpp"
#include "NPC.hpp"
#include "HUD.hpp"

enum class GameState { Menu, Playing, Paused, Credits };
enum class Direction { Right, Left, Up, Down };

class Player : public IEntity {
public:
    Player(sf::Vector2f size, float speed);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const override;
    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getPosition() const override;
    Direction getDirection() const { return direction_; }
private:
    sf::RectangleShape shape_;
    float speed_;
    Direction direction_;
};

struct TriggerZone {
    sf::FloatRect bounds;
    std::string currentArea;
    std::string targetArea;
    bool used = false;
};

// Type pour stocker différents types de formes
using DecorShape = std::variant<sf::RectangleShape, sf::CircleShape>;

class Game {
public:
    Game();
    void run();
private:
    void processEvents();
    void update(sf::Time dt);
    void render();
    void setupMenu();
    void resetGame();
    void setupPauseMenu();
    void setupTriggers();
    void setupFurniture();
    void setupStreetDecor();
    void setupManifDecor();
    void setupGareDecor();
    void setupTrainDecor();
    void setupSchoolDecor();
    void setupCredits();
    void setupNPCs();
    bool checkCollision(const sf::FloatRect& playerBounds, const sf::FloatRect& zoneBounds);
    sf::Vector2f calculateSpawnPosition(const std::string& newArea);
    void checkFurnitureCollision();
    void checkNPCInteractions();

    sf::RenderWindow window_;
    GameState state_;
    sf::Clock clock_;

    // Menu
    sf::Font font_;
    std::optional<sf::Text> titleText_;
    sf::RectangleShape playButtonRect_;
    std::optional<sf::Text> playButtonText_;

    // Menu Pause
    sf::RectangleShape pauseOverlay_;
    sf::RectangleShape resumeButtonRect_;
    sf::RectangleShape quitButtonRect_;
    std::optional<sf::Text> pauseTitleText_;
    std::optional<sf::Text> resumeButtonText_;
    std::optional<sf::Text> quitButtonText_;

    // Player
    std::unique_ptr<Player> player_;

    // NPCs
    std::vector<std::unique_ptr<NPC>> npcs_;
    NPC* currentNPCInRange_;

    // HUD
    std::unique_ptr<HUD> hud_;

    // Zones trigger
    std::vector<TriggerZone> triggers_;
    std::string currentArea_;
    std::string nextArea_;
    sf::Vector2f nextSpawnPos_;
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

    // Décors de la rue (avec variant pour supporter RectangleShape et CircleShape)
    std::vector<DecorShape> streetDecor_;
    std::vector<DecorShape> manifDecor_;
    std::vector<DecorShape> gareDecor_;  
    std::vector<DecorShape> trainDecor_;  
    std::vector<DecorShape> schoolDecor_;  

    // Crédits du jeu
    bool creditsStarted_;
    float creditsScroll_;
    float creditsSpeed_;
    std::vector<sf::Text> creditsTexts_;
};