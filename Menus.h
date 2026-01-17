// ===================== MENUS.H — ALL MENUS IN ONE CLEAN HEADER =====================
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
const int MAX_MENU_ITEMS = 10;
struct Star {
    sf::CircleShape shape;
    float speed;
};

// ===================== BASE MENU =====================
class BaseMenu {
protected:
    int selectedIndex = 0;
    int width, height;
    sf::Font font;
public:
    static const int MAX_ITEMS = 10;
    BaseMenu(int w, int h, const sf::Font& f) : width(w), height(h), font(f) {}
    virtual ~BaseMenu() = default;
    virtual void moveUp() { if (selectedIndex > 0) selectedIndex--; }
    virtual void moveDown() { selectedIndex++; }
    int getSelectedIndex() const { return selectedIndex; }
    virtual void update(float dt) {}
    virtual void draw(sf::RenderWindow& window) = 0;
};

// ===================== MAIN MENU =====================
class MainMenu : public BaseMenu {
private:
    sf::Text items[MAX_ITEMS];
    std::string labels[MAX_ITEMS];
    int itemCount = 0;
    sf::Text title;
    Star stars[100];
    void updateGlow(float pulse) {
        for (int i = 0; i < itemCount; i++) {
            if (i == selectedIndex) {
                float glow = 200 + 55 * sin(pulse * 5);
                items[i].setFillColor(sf::Color(255, 255, 100 + (int)glow, 255));
                items[i].setOutlineThickness(4);
                items[i].setOutlineColor(sf::Color(255, 215, 0));
            }
            else {
                items[i].setFillColor(sf::Color::White);
                items[i].setOutlineThickness(1);
                items[i].setOutlineColor(sf::Color(0, 0, 0, 100));
            }
        }
    }
public:
    MainMenu(int w, int h, const sf::Font& f) : BaseMenu(w, h, f) {
        title.setFont(font);
        title.setString("GALAXY WAR");
        title.setCharacterSize(80);
        title.setFillColor(sf::Color(255, 50, 50));
        title.setOutlineColor(sf::Color(255, 200, 0));
        title.setOutlineThickness(5);
        title.setLetterSpacing(2);
        title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
        title.setPosition(w / 2.0f, 100);

        for (int i = 0; i < 100; i++) {
            float size = (rand() % 3) + 1.0f;
            stars[i].shape = sf::CircleShape(size);
            stars[i].shape.setFillColor(sf::Color(255, 255, 255, rand() % 150 + 100));
            stars[i].shape.setPosition(rand() % w, rand() % h);
            stars[i].speed = 0.2f + (rand() % 100) / 200.0f;
        }
    }

    void setupMenu(const std::string menuLabels[], int count) {
        itemCount = count > MAX_ITEMS ? MAX_ITEMS : count;
        selectedIndex = 0;
        for (int i = 0; i < itemCount; i++) {
            labels[i] = menuLabels[i];
            items[i].setFont(font);
            items[i].setString(labels[i]);
            items[i].setCharacterSize(48);
            items[i].setLetterSpacing(1.5f);
            items[i].setOrigin(items[i].getLocalBounds().width / 2, items[i].getLocalBounds().height / 2);
            items[i].setPosition(width / 2.0f, height / 2.0f - 80 + i * 90);
        }
    }

    void update(float dt) override {
        static float t = 0; t += dt;
        updateGlow(t);
        for (auto& s : stars) {
            s.shape.move(0, s.speed * 60 * dt);
            if (s.shape.getPosition().y > height)
                s.shape.setPosition(rand() % width, -10);
        }
    }

    void draw(sf::RenderWindow& window) override {
        for (const auto& s : stars) window.draw(s.shape);
        window.draw(title);
        for (int i = 0; i < itemCount; i++) window.draw(items[i]);
    }

    void moveUp(class SoundManager& sm) { if (selectedIndex > 0) { selectedIndex--; sm.playMove(); } }
    void moveDown(class SoundManager& sm) { if (selectedIndex < itemCount - 1) { selectedIndex++; sm.playMove(); } }
};

