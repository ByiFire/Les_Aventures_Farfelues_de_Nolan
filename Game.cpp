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

// ---------- Player ----------
Player::Player(sf::Vector2f size, float speed) : speed_(speed), direction_(Direction::Right) {
    shape_.setSize(size);
    shape_.setFillColor(sf::Color::Cyan);
    shape_.setPosition({ 380.f, 500.f });
}

void Player::update(sf::Time dt) {
    sf::Vector2f movement{ 0.f, 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        movement.y -= speed_ * dt.asSeconds();
        direction_ = Direction::Up;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        movement.y += speed_ * dt.asSeconds();
        direction_ = Direction::Down;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        movement.x -= speed_ * dt.asSeconds();
        direction_ = Direction::Left;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        movement.x += speed_ * dt.asSeconds();
        direction_ = Direction::Right;
    }

    shape_.move(movement);

    sf::Vector2f pos = shape_.getPosition();
    if (pos.x < 0.f) shape_.setPosition({ 0.f, pos.y });
    if (pos.y < 0.f) shape_.setPosition({ pos.x, 0.f });
    if (pos.x + shape_.getSize().x > WINDOW_WIDTH) shape_.setPosition({ WINDOW_WIDTH - shape_.getSize().x, pos.y });
    if (pos.y + shape_.getSize().y > WINDOW_HEIGHT) shape_.setPosition({ pos.x, WINDOW_HEIGHT - shape_.getSize().y });
}

void Player::draw(sf::RenderWindow& window) {
    sf::Vector2f pos = shape_.getPosition();

    sf::CircleShape head(12.f);
    head.setFillColor(sf::Color(210, 180, 140));
    head.setPosition({ pos.x + 8.f, pos.y + 8.f });
    window.draw(head);

    sf::CircleShape shoulder1(9.f);
    shoulder1.setFillColor(sf::Color(255, 192, 203));

    sf::CircleShape shoulder2(9.f);
    shoulder2.setFillColor(sf::Color(255, 192, 203));

    switch (direction_) {
    case Direction::Right:
        shoulder1.setScale({ 0.6f, 1.2f });
        shoulder1.setPosition({ pos.x, pos.y + 14.f });
        shoulder2.setScale({ 0.6f, 1.2f });
        shoulder2.setPosition({ pos.x + 29.f, pos.y + 14.f });
        break;
    case Direction::Left:
        shoulder1.setScale({ 0.6f, 1.2f });
        shoulder1.setPosition({ pos.x + 29.f, pos.y + 14.f });
        shoulder2.setScale({ 0.6f, 1.2f });
        shoulder2.setPosition({ pos.x, pos.y + 14.f });
        break;
    case Direction::Up:
        shoulder1.setScale({ 1.2f, 0.6f });
        shoulder1.setPosition({ pos.x + 4.f, pos.y + 20.f });
        shoulder2.setScale({ 1.2f, 0.6f });
        shoulder2.setPosition({ pos.x + 20.f, pos.y + 20.f });
        break;
    case Direction::Down:
        shoulder1.setScale({ 1.2f, 0.6f });
        shoulder1.setPosition({ pos.x + 4.f, pos.y + 16.f });
        shoulder2.setScale({ 1.2f, 0.6f });
        shoulder2.setPosition({ pos.x + 20.f, pos.y + 16.f });
        break;
    }

    window.draw(shoulder1);
    window.draw(shoulder2);

    sf::CircleShape eye1(2.f);
    sf::CircleShape eye2(2.f);
    eye1.setFillColor(sf::Color::Black);
    eye2.setFillColor(sf::Color::Black);

    switch (direction_) {
    case Direction::Right:
        eye1.setPosition({ pos.x + 24.f, pos.y + 14.f });
        window.draw(eye1);
        break;
    case Direction::Left:
        eye1.setPosition({ pos.x + 14.f, pos.y + 14.f });
        window.draw(eye1);
        break;
    case Direction::Up:
        break;
    case Direction::Down:
        eye1.setPosition({ pos.x + 14.f, pos.y + 16.f });
        eye2.setPosition({ pos.x + 24.f, pos.y + 16.f });
        window.draw(eye1);
        window.draw(eye2);
        break;
    }
}

sf::FloatRect Player::getGlobalBounds() const { return shape_.getGlobalBounds(); }

void Player::setPosition(sf::Vector2f pos) {
    shape_.setPosition(pos);
}

sf::Vector2f Player::getPosition() const {
    return shape_.getPosition();
}

// ---------- Game ----------
Game::Game()
    : window_(sf::VideoMode({ 800, 600 }), "Les aventures farfelues de Nolan")
    , state_(GameState::Menu)
    , currentArea_("maison")
    , gameFinished_(false)
    , fading_(false)
    , fadeAlpha_(0.f)
    , fadeSpeed_(600.f)
    , fadeIn_(false)
    , currentNPCInRange_(nullptr)
    , creditsStarted_(false)
    , creditsScroll_(600.f)
    , creditsSpeed_(50.f)

{
    if (!font_.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Impossible de charger la police\n";
    }

    player_ = std::make_unique<Player>(sf::Vector2f{ PLAYER_SIZE, PLAYER_SIZE }, 200.f);
    hud_ = std::make_unique<HUD>();
    hud_->setFont(font_);
    hud_->setCurrentArea(currentArea_);

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
    setupPauseMenu();
    setupTriggers();
    setupFurniture();
    setupStreetDecor();
    setupManifDecor();
    setupGareDecor();
    setupTrainDecor();
    setupSchoolDecor();
    setupNPCs();
    setupCredits();
}

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

void Game::setupPauseMenu() {
    // Overlay semi-transparent
    pauseOverlay_.setSize({ 800.f, 600.f });
    pauseOverlay_.setFillColor(sf::Color(0, 0, 0, 180));
    pauseOverlay_.setPosition({ 0.f, 0.f });

    // Titre "PAUSE"
    pauseTitleText_.emplace(font_, "PAUSE", 64u);
    pauseTitleText_->setFillColor(sf::Color::White);
    pauseTitleText_->setPosition({ 300.f, 150.f });

    // Bouton Reprendre
    resumeButtonRect_.setSize({ 200.f, 60.f });
    resumeButtonRect_.setFillColor(sf::Color(100, 200, 100));
    resumeButtonRect_.setPosition({ 300.f, 300.f });

    resumeButtonText_.emplace(font_, "REPRENDRE", 28u);
    resumeButtonText_->setFillColor(sf::Color::Black);
    resumeButtonText_->setPosition({ 315.f, 315.f });

    // Bouton Quitter
    quitButtonRect_.setSize({ 200.f, 60.f });
    quitButtonRect_.setFillColor(sf::Color(200, 100, 100));
    quitButtonRect_.setPosition({ 300.f, 400.f });

    quitButtonText_.emplace(font_, "QUITTER", 28u);
    quitButtonText_->setFillColor(sf::Color::Black);
    quitButtonText_->setPosition({ 335.f, 415.f });
}

void Game::setupTriggers() {
    triggers_.clear();

    // Maison -> Rue (porte du haut)
    triggers_.push_back({
        sf::FloatRect({(WINDOW_WIDTH - DOOR_WIDTH) / 2.f, -50.f}, {DOOR_WIDTH, 60.f}),
        "maison", "rue", false
        });

    // Rue -> Maison (porte du bas, dans la rue)
    triggers_.push_back({
        sf::FloatRect({140.f, 590.f}, {DOOR_WIDTH, DOOR_HEIGHT}),
        "rue", "maison", false
        });

    // Rue -> Rue_manif (côté droit)
    triggers_.push_back({
        sf::FloatRect({WINDOW_WIDTH - 20.f, 400.f}, {20.f, 200.f}),
        "rue", "rue_manif", false
        });

    // Rue_manif -> Rue (côté gauche)
    triggers_.push_back({
        sf::FloatRect({-50.f, 400.f}, {60.f, 200.f}),
        "rue_manif", "rue", false
        });

    // Rue_manif -> Gare (sur les escaliers en bas à droite)
    triggers_.push_back({
        sf::FloatRect({660.f, 490.f}, {120.f, 110.f}),
        "rue_manif", "gare", false
        });

    // Gare -> Rue_manif (escalier du haut)
    triggers_.push_back({
        sf::FloatRect({350.f, 0.f}, {100.f, 120.f}),
        "gare", "rue_manif", false
        });

    // Gare -> Train_interieur (monter dans le train de droite)
    triggers_.push_back({
        sf::FloatRect({600.f, 280.f}, {50.f, 90.f}),
        "gare", "train_interieur", false
        });

    // Train_interieur -> Ecole (sortie du train - à la limite entre la porte et la bande noire)
    triggers_.push_back({
        sf::FloatRect({78.f, WINDOW_HEIGHT / 2.f - 60.f}, {10.f, 120.f}),
        "train_interieur", "ecole", false
        });
}

