#include "Game.hpp"
#include <iostream>

// ---------------- Player ----------------
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

    // Limites fenêtre
    sf::Vector2f pos = shape_.getPosition();
    if (pos.x < 0.f) shape_.setPosition({ 0.f, pos.y });
    if (pos.y < 0.f) shape_.setPosition({ pos.x, 0.f });
    if (pos.x + shape_.getSize().x > 800.f) shape_.setPosition({ 800.f - shape_.getSize().x, pos.y });
    if (pos.y + shape_.getSize().y > 600.f) shape_.setPosition({ pos.x, 600.f - shape_.getSize().y });
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape_);
}

sf::FloatRect Player::getGlobalBounds() const { return shape_.getGlobalBounds(); }

// ---------------- Game ----------------
Game::Game()
    : window_(sf::VideoMode({ 800, 600 }), "Les aventures farfelues de Nolan")
    , state_(GameState::Menu)
    , player_({ 40.f, 40.f }, 200.f)
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

    // Création des portes
    doorRect_.setSize({ 60.f, 100.f });
    doorRect_.setFillColor(sf::Color(150, 75, 0));
    doorRect_.setPosition({ 370.f, 0.f });

    doorRueRect_.setSize({ 60.f, 100.f });
    doorRueRect_.setFillColor(sf::Color(150, 75, 0));
    doorRueRect_.setPosition({ 140.f, 500.f });

    setupMenu();
    setupTriggers();
}

// ---------------- Menu ----------------
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

// ---------------- Triggers ----------------
void Game::setupTriggers() {
    triggers_.push_back({ sf::FloatRect({0.f, 0.f}, {200.f, 600.f}), "rue", "Tu sors de la maison !" });
    triggers_.push_back({ sf::FloatRect({200.f, 0.f}, {400.f, 600.f}), "gare", "Une manifestation bloque la rue !"});
    triggers_.push_back({ sf::FloatRect({600.f, 0.f}, {200.f, 600.f}), "train", "Le train est en retard..."});
    triggers_.push_back({ sf::FloatRect({750.f, 500.f}, {50.f, 100.f}), "ecole", "Pas de cours aujourd'hui !"});
}

bool Game::checkCollision(const sf::FloatRect& playerBounds, const sf::FloatRect& zoneBounds) {
    auto containsPoint = [&](sf::Vector2f point) {
        return point.x >= zoneBounds.position.x &&
            point.x <= zoneBounds.position.x + zoneBounds.size.x &&
            point.y >= zoneBounds.position.y &&
            point.y <= zoneBounds.position.y + zoneBounds.size.y;
        };

    sf::Vector2f topLeft = playerBounds.position;
    sf::Vector2f topRight = { playerBounds.position.x + playerBounds.size.x, playerBounds.position.y };
    sf::Vector2f bottomLeft = { playerBounds.position.x, playerBounds.position.y + playerBounds.size.y };
    sf::Vector2f bottomRight = { playerBounds.position.x + playerBounds.size.x, playerBounds.position.y + playerBounds.size.y };

    return containsPoint(topLeft) || containsPoint(topRight) ||
        containsPoint(bottomLeft) || containsPoint(bottomRight);
}

// ---------------- Boucle ----------------
void Game::run() {
    while (window_.isOpen()) {
        sf::Time dt = clock_.restart();
        processEvents();
        update(dt);
        render();
    }
}

// ---------------- Événements ----------------
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

// ---------------- Update ----------------
void Game::update(sf::Time dt) {
    if (state_ == GameState::Playing && !gameFinished_) {
        player_.update(dt);

        // Porte maison -> rue
        if (!fading_ && currentArea_ == "maison") {
            if (checkCollision(player_.getGlobalBounds(), doorRect_.getGlobalBounds())) {
                nextArea_ = "rue";
                eventText_->setString("Tu sors de la maison !");
                fading_ = true;
                fadeIn_ = false;
            }
        }

        // Porte rue -> maison
        if (!fading_ && currentArea_ == "rue") {
            if (checkCollision(player_.getGlobalBounds(), doorRueRect_.getGlobalBounds())) {
                nextArea_ = "maison";
                eventText_->setString("Tu rentres à la maison !");
                fading_ = true;
                fadeIn_ = false;
            }
        }

        // Triggers (chaque trigger ne se déclenche qu’une seule fois)
        if (!fading_ && currentArea_ != "maison") {
            for (auto& trigger : triggers_) {
                if (!trigger.used && checkCollision(player_.getGlobalBounds(), trigger.bounds)) {
                    nextArea_ = trigger.targetArea;
                    eventText_->setString(trigger.eventText);
                    fading_ = true;
                    fadeIn_ = false;
                    trigger.used = true;
                    if (nextArea_ == "ecole") gameFinished_ = true;
                }
            }
        }
    }

    // Gestion fade
    if (fading_) {
        if (!fadeIn_) { // fade-out
            fadeAlpha_ += fadeSpeed_ * dt.asSeconds();
            if (fadeAlpha_ >= 255.f) {
                fadeAlpha_ = 255.f;
                currentArea_ = nextArea_;
                fadeIn_ = true;
            }
        }
        else { // fade-in
            fadeAlpha_ -= fadeSpeed_ * dt.asSeconds();
            if (fadeAlpha_ <= 0.f) {
                fadeAlpha_ = 0.f;
                fading_ = false;
            }
        }
    }
}

// ---------------- Render ----------------
void Game::render() {
    if (state_ == GameState::Menu) window_.clear(sf::Color{ 50, 50, 50 });
    else if (currentArea_ == "maison") window_.clear(sf::Color{ 200, 180, 150 });
    else if (currentArea_ == "rue") window_.clear(sf::Color{ 100, 150, 200 });
    else if (currentArea_ == "gare") window_.clear(sf::Color{ 180, 180, 180 });
    else if (currentArea_ == "train") window_.clear(sf::Color{ 150, 150, 100 });
    else if (currentArea_ == "ecole") window_.clear(sf::Color{ 50, 200, 50 });

    if (state_ == GameState::Menu) {
        if (titleText_) window_.draw(*titleText_);
        window_.draw(playButtonRect_);
        if (playButtonText_) window_.draw(*playButtonText_);
    }
    else if (state_ == GameState::Playing) {
        player_.draw(window_);
        if (eventText_) window_.draw(*eventText_);

        if (currentArea_ == "maison") window_.draw(doorRect_);
        if (currentArea_ == "rue") window_.draw(doorRueRect_);

        if (gameFinished_) {
            sf::Text endText(font_, "Bravo ! Nolan n'a pas cours aujourd'hui !", 32u);
            endText.setFillColor(sf::Color::Yellow);
            endText.setPosition({ 100.f, 300.f });
            window_.draw(endText);
        }
    }

    // Fade rectangle
    if (fading_) {
        sf::RectangleShape fadeRect({ 800.f, 600.f });
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<unsigned char>(fadeAlpha_)));
        window_.draw(fadeRect);
    }

    window_.display();
}
