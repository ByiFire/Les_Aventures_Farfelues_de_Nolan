#include "Game.hpp"
#include <iostream>

// Constants
const float WINDOW_WIDTH = 800.f;
const float WINDOW_HEIGHT = 600.f;
const float DOOR_WIDTH = 60.f;
const float DOOR_HEIGHT = 10.f;
const float PLAYER_SIZE = 40.f;
const float TRAIN_WIDTH = 150.f;
const float TRAIN_HEIGHT = 200.f;
const float PATH_WIDTH = 80.f;
const float PATH_HEIGHT = 80.f;

// ---------- Player ----------
Player::Player(sf::Vector2f size, float speed) : speed_(speed) {
    shape_.setSize(size);
    shape_.setFillColor(sf::Color::Cyan);
    shape_.setPosition({ 380.f, 500.f });
}

void Player::update(sf::Time dt) {
    sf::Vector2f movement{ 0.f, 0.f };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        movement.y -= speed_ * dt.asSeconds();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        movement.y += speed_ * dt.asSeconds();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        movement.x -= speed_ * dt.asSeconds();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        movement.x += speed_ * dt.asSeconds();

    shape_.move(movement);

    sf::Vector2f pos = shape_.getPosition();
    if (pos.x < 0.f) shape_.setPosition({ 0.f, pos.y });
    if (pos.y < 0.f) shape_.setPosition({ pos.x, 0.f });
    if (pos.x + shape_.getSize().x > WINDOW_WIDTH) shape_.setPosition({ WINDOW_WIDTH - shape_.getSize().x, pos.y });
    if (pos.y + shape_.getSize().y > WINDOW_HEIGHT) shape_.setPosition({ pos.x, WINDOW_HEIGHT - shape_.getSize().y });
}

void Player::draw(sf::RenderWindow& window) {
    sf::Vector2f pos = shape_.getPosition();

    // Tête (cercle parfait beige au centre)
    sf::CircleShape head(12.f);
    head.setFillColor(sf::Color(210, 180, 140));  // Beige
    head.setPosition({ pos.x + 8.f, pos.y + 8.f });
    window.draw(head);

    // Épaule gauche (cercle allongé rose pâle collé au cercle principal)
    sf::CircleShape shoulder1(9.f);
    shoulder1.setFillColor(sf::Color(255, 192, 203));  // Rose pâle
    shoulder1.setScale({ 0.6f, 1.2f });
    shoulder1.setPosition({ pos.x - 6.f, pos.y + 14.f });
    window.draw(shoulder1);

    // Épaule droite (cercle allongé rose pâle collé au cercle principal)
    sf::CircleShape shoulder2(9.f);
    shoulder2.setFillColor(sf::Color(255, 192, 203));  // Rose pâle
    shoulder2.setScale({ 0.6f, 1.2f });
    shoulder2.setPosition({ pos.x + 38.f, pos.y + 14.f });
    window.draw(shoulder2);
}

sf::FloatRect Player::getGlobalBounds() const { return shape_.getGlobalBounds(); }

void Player::setPosition(sf::Vector2f pos) {
    shape_.setPosition(pos);
}