void Game::setupFurniture() {
    furniture_.clear();

    sf::RectangleShape bed1({ 120.f, 80.f });
    bed1.setFillColor(sf::Color(200, 100, 100));
    bed1.setPosition({ 600.f, 100.f });
    furniture_.push_back(bed1);

    sf::RectangleShape bed2({ 110.f, 10.f });
    bed2.setFillColor(sf::Color(150, 50, 50));
    bed2.setPosition({ 605.f, 85.f });
    furniture_.push_back(bed2);

    sf::RectangleShape desk({ 100.f, 70.f });
    desk.setFillColor(sf::Color(139, 69, 19));
    desk.setPosition({ 600.f, 300.f });
    furniture_.push_back(desk);

    sf::RectangleShape chairSeat({ 40.f, 40.f });
    chairSeat.setFillColor(sf::Color(100, 100, 100));
    chairSeat.setPosition({ 520.f, 320.f });
    furniture_.push_back(chairSeat);

    sf::RectangleShape secondFloor({ 200.f, 60.f });
    secondFloor.setFillColor(sf::Color(50, 50, 50));
    secondFloor.setPosition({ 0.f, 0.f });
    furniture_.push_back(secondFloor);

    sf::RectangleShape floorEdge({ 200.f, 3.f });
    floorEdge.setFillColor(sf::Color(30, 30, 30));
    floorEdge.setPosition({ 0.f, 60.f });
    furniture_.push_back(floorEdge);

    for (int i = 0; i < 15; i++) {
        sf::RectangleShape stair({ 200.f, 15.f });
        stair.setFillColor(sf::Color(139, 69, 19));
        stair.setPosition({ 0.f, 60.f + (i * 20.f) });
        furniture_.push_back(stair);
    }

    sf::RectangleShape rugFront({ 150.f, 80.f });
    rugFront.setFillColor(sf::Color(200, 150, 100));
    rugFront.setPosition({ 325.f, 450.f });
    furniture_.push_back(rugFront);

    sf::RectangleShape window1({ 60.f, 40.f });
    window1.setFillColor(sf::Color(100, 200, 255));
    window1.setPosition({ 640.f, 20.f });
    furniture_.push_back(window1);

    sf::RectangleShape kitchenCounter({ 120.f, 80.f });
    kitchenCounter.setFillColor(sf::Color(160, 160, 160));
    kitchenCounter.setPosition({ 620.f, 480.f });
    furniture_.push_back(kitchenCounter);

    sf::RectangleShape stove({ 40.f, 40.f });
    stove.setFillColor(sf::Color(50, 50, 50));
    stove.setPosition({ 650.f, 490.f });
    furniture_.push_back(stove);

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

void Game::setupStreetDecor() {
    streetDecor_.clear();

    // ========== RUE - ZONES DE BASE ==========

    // Trottoir du haut
    sf::RectangleShape sidewalkTop({ 800.f, 80.f });
    sidewalkTop.setFillColor(sf::Color(180, 180, 180));
    sidewalkTop.setPosition({ 0.f, 0.f });
    streetDecor_.push_back(sidewalkTop);

    // Herbe (après le trottoir du haut)
    sf::RectangleShape grass({ 800.f, 120.f });
    grass.setFillColor(sf::Color(34, 139, 34));
    grass.setPosition({ 0.f, 80.f });
    streetDecor_.push_back(grass);

    // Route (au centre, rétrécie)
    sf::RectangleShape road({ 800.f, 200.f });
    road.setFillColor(sf::Color(70, 70, 70));
    road.setPosition({ 0.f, 200.f });
    streetDecor_.push_back(road);

    // Lignes blanches sur la route (horizontales)
    for (int i = 0; i < 20; i++) {
        sf::RectangleShape roadLine({ 30.f, 5.f });
        roadLine.setFillColor(sf::Color::White);
        roadLine.setPosition({ static_cast<float>(i * 45), 297.5f });
        streetDecor_.push_back(roadLine);
    }

    // Trottoir du bas
    sf::RectangleShape sidewalkBottom({ 800.f, 200.f });
    sidewalkBottom.setFillColor(sf::Color(180, 180, 180));
    sidewalkBottom.setPosition({ 0.f, 400.f });
    streetDecor_.push_back(sidewalkBottom);

    // ========== CLÔTURE (horizontale entre herbe et route) ==========

    // Poteaux de clôture
    for (int i = 0; i < 16; i++) {
        sf::RectangleShape fencePost({ 60.f, 8.f });
        fencePost.setFillColor(sf::Color(139, 90, 43));
        fencePost.setPosition({ static_cast<float>(i * 50), 192.f });
        streetDecor_.push_back(fencePost);
    }

    // Barres verticales de la clôture
    for (int i = 0; i < 16; i++) {
        // Barre gauche
        sf::RectangleShape fenceBar1({ 5.f, 12.f });
        fenceBar1.setFillColor(sf::Color(160, 110, 60));
        fenceBar1.setPosition({ static_cast<float>(i * 50 + 15), 188.f });
        streetDecor_.push_back(fenceBar1);

        // Barre droite
        sf::RectangleShape fenceBar2({ 5.f, 12.f });
        fenceBar2.setFillColor(sf::Color(160, 110, 60));
        fenceBar2.setPosition({ static_cast<float>(i * 50 + 40), 188.f });
        streetDecor_.push_back(fenceBar2);
    }

    // ========== DÉCORS TROTTOIR DU HAUT ==========

    // Lampadaires sur le trottoir du haut (2)
    for (int i = 0; i < 2; i++) {
        // Poteau
        sf::RectangleShape lampPost({ 8.f, 60.f });
        lampPost.setFillColor(sf::Color(80, 80, 80));
        lampPost.setPosition({ static_cast<float>(200 + i * 350), 10.f });
        streetDecor_.push_back(lampPost);

        // Lampe
        sf::CircleShape lamp(12.f);
        lamp.setFillColor(sf::Color(255, 255, 150));
        lamp.setPosition({ static_cast<float>(192 + i * 350), 0.f });
        streetDecor_.push_back(lamp);
    }

    // Banc sur le trottoir du haut
    sf::RectangleShape benchSeatTop({ 60.f, 20.f });
    benchSeatTop.setFillColor(sf::Color(139, 69, 19));
    benchSeatTop.setPosition({ 500.f, 35.f });
    streetDecor_.push_back(benchSeatTop);

    sf::RectangleShape benchLeg1Top({ 8.f, 15.f });
    benchLeg1Top.setFillColor(sf::Color(80, 80, 80));
    benchLeg1Top.setPosition({ 505.f, 55.f });
    streetDecor_.push_back(benchLeg1Top);

    sf::RectangleShape benchLeg2Top({ 8.f, 15.f });
    benchLeg2Top.setFillColor(sf::Color(80, 80, 80));
    benchLeg2Top.setPosition({ 547.f, 55.f });
    streetDecor_.push_back(benchLeg2Top);

    // ========== DÉCORS SUR L'HERBE ==========

    // Buissons sur l'herbe (5 buissons)
    for (int i = 0; i < 5; i++) {
        sf::CircleShape bush(20.f);
        bush.setFillColor(sf::Color(20, 100, 20));
        bush.setPosition({ 80.f + (i * 140.f), 110.f + ((i % 2) * 30.f) });
        streetDecor_.push_back(bush);
    }

    // Petites fleurs (touches de couleur sur l'herbe)
    for (int i = 0; i < 8; i++) {
        sf::CircleShape flower(4.f);
        flower.setFillColor(sf::Color(255, 100 + (i * 15), 150));
        flower.setPosition({
            120.f + (i * 80.f),
            130.f + ((i % 2) * 20.f)
            });
        streetDecor_.push_back(flower);
    }

    // Arbre sur l'herbe
    sf::RectangleShape treeTrunk({ 15.f, 40.f });
    treeTrunk.setFillColor(sf::Color(101, 67, 33));
    treeTrunk.setPosition({ 50.f, 140.f });
    streetDecor_.push_back(treeTrunk);

    sf::CircleShape treeLeaves(25.f);
    treeLeaves.setFillColor(sf::Color(34, 139, 34));
    treeLeaves.setPosition({ 32.f, 110.f });
    streetDecor_.push_back(treeLeaves);

    // ========== DÉCORS TROTTOIR DU BAS ==========

    // Poubelles sur le trottoir du bas (3 poubelles)
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape bin({ 25.f, 35.f });
        bin.setFillColor(sf::Color(40, 40, 40));
        bin.setPosition({ static_cast<float>(100 + i * 280), 520.f });
        streetDecor_.push_back(bin);

        // Couvercle de poubelle
        sf::RectangleShape binLid({ 28.f, 5.f });
        binLid.setFillColor(sf::Color(60, 60, 60));
        binLid.setPosition({ static_cast<float>(98.5f + i * 280), 518.f });
        streetDecor_.push_back(binLid);
    }

    // Lampadaires sur le trottoir du bas (2)
    for (int i = 0; i < 2; i++) {
        // Poteau
        sf::RectangleShape lampPost({ 8.f, 100.f });
        lampPost.setFillColor(sf::Color(80, 80, 80));
        lampPost.setPosition({ static_cast<float>(250 + i * 300), 450.f });
        streetDecor_.push_back(lampPost);

        // Lampe
        sf::CircleShape lamp(12.f);
        lamp.setFillColor(sf::Color(255, 255, 150));
        lamp.setPosition({ static_cast<float>(242 + i * 300), 440.f });
        streetDecor_.push_back(lamp);
    }

    // Bancs sur le trottoir du bas (2 bancs)
    for (int i = 0; i < 2; i++) {
        // Assise du banc
        sf::RectangleShape benchSeat({ 60.f, 20.f });
        benchSeat.setFillColor(sf::Color(139, 69, 19));
        benchSeat.setPosition({ static_cast<float>(150 + i * 450), 500.f });
        streetDecor_.push_back(benchSeat);

        // Pieds du banc
        sf::RectangleShape benchLeg1({ 8.f, 15.f });
        benchLeg1.setFillColor(sf::Color(80, 80, 80));
        benchLeg1.setPosition({ static_cast<float>(155 + i * 450), 520.f });
        streetDecor_.push_back(benchLeg1);

        sf::RectangleShape benchLeg2({ 8.f, 15.f });
        benchLeg2.setFillColor(sf::Color(80, 80, 80));
        benchLeg2.setPosition({ static_cast<float>(197 + i * 450), 520.f });
        streetDecor_.push_back(benchLeg2);
    }

    // Panneau de signalisation
    sf::RectangleShape signPost({ 6.f, 80.f });
    signPost.setFillColor(sf::Color(80, 80, 80));
    signPost.setPosition({ 690.f, 470.f });
    streetDecor_.push_back(signPost);

    sf::RectangleShape signBoard({ 40.f, 40.f });
    signBoard.setFillColor(sf::Color(255, 255, 255));
    signBoard.setPosition({ 670.f, 440.f });
    streetDecor_.push_back(signBoard);

    sf::CircleShape signBorder(22.f);
    signBorder.setFillColor(sf::Color::Transparent);
    signBorder.setOutlineColor(sf::Color::Red);
    signBorder.setOutlineThickness(3.f);
    signBorder.setPosition({ 668.f, 438.f });
    streetDecor_.push_back(signBorder);
}