// ===================== LEVEL MENU =====================
class LevelMenu : public BaseMenu {
private:
    sf::Text items[3];
    sf::RectangleShape bg, border;
    sf::Text title;
public:
    LevelMenu(int w, int h, const sf::Font& f) : BaseMenu(w, h, f) {
        bg.setSize(sf::Vector2f(500, 400));
        bg.setFillColor(sf::Color(0, 0, 50, 230));
        bg.setPosition(w / 2 - 250, h / 2 - 200);
        border.setSize(sf::Vector2f(510, 410));
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineColor(sf::Color(0, 200, 255));
        border.setOutlineThickness(6);
        border.setPosition(w / 2 - 255, h / 2 - 205);

        title.setFont(font);
        title.setString("SELECT DIFFICULTY");
        title.setCharacterSize(50);
        title.setFillColor(sf::Color::Cyan);
        title.setOutlineColor(sf::Color::Black);
        title.setOutlineThickness(3);
        title.setPosition(w / 2, 120);
        title.setOrigin(title.getLocalBounds().width / 2, 0);

        const std::string names[] = { "EASY", "MEDIUM", "HARD" };
        const sf::Color colors[] = { sf::Color(100,255,100), sf::Color(255,200,0), sf::Color(255,50,50) };
        for (int i = 0; i < 3; i++) {
            items[i].setFont(font);
            items[i].setString(names[i]);
            items[i].setCharacterSize(60);
            items[i].setFillColor(colors[i]);
            items[i].setOutlineThickness(2);
            items[i].setOutlineColor(sf::Color::Black);
            items[i].setOrigin(items[i].getLocalBounds().width / 2, items[i].getLocalBounds().height / 2);
            items[i].setPosition(w / 2.0f, h / 2.0f - 80 + i * 110);
        }
    }

    void draw(sf::RenderWindow& window) override {
        window.draw(bg);
        window.draw(border);
        window.draw(title);
        for (int i = 0; i < 3; i++) {
            items[i].setOutlineThickness(i == selectedIndex ? 5 : 2);
            items[i].setOutlineColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::Black);
            window.draw(items[i]);
        }
    }

    void moveUp()   override { if (selectedIndex > 0) selectedIndex--; }
    void moveDown() override { if (selectedIndex < 2) selectedIndex++; }
    int getSelectedLevel() const { return selectedIndex; }
};

// ===================== PAUSE MENU =====================
class PauseMenu : public BaseMenu {
private:
    sf::Text items[MAX_ITEMS];
    int itemCount = 0;
    sf::RectangleShape overlay;
public:
    PauseMenu(int w, int h, const sf::Font& f) : BaseMenu(w, h, f) {
        overlay.setSize(sf::Vector2f(w, h));
        overlay.setFillColor(sf::Color(0, 0, 30, 220));
    }
    void setupMenu(const std::string labels[], int count) {
        itemCount = count > MAX_ITEMS ? MAX_ITEMS : count;
        for (int i = 0; i < itemCount; i++) {
            items[i].setFont(font);
            items[i].setString(labels[i]);
            items[i].setCharacterSize(60);
            items[i].setLetterSpacing(2);
            items[i].setOutlineThickness(3);
            items[i].setOutlineColor(sf::Color::Black);
            items[i].setOrigin(items[i].getLocalBounds().width / 2, items[i].getLocalBounds().height / 2);
            items[i].setPosition(width / 2.0f, height / 2.0f - 100 + i * 110);
        }
    }
    void draw(sf::RenderWindow& window) override {
        window.draw(overlay);
        for (int i = 0; i < itemCount; i++) {
            items[i].setFillColor(i == selectedIndex ? sf::Color::Cyan : sf::Color(200, 200, 255));
            window.draw(items[i]);
        }
    }
    void moveUp()   override { if (selectedIndex > 0) selectedIndex--; }
    void moveDown() override { if (selectedIndex < itemCount - 1) selectedIndex++; }
};

