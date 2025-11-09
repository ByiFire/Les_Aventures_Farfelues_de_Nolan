#pragma once
#include "IEntity.hpp"
#include <string>
#include <vector>

enum class NPCType {
    Parent,      // Parent dans la maison
    Manifestant, // Manifestant dans la rue
    Controleur,  // Contrôleur dans le train
    Ami,         // Ami à l'école
    Passant,
    Agent,
    Passager,
    NPCgare,
};

class NPC : public IEntity {
public:
    NPC(sf::Vector2f position, NPCType type, const std::vector<std::string>& dialogues);
    NPC(sf::Vector2f position, NPCType type, const std::string& singleDialogue);

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getGlobalBounds() const override;
    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getPosition() const override;

    // Méthodes spécifiques aux NPCs
    std::string getCurrentDialogue() const;
    void advanceDialogue();
    bool hasMoreDialogues() const;
    NPCType getType() const { return type_; }
    bool isInRange(const sf::FloatRect& playerBounds) const;
    void setDialogueShown(bool shown) { dialogueShown_ = shown; }
    bool isDialogueShown() const { return dialogueShown_; }
    void resetDialogue() { currentDialogueIndex_ = 0; dialogueShown_ = false; }

private:
    sf::RectangleShape shape_;
    sf::Vector2f position_;
    NPCType type_;
    std::vector<std::string> dialogues_;
    int currentDialogueIndex_;
    bool dialogueShown_;

    // Animation simple
    float animationTime_;
    float bobOffset_; // Pour faire "bouger" le NPC
};