void Game::setupManifDecor() {
    manifDecor_.clear();

    // ========== MÊME BASE QUE LA RUE NORMALE ==========

    // Trottoir du haut
    sf::RectangleShape sidewalkTop({ 800.f, 80.f });
    sidewalkTop.setFillColor(sf::Color(180, 180, 180));
    sidewalkTop.setPosition({ 0.f, 0.f });
    manifDecor_.push_back(sidewalkTop);

    // Herbe
    sf::RectangleShape grass({ 800.f, 120.f });
    grass.setFillColor(sf::Color(34, 139, 34));
    grass.setPosition({ 0.f, 80.f });
    manifDecor_.push_back(grass);

    // Route
    sf::RectangleShape road({ 800.f, 200.f });
    road.setFillColor(sf::Color(70, 70, 70));
    road.setPosition({ 0.f, 200.f });
    manifDecor_.push_back(road);

    // Lignes blanches
    for (int i = 0; i < 20; i++) {
        sf::RectangleShape roadLine({ 30.f, 5.f });
        roadLine.setFillColor(sf::Color::White);
        roadLine.setPosition({ static_cast<float>(i * 45), 297.5f });
        manifDecor_.push_back(roadLine);
    }

    // Trottoir du bas
    sf::RectangleShape sidewalkBottom({ 800.f, 200.f });
    sidewalkBottom.setFillColor(sf::Color(180, 180, 180));
    sidewalkBottom.setPosition({ 0.f, 400.f });
    manifDecor_.push_back(sidewalkBottom);

    // Clôture
    for (int i = 0; i < 16; i++) {
        sf::RectangleShape fencePost({ 60.f, 8.f });
        fencePost.setFillColor(sf::Color(139, 90, 43));
        fencePost.setPosition({ static_cast<float>(i * 50), 192.f });
        manifDecor_.push_back(fencePost);

        sf::RectangleShape fenceBar1({ 5.f, 12.f });
        fenceBar1.setFillColor(sf::Color(160, 110, 60));
        fenceBar1.setPosition({ static_cast<float>(i * 50 + 15), 188.f });
        manifDecor_.push_back(fenceBar1);

        sf::RectangleShape fenceBar2({ 5.f, 12.f });
        fenceBar2.setFillColor(sf::Color(160, 110, 60));
        fenceBar2.setPosition({ static_cast<float>(i * 50 + 40), 188.f });
        manifDecor_.push_back(fenceBar2);
    }

    // ========== BARRIÈRE DE MANIFESTATION AU CENTRE ==========

    // Barrières métalliques (4 sections au centre de la route)
    for (int i = 0; i < 4; i++) {
        // Base de la barrière
        sf::RectangleShape barrierBase({ 120.f, 15.f });
        barrierBase.setFillColor(sf::Color(200, 200, 200));
        barrierBase.setPosition({ 200.f + (i * 130.f), 315.f });
        manifDecor_.push_back(barrierBase);

        // Poteaux verticaux
        for (int j = 0; j < 3; j++) {
            sf::RectangleShape post({ 8.f, 80.f });
            post.setFillColor(sf::Color(150, 150, 150));
            post.setPosition({ 210.f + (i * 130.f) + (j * 50.f), 240.f });
            manifDecor_.push_back(post);
        }

        // Barres horizontales orange
        sf::RectangleShape bar1({ 110.f, 5.f });
        bar1.setFillColor(sf::Color(255, 100, 0));
        bar1.setPosition({ 205.f + (i * 130.f), 260.f });
        manifDecor_.push_back(bar1);

        sf::RectangleShape bar2({ 110.f, 5.f });
        bar2.setFillColor(sf::Color(255, 100, 0));
        bar2.setPosition({ 205.f + (i * 130.f), 290.f });
        manifDecor_.push_back(bar2);
    }

    // Panneau "ROUTE FERMÉE"
    sf::RectangleShape signBoard({ 180.f, 50.f });
    signBoard.setFillColor(sf::Color(255, 50, 50));
    signBoard.setPosition({ 310.f, 220.f });
    manifDecor_.push_back(signBoard);

    sf::RectangleShape signBorder({ 180.f, 50.f });
    signBorder.setFillColor(sf::Color::Transparent);
    signBorder.setOutlineColor(sf::Color::White);
    signBorder.setOutlineThickness(3.f);
    signBorder.setPosition({ 310.f, 220.f });
    manifDecor_.push_back(signBorder);

    // ========== CAGE D'ESCALIER EN BAS À DROITE ==========

    // Sol autour de l'escalier
    sf::RectangleShape stairPlatform({ 150.f, 120.f });
    stairPlatform.setFillColor(sf::Color(140, 140, 140));
    stairPlatform.setPosition({ 650.f, 480.f });
    manifDecor_.push_back(stairPlatform);

    // Bordure de l'escalier
    sf::RectangleShape stairBorder({ 150.f, 120.f });
    stairBorder.setFillColor(sf::Color::Transparent);
    stairBorder.setOutlineColor(sf::Color(80, 80, 80));
    stairBorder.setOutlineThickness(3.f);
    stairBorder.setPosition({ 650.f, 480.f });
    manifDecor_.push_back(stairBorder);

    // Marches d'escalier (qui descendent)
    for (int i = 0; i < 8; i++) {
        sf::RectangleShape step({ 120.f, 12.f });
        step.setFillColor(sf::Color(100 - i * 5, 100 - i * 5, 100 - i * 5));
        step.setPosition({ 660.f, 490.f + (i * 13.f) });
        manifDecor_.push_back(step);

        // Petite ombre pour effet 3D
        sf::RectangleShape shadow({ 120.f, 2.f });
        shadow.setFillColor(sf::Color(40, 40, 40));
        shadow.setPosition({ 660.f, 501.f + (i * 13.f) });
        manifDecor_.push_back(shadow);
    }

    // Rampe gauche
    sf::RectangleShape railLeft({ 8.f, 100.f });
    railLeft.setFillColor(sf::Color(180, 180, 180));
    railLeft.setPosition({ 655.f, 490.f });
    manifDecor_.push_back(railLeft);

    // Rampe droite
    sf::RectangleShape railRight({ 8.f, 100.f });
    railRight.setFillColor(sf::Color(180, 180, 180));
    railRight.setPosition({ 777.f, 490.f });
    manifDecor_.push_back(railRight);

    // Flèche vers le bas pour indiquer la descente
    sf::CircleShape arrowCircle(20.f);
    arrowCircle.setFillColor(sf::Color(50, 150, 255));
    arrowCircle.setPosition({ 705.f, 440.f });
    manifDecor_.push_back(arrowCircle);

    // Triangle pour la flèche
    sf::RectangleShape arrowDown({ 15.f, 25.f });
    arrowDown.setFillColor(sf::Color::White);
    arrowDown.setPosition({ 717.f, 450.f });
    manifDecor_.push_back(arrowDown);

    // Quelques pancartes abandonnées sur le trottoir
    for (int i = 0; i < 3; i++) {
        // Manche de pancarte
        sf::RectangleShape signStick({ 4.f, 60.f });
        signStick.setFillColor(sf::Color(139, 90, 43));
        signStick.setPosition({ 100.f + (i * 150.f), 520.f });
        manifDecor_.push_back(signStick);

        // Pancarte
        sf::RectangleShape signCard({ 50.f, 40.f });
        signCard.setFillColor(sf::Color(255, 255, 200));
        signCard.setPosition({ 77.f + (i * 150.f), 490.f });
        manifDecor_.push_back(signCard);
    }

    // Cônes de signalisation
    for (int i = 0; i < 5; i++) {
        sf::CircleShape cone(12.f, 3);
        cone.setFillColor(sf::Color(255, 100, 0));
        cone.setPosition({ 180.f + (i * 120.f), 325.f });
        manifDecor_.push_back(cone);

        // Bande blanche sur le cône
        sf::CircleShape coneStripe(10.f, 3);
        coneStripe.setFillColor(sf::Color::White);
        coneStripe.setPosition({ 182.f + (i * 120.f), 330.f });
        manifDecor_.push_back(coneStripe);
    }
}