// ===================== GAME OVER / VICTORY MENU =====================
class GameOverMenu : public BaseMenu {
private:
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    bool textureLoaded = false;
    sf::Text title, scoreText, hintText;
    bool victory = false;
    float animTime = 0.0f;
public:
    GameOverMenu(int w, int h, const sf::Font& f) : BaseMenu(w, h, f) {
        if (bgTexture.loadFromFile("C:\\Users\\HP\\OneDrive\\Desktop\\OOP_Project\\OOP_Project\\images\\gameover_bg.jpg")) {
            bgSprite.setTexture(bgTexture);
            float sx = (float)w / bgTexture.getSize().x;
            float sy = (float)h / bgTexture.getSize().y;
            bgSprite.setScale(sx, sy);
            textureLoaded = true;
        }

        title.setFont(font);
        title.setCharacterSize(90);
        title.setOutlineThickness(6);
        title.setOutlineColor(sf::Color::Black);
        title.setPosition(w / 2.0f, 200);
        title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);

        scoreText.setFont(font);
        scoreText.setCharacterSize(45);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setOutlineThickness(3);
        scoreText.setOutlineColor(sf::Color::Black);
        scoreText.setPosition(w / 2.0f, 320);

        hintText.setFont(font);
        hintText.setCharacterSize(32);
        hintText.setFillColor(sf::Color::Cyan);
        hintText.setString("PRESS ENTER TO RETURN TO MAIN MENU");
        hintText.setOrigin(hintText.getLocalBounds().width / 2, 0);
        hintText.setPosition(w / 2.0f, 460);
    }

    void show(bool win, int finalScore) {
        victory = win;
        animTime = 0;
        title.setString(win ? "VICTORY!" : "GAME OVER");
        title.setFillColor(win ? sf::Color(100, 255, 100) : sf::Color(255, 80, 80));
        title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
        scoreText.setString("FINAL SCORE: " + std::to_string(finalScore));
        scoreText.setOrigin(scoreText.getLocalBounds().width / 2, 0);
    }

    void update(float dt) override {
        animTime += dt;
        float pulse = sin(animTime * 4) * 0.4f + 0.6f;
        sf::Color c = victory ? sf::Color(100, 255, 100) : sf::Color(255, 80, 80);
        title.setFillColor(sf::Color(c.r, (sf::Uint8)(c.g * pulse), (sf::Uint8)(c.b * pulse)));
        float scale = 1.0f + 0.08f * sin(animTime * 5);
        title.setScale(scale, scale);
        sf::Color h = hintText.getFillColor();
        h.a = (sf::Uint8)((sin(animTime * 4) * 0.5f + 0.5f) * 255);
        hintText.setFillColor(h);
    }

    void draw(sf::RenderWindow& window) override {
        if (textureLoaded) window.draw(bgSprite);
        else {
            sf::RectangleShape dark(sf::Vector2f(width, height));
            dark.setFillColor(sf::Color(0, 0, 30, 230));
            window.draw(dark);
        }
        window.draw(title);
        window.draw(scoreText);
        window.draw(hintText);
    }
};