// ---------- Game ----------
Game::Game()
    : window_(sf::VideoMode({ 800, 600 }), "Les aventures farfelues de Nolan")
    , state_(GameState::Menu)
    , player_({ PLAYER_SIZE, PLAYER_SIZE }, 200.f)
    , currentArea_("maison")
    , gameFinished_(false)
    , fading_(false)
    , fadeAlpha_(0.f)
    , fadeSpeed_(600.f)
    , fadeIn_(false)
{
    if (!font_.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Impossible de charger la police\n";
    }

    eventText_.emplace(font_, "", 24u);
    eventText_->setFillColor(sf::Color::Yellow);
    eventText_->setPosition({ 20.f, 20.f });

    doorRect_.setSize({ DOOR_WIDTH, DOOR_HEIGHT });
    doorRect_.setFillColor(sf::Color(150, 75, 0));
    doorRect_.setPosition({ (WINDOW_WIDTH - DOOR_WIDTH) / 2.f, 0.f });

    doorRueRect_.setSize({ DOOR_WIDTH, DOOR_HEIGHT });
    doorRueRect_.setFillColor(sf::Color(150, 75, 0));
    doorRueRect_.setPosition({ 140.f, 590.f });

    pathRect_.setSize({ 20.f, 200.f });
    pathRect_.setFillColor(sf::Color(200, 200, 100));
    pathRect_.setPosition({ WINDOW_WIDTH - 20.f, 200.f });

    pathManifRect_.setSize({ 20.f, 300.f });
    pathManifRect_.setFillColor(sf::Color(200, 200, 100));
    pathManifRect_.setPosition({ WINDOW_WIDTH - 20.f, 300.f });

    trainRect_.setSize({ TRAIN_WIDTH, TRAIN_HEIGHT });
    trainRect_.setFillColor(sf::Color(100, 100, 100));
    trainRect_.setPosition({ WINDOW_WIDTH - TRAIN_WIDTH - 20.f, WINDOW_HEIGHT / 2.f - TRAIN_HEIGHT / 2.f });

    trainDoorRect_.setSize({ 40.f, 60.f });
    trainDoorRect_.setFillColor(sf::Color(150, 100, 50));
    trainDoorRect_.setPosition({ WINDOW_WIDTH - TRAIN_WIDTH - 20.f - 50.f, WINDOW_HEIGHT / 2.f - 30.f });

    trainExitDoorRect_.setSize({ 40.f, 100.f });
    trainExitDoorRect_.setFillColor(sf::Color(150, 100, 50));
    trainExitDoorRect_.setPosition({ -50.f, WINDOW_HEIGHT / 2.f - 50.f });

    setupMenu();
    setupTriggers();
    setupFurniture();
}

// ---------- Menu ----------
void Game::setupMenu() {
    titleText_.emplace(font_, "Les aventures farfelues de Nolan", 48u);
    titleText_->setFillColor(sf::Color::White);
    titleText_->setPosition({ 80.f, 150.f });

    playButtonRect_.setSize({ 200.f, 60.f });
    playButtonRect_.setFillColor(sf::Color{ 100, 200, 100 });
    playButtonRect_.setPosition({ 300.f, 350.f });

    playButtonText_.emplace(font_, "PLAY", 32u);
    playButtonText_->setFillColor(sf::Color::Black);
    playButtonText_->setPosition({
        playButtonRect_.getPosition().x + playButtonRect_.getSize().x / 2.f - 30.f,
        playButtonRect_.getPosition().y + playButtonRect_.getSize().y / 2.f - 16.f
        });
}

// ---------- Triggers ----------
void Game::setupTriggers() {
    triggers_.clear();

    triggers_.push_back({
        sf::FloatRect({(WINDOW_WIDTH - DOOR_WIDTH) / 2.f, -50.f}, {DOOR_WIDTH, 60.f}),
        "maison", "rue", false
        });

    triggers_.push_back({
        sf::FloatRect({140.f, 590.f}, {DOOR_WIDTH, DOOR_HEIGHT}),
        "rue", "maison", false
        });

    triggers_.push_back({
        sf::FloatRect({WINDOW_WIDTH - 20.f, 200.f}, {20.f, 200.f}),
        "rue", "rue_manif", false
        });

    triggers_.push_back({
        sf::FloatRect({-50.f, 0.f}, {60.f, WINDOW_HEIGHT}),
        "rue_manif", "rue", false
        });

    triggers_.push_back({
        sf::FloatRect({WINDOW_WIDTH - 20.f, 300.f}, {20.f, 300.f}),
        "rue_manif", "gare", false
        });

    triggers_.push_back({
        sf::FloatRect({WINDOW_WIDTH - TRAIN_WIDTH - 20.f, WINDOW_HEIGHT / 2.f - TRAIN_HEIGHT / 2.f}, {TRAIN_WIDTH, TRAIN_HEIGHT}),
        "gare", "train_interieur", false
        });

    triggers_.push_back({
        sf::FloatRect({-50.f, WINDOW_HEIGHT / 2.f - 50.f}, {60.f, 100.f}),
        "train_interieur", "ecole", false
        });
}