void Game::setupGareDecor() {
    gareDecor_.clear();

    // ========== QUAI CENTRAL (SOL) ==========
    sf::RectangleShape quaiCentral({ 360.f, 600.f });
    quaiCentral.setFillColor(sf::Color(160, 160, 160));
    quaiCentral.setPosition({ 220.f, 0.f });
    gareDecor_.push_back(quaiCentral);

    // Motif de carrelage sur le quai
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 6; j++) {
            if ((i + j) % 2 == 0) {
                sf::RectangleShape tile({ 60.f, 50.f });
                tile.setFillColor(sf::Color(150, 150, 150));
                tile.setPosition({ 220.f + j * 60.f, i * 50.f });
                gareDecor_.push_back(tile);
            }
        }
    }

    // ========== ESCALIER EN HAUT ==========

    // Structure principale escalier (murs latéraux)
    sf::RectangleShape murGauche({ 10.f, 120.f });
    murGauche.setFillColor(sf::Color(100, 100, 100));
    murGauche.setPosition({ 340.f, 0.f });
    gareDecor_.push_back(murGauche);

    sf::RectangleShape murDroit({ 10.f, 120.f });
    murDroit.setFillColor(sf::Color(100, 100, 100));
    murDroit.setPosition({ 450.f, 0.f });
    gareDecor_.push_back(murDroit);

    // Marches descendantes avec effet 3D
    for (int i = 0; i < 10; i++) {
        // Marche
        sf::RectangleShape marche({ 100.f, 12.f });
        marche.setFillColor(sf::Color(130 - i * 3, 130 - i * 3, 130 - i * 3));
        marche.setPosition({ 350.f, i * 12.f });
        gareDecor_.push_back(marche);

        // Ombre de marche
        sf::RectangleShape ombre({ 100.f, 2.f });
        ombre.setFillColor(sf::Color(50, 50, 50));
        ombre.setPosition({ 350.f, (i + 1) * 12.f - 2.f });
        gareDecor_.push_back(ombre);
    }

    // Rampes avec détails
    sf::RectangleShape rampeG({ 6.f, 120.f });
    rampeG.setFillColor(sf::Color(200, 200, 200));
    rampeG.setPosition({ 345.f, 0.f });
    gareDecor_.push_back(rampeG);

    sf::RectangleShape rampeD({ 6.f, 120.f });
    rampeD.setFillColor(sf::Color(200, 200, 200));
    rampeD.setPosition({ 449.f, 0.f });
    gareDecor_.push_back(rampeD);

    // Poteaux de rampe
    for (int i = 0; i < 5; i++) {
        sf::RectangleShape poteau({ 4.f, 25.f });
        poteau.setFillColor(sf::Color(150, 150, 150));
        poteau.setPosition({ 343.f, i * 25.f });
        gareDecor_.push_back(poteau);

        sf::RectangleShape poteau2({ 4.f, 25.f });
        poteau2.setFillColor(sf::Color(150, 150, 150));
        poteau2.setPosition({ 451.f, i * 25.f });
        gareDecor_.push_back(poteau2);
    }

    // ========== TRAIN GAUCHE (DÉCORATIF) ==========

    // Rails gauche
    for (int i = 0; i < 20; i++) {
        sf::RectangleShape traverse({ 180.f, 6.f });
        traverse.setFillColor(sf::Color(80, 60, 40));
        traverse.setPosition({ 30.f, i * 30.f });
        gareDecor_.push_back(traverse);
    }

    // Corps du train
    sf::RectangleShape trainG({ 180.f, 600.f });
    trainG.setFillColor(sf::Color(220, 50, 50));
    trainG.setPosition({ 30.f, 0.f });
    gareDecor_.push_back(trainG);

    // Bande décorative
    sf::RectangleShape bandeG({ 180.f, 20.f });
    bandeG.setFillColor(sf::Color(180, 180, 180));
    bandeG.setPosition({ 30.f, 30.f });
    gareDecor_.push_back(bandeG);

    // Fenêtres avec cadres
    for (int i = 0; i < 7; i++) {
        // Cadre fenêtre
        sf::RectangleShape cadre({ 70.f, 70.f });
        cadre.setFillColor(sf::Color(100, 100, 100));
        cadre.setPosition({ 80.f, 60.f + i * 80.f });
        gareDecor_.push_back(cadre);

        // Vitre
        sf::RectangleShape vitre({ 60.f, 60.f });
        vitre.setFillColor(sf::Color(150, 200, 255));
        vitre.setPosition({ 85.f, 65.f + i * 80.f });
        gareDecor_.push_back(vitre);

        // Reflet
        sf::RectangleShape reflet({ 25.f, 25.f });
        reflet.setFillColor(sf::Color(200, 230, 255, 150));
        reflet.setPosition({ 90.f, 70.f + i * 80.f });
        gareDecor_.push_back(reflet);
    }

    // Portes train gauche (vers le bord intérieur)
    sf::RectangleShape porteG1({ 50.f, 90.f });
    porteG1.setFillColor(sf::Color(180, 180, 180));
    porteG1.setPosition({ 150.f, 220.f });
    gareDecor_.push_back(porteG1);

    sf::RectangleShape porteG2({ 50.f, 90.f });
    porteG2.setFillColor(sf::Color(180, 180, 180));
    porteG2.setPosition({ 150.f, 420.f });
    gareDecor_.push_back(porteG2);

    // Bordure sécurité gauche
    sf::RectangleShape bordureG({ 8.f, 600.f });
    bordureG.setFillColor(sf::Color::Yellow);
    bordureG.setPosition({ 212.f, 0.f });
    gareDecor_.push_back(bordureG);

    // ========== TRAIN DROIT (INTERACTIF) ==========

    // Rails droite
    for (int i = 0; i < 20; i++) {
        sf::RectangleShape traverse({ 180.f, 6.f });
        traverse.setFillColor(sf::Color(80, 60, 40));
        traverse.setPosition({ 590.f, i * 30.f });
        gareDecor_.push_back(traverse);
    }

    // Corps du train
    sf::RectangleShape trainD({ 180.f, 600.f });
    trainD.setFillColor(sf::Color(50, 100, 220));
    trainD.setPosition({ 590.f, 0.f });
    gareDecor_.push_back(trainD);

    // Bande décorative
    sf::RectangleShape bandeD({ 180.f, 20.f });
    bandeD.setFillColor(sf::Color(180, 180, 180));
    bandeD.setPosition({ 590.f, 30.f });
    gareDecor_.push_back(bandeD);

    // Fenêtres avec cadres
    for (int i = 0; i < 7; i++) {
        // Cadre fenêtre
        sf::RectangleShape cadre({ 70.f, 70.f });
        cadre.setFillColor(sf::Color(100, 100, 100));
        cadre.setPosition({ 640.f, 60.f + i * 80.f });
        gareDecor_.push_back(cadre);

        // Vitre
        sf::RectangleShape vitre({ 60.f, 60.f });
        vitre.setFillColor(sf::Color(150, 200, 255));
        vitre.setPosition({ 645.f, 65.f + i * 80.f });
        gareDecor_.push_back(vitre);

        // Reflet
        sf::RectangleShape reflet({ 25.f, 25.f });
        reflet.setFillColor(sf::Color(200, 230, 255, 150));
        reflet.setPosition({ 650.f, 70.f + i * 80.f });
        gareDecor_.push_back(reflet);
    }

    // Porte d'entrée (celle qu'on peut utiliser - vers le bord intérieur)
    sf::RectangleShape porteD1({ 50.f, 90.f });
    porteD1.setFillColor(sf::Color(100, 200, 100));
    porteD1.setPosition({ 600.f, 280.f });
    gareDecor_.push_back(porteD1);

    // Indicateur porte ouverte
    sf::RectangleShape indicateur({ 40.f, 10.f });
    indicateur.setFillColor(sf::Color(0, 255, 0));
    indicateur.setPosition({ 605.f, 265.f });
    gareDecor_.push_back(indicateur);

    // Autre porte
    sf::RectangleShape porteD2({ 50.f, 90.f });
    porteD2.setFillColor(sf::Color(180, 180, 180));
    porteD2.setPosition({ 600.f, 450.f });
    gareDecor_.push_back(porteD2);

    // Bordure sécurité droite
    sf::RectangleShape bordureD({ 8.f, 600.f });
    bordureD.setFillColor(sf::Color::Yellow);
    bordureD.setPosition({ 580.f, 0.f });
    gareDecor_.push_back(bordureD);

    // ========== DÉCO QUAI ==========

    // Panneau directionnel
    sf::RectangleShape poteau({ 10.f, 80.f });
    poteau.setFillColor(sf::Color(100, 100, 100));
    poteau.setPosition({ 395.f, 160.f });
    gareDecor_.push_back(poteau);

    sf::RectangleShape panneauQuai({ 120.f, 50.f });
    panneauQuai.setFillColor(sf::Color(0, 80, 180));
    panneauQuai.setPosition({ 340.f, 130.f });
    gareDecor_.push_back(panneauQuai);

    // Bancs modernes
    for (int i = 0; i < 2; i++) {
        // Dossier
        sf::RectangleShape dossier({ 100.f, 40.f });
        dossier.setFillColor(sf::Color(200, 200, 200));
        dossier.setPosition({ 350.f, 350.f + i * 150.f });
        gareDecor_.push_back(dossier);

        // Assise
        sf::RectangleShape assise({ 100.f, 15.f });
        assise.setFillColor(sf::Color(180, 180, 180));
        assise.setPosition({ 350.f, 390.f + i * 150.f });
        gareDecor_.push_back(assise);

        // Pieds
        sf::RectangleShape pied1({ 12.f, 20.f });
        pied1.setFillColor(sf::Color(120, 120, 120));
        pied1.setPosition({ 358.f, 405.f + i * 150.f });
        gareDecor_.push_back(pied1);

        sf::RectangleShape pied2({ 12.f, 20.f });
        pied2.setFillColor(sf::Color(120, 120, 120));
        pied2.setPosition({ 430.f, 405.f + i * 150.f });
        gareDecor_.push_back(pied2);
    }

    // Distributeur de tickets
    sf::RectangleShape distrib({ 60.f, 100.f });
    distrib.setFillColor(sf::Color(80, 80, 80));
    distrib.setPosition({ 270.f, 200.f });
    gareDecor_.push_back(distrib);

    sf::RectangleShape ecran({ 50.f, 30.f });
    ecran.setFillColor(sf::Color(50, 150, 50));
    ecran.setPosition({ 275.f, 220.f });
    gareDecor_.push_back(ecran);
}

