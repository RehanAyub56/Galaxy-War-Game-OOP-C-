#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>



// ===================== PARTICLE EFFECT CLASS =====================
class ParticleEffect {
private:
    struct Particle {
        sf::CircleShape shape;
        sf::Vector2f velocity;
        float lifetime;
        bool active;
    };

    Particle particles[50];
    int particleCount;

public:
    ParticleEffect() : particleCount(50) {
        for (int i = 0; i < particleCount; i++) {
            particles[i].active = false;
        }
    }

    void trigger(float x, float y, sf::Color color) {
        for (int i = 0; i < particleCount; i++) {
            particles[i].shape.setRadius(3 + rand() % 5);
            particles[i].shape.setFillColor(color);
            particles[i].shape.setPosition(x, y);

            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 100 + rand() % 200;
            particles[i].velocity = sf::Vector2f(cos(angle) * speed, sin(angle) * speed);
            particles[i].lifetime = 1.0f;
            particles[i].active = true;
        }
    }

    void update(float dt) {
        for (int i = 0; i < particleCount; i++) {
            if (particles[i].active) {
                particles[i].shape.move(particles[i].velocity.x * dt, particles[i].velocity.y * dt);
                particles[i].lifetime -= dt;

                sf::Color c = particles[i].shape.getFillColor();
                c.a = (particles[i].lifetime / 1.0f) * 255;
                particles[i].shape.setFillColor(c);

                if (particles[i].lifetime <= 0) particles[i].active = false;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        for (int i = 0; i < particleCount; i++) {
            if (particles[i].active) window.draw(particles[i].shape);
        }
    }

    bool isActive() {
        for (int i = 0; i < particleCount; i++) {
            if (particles[i].active) return true;
        }
        return false;
    }
};

// ===================== BOSS ENEMY CLASS =====================
class BossEnemy {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    float health;
    float maxHealth;
    float xPos, yPos;
    float moveTimer;
    float shootTimer;
    bool active;
    bool movingRight;

    sf::RectangleShape healthBarBg;
    sf::RectangleShape healthBarFg;

public:
    BossEnemy() : health(80), maxHealth(80), xPos(400), yPos(50), moveTimer(0), shootTimer(0), active(false), movingRight(true) {
        texture.loadFromFile("C:\\Users\\HP\\OneDrive\\Desktop\\OOP_Project\\OOP_Project\\images\\enemy1.png");
        sprite.setTexture(texture);
        sprite.setScale(0.5f, 0.5f);
        sprite.setPosition(xPos, yPos);

        healthBarBg.setSize(sf::Vector2f(200, 15));
        healthBarBg.setFillColor(sf::Color(50, 50, 50));
        healthBarBg.setOutlineColor(sf::Color::Red);
        healthBarBg.setOutlineThickness(2);

        healthBarFg.setSize(sf::Vector2f(200, 15));
        healthBarFg.setFillColor(sf::Color::Red);
    }

    void spawn() {
        active = true;
        health = maxHealth;
        xPos = 400;
        yPos = 50;
        sprite.setPosition(xPos, yPos);
        moveTimer = 0;
        shootTimer = 0;
        movingRight = true;
    }

    void update(float dt) {
        if (!active) return;

        moveTimer += dt;
        shootTimer += dt;

        if (moveTimer > 0.02f) {
            float moveSpeed = 150.0f;
            if (movingRight) {
                xPos += moveSpeed * dt;
                if (xPos > 800) movingRight = false;
            }
            else {
                xPos -= moveSpeed * dt;
                if (xPos < 100) movingRight = true;
            }
            sprite.setPosition(xPos, yPos);
            moveTimer = 0;
        }

        healthBarBg.setPosition(xPos - 50, yPos - 30);
        float healthPercent = health / maxHealth;
        healthBarFg.setSize(sf::Vector2f(200 * healthPercent, 15));
        healthBarFg.setPosition(xPos - 50, yPos - 30);
    }

    void takeDamage() {
        if (active && health > 0) {
            health--;
            if (health <= 0) active = false;
        }
    }

    bool shouldShoot() {
        if (!active) return false;
        if (shootTimer > 1.5f) {
            shootTimer = 0;
            return true;
        }
        return false;
    }

    sf::Vector2f getBulletSpawnPos() {
        return sf::Vector2f(xPos + sprite.getGlobalBounds().width / 2, yPos + sprite.getGlobalBounds().height);
    }

    void draw(sf::RenderWindow& window) {
        if (active) {
            if (health < maxHealth * 0.3f) {
                int flash = (int)(moveTimer * 20) % 2;
                if (flash == 0) sprite.setColor(sf::Color::Red);
                else sprite.setColor(sf::Color::White);
            }
            else {
                sprite.setColor(sf::Color::White);
            }

            window.draw(sprite);
            window.draw(healthBarBg);
            window.draw(healthBarFg);
        }
    }

    bool isActive() const { return active; }
    bool isDead() const { return !active && health <= 0; }
    sf::FloatRect getBounds() const { return sprite.getGlobalBounds(); }
};

// ===================== SOUND MANAGER CLASS =====================
class SoundManager {
private:
    sf::Music backgroundMusic;
    sf::SoundBuffer shootBuffer, explosionBuffer, powerUp1Buffer, powerUp2Buffer, powerUp3Buffer;
    sf::SoundBuffer hitBuffer, moveBuffer, pauseBuffer;
    sf::Sound shootSound, explosionSound, powerUp1Sound, powerUp2Sound, powerUp3Sound;
    sf::Sound hitSound, moveSound, pauseSound;

public:
    SoundManager() {
        const std::string path = "C:\\Users\\HP\\OneDrive\\Desktop\\OOP_Project\\OOP_Project\\sounds\\";

        if (!backgroundMusic.openFromFile(path + "backgroundmusic.ogg")) {
            std::cout << "ERROR: backgroundmusic.wav not found!\n";
        }
        else {
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(10);
            backgroundMusic.play();
        }

        if (shootBuffer.loadFromFile(path + "live_dec.wav")) shootSound.setBuffer(shootBuffer);
        if (powerUp1Buffer.loadFromFile(path + "powerup1.wav")) powerUp1Sound.setBuffer(powerUp1Buffer);
        if (powerUp2Buffer.loadFromFile(path + "powerup2.wav")) powerUp2Sound.setBuffer(powerUp2Buffer);
        if (powerUp3Buffer.loadFromFile(path + "powerup3.wav")) powerUp3Sound.setBuffer(powerUp3Buffer);
        if (hitBuffer.loadFromFile(path + "live_dec.wav")) hitSound.setBuffer(hitBuffer);
        if (moveBuffer.loadFromFile(path + "move.wav")) moveSound.setBuffer(moveBuffer);
        if (pauseBuffer.loadFromFile(path + "pause.wav")) pauseSound.setBuffer(pauseBuffer);

        shootSound.setVolume(60);
        explosionSound.setVolume(85);
        powerUp1Sound.setVolume(75);
        powerUp2Sound.setVolume(75);
        powerUp3Sound.setVolume(75);
        hitSound.setVolume(80);
        moveSound.setVolume(65);
        pauseSound.setVolume(80);
    }

    void playShoot() { shootSound.play(); }
    void playExplosion() { explosionSound.play(); }
    void playPowerUp1() { powerUp1Sound.play(); }
    void playPowerUp2() { powerUp2Sound.play(); }
    void playPowerUp3() { powerUp3Sound.play(); }
    void playHit() { hitSound.play(); }
    void playMove() { moveSound.play(); }
    void playPauseFx() { pauseSound.play(); }
    void pauseMusic() { backgroundMusic.pause(); }
    void resumeMusic() { if (backgroundMusic.getStatus() != sf::Music::Playing) backgroundMusic.play(); }
    void stopMusic() { backgroundMusic.stop(); }
};