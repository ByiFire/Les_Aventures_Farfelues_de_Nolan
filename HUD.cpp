#include "HUD.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

HUD::HUD()
    : gameTime_(0.f)
    , dialogueVisible_(false)
    , interactionAvailable_(false)
    , blinkTime_(0.f)
{
    // Boîte de dialogue (bas de l'écran)
    dialogueBox_.setSize({ 760.f, 120.f });
    dialogueBox_.setFillColor(sf::Color(0, 0, 0, 200));
    dialogueBox_.setPosition({ 20.f, 460.f });
    dialogueBox_.setOutlineColor(sf::Color::White);
    dialogueBox_.setOutlineThickness(2.f);
}

void HUD::setFont(const sf::Font& font) {
    font_ = font;

    // Zone actuelle (en haut à gauche)
    areaText_.emplace(font_, "", 20u);
    areaText_->setFillColor(sf::Color::White);
    areaText_->setPosition({ 10.f, 10.f });

    // Timer (en haut à droite)
    timerText_.emplace(font_, "00:00", 20u);
    timerText_->setFillColor(sf::Color::White);
    timerText_->setPosition({ 700.f, 10.f });

    // Instructions (en haut centre)
    instructionText_.emplace(font_, "", 18u);
    instructionText_->setFillColor(sf::Color::Yellow);
    instructionText_->setPosition({ 250.f, 10.f });

    // Indicateur d'interaction (centre-bas)
    interactionHint_.emplace(font_, "Appuie sur E pour parler", 16u);
    interactionHint_->setFillColor(sf::Color::Yellow);
    interactionHint_->setPosition({ 270.f, 520.f });

    // Nom du personnage qui parle
    speakerText_.emplace(font_, "", 18u);
    speakerText_->setFillColor(sf::Color::Yellow);
    speakerText_->setPosition({ 30.f, 470.f });
    speakerText_->setStyle(sf::Text::Bold);

    // Texte du dialogue
    dialogueText_.emplace(font_, "", 16u);
    dialogueText_->setFillColor(sf::Color::White);
    dialogueText_->setPosition({ 30.f, 500.f });
}

void HUD::update(sf::Time dt) {
    gameTime_ += dt.asSeconds();

    // Mise à jour du timer
    int minutes = static_cast<int>(gameTime_) / 60;
    int seconds = static_cast<int>(gameTime_) % 60;
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;

    if (timerText_) {
        timerText_->setString(oss.str());
    }

    // Animation clignotante pour l'indicateur d'interaction
    if (interactionHint_) {
        blinkTime_ += dt.asSeconds() * 3.f;
        float alpha = (std::sin(blinkTime_) + 1.f) / 2.f;
        sf::Color color = interactionHint_->getFillColor();
        color.a = static_cast<unsigned char>(150 + alpha * 105);
        interactionHint_->setFillColor(color);
    }
}

void HUD::draw(sf::RenderWindow& window) {
    // Affichage permanent
    if (areaText_) window.draw(*areaText_);
    if (timerText_) window.draw(*timerText_);

    if (instructionText_ && !instructionText_->getString().isEmpty()) {
        window.draw(*instructionText_);
    }

    // Indicateur d'interaction
    if (interactionAvailable_ && !dialogueVisible_ && interactionHint_) {
        window.draw(*interactionHint_);
    }

    // Dialogue
    if (dialogueVisible_) {
        window.draw(dialogueBox_);
        if (speakerText_) window.draw(*speakerText_);
        if (dialogueText_) window.draw(*dialogueText_);
    }
}

void HUD::setCurrentArea(const std::string& area) {
    currentArea_ = area;

    if (!areaText_) return;

    // Conversion des noms de zones en français
    if (area == "maison") {
        areaText_->setString("Maison de Nolan");
    }
    else if (area == "rue") {
        areaText_->setString("Rue");
    }
    else if (area == "rue_manif") {
        areaText_->setString("Rue (Manifestation)");
    }
    else if (area == "gare") {
        areaText_->setString("Gare");
    }
    else if (area == "train_interieur") {
        areaText_->setString("Dans le train");
    }
    else if (area == "ecole") {
        areaText_->setString("Ecole");
    }
    else {
        areaText_->setString(area);
    }
}

void HUD::setGameTime(float seconds) {
    gameTime_ = seconds;
}

void HUD::showInstruction(const std::string& instruction) {
    if (instructionText_) {
        instructionText_->setString(instruction);
    }
}

void HUD::showDialogue(const std::string& dialogue, const std::string& speaker) {
    dialogueVisible_ = true;
    if (dialogueText_) dialogueText_->setString(dialogue);
    if (speakerText_) speakerText_->setString(speaker + " :");
}

void HUD::hideDialogue() {
    dialogueVisible_ = false;
    if (dialogueText_) dialogueText_->setString("");
    if (speakerText_) speakerText_->setString("");
}

void HUD::setInteractionAvailable(bool available) {
    interactionAvailable_ = available;
}