void Game::setupTrainDecor() {
    trainDecor_.clear();

    // ========== BANDES NOIRES SUR LES CÔTÉS ==========

    // Bande noire gauche
    sf::RectangleShape blackBandLeft({ 80.f, 600.f });
    blackBandLeft.setFillColor(sf::Color(20, 20, 20));
    blackBandLeft.setPosition({ 0.f, 0.f });
    trainDecor_.push_back(blackBandLeft);

    // Bande noire droite
    sf::RectangleShape blackBandRight({ 80.f, 600.f });
    blackBandRight.setFillColor(sf::Color(20, 20, 20));
    blackBandRight.setPosition({ 720.f, 0.f });
    trainDecor_.push_back(blackBandRight);

    // ========== PORTE DE SORTIE À GAUCHE ==========

    // Cadre de porte
    sf::RectangleShape doorFrame({ 60.f, 120.f });
    doorFrame.setFillColor(sf::Color(80, 80, 80));
    doorFrame.setPosition({ 20.f, WINDOW_HEIGHT / 2.f - 60.f });
    trainDecor_.push_back(doorFrame);

    // Porte elle-même (avec vitre)
    sf::RectangleShape door({ 50.f, 110.f });
    door.setFillColor(sf::Color(150, 150, 150));
    door.setPosition({ 25.f, WINDOW_HEIGHT / 2.f - 55.f });
    trainDecor_.push_back(door);

    // Vitre de la porte
    sf::RectangleShape doorWindow({ 40.f, 50.f });
    doorWindow.setFillColor(sf::Color(100, 150, 200, 180));
    doorWindow.setPosition({ 30.f, WINDOW_HEIGHT / 2.f - 45.f });
    trainDecor_.push_back(doorWindow);

    // Poignée de porte
    sf::RectangleShape doorHandle({ 15.f, 5.f });
    doorHandle.setFillColor(sf::Color(200, 200, 0));
    doorHandle.setPosition({ 60.f, WINDOW_HEIGHT / 2.f });
    trainDecor_.push_back(doorHandle);

    // Panneau "SORTIE" au-dessus de la porte
    sf::RectangleShape exitSign({ 50.f, 20.f });
    exitSign.setFillColor(sf::Color(0, 200, 0));
    exitSign.setPosition({ 25.f, WINDOW_HEIGHT / 2.f - 85.f });
    trainDecor_.push_back(exitSign);

    // ========== SOL ET PLAFOND ==========

    // Sol avec motif
    sf::RectangleShape floor({ 640.f, 600.f });
    floor.setFillColor(sf::Color(60, 60, 80));
    floor.setPosition({ 80.f, 0.f });
    trainDecor_.push_back(floor);

    // Lignes sur le sol
    for (int i = 0; i < 20; i++) {
        sf::RectangleShape floorLine({ 640.f, 2.f });
        floorLine.setFillColor(sf::Color(40, 40, 60));
        floorLine.setPosition({ 80.f, i * 30.f });
        trainDecor_.push_back(floorLine);
    }

    // ========== BARRES VERTICALES (POUR SE TENIR) ==========

    // Barres centrales (allée)
    for (int i = 0; i < 5; i++) {
        // Poteau
        sf::RectangleShape pole({ 15.f, 250.f });
        pole.setFillColor(sf::Color(200, 200, 200));
        pole.setPosition({ 390.f, 20.f + i * 120.f });
        trainDecor_.push_back(pole);

        // Reflet sur le poteau
        sf::RectangleShape poleShine({ 5.f, 250.f });
        poleShine.setFillColor(sf::Color(230, 230, 230, 150));
        poleShine.setPosition({ 392.f, 20.f + i * 120.f });
        trainDecor_.push_back(poleShine);
    }

    // ========== SIÈGES CÔTÉ GAUCHE ==========

    for (int i = 0; i < 5; i++) {
        // Dossier du siège
        sf::RectangleShape seatBack({ 120.f, 50.f });
        seatBack.setFillColor(sf::Color(80, 120, 180));
        seatBack.setPosition({ 100.f, 30.f + i * 110.f });
        trainDecor_.push_back(seatBack);

        // Assise
        sf::RectangleShape seatBase({ 120.f, 30.f });
        seatBase.setFillColor(sf::Color(70, 110, 170));
        seatBase.setPosition({ 100.f, 80.f + i * 110.f });
        trainDecor_.push_back(seatBase);

        // Bande décorative
        sf::RectangleShape seatStripe({ 120.f, 5.f });
        seatStripe.setFillColor(sf::Color(200, 200, 0));
        seatStripe.setPosition({ 100.f, 60.f + i * 110.f });
        trainDecor_.push_back(seatStripe);

        // Pieds du siège
        sf::RectangleShape leg1({ 8.f, 15.f });
        leg1.setFillColor(sf::Color(100, 100, 100));
        leg1.setPosition({ 110.f, 110.f + i * 110.f });
        trainDecor_.push_back(leg1);

        sf::RectangleShape leg2({ 8.f, 15.f });
        leg2.setFillColor(sf::Color(100, 100, 100));
        leg2.setPosition({ 202.f, 110.f + i * 110.f });
        trainDecor_.push_back(leg2);
    }

    // ========== SIÈGES CÔTÉ DROIT ==========

    for (int i = 0; i < 5; i++) {
        // Dossier du siège
        sf::RectangleShape seatBack({ 120.f, 50.f });
        seatBack.setFillColor(sf::Color(80, 120, 180));
        seatBack.setPosition({ 480.f, 30.f + i * 110.f });
        trainDecor_.push_back(seatBack);

        // Assise
        sf::RectangleShape seatBase({ 120.f, 30.f });
        seatBase.setFillColor(sf::Color(70, 110, 170));
        seatBase.setPosition({ 480.f, 80.f + i * 110.f });
        trainDecor_.push_back(seatBase);

        // Bande décorative
        sf::RectangleShape seatStripe({ 120.f, 5.f });
        seatStripe.setFillColor(sf::Color(200, 200, 0));
        seatStripe.setPosition({ 480.f, 60.f + i * 110.f });
        trainDecor_.push_back(seatStripe);

        // Pieds du siège
        sf::RectangleShape leg1({ 8.f, 15.f });
        leg1.setFillColor(sf::Color(100, 100, 100));
        leg1.setPosition({ 490.f, 110.f + i * 110.f });
        trainDecor_.push_back(leg1);

        sf::RectangleShape leg2({ 8.f, 15.f });
        leg2.setFillColor(sf::Color(100, 100, 100));
        leg2.setPosition({ 582.f, 110.f + i * 110.f });
        trainDecor_.push_back(leg2);
    }

    // ========== BARRES HORIZONTALES AU PLAFOND ==========

    for (int i = 0; i < 3; i++) {
        // Barre gauche
        sf::RectangleShape railLeft({ 200.f, 8.f });
        railLeft.setFillColor(sf::Color(180, 180, 180));
        railLeft.setPosition({ 130.f, 10.f + i * 180.f });
        trainDecor_.push_back(railLeft);

        // Barre droite
        sf::RectangleShape railRight({ 200.f, 8.f });
        railRight.setFillColor(sf::Color(180, 180, 180));
        railRight.setPosition({ 470.f, 10.f + i * 180.f });
        trainDecor_.push_back(railRight);
    }

    // ========== FENÊTRES ==========

    // Fenêtres côté gauche (entre les sièges)
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape window({ 100.f, 40.f });
        window.setFillColor(sf::Color(50, 100, 150, 200));
        window.setPosition({ 110.f, 5.f + i * 140.f });
        trainDecor_.push_back(window);

        // Reflet sur la vitre
        sf::RectangleShape windowReflect({ 30.f, 15.f });
        windowReflect.setFillColor(sf::Color(150, 200, 255, 100));
        windowReflect.setPosition({ 120.f, 10.f + i * 140.f });
        trainDecor_.push_back(windowReflect);
    }

    // Fenêtres côté droit
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape window({ 100.f, 40.f });
        window.setFillColor(sf::Color(50, 100, 150, 200));
        window.setPosition({ 490.f, 5.f + i * 140.f });
        trainDecor_.push_back(window);

        // Reflet sur la vitre
        sf::RectangleShape windowReflect({ 30.f, 15.f });
        windowReflect.setFillColor(sf::Color(150, 200, 255, 100));
        windowReflect.setPosition({ 500.f, 10.f + i * 140.f });
        trainDecor_.push_back(windowReflect);
    }

    // ========== PANNEAUX INFORMATIFS ==========

    // Panneau électronique en haut
    sf::RectangleShape infoPanel({ 250.f, 40.f });
    infoPanel.setFillColor(sf::Color(20, 20, 20));
    infoPanel.setPosition({ 275.f, 50.f });
    trainDecor_.push_back(infoPanel);

    // Écran du panneau
    sf::RectangleShape infoScreen({ 240.f, 30.f });
    infoScreen.setFillColor(sf::Color(255, 100, 0));
    infoScreen.setPosition({ 280.f, 55.f });
    trainDecor_.push_back(infoScreen);

    // ========== DÉTAILS DÉCORATIFS ==========

    // Bandes jaunes de sécurité au sol (près de la porte)
    sf::RectangleShape safetyLine1({ 3.f, 120.f });
    safetyLine1.setFillColor(sf::Color::Yellow);
    safetyLine1.setPosition({ 85.f, WINDOW_HEIGHT / 2.f - 60.f });
    trainDecor_.push_back(safetyLine1);

    // Extincteur sur le mur
    sf::RectangleShape extinguisher({ 20.f, 40.f });
    extinguisher.setFillColor(sf::Color::Red);
    extinguisher.setPosition({ 250.f, 150.f });
    trainDecor_.push_back(extinguisher);

    sf::RectangleShape extinguisherTop({ 20.f, 8.f });
    extinguisherTop.setFillColor(sf::Color(150, 0, 0));
    extinguisherTop.setPosition({ 250.f, 145.f });
    trainDecor_.push_back(extinguisherTop);

    // Bouton d'arrêt d'urgence
    sf::CircleShape emergencyButton(15.f);
    emergencyButton.setFillColor(sf::Color(200, 0, 0));
    emergencyButton.setPosition({ 640.f, 200.f });
    trainDecor_.push_back(emergencyButton);

    sf::CircleShape emergencyButtonBorder(18.f);
    emergencyButtonBorder.setFillColor(sf::Color::Transparent);
    emergencyButtonBorder.setOutlineColor(sf::Color::Yellow);
    emergencyButtonBorder.setOutlineThickness(2.f);
    emergencyButtonBorder.setPosition({ 637.f, 197.f });
    trainDecor_.push_back(emergencyButtonBorder);
}

void Game::setupCredits() {
    creditsTexts_.clear();

    std::vector<std::string> lines = {
        "",
        "",
        "LES AVENTURES FARFELUES DE NOLAN",
        "",
        "Un jeu par Nolan REUX-NAUDEIX",
        "",
        "",
        "REALISATEUR",
        "Moi meme (Nolan)",
        "",
        "",
        "PROGRAMMATION",
        "Toujours moi (Nolan)",
        "",
        "",
        "GRAPHISMES",
        "Mon etre (Nolan)",
        "",
        "",
        "SCENARIO",
        "Mon humble personne (Nolan)",
        "",
        "",
        "MUSIQUE",
        "Aucune (mais on a essaye)",
        "",
        "",
        "REMERCIEMENTS SPECIAUX",
        "Claude AI et ChatGPT - Pour l'aide au code",
        "William pour les cours ! (le GOAT)",
        "SFML - Pour la bibliotheque graphique",
        "Toi - Pour avoir joue !",
        "Moi meme parce que c'etait dur",
        "Je vous retiens encore un peu juste histoire de",
        "",
        "",
        "MERCI D'AVOIR JOUE !",
        "",
        "",
        "Ciao"
    };

    for (size_t i = 0; i < lines.size(); i++) {
        sf::Text creditText(font_, lines[i], 24u);
        creditText.setFillColor(sf::Color::White);

        // Centre le texte
        sf::FloatRect bounds = creditText.getLocalBounds();
        creditText.setPosition({ 400.f - bounds.size.x / 2.f, 600.f + static_cast<float>(i) * 50.f });

        creditsTexts_.push_back(creditText);
    }
}