// ---------- Furniture ----------
void Game::setupFurniture() {
    furniture_.clear();

    // Lit (rectangles empilés) - haut droit
    sf::RectangleShape bed1({ 120.f, 80.f });
    bed1.setFillColor(sf::Color(200, 100, 100));
    bed1.setPosition({ 600.f, 100.f });
    furniture_.push_back(bed1);

    sf::RectangleShape bed2({ 110.f, 10.f });
    bed2.setFillColor(sf::Color(150, 50, 50));
    bed2.setPosition({ 605.f, 85.f });
    furniture_.push_back(bed2);

    // Bureau (table) - droite
    sf::RectangleShape desk({ 100.f, 70.f });
    desk.setFillColor(sf::Color(139, 69, 19));
    desk.setPosition({ 600.f, 300.f });
    furniture_.push_back(desk);

    // Chaise au bureau - rectangles
    sf::RectangleShape chairSeat({ 40.f, 40.f });
    chairSeat.setFillColor(sf::Color(100, 100, 100));
    chairSeat.setPosition({ 520.f, 320.f });
    furniture_.push_back(chairSeat);

    // Étage en haut (rectangle noir petit - 200px de large)
    sf::RectangleShape secondFloor({ 200.f, 60.f });
    secondFloor.setFillColor(sf::Color(50, 50, 50));
    secondFloor.setPosition({ 0.f, 0.f });
    furniture_.push_back(secondFloor);

    // Cadre/Mur de l'étage supérieur
    sf::RectangleShape floorEdge({ 200.f, 3.f });
    floorEdge.setFillColor(sf::Color(30, 30, 30));
    floorEdge.setPosition({ 0.f, 60.f });
    furniture_.push_back(floorEdge);

    // Escalier - gauche (inaccessible, juste décoratif) 
    for (int i = 0; i < 15; i++) {
        sf::RectangleShape stair({ 200.f, 15.f });
        stair.setFillColor(sf::Color(139, 69, 19));
        stair.setPosition({ 0.f, 60.f + (i * 20.f) });
        furniture_.push_back(stair);
    }

    // Tapis devant la porte (du milieu)
    sf::RectangleShape rugFront({ 150.f, 80.f });
    rugFront.setFillColor(sf::Color(200, 150, 100));
    rugFront.setPosition({ 325.f, 450.f });
    furniture_.push_back(rugFront);

    // Fenêtre - haut droit (seule fenêtre qui reste)
    sf::RectangleShape window1({ 60.f, 40.f });
    window1.setFillColor(sf::Color(100, 200, 255));
    window1.setPosition({ 640.f, 20.f });
    furniture_.push_back(window1);

    // Cuisine - bas droite
    // Comptoir
    sf::RectangleShape kitchenCounter({ 120.f, 80.f });
    kitchenCounter.setFillColor(sf::Color(160, 160, 160));
    kitchenCounter.setPosition({ 620.f, 480.f });
    furniture_.push_back(kitchenCounter);

    // Poêle/Cuisinière (petit carré sur le comptoir)
    sf::RectangleShape stove({ 40.f, 40.f });
    stove.setFillColor(sf::Color(50, 50, 50));
    stove.setPosition({ 650.f, 490.f });
    furniture_.push_back(stove);

    // Brûleurs (petits carrés)
    sf::RectangleShape burner1({ 8.f, 8.f });
    burner1.setFillColor(sf::Color(100, 100, 100));
    burner1.setPosition({ 658.f, 497.f });
    furniture_.push_back(burner1);

    sf::RectangleShape burner2({ 8.f, 8.f });
    burner2.setFillColor(sf::Color(100, 100, 100));
    burner2.setPosition({ 673.f, 497.f });
    furniture_.push_back(burner2);

    sf::RectangleShape burner3({ 8.f, 8.f });
    burner3.setFillColor(sf::Color(100, 100, 100));
    burner3.setPosition({ 658.f, 512.f });
    furniture_.push_back(burner3);

    sf::RectangleShape burner4({ 8.f, 8.f });
    burner4.setFillColor(sf::Color(100, 100, 100));
    burner4.setPosition({ 673.f, 512.f });
    furniture_.push_back(burner4);
}