// ===================== ENTER NAME MENU =====================
class EnterNameMenu : public BaseMenu {
private:
    sf::Text title, prompt, nameText, instruction;
    std::string playerName;
    float blinkTimer;
    bool showCursor;
    bool isFinished;

public:
    EnterNameMenu(int w, int h, const sf::Font& f) : BaseMenu(w, h, f), blinkTimer(0.0f), showCursor(true), isFinished(false) {
        title.setFont(font);
        title.setString("GALACTIC HERO");
        title.setCharacterSize(90);
        title.setFillColor(sf::Color(255, 100, 100));
        title.setOutlineColor(sf::Color(255, 215, 0));
        title.setOutlineThickness(6);
        title.setLetterSpacing(2.5f);
        title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
        title.setPosition(w / 2.0f, 150);

        prompt.setFont(font);
        prompt.setString("ENTER YOUR NAME:");
        prompt.setCharacterSize(48);
        prompt.setFillColor(sf::Color::Cyan);
        prompt.setOutlineColor(sf::Color::Black);
        prompt.setOutlineThickness(3);
        prompt.setOrigin(prompt.getLocalBounds().width / 2, 0);
        prompt.setPosition(w / 2.0f, 260);

        nameText.setFont(font);
        nameText.setCharacterSize(70);
        nameText.setFillColor(sf::Color::Yellow);
        nameText.setOutlineColor(sf::Color::Black);
        nameText.setOutlineThickness(3);
        nameText.setPosition(w / 2.0f, 380);
        nameText.setOrigin(0, nameText.getLocalBounds().height / 2);

        instruction.setFont(font);
        instruction.setString("PRESS ENTER TO CONFIRM");
        instruction.setCharacterSize(32);
        instruction.setFillColor(sf::Color::White);
        instruction.setStyle(sf::Text::Italic);
        instruction.setOrigin(instruction.getLocalBounds().width / 2, 0);
        instruction.setPosition(w / 2.0f, 480);
    }

    void handleInput(sf::Event& event) {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b' && !playerName.empty()) {
                playerName.pop_back();
            }
            else if (event.text.unicode == '\r' || event.text.unicode == '\n' || event.text.unicode == 13) {
                if (playerName.empty()) playerName = "PLAYER";
                isFinished = true;
            }
            else if (playerName.length() < 12 && event.text.unicode >= 32 && event.text.unicode < 128) {
                char c = static_cast<char>(event.text.unicode);
                if (playerName.empty()) c = std::toupper(c);
                playerName += c;
            }
        }

        // Also handle KeyPressed event for Enter key
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Enter) {
                if (playerName.empty()) playerName = "PLAYER";
                isFinished = true;
            }
        }
    }

    void update(float dt) override {
        blinkTimer += dt * 4.0f;
        showCursor = sin(blinkTimer) > 0;

        // Pulsing title glow
        float glow = 100 + 155 * (sin(blinkTimer * 1.5f) * 0.5f + 0.5f);
        title.setFillColor(sf::Color(255, 80 + (int)glow, 80 + (int)glow));

        // Update name with blinking cursor
        std::string display = playerName;
        if (showCursor && playerName.length() < 12) display += "_";
        nameText.setString(display.empty() ? "_" : display);
        nameText.setOrigin(nameText.getLocalBounds().width / 2, nameText.getLocalBounds().height / 2);
        nameText.setPosition(width / 2.0f, 380);
    }

    void draw(sf::RenderWindow& window) override {
        // Animated starfield background
        for (int i = 0; i < 120; ++i) {
            float angle = i * 3.0f + blinkTimer * 30;
            float radius = 50 + i * 4;
            float x = width / 2 + cos(angle * 3.14159f / 180) * radius;
            float y = height / 2 + sin(angle * 3.14159f / 180) * radius;
            sf::CircleShape star(1.0f + sin(i + blinkTimer * 2) * 0.8f);
            star.setFillColor(sf::Color(255, 255, 255, 100 + (int)(155 * sin(i + blinkTimer))));
            star.setPosition(x, y);
            window.draw(star);
        }

        window.draw(title);
        window.draw(prompt);

        // Name box
        sf::RectangleShape box(sf::Vector2f(520, 100));
        box.setPosition(width / 2.0f - 260, 340);
        box.setFillColor(sf::Color(0, 0, 50, 200));
        box.setOutlineColor(sf::Color::Cyan);
        box.setOutlineThickness(5);
        window.draw(box);

        window.draw(nameText);
        window.draw(instruction);
    }

    std::string getPlayerName() const { return playerName.empty() ? "PLAYER" : playerName; }
    bool finished() const { return isFinished; }
    void reset() { playerName.clear(); isFinished = false; blinkTimer = 0; }
};