void Game::setupSchoolDecor() {
    schoolDecor_.clear();

    // ========== COUR DE L'ÉCOLE ==========

    // Sol de la cour (béton)
    sf::RectangleShape courtyard({ 800.f, 600.f });
    courtyard.setFillColor(sf::Color(140, 140, 140));
    courtyard.setPosition({ 0.f, 0.f });
    schoolDecor_.push_back(courtyard);

    // Lignes de terrain de sport
    sf::RectangleShape centerLine({ 5.f, 300.f });
    centerLine.setFillColor(sf::Color::White);
    centerLine.setPosition({ 397.5f, 150.f });
    schoolDecor_.push_back(centerLine);

    // Cercle central
    sf::CircleShape centerCircle(50.f);
    centerCircle.setFillColor(sf::Color::Transparent);
    centerCircle.setOutlineColor(sf::Color::White);
    centerCircle.setOutlineThickness(3.f);
    centerCircle.setPosition({ 350.f, 250.f });
    schoolDecor_.push_back(centerCircle);

    // ========== BÂTIMENT DE L'ÉCOLE (en haut) ==========

    // Mur principal
    sf::RectangleShape schoolBuilding({ 600.f, 150.f });
    schoolBuilding.setFillColor(sf::Color(180, 140, 100));
    schoolBuilding.setPosition({ 100.f, 0.f });
    schoolDecor_.push_back(schoolBuilding);

    // Toit
    sf::RectangleShape roof({ 620.f, 30.f });
    roof.setFillColor(sf::Color(150, 80, 60));
    roof.setPosition({ 90.f, -30.f });
    schoolDecor_.push_back(roof);

    // Fenêtres de l'école (3 rangées)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 8; col++) {
            sf::RectangleShape window({ 50.f, 30.f });
            window.setFillColor(sf::Color(100, 180, 220));
            window.setPosition({ 130.f + col * 70.f, 20.f + row * 45.f });
            schoolDecor_.push_back(window);

            // Croisillons de fenêtre
            sf::RectangleShape crossV({ 2.f, 30.f });
            crossV.setFillColor(sf::Color(80, 80, 80));
            crossV.setPosition({ 154.f + col * 70.f, 20.f + row * 45.f });
            schoolDecor_.push_back(crossV);

            sf::RectangleShape crossH({ 50.f, 2.f });
            crossH.setFillColor(sf::Color(80, 80, 80));
            crossH.setPosition({ 130.f + col * 70.f, 34.f + row * 45.f });
            schoolDecor_.push_back(crossH);
        }
    }

    // Porte d'entrée principale (grande)
    sf::RectangleShape mainDoor({ 80.f, 120.f });
    mainDoor.setFillColor(sf::Color(100, 60, 40));
    mainDoor.setPosition({ 360.f, 30.f });
    schoolDecor_.push_back(mainDoor);

    // Vitre de la porte
    sf::RectangleShape doorGlass({ 60.f, 80.f });
    doorGlass.setFillColor(sf::Color(120, 180, 220, 150));
    doorGlass.setPosition({ 370.f, 40.f });
    schoolDecor_.push_back(doorGlass);

    // Poignée
    sf::CircleShape doorHandle(5.f);
    doorHandle.setFillColor(sf::Color(200, 180, 0));
    doorHandle.setPosition({ 420.f, 85.f });
    schoolDecor_.push_back(doorHandle);

    // Escalier devant la porte
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape step({ 100.f, 15.f });
        step.setFillColor(sf::Color(120, 120, 120));
        step.setPosition({ 350.f, 150.f + i * 15.f });
        schoolDecor_.push_back(step);
    }

    // ========== PANNEAU "COLLÈGE" ==========

    sf::RectangleShape signBoard({ 200.f, 50.f });
    signBoard.setFillColor(sf::Color(50, 100, 150));
    signBoard.setPosition({ 300.f, 160.f });
    schoolDecor_.push_back(signBoard);

    sf::RectangleShape signBorder({ 200.f, 50.f });
    signBorder.setFillColor(sf::Color::Transparent);
    signBorder.setOutlineColor(sf::Color::White);
    signBorder.setOutlineThickness(3.f);
    signBorder.setPosition({ 300.f, 160.f });
    schoolDecor_.push_back(signBorder);

    // ========== CAGES DE FOOT ==========

    // Cage gauche
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape post({ 8.f, 80.f });
        post.setFillColor(sf::Color::White);
        post.setPosition({ 50.f + i * 60.f, 320.f });
        schoolDecor_.push_back(post);
    }
    sf::RectangleShape topBarLeft({ 120.f, 8.f });
    topBarLeft.setFillColor(sf::Color::White);
    topBarLeft.setPosition({ 50.f, 320.f });
    schoolDecor_.push_back(topBarLeft);

    // Filet cage gauche (simplifié)
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape net({ 2.f, 80.f });
        net.setFillColor(sf::Color(200, 200, 200, 150));
        net.setPosition({ 60.f + i * 25.f, 320.f });
        schoolDecor_.push_back(net);
    }

    // Cage droite
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape post({ 8.f, 80.f });
        post.setFillColor(sf::Color::White);
        post.setPosition({ 630.f + i * 60.f, 320.f });
        schoolDecor_.push_back(post);
    }
    sf::RectangleShape topBarRight({ 120.f, 8.f });
    topBarRight.setFillColor(sf::Color::White);
    topBarRight.setPosition({ 630.f, 320.f });
    schoolDecor_.push_back(topBarRight);

    // Filet cage droite
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape net({ 2.f, 80.f });
        net.setFillColor(sf::Color(200, 200, 200, 150));
        net.setPosition({ 640.f + i * 25.f, 320.f });
        schoolDecor_.push_back(net);
    }

    // ========== BANCS DANS LA COUR ==========

    for (int i = 0; i < 3; i++) {
        // Assise
        sf::RectangleShape benchSeat({ 80.f, 15.f });
        benchSeat.setFillColor(sf::Color(139, 90, 43));
        benchSeat.setPosition({ 100.f + i * 250.f, 480.f });
        schoolDecor_.push_back(benchSeat);

        // Dossier
        sf::RectangleShape benchBack({ 80.f, 30.f });
        benchBack.setFillColor(sf::Color(120, 70, 30));
        benchBack.setPosition({ 100.f + i * 250.f, 450.f });
        schoolDecor_.push_back(benchBack);

        // Pieds
        sf::RectangleShape leg1({ 8.f, 20.f });
        leg1.setFillColor(sf::Color(60, 60, 60));
        leg1.setPosition({ 110.f + i * 250.f, 495.f });
        schoolDecor_.push_back(leg1);

        sf::RectangleShape leg2({ 8.f, 20.f });
        leg2.setFillColor(sf::Color(60, 60, 60));
        leg2.setPosition({ 162.f + i * 250.f, 495.f });
        schoolDecor_.push_back(leg2);
    }

    // ========== ARBRES ==========

    // Arbre 1 (coin bas gauche)
    sf::RectangleShape trunk1({ 20.f, 50.f });
    trunk1.setFillColor(sf::Color(101, 67, 33));
    trunk1.setPosition({ 30.f, 500.f });
    schoolDecor_.push_back(trunk1);

    sf::CircleShape leaves1(35.f);
    leaves1.setFillColor(sf::Color(34, 139, 34));
    leaves1.setPosition({ 5.f, 455.f });
    schoolDecor_.push_back(leaves1);

    // Arbre 2 (coin bas droit)
    sf::RectangleShape trunk2({ 20.f, 50.f });
    trunk2.setFillColor(sf::Color(101, 67, 33));
    trunk2.setPosition({ 740.f, 500.f });
    schoolDecor_.push_back(trunk2);

    sf::CircleShape leaves2(35.f);
    leaves2.setFillColor(sf::Color(34, 139, 34));
    leaves2.setPosition({ 715.f, 455.f });
    schoolDecor_.push_back(leaves2);

    // ========== POUBELLES ET DÉTAILS ==========

    // Poubelles (3)
    for (int i = 0; i < 3; i++) {
        sf::RectangleShape bin({ 30.f, 40.f });
        bin.setFillColor(sf::Color(60, 120, 60));
        bin.setPosition({ 250.f + i * 150.f, 520.f });
        schoolDecor_.push_back(bin);

        sf::RectangleShape binLid({ 32.f, 5.f });
        binLid.setFillColor(sf::Color(50, 100, 50));
        binLid.setPosition({ 249.f + i * 150.f, 518.f });
        schoolDecor_.push_back(binLid);
    }

    // Ballon de foot abandonné
    sf::CircleShape ball(15.f);
    ball.setFillColor(sf::Color::White);
    ball.setPosition({ 500.f, 400.f });
    schoolDecor_.push_back(ball);

    // Motifs sur le ballon
    sf::CircleShape ballPattern(12.f, 5);
    ballPattern.setFillColor(sf::Color::Black);
    ballPattern.setPosition({ 503.f, 403.f });
    schoolDecor_.push_back(ballPattern);
}

