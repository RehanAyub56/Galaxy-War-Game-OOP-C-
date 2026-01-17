#include <iostream> 
#include "GameClasses.h" 
#include "Menus.h" 
#include <fstream> 
#include <sstream> 
#include <SFML/Graphics.hpp> 
#include <SFML/Audio.hpp> 
#include <string> 
#include <cmath> 
#include <ctime> 
#include <cctype>  

using namespace sf;

const std::string SCORE_FILE = "scores.txt";

// Function to save score 
void saveScore(const std::string& name, int score) {
    std::ofstream file(SCORE_FILE, std::ios::app);  // append mode 
    if (file.is_open()) {
        file << name << " " << score << "\n";
        file.close();
    }
}

int main() {
    srand((unsigned)time(0));
    RenderWindow window(VideoMode(1000, 650), "Galaxy War", Style::Titlebar | Style::Close);
    Event event;
    Clock clock;
    Font gameFont;

    if (!gameFont.loadFromFile("C:\\Users\\HP\\OneDrive\\Desktop\\OOP_Project\\OOP_Project\\images\\airstrike.ttf")) {
        std::cout << "Font not found!\n";
        return -1;
    }

    SoundManager sound;

    MainMenu mainMenu(1000, 650, gameFont);
    std::string mainItems[] = { "Start Game", "Levels", "High Scores", "Exit" };
    mainMenu.setupMenu(mainItems, 4);

    LevelMenu levelMenu(1000, 650, gameFont);
    PauseMenu pauseMenu(1000, 650, gameFont);
    std::string pauseItems[] = { "Resume", "Restart", "Quit" };
    pauseMenu.setupMenu(pauseItems, 3);

    EnterNameMenu enterNameMenu(1000, 650, gameFont);
    GameOverMenu gameOverMenu(1000, 650, gameFont);

    bool inMainMenu = true, inLevelMenu = false, inEnterName = false, inGame = false, isPaused = false, inGameOver = false;
    int selectedDifficulty = 0;
    std::string playerName = "PLAYER";

    // restart flag 
    bool resetGameState = false;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                sound.playPauseFx();
                if (inGame && !isPaused && !inGameOver) isPaused = true;
                else if (isPaused) isPaused = false;
                else if (inEnterName) { inEnterName = false; inMainMenu = true; }
                else if (inLevelMenu) { inLevelMenu = false; inMainMenu = true; }
                else window.close();
            }

            if (inMainMenu && event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Up) mainMenu.moveUp(sound);
                if (event.key.code == Keyboard::Down) mainMenu.moveDown(sound);
                if (event.key.code == Keyboard::Enter) {
                    sound.playPauseFx();
                    int c = mainMenu.getSelectedIndex();
                    if (c == 0) {
                        inMainMenu = false;
                        inEnterName = true;
                        selectedDifficulty = 0; // Default to easy
                        resetGameState = true; // Reset game when starting new game
                    }
                    if (c == 1) {
                        inMainMenu = false;
                        inLevelMenu = true;
                    }
                    if (c == 3) window.close();
                }
            }
            else if (inLevelMenu && event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Up) levelMenu.moveUp();
                if (event.key.code == Keyboard::Down) levelMenu.moveDown();
                if (event.key.code == Keyboard::Enter) {
                    sound.playPauseFx();
                    selectedDifficulty = levelMenu.getSelectedLevel();
                    inLevelMenu = false;
                    inEnterName = true;
                    resetGameState = true;
                }
            }
            else if (inEnterName) {
                enterNameMenu.handleInput(event);

                if (enterNameMenu.finished()) {
                    playerName = enterNameMenu.getPlayerName();
                    inEnterName = false;
                    inGame = true;
                    enterNameMenu.reset();
                    resetGameState = true;
                }
            }
            else if (isPaused && !inGameOver && event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Up) pauseMenu.moveUp();
                if (event.key.code == Keyboard::Down) pauseMenu.moveDown();
                if (event.key.code == Keyboard::Enter) {
                    sound.playPauseFx();
                    int c = pauseMenu.getSelectedIndex();
                    if (c == 0 || c == 1) {
                        if (c == 1) resetGameState = true;
                        isPaused = false;
                    }
                    if (c == 2) window.close();
                }
            }
            else if (inGame && !isPaused && !inGameOver && event.type == Event::KeyPressed && (event.key.code == Keyboard::P)) {
                sound.playPauseFx();
                isPaused = true;
            }
            else if (inGameOver && event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
                inGameOver = false;
                inMainMenu = true;
                sound.resumeMusic();
            }
        }

        window.clear(sf::Color(5, 0, 20));

        if (inMainMenu) { mainMenu.update(dt); mainMenu.draw(window); }
        else if (inLevelMenu) levelMenu.draw(window);
        else if (inEnterName) {
            enterNameMenu.update(dt);  // This line is now uncommented!
            enterNameMenu.draw(window);
        }
        else if (inGameOver) { gameOverMenu.update(dt); gameOverMenu.draw(window); }
        else if (inGame) {
            static sf::Texture bgTex, playerTex, bulletTex, enemyTex;
            static sf::Texture powerTex1, powerTex2, powerTex3;
            static Sprite background, player;
            static bool loaded = false;
            static int lives = 3, score = 0;
            static bool gameOver = false;
            static bool doubleFireActive = false;
            static float doubleFireTimer = 0.0f;
            static bool shieldActive = false;
            static float shieldTimer = 0.0f;
            static float blinkTimer = 0.0f;
            static bool playerVisible = true;
            static bool slowMotionActive = false;
            static float slowMotionTimer = 0.0f;

            static int lastBgDifficulty = -1;

            static Sprite powerUp1, powerUp2, powerUp3;
            static bool powerUp1Active = false, powerUp2Active = false, powerUp3Active = false;
            static float powerUp1Y = -100, powerUp2Y = -100, powerUp3Y = -100;

            static BossEnemy boss;
            static bool bossSpawned = false;
            static ParticleEffect powerUpEffect;
            static ParticleEffect bossDeathEffect;
            static float powerUpEffectTimer = 0;
            static float bossDeathTimer = 0;
            static bool bossJustDied = false;

            const int MAX_BOSS_BULLETS = 20;
            static Sprite bossBullets[MAX_BOSS_BULLETS];
            static bool bossBulletActive[MAX_BOSS_BULLETS] = { false };

            static const int WINGMEN_COUNT = 10;
            static Sprite wingmen[WINGMEN_COUNT];
            static bool wingmenActive = false;
            static float wingmenTimer = 0.0f;
            static float wingmenFireTimer = 0.0f;

            const int MAX_WINGMEN_BULLETS = 100;
            static Sprite wingmenBullets[MAX_WINGMEN_BULLETS];
            static bool wingmenBulletActive[MAX_WINGMEN_BULLETS] = { false };

            const int MAX_BULLETS = 100;
            static Sprite bullets[MAX_BULLETS];
            static bool bulletActive[MAX_BULLETS] = { false };
            static float fireTimer = 0;

            const int MAX_ENEMIES = 20;
            static Sprite enemies[MAX_ENEMIES];
            static bool enemyActive[MAX_ENEMIES] = { false };
            static float enemyY[MAX_ENEMIES];
            static float spawnTimer = 0;

            if (resetGameState) {
                lives = 3;
                score = 0;
                gameOver = false;
                doubleFireActive = false;
                doubleFireTimer = 0.0f;
                shieldActive = false;
                shieldTimer = 0.0f;
                blinkTimer = 0.0f;
                playerVisible = true;
                slowMotionActive = false;
                slowMotionTimer = 0.0f;

                powerUp1Active = false; powerUp2Active = false; powerUp3Active = false;
                powerUp1Y = -100; powerUp2Y = -100; powerUp3Y = -100;

                bossSpawned = false;
                bossJustDied = false;
                bossDeathTimer = 0.0f;
                powerUpEffectTimer = 0.0f;

                wingmenActive = false;
                wingmenTimer = 0.0f;
                wingmenFireTimer = 0.0f;

                fireTimer = 0.0f;
                spawnTimer = 0.0f;

                for (int i = 0; i < MAX_BULLETS; ++i) bulletActive[i] = false;
                for (int i = 0; i < MAX_ENEMIES; ++i) enemyActive[i] = false;
                for (int i = 0; i < MAX_BOSS_BULLETS; ++i) bossBulletActive[i] = false;
                for (int i = 0; i < MAX_WINGMEN_BULLETS; ++i) wingmenBulletActive[i] = false;

                if (loaded) {
                    player.setPosition(500, 520);
                }

                lastBgDifficulty = -1;
                resetGameState = false;
            }

            if (!loaded) {
                std::string imgPath = "C:\\Users\\HP\\OneDrive\\Desktop\\OOP_Project\\OOP_Project\\images\\";

                playerTex.loadFromFile(imgPath + "ship1.png");
                player.setTexture(playerTex);
                player.setScale(0.55f, 0.55f);
                player.setPosition(500, 520);

                bulletTex.loadFromFile(imgPath + "bullet.png");
                enemyTex.loadFromFile(imgPath + "enemy1.png");

                powerTex1.loadFromFile(imgPath + "powerup1.png");
                powerTex2.loadFromFile(imgPath + "powerup2.png");
                powerTex3.loadFromFile(imgPath + "powerup3.png");

                powerUp1.setTexture(powerTex1);
                powerUp2.setTexture(powerTex2);
                powerUp3.setTexture(powerTex3);
                powerUp1.setScale(0.6f, 0.6f);
                powerUp2.setScale(0.6f, 0.6f);
                powerUp3.setScale(0.6f, 0.6f);

                for (int i = 0; i < WINGMEN_COUNT; i++) {
                    wingmen[i].setTexture(playerTex);
                    wingmen[i].setScale(0.55f, 0.55f);
                    wingmen[i].setColor(sf::Color(150, 150, 255));
                }

                loaded = true;
            }

            if (lastBgDifficulty != selectedDifficulty) {
                std::string imgPath = "C:\\Users\\HP\\OneDrive\\Desktop\\OOP_Project\\OOP_Project\\images\\";
                std::string bgFile;
                if (selectedDifficulty == 0) {
                    bgFile = "Gamebackground.png";
                }
                else if (selectedDifficulty == 1) {
                    bgFile = "medium.jpeg";
                }
                else {
                    bgFile = "hard.jpeg";
                }
                if (!bgTex.loadFromFile(imgPath + bgFile)) {
                    std::cout << "Background '" << bgFile << "' not found! Using default.\n";
                    bgTex.loadFromFile(imgPath + "Gamebackground.png");
                }
                background.setTexture(bgTex, true);
                background.setScale(1000.0f / bgTex.getSize().x, 650.0f / bgTex.getSize().y);
                lastBgDifficulty = selectedDifficulty;
            }

            window.draw(background);

            if (!gameOver && !isPaused) {
                static bool moving = false;
                bool isMoving = (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right) ||
                    Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::D));

                if (isMoving && !moving) {
                    sound.playMove();
                    moving = true;
                }
                else if (!isMoving) {
                    moving = false;
                }

                float speed = 400.0f;
                if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
                    player.move(-speed * dt, 0);
                if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
                    player.move(speed * dt, 0);
                if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
                    player.move(0, -speed * dt);
                if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
                    player.move(0, speed * dt);

                sf::Vector2f pos = player.getPosition();
                if (pos.x < 0) pos.x = 0;
                if (pos.x > 1000 - player.getGlobalBounds().width)
                    pos.x = 1000 - player.getGlobalBounds().width;
                if (pos.y < 0) pos.y = 0;
                if (pos.y > 650 - player.getGlobalBounds().height)
                    pos.y = 650 - player.getGlobalBounds().height;
                player.setPosition(pos);
            }

            if (!gameOver && !isPaused) {
                fireTimer += dt;
                float fireRate = (doubleFireActive || slowMotionActive) ? 0.05f : 0.15f;

                if (Keyboard::isKeyPressed(Keyboard::Space) && fireTimer > fireRate) {
                    int shots = doubleFireActive ? 2 : 1;
                    for (int s = 0; s < shots; s++) {
                        for (int i = 0; i < MAX_BULLETS; i++) {
                            if (!bulletActive[i]) {
                                bullets[i].setTexture(bulletTex);
                                bullets[i].setScale(0.6f, 0.6f);
                                float offset = (s == 0) ? -20 : 20;
                                bullets[i].setPosition(
                                    player.getPosition().x + player.getGlobalBounds().width / 2 - 12 + offset,
                                    player.getPosition().y
                                );
                                bulletActive[i] = true;
                                break;
                            }
                        }
                    }
                    sound.playShoot();
                    fireTimer = 0;
                }

                if (doubleFireActive) {
                    doubleFireTimer += dt;
                    if (doubleFireTimer > 8.0f) doubleFireActive = false;
                }
                if (slowMotionActive) {
                    slowMotionTimer += dt;
                    if (slowMotionTimer > 6.0f) slowMotionActive = false;
                }
            }

            if (wingmenActive && !gameOver && !isPaused) {
                wingmenTimer += dt;

                if (wingmenTimer > 6.0f) {
                    wingmenActive = false;
                    wingmenTimer = 0.0f;
                }

                sf::Vector2f playerPos = player.getPosition();
                float playerWidth = player.getGlobalBounds().width;
                float wingmanWidth = wingmen[0].getGlobalBounds().width;
                float horizontalSpacing = wingmanWidth + 10.0f;

                for (int i = 0; i < 5; i++) {
                    float xOffset = (i + 1) * horizontalSpacing;
                    wingmen[i].setPosition(playerPos.x - xOffset, playerPos.y);
                }
                for (int i = 5; i < 10; i++) {
                    float xOffset = playerWidth + ((i - 4) * horizontalSpacing);
                    wingmen[i].setPosition(playerPos.x + xOffset, playerPos.y);
                }

                wingmenFireTimer += dt;
                if (wingmenFireTimer > 0.4f) {
                    for (int w = 0; w < WINGMEN_COUNT; w++) {
                        for (int i = 0; i < MAX_WINGMEN_BULLETS; i++) {
                            if (!wingmenBulletActive[i]) {
                                wingmenBullets[i].setTexture(bulletTex);
                                wingmenBullets[i].setScale(0.6f, 0.6f);
                                wingmenBullets[i].setColor(sf::Color(150, 150, 255));
                                sf::Vector2f wingmanPos = wingmen[w].getPosition();
                                wingmenBullets[i].setPosition(
                                    wingmanPos.x + wingmen[w].getGlobalBounds().width / 2 - 8,
                                    wingmanPos.y
                                );
                                wingmenBulletActive[i] = true;
                                break;
                            }
                        }
                    }
                    wingmenFireTimer = 0;
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bulletActive[i]) {
                    bullets[i].move(0, -1000 * dt);
                    if (bullets[i].getPosition().y < -50) bulletActive[i] = false;
                }
            }

            for (int i = 0; i < MAX_WINGMEN_BULLETS; i++) {
                if (wingmenBulletActive[i]) {
                    wingmenBullets[i].move(0, -800 * dt);
                    if (wingmenBullets[i].getPosition().y < -50) wingmenBulletActive[i] = false;
                }
            }

            if (!gameOver && !isPaused && !bossSpawned) {
                spawnTimer += dt;
                float baseSpawnRate = 0.8f;
                float baseEnemySpeed = 120.0f;
                if (selectedDifficulty == 1) {
                    baseSpawnRate = 0.5f;
                    baseEnemySpeed = 180.0f;
                }
                else if (selectedDifficulty == 2) {
                    baseSpawnRate = 0.3f;
                    baseEnemySpeed = 240.0f;
                }

                if (spawnTimer > baseSpawnRate) {
                    int spawned = 0;
                    for (int i = 0; i < MAX_ENEMIES && spawned < 3; i++) {
                        if (!enemyActive[i]) {
                            enemies[i].setTexture(enemyTex);
                            enemies[i].setScale(0.1f, 0.1f);
                            enemies[i].setPosition(80 + rand() % 840, -120 - spawned * 80);
                            enemyY[i] = -120 - spawned * 80;
                            enemyActive[i] = true;
                            spawned++;
                        }
                    }
                    spawnTimer = 0;
                }

                float enemySpeed = slowMotionActive ? 40.0f : baseEnemySpeed;

                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (enemyActive[i]) {
                        enemyY[i] += enemySpeed * dt;
                        enemies[i].setPosition(enemies[i].getPosition().x, enemyY[i]);
                        if (enemyY[i] > 700) enemyActive[i] = false;

                        if (enemies[i].getGlobalBounds().intersects(player.getGlobalBounds())) {
                            enemyActive[i] = false;
                            if (!shieldActive) {
                                lives--;
                                sound.playHit();
                                if (lives <= 0) {
                                    saveScore(playerName, score);
                                    gameOver = true;
                                    sound.stopMusic();
                                    gameOverMenu.show(false, score);
                                    inGame = false;
                                    inGameOver = true;
                                    lives = 3; score = 0;
                                    doubleFireActive = false; shieldActive = false; slowMotionActive = false;
                                    powerUp1Active = false; powerUp2Active = false; powerUp3Active = false;
                                    bossSpawned = false; bossJustDied = false; wingmenActive = false;
                                    for (int j = 0; j < MAX_BULLETS; j++) bulletActive[j] = false;
                                    for (int j = 0; j < MAX_ENEMIES; j++) enemyActive[j] = false;
                                    for (int j = 0; j < MAX_BOSS_BULLETS; j++) bossBulletActive[j] = false;
                                    for (int j = 0; j < MAX_WINGMEN_BULLETS; j++) wingmenBulletActive[j] = false;
                                    sound.resumeMusic();
                                }
                            }
                        }
                    }
                }
            }

            for (int b = 0; b < MAX_BULLETS; b++) {
                if (!bulletActive[b]) continue;
                for (int e = 0; e < MAX_ENEMIES; e++) {
                    if (enemyActive[e] &&
                        bullets[b].getGlobalBounds().intersects(enemies[e].getGlobalBounds())) {
                        bulletActive[b] = false;
                        enemyActive[e] = false;
                        score += 10;
                        sound.playExplosion();

                        if (score == 100 && !powerUp1Active) {
                            powerUp1.setPosition(450 + rand() % 100, -100);
                            powerUp1Y = -100;
                            powerUp1Active = true;
                        }
                        if (score == 200 && !powerUp2Active) {
                            powerUp2.setPosition(450 + rand() % 100, -100);
                            powerUp2Y = -100;
                            powerUp2Active = true;
                        }
                        if (score == 300 && !powerUp3Active && selectedDifficulty == 2) {
                            powerUp3.setPosition(450 + rand() % 100, -100);
                            powerUp3Y = -100;
                            powerUp3Active = true;
                        }
                        break;
                    }
                }
            }

            for (int b = 0; b < MAX_WINGMEN_BULLETS; b++) {
                if (!wingmenBulletActive[b]) continue;
                for (int e = 0; e < MAX_ENEMIES; e++) {
                    if (enemyActive[e] &&
                        wingmenBullets[b].getGlobalBounds().intersects(enemies[e].getGlobalBounds())) {
                        wingmenBulletActive[b] = false;
                        enemyActive[e] = false;
                        score += 10;
                        sound.playExplosion();
                        break;
                    }
                }
            }

            if (score >= 500 && !bossSpawned) {
                boss.spawn();
                bossSpawned = true;
            }

            if (bossSpawned && !bossJustDied) {
                boss.update(dt);

                if (boss.shouldShoot() && !gameOver && !isPaused) {
                    for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
                        if (!bossBulletActive[i]) {
                            bossBullets[i].setTexture(bulletTex);
                            bossBullets[i].setScale(0.5f, 0.5f);
                            bossBullets[i].setColor(sf::Color::Red);
                            sf::Vector2f pos = boss.getBulletSpawnPos();
                            bossBullets[i].setPosition(pos.x, pos.y);
                            bossBulletActive[i] = true;
                            sound.playShoot();
                            break;
                        }
                    }
                }

                for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
                    if (bossBulletActive[i]) {
                        bossBullets[i].move(0, 300 * dt);
                        if (bossBullets[i].getPosition().y > 700) bossBulletActive[i] = false;

                        if (bossBullets[i].getGlobalBounds().intersects(player.getGlobalBounds())) {
                            bossBulletActive[i] = false;
                            if (!shieldActive) {
                                lives--;
                                sound.playHit();
                                if (lives <= 0) {
                                    gameOver = true;
                                    sound.stopMusic();
                                    gameOverMenu.show(false, score);
                                    inGame = false;
                                    inGameOver = true;
                                    lives = 3; score = 0;
                                    doubleFireActive = false; shieldActive = false; slowMotionActive = false;
                                    powerUp1Active = false; powerUp2Active = false; powerUp3Active = false;
                                    bossSpawned = false; bossJustDied = false; wingmenActive = false;
                                    for (int j = 0; j < MAX_BULLETS; j++) bulletActive[j] = false;
                                    for (int j = 0; j < MAX_ENEMIES; j++) enemyActive[j] = false;
                                    for (int j = 0; j < MAX_BOSS_BULLETS; j++) bossBulletActive[j] = false;
                                    for (int j = 0; j < MAX_WINGMEN_BULLETS; j++) wingmenBulletActive[j] = false;
                                    sound.resumeMusic();
                                }
                            }
                        }
                    }
                }

                for (int b = 0; b < MAX_BULLETS; b++) {
                    if (bulletActive[b] && boss.isActive() &&
                        bullets[b].getGlobalBounds().intersects(boss.getBounds())) {
                        bulletActive[b] = false;
                        boss.takeDamage();
                        score += 5;
                        sound.playExplosion();

                        if (boss.isDead() && !bossJustDied) {
                            bossJustDied = true;
                            bossDeathEffect.trigger(boss.getBounds().left + boss.getBounds().width / 2,
                                boss.getBounds().top + boss.getBounds().height / 2,
                                sf::Color(255, 100, 255));
                            bossDeathTimer = 3.0f;
                            score += 300;
                        }
                    }
                }

                for (int b = 0; b < MAX_WINGMEN_BULLETS; b++) {
                    if (wingmenBulletActive[b] && boss.isActive() &&
                        wingmenBullets[b].getGlobalBounds().intersects(boss.getBounds())) {
                        wingmenBulletActive[b] = false;
                        boss.takeDamage();
                        score += 5;
                        sound.playExplosion();

                        if (boss.isDead() && !bossJustDied) {
                            bossJustDied = true;
                            bossDeathEffect.trigger(boss.getBounds().left + boss.getBounds().width / 2,
                                boss.getBounds().top + boss.getBounds().height / 2,
                                sf::Color(255, 100, 255));
                            bossDeathTimer = 3.0f;
                            score += 300;
                        }
                    }
                }
            }

            if (bossJustDied) {
                bossDeathTimer -= dt;
                if (bossDeathTimer <= 0) {
                    saveScore(playerName, score);
                    gameOver = true;
                    sound.stopMusic();
                    gameOverMenu.show(true, score);
                    inGame = false;
                    inGameOver = true;
                    lives = 3; score = 0;
                    doubleFireActive = false; shieldActive = false; slowMotionActive = false;
                    powerUp1Active = false; powerUp2Active = false; powerUp3Active = false;
                    bossSpawned = false; bossJustDied = false; wingmenActive = false;
                    for (int j = 0; j < MAX_BULLETS; j++) bulletActive[j] = false;
                    for (int j = 0; j < MAX_ENEMIES; j++) enemyActive[j] = false;
                    for (int j = 0; j < MAX_BOSS_BULLETS; j++) bossBulletActive[j] = false;
                    for (int j = 0; j < MAX_WINGMEN_BULLETS; j++) wingmenBulletActive[j] = false;
                    sound.resumeMusic();
                }
            }

            if (powerUp1Active) {
                powerUp1Y += 80 * dt;
                powerUp1.setPosition(powerUp1.getPosition().x, powerUp1Y);
                if (powerUp1Y > 700) powerUp1Active = false;
                if (powerUp1.getGlobalBounds().intersects(player.getGlobalBounds())) {
                    powerUp1Active = false;
                    doubleFireActive = true;
                    doubleFireTimer = 0;
                    sound.playPowerUp1();
                    powerUpEffect.trigger(powerUp1.getPosition().x, powerUp1.getPosition().y, sf::Color::Cyan);
                    powerUpEffectTimer = 2.0f;
                }
            }
            if (powerUp2Active) {
                powerUp2Y += 80 * dt;
                powerUp2.setPosition(powerUp2.getPosition().x, powerUp2Y);
                if (powerUp2Y > 700) powerUp2Active = false;
                if (powerUp2.getGlobalBounds().intersects(player.getGlobalBounds())) {
                    powerUp2Active = false;
                    shieldActive = true;
                    shieldTimer = 0;
                    blinkTimer = 0;
                    sound.playPowerUp2();
                    powerUpEffect.trigger(powerUp2.getPosition().x, powerUp2.getPosition().y, sf::Color::Green);
                    powerUpEffectTimer = 2.0f;
                }
            }
            if (powerUp3Active) {
                powerUp3Y += 80 * dt;
                powerUp3.setPosition(powerUp3.getPosition().x, powerUp3Y);
                if (powerUp3Y > 700) powerUp3Active = false;
                if (selectedDifficulty == 2 && powerUp3.getGlobalBounds().intersects(player.getGlobalBounds())) {
                    powerUp3Active = false;
                    wingmenActive = true;
                    wingmenTimer = 0;
                    wingmenFireTimer = 0;
                    sound.playPowerUp3();
                    powerUpEffect.trigger(powerUp3.getPosition().x, powerUp3.getPosition().y, sf::Color::Magenta);
                    powerUpEffectTimer = 2.0f;
                }
            }

            if (shieldActive) {
                shieldTimer += dt;
                if (shieldTimer < 3.0f) {
                    blinkTimer += dt;
                    playerVisible = (int(blinkTimer * 10) % 2) == 0;
                }
                else playerVisible = true;
                if (shieldTimer > 13.0f) shieldActive = false;
            }
            else playerVisible = true;

            if (powerUpEffectTimer > 0) {
                powerUpEffectTimer -= dt;
                powerUpEffect.update(dt);
            }
            if (bossDeathTimer > 0) {
                bossDeathEffect.update(dt);
            }

            if (playerVisible) window.draw(player);

            if (wingmenActive) {
                for (int i = 0; i < WINGMEN_COUNT; i++) {
                    window.draw(wingmen[i]);
                }
            }

            for (int i = 0; i < MAX_BULLETS; i++) if (bulletActive[i]) window.draw(bullets[i]);
            for (int i = 0; i < MAX_WINGMEN_BULLETS; i++) if (wingmenBulletActive[i]) window.draw(wingmenBullets[i]);
            for (int i = 0; i < MAX_ENEMIES; i++) if (enemyActive[i]) window.draw(enemies[i]);
            for (int i = 0; i < MAX_BOSS_BULLETS; i++) if (bossBulletActive[i]) window.draw(bossBullets[i]);
            if (powerUp1Active) window.draw(powerUp1);
            if (powerUp2Active) window.draw(powerUp2);
            if (powerUp3Active) window.draw(powerUp3);

            if (!bossJustDied) boss.draw(window);
            powerUpEffect.draw(window);
            bossDeathEffect.draw(window);

            sf::Color uiColor = sf::Color::Yellow;
            if (doubleFireActive) uiColor = sf::Color::Cyan;
            if (shieldActive) uiColor = sf::Color::Green;
            if (wingmenActive) uiColor = sf::Color::Magenta;

            sf::Text scoreText("SCORE: " + std::to_string(score), gameFont, 40);
            scoreText.setFillColor(sf::Color::Yellow);
            scoreText.setPosition(20, 8);

            sf::Text levelText((selectedDifficulty == 0 ? "EASY" :
                selectedDifficulty == 1 ? "MEDIUM" : "HARD"), gameFont, 48);
            levelText.setFillColor(selectedDifficulty == 0 ? sf::Color(100, 255, 100) :
                selectedDifficulty == 1 ? sf::Color(255, 200, 0) : sf::Color(255, 50, 50));
            levelText.setStyle(sf::Text::Bold);
            levelText.setOutlineColor(sf::Color::Black);
            levelText.setOutlineThickness(3);
            levelText.setOrigin(levelText.getLocalBounds().width / 2, 0);
            levelText.setPosition(500, 5);

            sf::Text livesText("LIVES: " + std::to_string(lives), gameFont, 40);
            livesText.setFillColor(sf::Color::Red);
            livesText.setStyle(sf::Text::Bold);
            livesText.setOutlineColor(sf::Color::Black);
            livesText.setOutlineThickness(2);
            livesText.setOrigin(livesText.getLocalBounds().width, 0);
            livesText.setPosition(980, 8);

            window.draw(scoreText);
            window.draw(levelText);
            window.draw(livesText);

            if (doubleFireActive) {
                Text p("DOUBLE FIRE!", gameFont, 40);
                p.setFillColor(sf::Color(100, 255, 255));
                p.setPosition(20, 60);
                window.draw(p);
            }
            if (shieldActive) {
                Text p("SHIELD ACTIVE!", gameFont, 40);
                p.setFillColor(sf::Color(100, 255, 100));
                p.setPosition(20, 110);
                window.draw(p);
            }
            if (wingmenActive) {
                Text p("WINGMEN SQUADRON ACTIVE!", gameFont, 40);
                p.setFillColor(sf::Color(255, 100, 255));
                p.setPosition(20, 160);
                window.draw(p);
            }

            if (bossJustDied && bossDeathTimer > 0) {
                Text victory("BOSS DEFEATED!", gameFont, 80);
                float pulse = sin(bossDeathTimer * 10) * 50 + 200;
                victory.setFillColor(sf::Color(255, pulse, 255));
                victory.setOutlineColor(sf::Color(pulse, 50, 255));
                victory.setOutlineThickness(5);
                victory.setOrigin(victory.getLocalBounds().width / 2, victory.getLocalBounds().height / 2);
                victory.setPosition(500, 300);
                window.draw(victory);
            }

            if (isPaused && !gameOver) {
                static bool pausePlayed = false;
                if (!pausePlayed) {
                    sound.playPauseFx();
                    pausePlayed = true;
                }
                sound.pauseMusic();
                RectangleShape dark(Vector2f(1000, 650));
                dark.setFillColor(Color(0, 0, 0, 180));
                window.draw(dark);
                pauseMenu.draw(window);
            }
            else if (!isPaused && !gameOver) {
                sound.resumeMusic();
                static bool pausePlayed = true;
                pausePlayed = false;
            }
        }

        window.display();
    }
    return 0;
}