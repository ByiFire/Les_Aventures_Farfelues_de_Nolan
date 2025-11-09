#include "NPC.hpp"
#include <cmath>

// Constructeur principal avec vector de dialogues
NPC::NPC(sf::Vector2f position, NPCType type, const std::vector<std::string>& dialogues)
    : position_(position)
    , type_(type)
    , dialogues_(dialogues)
    , currentDialogueIndex_(0)
    , dialogueShown_(false)
    , animationTime_(0.f)
    , bobOffset_(0.f)
{
    shape_.setSize({ 30.f, 40.f });
    shape_.setPosition(position_);

    switch (type_) {
    case NPCType::Parent:
        shape_.setFillColor(sf::Color(100, 50, 150));
        break;
    case NPCType::Manifestant:
        shape_.setFillColor(sf::Color(200, 100, 0));
        break;
    case NPCType::Controleur:
        shape_.setFillColor(sf::Color(50, 50, 150));
        break;
    case NPCType::Ami:
        shape_.setFillColor(sf::Color(50, 200, 50));
        break;
    case NPCType::Passant:
        shape_.setFillColor(sf::Color(100, 100, 200));
        break;
    case NPCType::Agent:
        shape_.setFillColor(sf::Color(150, 150, 50));
        break;
    }
}

// Constructeur avec un seul dialogue
NPC::NPC(sf::Vector2f position, NPCType type, const std::string& singleDialogue)
    : position_(position)
    , type_(type)
    , dialogues_()
    , currentDialogueIndex_(0)
    , dialogueShown_(false)
    , animationTime_(0.f)
    , bobOffset_(0.f)
{
    dialogues_.push_back(singleDialogue);

    shape_.setSize({ 30.f, 40.f });
    shape_.setPosition(position_);

    switch (type_) {
    case NPCType::Parent:
        shape_.setFillColor(sf::Color(100, 50, 150));
        break;
    case NPCType::Manifestant:
        shape_.setFillColor(sf::Color(200, 100, 0));
        break;
    case NPCType::Controleur:
        shape_.setFillColor(sf::Color(50, 50, 150));
        break;
    case NPCType::Ami:
        shape_.setFillColor(sf::Color(50, 200, 50));
        break;
    case NPCType::Passant:
        shape_.setFillColor(sf::Color(100, 100, 200));
        break;
    case NPCType::Passager:
        shape_.setFillColor(sf::Color(100, 100, 200));
        break;
    case NPCType::NPCgare:
        shape_.setFillColor(sf::Color(100, 100, 200));
        break;
    case NPCType::Agent:
        shape_.setFillColor(sf::Color(150, 150, 50));
        break;
    }
}

void NPC::update(sf::Time dt) {
    animationTime_ += dt.asSeconds() * 2.f;
    bobOffset_ = std::sin(animationTime_) * 3.f;

    if (type_ == NPCType::Manifestant) {
        float moveOffset = std::sin(animationTime_ * 0.5f) * 20.f;
        shape_.setPosition({ position_.x + moveOffset, position_.y + bobOffset_ });
    }
    else {
        shape_.setPosition({ position_.x, position_.y + bobOffset_ });
    }
}

void NPC::draw(sf::RenderWindow& window) {
    // Corps
    window.draw(shape_);

    // Tête (cercle)
    sf::CircleShape head(10.f);
    head.setFillColor(sf::Color(210, 180, 140));
    sf::Vector2f shapePos = shape_.getPosition();
    head.setPosition({ shapePos.x + 5.f, shapePos.y - 15.f });
    window.draw(head);

    // Yeux
    sf::CircleShape eye1(2.f);
    sf::CircleShape eye2(2.f);
    eye1.setFillColor(sf::Color::Black);
    eye2.setFillColor(sf::Color::Black);
    eye1.setPosition({ shapePos.x + 10.f, shapePos.y - 10.f });
    eye2.setPosition({ shapePos.x + 18.f, shapePos.y - 10.f });
    window.draw(eye1);
    window.draw(eye2);

    // Indicateur d'interaction (point jaune au-dessus si pas encore parlé)
    if (!dialogueShown_) {
        sf::CircleShape indicator(5.f);
        indicator.setFillColor(sf::Color::Yellow);
        indicator.setPosition({ shapePos.x + 10.f, shapePos.y - 30.f });
        window.draw(indicator);
    }
}

sf::FloatRect NPC::getGlobalBounds() const {
    return shape_.getGlobalBounds();
}

void NPC::setPosition(sf::Vector2f pos) {
    position_ = pos;
    shape_.setPosition(pos);
}

sf::Vector2f NPC::getPosition() const {
    return position_;
}

bool NPC::isInRange(const sf::FloatRect& playerBounds) const {
    sf::FloatRect npcBounds = getGlobalBounds();

    // Zone d'interaction agrandie autour du NPC
    sf::FloatRect interactionZone = npcBounds;
    interactionZone.position.x -= 30.f;
    interactionZone.position.y -= 30.f;
    interactionZone.size.x += 60.f;
    interactionZone.size.y += 60.f;

    // Vérifie si le centre du joueur est dans la zone
    return interactionZone.contains({
        playerBounds.position.x + playerBounds.size.x / 2.f,
        playerBounds.position.y + playerBounds.size.y / 2.f
        });
}

std::string NPC::getCurrentDialogue() const {
    if (currentDialogueIndex_ >= 0 && currentDialogueIndex_ < static_cast<int>(dialogues_.size())) {
        return dialogues_[currentDialogueIndex_];
    }
    return "";
}

void NPC::advanceDialogue() {
    if (currentDialogueIndex_ < static_cast<int>(dialogues_.size()) - 1) {
        currentDialogueIndex_++;
    }
}

bool NPC::hasMoreDialogues() const {
    return currentDialogueIndex_ < static_cast<int>(dialogues_.size()) - 1;
}