void Game::setupNPCs() {
    npcs_.clear();

    // Parent dans la maison
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 250.f, 400.f },
        NPCType::Parent,
        std::vector<std::string>{
        "Coucou mon fils ca va ?",
            "Prends ton petit dejeuner, et ne soit pas en retard en cours!",
            "J'irai faire des courses ce soir j'aurai besoin de toi pour m'aider",
            "**Je vous epargne la suite du dialogue elle raconte sa vie entiere et je vais etre en retard**"
    }
    ));

    // Passant 1 - sur le trottoir du haut
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 500.f, 50.f },
        NPCType::Passant,
        std::vector<std::string>{
        "Ah les jeunes d'aujourd'hui...",
            "De mon temps on allait a l'ecole a pied, dans la neige, en montee...",
            "...dans les DEUX sens ! Allez file a l'ecole !",
            "**Bravo ! Nolan a gagne... bah rien parce que c'etait un vieil aigri**",
            "**Nolan a gagne le droit d'etre sidere et aller en cours**"
    }
    ));

    // Passant 2 
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 350.f, 480.f },
        NPCType::Passant,
        std::vector<std::string>{
        "Salut excuse moi t'aurais pas du feu ou une clope ?",
            "Ah tu fumes pas ?",
            "C'est grave la lose man t'es pas cool du tout...",
            "Moi j'ai quel age ? 15 ans pourquoi ?"
    }
    ));

    // Passant 3
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 650.f, 450.f },
        NPCType::Passant,
        std::vector<std::string>{
        "*haletant* Je... cours... depuis... 5 minutes...",
            "Pourquoi... c'est si... dur ?!",
            "Je crois que je vais... m'asseoir...",
            "T'aurais... pas... un peu... d'eau s'il te plait ?",
            "**Nolan donne donc genereusement sa bouteille d'eau**",
            "**Il sera malheureusement desseche en arrivant en cours**"
    }
    ));
    // Manif
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 300.f, 250.f },
        NPCType::Manifestant,
        std::vector<std::string>{
        "PLUS DE WEEKEND ! MOINS DE SEMAINE !",
            "...Attends, c'est dans quel sens deja ?",
            "MACRON DEMISSION !!",
            "MACRON... putain",
            "MACRON EXPLOSION !!"
    }
    ));

    // Manif
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 450.f, 350.f },
        NPCType::Manifestant,
        std::vector<std::string>{
        "J'ai oublie pourquoi on manifeste...",
            "Mais tant que ca me donne une excuse pour pas aller taffer...",
            "Tu veux venir avec nous ?",
            "**Nolan le regarde dedaigneux et plein de mepris et trace sa route**"
            
    }
    ));

    // Manif
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 200.f, 400.f },
        NPCType::Manifestant,
        std::vector<std::string>{
        "CHOUCHOUS, BEIGNETS !!",
            "MAIS CHAUD **(il est 8h du matin faut vraiment aller se faire voir)**",
            "Non ? Bon tant pis...",
            "Ca en fera plus pour moi"
    }
    ));
    // NPC 1 - Voyageur pressé dans la gare
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 300.f, 100.f },
        NPCType::NPCgare,
        std::vector<std::string>{
        "**Bouscule Nolan**",
            "Vas-y degage de la gros t'es sur le chemin",
            "**Le frerot avait 8 metres de marge entre moi et la fin du quai serieux.**",
            "**De plus il fait tomber le telephone de Nolan, ce qui participe a sa tension matinale.**"
    }
    ));

    // NPC 2 - Personne assise sur un banc
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 380.f, 410.f },
        NPCType::NPCgare,
        std::vector<std::string>{
        "Fais chier, il arrive quand le train jvais encore etre en retard.",
            "C'est un pur cauchemar la RATP.."
    }
    ));

    // NPC 3 - Agent de gare
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 300.f, 250.f },
        NPCType::Agent,
        std::vector<std::string>{
        "TRAIN VERS LA DEFENSE RETARDE !",
            "VEUILLEZ VOUS DIRIGER VERS DES ALTERNATIVES !",
            "**Heureusement dans toute cette malchance, Nolan a un peu de chance.**",
            "**Il peut prendre n'importe quel train, car comme un grand sage l'a dit un jour :",
            "tous les trains menent a La Defense, **"
    }
    ));

    // NPC 4 - Touriste perdu devant le distributeur
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 250.f, 260.f },
        NPCType::NPCgare,
        std::vector<std::string>{
        "Hey excuse me ? Could you help me to buy a ticket ?",
            "I don't understand anything about french...",
            "**Nolan l'aide alors car il pratique l'anglais depuis moultes annees**",
            "Oh thanks a lot ! Have a good day !",
            "**Ouais ouais c'est ca, jvais rater mon train a cause de ces conneries.**"
    }
    ));

    // Contrôleur dans le train
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 500.f, 300.f },
        NPCType::Controleur,
        std::vector<std::string>{
        "Bonjour monsieur,",
            "Titre de transport s'il vous plait",
            "**Nolan donne son Navigo que sa mere a durement paye a la sueur de ses mains",
            "car il est un honnete citoyen en regle**"
    }
    ));
    
    // Passager 1 
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 160.f, 90.f },
        NPCType::Passager,
        std::vector<std::string>{
        "Putain j'ai perdu ma Navigo...",
            "Casse les couilles j'espere y'aura pas de controleurs",
            "**Il voit le controleur**",
            "Eh merde, j'espere on arrive vite au prochain arret que je puisse fuir."
    }
    ));

    // Passagère 2 
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 540.f, 200.f },
        NPCType::Passager,
        std::vector<std::string>{
        "Je me sens vraiment pas bien la...",
            "Je crois que je vais m'evanouir",
            "**Nolan entend ca et commence a peter son crane interieurement**",
            "**Pitie ne t'evanouis pas je n'ai pas que ca a foutre ca arrive tous les jours**"
    }
    ));

    // Passager 3 
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 360.f, 350.f },
        NPCType::Passager,
        std::vector<std::string>{
        "*au telephone* Oui... oui... NON !",
            "T'AS DIS QUOI JE T'ENTENDS PAS LA CA BUG",
            "Putain technologie de merde ca marche jamais",
            "**Il est vachement con on est dans le train forcement ca marche pas v'la l'idiot**"
    }
    ));

    // Passagère 4 
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 160.f, 310.f },
        NPCType::Passager,
        std::vector<std::string>{
        "*Chante comme si elle etait toute seule dans le train* LA, laaa, laa",
            "**Nolan la regarde insistant, avec un regard assez noir et menacant**",
            "**car Nolan pense etre un dangereux criminel alors qu'il pese 65kg et se fait dechirer par n'importe qui**",
            "Ah desolee, j'ecoute de la musique...",
            "**Sans deconner j'avais pas remarque..**"
    }
    ));

    // Passager 5 
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 450.f, 450.f },
        NPCType::Passager,
        std::vector<std::string>{
        "Excusez-moi, c'est bien le train pour... euh...",
            "*regarde les arrets* Je suis completement perdu !",
            "Pourquoi les plans sont si compliques ?!",
            "**Alors qu'il y a juste a lire les panneaux avant de monter dans un train**",
            "Vous savez ou c'est Gare Montparnasse ?",
            "**Complet oppose le man est foutu il va faire 1h de plus de transports**"
    }
    ));

    // Ami à l'école
    npcs_.push_back(std::make_unique<NPC>(
        sf::Vector2f{ 400.f, 300.f },
        NPCType::Ami,
        std::vector<std::string>{
        "Nolan ! Bien ? J'ai une bonne et une mauvaise nouvelle.",
            "Bonne nouvelle.. T'es a l'heure.",
            "Mauvaise nouvelle..",
            "Le prof est en deplacement a Bordeaux donc y'a pas cours..."
    }
    ));
}

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
    // Positions de spawn basées sur d'où on vient
    if (newArea == "rue") {
        // Si on vient de la maison (par le bas)
        if (currentArea_ == "maison") {
            return { 150.f, 530.f };
        }
        // Si on vient de rue_manif (par la droite)
        else if (currentArea_ == "rue_manif") {
            return { 720.f, 500.f };
        }
    }
    else if (newArea == "maison") {
        // Retour depuis la rue
        return { (WINDOW_WIDTH - DOOR_WIDTH) / 2.f + DOOR_WIDTH / 2.f - PLAYER_SIZE / 2.f, 520.f };
    }
    else if (newArea == "rue_manif") {
        // Si on vient de la rue (par la gauche)
        if (currentArea_ == "rue") {
            return { 50.f, 500.f };
        }
        // Si on vient de la gare (par la droite)
        else if (currentArea_ == "gare") {
            return { 720.f, 300.f };
        }
    }
    else if (newArea == "gare") {
        // Si on vient de rue_manif (par le haut, l'escalier)
        if (currentArea_ == "rue_manif") {
            return { 380.f, 130.f };
        }
    }
    else if (newArea == "train_interieur") {
        return { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
    }
    else if (newArea == "ecole") {
        return { 100.f, 300.f };
    }

    // Position par défaut
    return { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
}
void Game::checkFurnitureCollision() {
    if (currentArea_ != "maison") return;

    sf::FloatRect playerRect = player_->getGlobalBounds();
    sf::Vector2f playerPos = playerRect.position;
    sf::Vector2f playerSize = playerRect.size;

    for (auto& furni : furniture_) {
        sf::FloatRect furniRect = furni.getGlobalBounds();

        if (checkCollision(playerRect, furniRect)) {
            float overlapLeft = (playerPos.x + playerSize.x) - furniRect.position.x;
            float overlapRight = (furniRect.position.x + furniRect.size.x) - playerPos.x;
            float overlapTop = (playerPos.y + playerSize.y) - furniRect.position.y;
            float overlapBottom = (furniRect.position.y + furniRect.size.y) - playerPos.y;

            float minOverlap = std::min({ overlapLeft, overlapRight, overlapTop, overlapBottom });

            if (minOverlap == overlapTop) {
                player_->setPosition({ playerPos.x, furniRect.position.y - playerSize.y });
            }
            else if (minOverlap == overlapBottom) {
                player_->setPosition({ playerPos.x, furniRect.position.y + furniRect.size.y });
            }
            else if (minOverlap == overlapLeft) {
                player_->setPosition({ furniRect.position.x - playerSize.x, playerPos.y });
            }
            else if (minOverlap == overlapRight) {
                player_->setPosition({ furniRect.position.x + furniRect.size.x, playerPos.y });
            }
        }
    }
}

void Game::checkNPCInteractions() {
    currentNPCInRange_ = nullptr;

    for (auto& npc : npcs_) {
        if (npc->isInRange(player_->getGlobalBounds())) {
            bool npcInCurrentArea = false;

            if (currentArea_ == "maison" && npc->getType() == NPCType::Parent) {
                npcInCurrentArea = true;
            }
            else if (currentArea_ == "rue" && npc->getType() == NPCType::Passant) {
                npcInCurrentArea = true;
            }
            else if (currentArea_ == "rue_manif" && npc->getType() == NPCType::Manifestant) {
                npcInCurrentArea = true;
            }
            else if (currentArea_ == "gare" && npc->getType() == NPCType::NPCgare) {
                npcInCurrentArea = true;
            }
            else if (currentArea_ == "gare" && npc->getType() == NPCType::Agent) {
                npcInCurrentArea = true;
            }
            else if (currentArea_ == "train_interieur" && npc->getType() == NPCType::Controleur) {
                npcInCurrentArea = true;
            }
            else if (currentArea_ == "train_interieur" && npc->getType() == NPCType::Passager) {
                npcInCurrentArea = true;
            }
            else if (currentArea_ == "ecole" && npc->getType() == NPCType::Ami) {
                npcInCurrentArea = true;
            }

            if (npcInCurrentArea) {
                currentNPCInRange_ = npc.get();
                hud_->setInteractionAvailable(true);
                return;
            }
        }
    }

    hud_->setInteractionAvailable(false);
}

void Game::run() {
    while (window_.isOpen()) {
        sf::Time dt = clock_.restart();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (auto maybeEvent = window_.pollEvent()) {
        if (!maybeEvent) break;

        if (maybeEvent->is<sf::Event::Closed>())
            window_.close();

        // Menu principal - Clic souris
        if (state_ == GameState::Menu && maybeEvent->is<sf::Event::MouseButtonPressed>()) {
            if (auto mouse = maybeEvent->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window_.mapPixelToCoords(mouse->position);
                    if (playButtonRect_.getGlobalBounds().contains(mousePos))
                        state_ = GameState::Playing;
                }
            }
        }

        // Menu principal - Touche Enter
        if (state_ == GameState::Menu && maybeEvent->is<sf::Event::KeyPressed>()) {
            if (auto key = maybeEvent->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Enter)
                    state_ = GameState::Playing;
            }
        }

        // Pause - Touche Échap pendant le jeu
        if (state_ == GameState::Playing && maybeEvent->is<sf::Event::KeyPressed>()) {
            if (auto key = maybeEvent->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    state_ = GameState::Paused;
                    continue; // Important : on sort pour ne pas traiter la touche E
                }
            }
        }

        // Reprendre - Touche Échap pendant la pause
        if (state_ == GameState::Paused && maybeEvent->is<sf::Event::KeyPressed>()) {
            if (auto key = maybeEvent->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    state_ = GameState::Playing;
                    continue;
                }
            }
        }

        // Clics sur les boutons du menu pause
        if (state_ == GameState::Paused && maybeEvent->is<sf::Event::MouseButtonPressed>()) {
            if (auto mouse = maybeEvent->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos = window_.mapPixelToCoords(mouse->position);

                    // Bouton Reprendre
                    if (resumeButtonRect_.getGlobalBounds().contains(mousePos)) {
                        state_ = GameState::Playing;
                    }

                    // Bouton Quitter
                    if (quitButtonRect_.getGlobalBounds().contains(mousePos)) {
                        // Réinitialiser le jeu et retourner au menu
                        resetGame();
                        state_ = GameState::Menu;
                    }
                }
            }
        }

        // Interactions avec les NPCs (seulement en jeu)
        if (state_ == GameState::Playing && maybeEvent->is<sf::Event::KeyPressed>()) {
            if (auto key = maybeEvent->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::E) {
                    if (currentNPCInRange_ && !hud_->isDialogueShown()) {
                        // Afficher le premier dialogue
                        std::string speaker;
                        switch (currentNPCInRange_->getType()) {
                        case NPCType::Parent: speaker = "Maman"; break;
                        case NPCType::Manifestant: speaker = "Manifestant"; break;
                        case NPCType::Controleur: speaker = "Controleur"; break;
                        case NPCType::Ami: speaker = "Ami"; break;
                        case NPCType::Passant: speaker = "Passant"; break;
                        case NPCType::Agent: speaker = "Agent"; break;
                        case NPCType::Passager: speaker = "Passager"; break;
                        case NPCType::NPCgare: speaker = "Voyageur"; break;
                        }
                        hud_->showDialogue(currentNPCInRange_->getCurrentDialogue(), speaker);
                        currentNPCInRange_->setDialogueShown(true);
                    }
                    else if (hud_->isDialogueShown() && currentNPCInRange_) {
                        // Si on est dans un dialogue et qu'il y a encore des dialogues
                        if (currentNPCInRange_->hasMoreDialogues()) {
                            currentNPCInRange_->advanceDialogue();
                            std::string speaker;
                            switch (currentNPCInRange_->getType()) {
                            case NPCType::Parent: speaker = "Maman"; break;
                            case NPCType::Manifestant: speaker = "Manifestant"; break;
                            case NPCType::Controleur: speaker = "Controleur"; break;
                            case NPCType::Ami: speaker = "Ami"; break;
                            case NPCType::Passant: speaker = "Passant"; break;
                            case NPCType::Agent: speaker = "Agent"; break;
                            case NPCType::Passager: speaker = "Passager"; break;
                            case NPCType::NPCgare: speaker = "Voyageur"; break;
                            }
                            hud_->showDialogue(currentNPCInRange_->getCurrentDialogue(), speaker);
                        }
                        else {
                            // Fin des dialogues
                            hud_->hideDialogue();

                            // Si c'est l'ami à l'école, on lance les crédits
                            if (currentNPCInRange_->getType() == NPCType::Ami && currentArea_ == "ecole") {
                                gameFinished_ = true;
                                creditsStarted_ = true;
                                state_ = GameState::Credits;
                            }

                            currentNPCInRange_->resetDialogue();
                        }
                    }
                }
            }
        }
    }
}