// ---------- Collision ----------
bool Game::checkCollision(const sf::FloatRect& a, const sf::FloatRect& b) {
    float aLeft = a.position.x;
    float aRight = a.position.x + a.size.x;
    float aTop = a.position.y;
    float aBottom = a.position.y + a.size.y;

    float bLeft = b.position.x;
    float bRight = b.position.x + b.size.x;
    float bTop = b.position.y;
    float bBottom = b.position.y + b.size.y;

    return !(aRight < bLeft || aLeft > bRight || aBottom < bTop || aTop > bBottom);
}

sf::Vector2f Game::calculateSpawnPosition(const std::string& newArea) {
    if (newArea == "rue") {
        return { (WINDOW_WIDTH - DOOR_WIDTH) / 2.f + DOOR_WIDTH / 2.f - PLAYER_SIZE / 2.f, 120.f };
    }
    else if (newArea == "maison") {
        return { (WINDOW_WIDTH - DOOR_WIDTH) / 2.f + DOOR_WIDTH / 2.f - PLAYER_SIZE / 2.f, 520.f };
    }
    else if (newArea == "rue_manif") {
        return { 100.f, 300.f };
    }
    else if (newArea == "gare") {
        return { 100.f, 300.f };
    }
    else if (newArea == "train_interieur") {
        return { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
    }
    else if (newArea == "ecole") {
        return { 100.f, 300.f };
    }

    return { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
}

// Collision avec les meubles
void Game::checkFurnitureCollision() {
    if (currentArea_ != "maison") return;

    sf::FloatRect playerRect = player_.getGlobalBounds();
    sf::Vector2f playerPos = playerRect.position;
    sf::Vector2f playerSize = playerRect.size;

    for (auto& furni : furniture_) {
        sf::FloatRect furniRect = furni.getGlobalBounds();

        if (checkCollision(playerRect, furniRect)) {
            // Calcul direction de collision (d'où vient le joueur)
            float overlapLeft = (playerPos.x + playerSize.x) - furniRect.position.x;
            float overlapRight = (furniRect.position.x + furniRect.size.x) - playerPos.x;
            float overlapTop = (playerPos.y + playerSize.y) - furniRect.position.y;
            float overlapBottom = (furniRect.position.y + furniRect.size.y) - playerPos.y;

            float minOverlap = std::min({ overlapLeft, overlapRight, overlapTop, overlapBottom });

            if (minOverlap == overlapTop) {
                // Collision par le haut
                player_.setPosition({ playerPos.x, furniRect.position.y - playerSize.y });
            }
            else if (minOverlap == overlapBottom) {
                // Collision par le bas
                player_.setPosition({ playerPos.x, furniRect.position.y + furniRect.size.y });
            }
            else if (minOverlap == overlapLeft) {
                // Collision par la gauche
                player_.setPosition({ furniRect.position.x - playerSize.x, playerPos.y });
            }
            else if (minOverlap == overlapRight) {
                // Collision par la droite
                player_.setPosition({ furniRect.position.x + furniRect.size.x, playerPos.y });
            }
        }
    }
}

// ---------- Main Loop ----------
void Game::run() {
    while (window_.isOpen()) {
        sf::Time dt = clock_.restart();
        processEvents();
        update(dt);
        render();
    }
}

// ---------- Events ----------
void Game::processEvents() {
    while (auto maybeEvent = window_.pollEvent()) {
        if (!maybeEvent) break;

        if (maybeEvent->is<sf::Event::Closed>())
            window_.close();

        if (state_ == GameState::Menu && maybeEvent->is<sf::Event::MouseButtonPressed>()) {
            if (auto mouse = maybeEvent->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window_.mapPixelToCoords(mouse->position);
                    if (playButtonRect_.getGlobalBounds().contains(mousePos))
                        state_ = GameState::Playing;
                }
            }
        }

        if (state_ == GameState::Menu && maybeEvent->is<sf::Event::KeyPressed>()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
                state_ = GameState::Playing;
        }
    }
}

// ---------- Update ----------
void Game::update(sf::Time dt) {
    if (state_ == GameState::Playing && !gameFinished_) {
        player_.update(dt);
        checkFurnitureCollision();

        if (!fading_) {
            for (auto& trigger : triggers_) {
                if (!trigger.used && trigger.currentArea == currentArea_ &&
                    checkCollision(player_.getGlobalBounds(), trigger.bounds)) {

                    nextArea_ = trigger.targetArea;
                    nextSpawnPos_ = calculateSpawnPosition(nextArea_);
                    fading_ = true;
                    fadeIn_ = false;
                    trigger.used = true;

                    if (nextArea_ == "maison") {
                        for (auto& t : triggers_) {
                            if (t.currentArea == "maison" || t.targetArea == "maison") {
                                t.used = false;
                            }
                        }
                    }

                    if (nextArea_ == "ecole") {
                        gameFinished_ = true;
                    }
                }
            }
        }
    }

    if (fading_) {
        if (!fadeIn_) {
            fadeAlpha_ += fadeSpeed_ * dt.asSeconds();
            if (fadeAlpha_ >= 255.f) {
                fadeAlpha_ = 255.f;
                currentArea_ = nextArea_;
                fadeIn_ = true;
                player_.setPosition(nextSpawnPos_);
            }
        }
        else {
            fadeAlpha_ -= fadeSpeed_ * dt.asSeconds();
            if (fadeAlpha_ <= 0.f) {
                fadeAlpha_ = 0.f;
                fading_ = false;
            }
        }
    }
}

// ---------- Render ----------
void Game::render() {
    if (state_ == GameState::Menu) window_.clear(sf::Color{ 50,50,50 });
    else if (currentArea_ == "maison") window_.clear(sf::Color{ 200,180,150 });
    else if (currentArea_ == "rue") window_.clear(sf::Color{ 100,150,200 });
    else if (currentArea_ == "rue_manif") window_.clear(sf::Color{ 80,120,180 });
    else if (currentArea_ == "gare") window_.clear(sf::Color{ 180,180,180 });
    else if (currentArea_ == "train_interieur") window_.clear(sf::Color{ 120,120,120 });
    else if (currentArea_ == "ecole") window_.clear(sf::Color{ 50,200,50 });

    if (state_ == GameState::Menu) {
        if (titleText_) window_.draw(*titleText_);
        window_.draw(playButtonRect_);
        if (playButtonText_) window_.draw(*playButtonText_);
    }
    else {
        player_.draw(window_);
        if (eventText_) window_.draw(*eventText_);

        if (currentArea_ == "maison") {
            window_.draw(doorRect_);
            for (auto& furni : furniture_) {
                window_.draw(furni);
            }
        }

        if (currentArea_ == "rue") {
            window_.draw(doorRueRect_);
            window_.draw(pathRect_);
        }

        if (currentArea_ == "rue_manif") {
            window_.draw(pathManifRect_);
        }

        if (currentArea_ == "gare") {
            window_.draw(trainRect_);
            window_.draw(trainDoorRect_);
        }

        if (currentArea_ == "train_interieur") {
            window_.draw(trainExitDoorRect_);
        }

        if (gameFinished_) {
            sf::Text endText(font_, "Bravo ! Nolan n'a pas cours aujourd'hui !", 32u);
            endText.setFillColor(sf::Color::Yellow);
            endText.setPosition({ 100.f, 300.f });
            window_.draw(endText);
        }
    }

    if (fading_) {
        sf::RectangleShape fadeRect({ WINDOW_WIDTH, WINDOW_HEIGHT });
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<unsigned char>(fadeAlpha_)));
        window_.draw(fadeRect);
    }

    window_.display();
}
