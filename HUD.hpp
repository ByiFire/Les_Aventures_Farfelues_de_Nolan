#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

class HUD {
public:
    HUD();  // Constructeur sans paramètres
    void setFont(const sf::Font& font);  // Ajout de setFont()
    void update(sf::Time dt);
    void draw(sf::RenderWindow& window);

    // Setters pour mettre à jour les infos affichées
    void setCurrentArea(const std::string& area);
    void setGameTime(float seconds);
    void showInstruction(const std::string& instruction);
    void showDialogue(const std::string& dialogue, const std::string& speaker);
    void hideDialogue();
    void setInteractionAvailable(bool available);

    // Getter
    bool isDialogueShown() const { return dialogueVisible_; }

private:
    sf::Font font_;

    // Éléments d'interface avec std::optional
    std::optional<sf::Text> areaText_;
    std::optional<sf::Text> timerText_;
    std::optional<sf::Text> instructionText_;
    std::optional<sf::Text> dialogueText_;
    std::optional<sf::Text> speakerText_;
    std::optional<sf::Text> interactionHint_;

    // Fond pour le dialogue
    sf::RectangleShape dialogueBox_;

    // État
    std::string currentArea_;
    float gameTime_;
    bool dialogueVisible_;
    bool interactionAvailable_;
    
    // Animation pour l'indicateur d'interaction
    float blinkTime_;
};