void Game::update(sf::Time dt) {
    // Gestion des crédits
    if (state_ == GameState::Credits) {
        for (size_t i = 0; i < creditsTexts_.size(); i++) {
            sf::Vector2f pos = creditsTexts_[i].getPosition();
            creditsTexts_[i].setPosition({ pos.x, pos.y - creditsSpeed_ * dt.asSeconds() });
        }

        // Si tous les crédits sont passés, retourner au menu
        if (!creditsTexts_.empty() && creditsTexts_.back().getPosition().y < -50.f) {
            resetGame();
            state_ = GameState::Menu;
        }
        return;
    }

    // Ne pas mettre à jour le jeu si en pause
    if (state_ == GameState::Paused) {
        return;
    }

    // Gestion du jeu normal
    if (state_ == GameState::Playing && !gameFinished_) {
        player_->update(dt);
        hud_->update(dt);

        for (auto& npc : npcs_) {
            npc->update(dt);
        }

        checkFurnitureCollision();
        checkNPCInteractions();

        if (!fading_) {
            for (auto& trigger : triggers_) {
                if (!trigger.used && trigger.currentArea == currentArea_ &&
                    checkCollision(player_->getGlobalBounds(), trigger.bounds)) {

                    nextArea_ = trigger.targetArea;
                    nextSpawnPos_ = calculateSpawnPosition(nextArea_);
                    fading_ = true;
                    fadeIn_ = false;
                    trigger.used = true;

                    for (auto& t : triggers_) {
                        if (&t != &trigger) {
                            t.used = false;
                        }
                    }

                    break;
                }
            }
        }
    }

    // Gestion du fading
    if (fading_) {
        if (!fadeIn_) {
            fadeAlpha_ += fadeSpeed_ * dt.asSeconds();
            if (fadeAlpha_ >= 255.f) {
                fadeAlpha_ = 255.f;
                currentArea_ = nextArea_;
                hud_->setCurrentArea(currentArea_);
                hud_->hideDialogue();
                fadeIn_ = true;
                player_->setPosition(nextSpawnPos_);

                for (auto& t : triggers_) {
                    t.used = false;
                }
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

void Game::resetGame() {
    // Réinitialiser l'état du jeu
    currentArea_ = "maison";
    gameFinished_ = false;
    fading_ = false;
    fadeAlpha_ = 0.f;
    fadeIn_ = false;
    currentNPCInRange_ = nullptr;
    creditsStarted_ = false;

    // Repositionner le joueur
    player_->setPosition({ 380.f, 500.f });

    // Réinitialiser le HUD
    hud_->setCurrentArea(currentArea_);
    hud_->hideDialogue();
    hud_->setGameTime(0.f);

    // Réinitialiser tous les NPCs
    for (auto& npc : npcs_) {
        npc->resetDialogue();
    }

    // Réinitialiser tous les triggers
    for (auto& t : triggers_) {
        t.used = false;
    }

    // Recréer les crédits
    setupCredits();
}

void Game::render() {
    if (state_ == GameState::Menu) window_.clear(sf::Color{ 50,50,50 });
    else if (state_ == GameState::Credits) window_.clear(sf::Color::Black);
    else if (currentArea_ == "maison") window_.clear(sf::Color{ 200,180,150 });
    else if (currentArea_ == "rue") window_.clear(sf::Color{ 135,206,235 });
    else if (currentArea_ == "rue_manif") window_.clear(sf::Color{ 80,120,180 });
    else if (currentArea_ == "gare") window_.clear(sf::Color{ 180,180,180 });
    else if (currentArea_ == "train_interieur") window_.clear(sf::Color{ 120,120,120 });
    else if (currentArea_ == "ecole") window_.clear(sf::Color{ 50,200,50 });

    if (state_ == GameState::Menu) {
        if (titleText_) window_.draw(*titleText_);
        window_.draw(playButtonRect_);
        if (playButtonText_) window_.draw(*playButtonText_);
    }
    else if (state_ == GameState::Credits) {
        for (size_t i = 0; i < creditsTexts_.size(); i++) {
            window_.draw(creditsTexts_[i]);
        }
    }
    else if (state_ == GameState::Playing || state_ == GameState::Paused) {
        // Afficher le jeu normalement
        if (currentArea_ == "maison") {
            window_.draw(doorRect_);
            for (auto& furni : furniture_) {
                window_.draw(furni);
            }
        }

        if (currentArea_ == "rue") {
            for (auto& decor : streetDecor_) {
                std::visit([this](auto& shape) { window_.draw(shape); }, decor);
            }
            window_.draw(doorRueRect_);
        }

        if (currentArea_ == "rue_manif") {
            for (auto& decor : manifDecor_) {
                std::visit([this](auto& shape) { window_.draw(shape); }, decor);
            }
        }

        if (currentArea_ == "gare") {
            for (auto& decor : gareDecor_) {
                std::visit([this](auto& shape) { window_.draw(shape); }, decor);
            }
        }

        if (currentArea_ == "train_interieur") {
            for (auto& decor : trainDecor_) {
                std::visit([this](auto& shape) { window_.draw(shape); }, decor);
            }
        }

        if (currentArea_ == "ecole") {
            for (auto& decor : schoolDecor_) {
                std::visit([this](auto& shape) { window_.draw(shape); }, decor);
            }
        }

        for (auto& npc : npcs_) {
            bool shouldDraw = false;

            if (currentArea_ == "maison" && npc->getType() == NPCType::Parent) {
                shouldDraw = true;
            }
            else if (currentArea_ == "rue" && npc->getType() == NPCType::Passant) {
                shouldDraw = true;
            }
            else if (currentArea_ == "rue_manif" && npc->getType() == NPCType::Manifestant) {
                shouldDraw = true;
            }
            else if (currentArea_ == "gare" && npc->getType() == NPCType::Agent) {
                shouldDraw = true;
            }
            else if (currentArea_ == "gare" && npc->getType() == NPCType::NPCgare) {
                shouldDraw = true;
            }
            else if (currentArea_ == "train_interieur" && npc->getType() == NPCType::Controleur) {
                shouldDraw = true;
            }
            else if (currentArea_ == "train_interieur" && npc->getType() == NPCType::Passager) {
                shouldDraw = true;
            }
            else if (currentArea_ == "ecole" && npc->getType() == NPCType::Ami) {
                shouldDraw = true;
            }

            if (shouldDraw) {
                npc->draw(window_);
            }
        }

        player_->draw(window_);
        hud_->draw(window_);

        // Afficher le menu pause par-dessus si nécessaire
        if (state_ == GameState::Paused) {
            window_.draw(pauseOverlay_);
            if (pauseTitleText_) window_.draw(*pauseTitleText_);
            window_.draw(resumeButtonRect_);
            if (resumeButtonText_) window_.draw(*resumeButtonText_);
            window_.draw(quitButtonRect_);
            if (quitButtonText_) window_.draw(*quitButtonText_);
        }
    }

    if (fading_) {
        sf::RectangleShape fadeRect({ 800.f, 600.f });
        fadeRect.setFillColor(sf::Color(0, 0, 0, static_cast<unsigned char>(fadeAlpha_)));
        window_.draw(fadeRect);
    }

    window_.display();
}