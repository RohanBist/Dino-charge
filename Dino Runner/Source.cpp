#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace sf;

const int jumpPixel = 200;

int pixels = 0;
int frame = 0;
int speedgame = 10;
int score = 0;
int highScore = 0;
int scoreCounter = 0;

bool onGround = true;
bool jump = false;
bool game = false;
bool gameover = false;
bool isDay = true;

enum class GameState {
    Menu,
    Playing,
    GameOver
};

std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);
    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%F %T");
    return ss.str();
}

int main()
{
    RenderWindow window(VideoMode(800, 400), "DINO");
    window.setFramerateLimit(60);

    // Load font
    Font font;
    font.loadFromFile("Fonts/Font.ttf");

    // Load textures
    Texture DinoT, backg, texture, End, Cactus, Restart, CloudTexture;
    DinoT.loadFromFile("resources/dinosaur.png");
    backg.loadFromFile("resources/menu.png");
    texture.loadFromFile("resources/DINO.png");
    End.loadFromFile("resources/gameover.png");
    Cactus.loadFromFile("resources/cactus.png");
    Restart.loadFromFile("resources/restart.png");
    CloudTexture.loadFromFile("resources/clouds.png");

    // Load sound buffers
    SoundBuffer jumpBuffer, dieBuffer, pointBuffer;
    if (!jumpBuffer.loadFromFile("audio/jump.wav") ||
        !dieBuffer.loadFromFile("audio/die.wav") ||
        !pointBuffer.loadFromFile("audio/point.wav")) {
        // Handle error loading sound files
        return EXIT_FAILURE;
    }

    // Create sound objects
    Sound jumpSound(jumpBuffer), dieSound(dieBuffer), pointSound(pointBuffer);

    // Create sprites and other game objects...
    Sprite dino(DinoT, { 0,0,44,47 });
    Sprite cactus(Cactus);
    Sprite end(End);
    Sprite restart(Restart);
    Sprite cloud(CloudTexture);
    Sprite Background(backg);

    dino.setScale(2, 2);
    dino.setPosition(50, 230);
    cactus.setScale(1.5, 1.5);
    cactus.setPosition(window.getSize().x, 250);
    end.setPosition(215, 100);
    end.setScale(2, 2);
    restart.setPosition(346, 170);
    restart.setScale(2, 2);
    cloud.setPosition(800, 50);
    cloud.setScale(1.5, 1.5);
    Background.setScale(1.5, 1.5);
    Background.setPosition(0, 0);

    Clock cloudSpawnTimer;
    Clock timeCycleTimer;

    Color dayColor(225, 225, 225); // Light
    Color nightColor(0, 0, 0); // Dark 
    Color currentColor = dayColor;

    RectangleShape ground(Vector2f(window.getSize().x, 50));
    ground.setFillColor(Color(100, 100, 100));
    ground.setPosition(0, window.getSize().y - 50);

    GameState state = GameState::Menu;

    std::ifstream highScoreFile("highscore.txt");
    if (highScoreFile.is_open()) {
        highScoreFile >> highScore;
        highScoreFile.close();
    }

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Space)
                {
                    if (state == GameState::Menu)
                    {
                        state = GameState::Playing;
                    }
                    else if (!game && state == GameState::Playing)
                    {
                        game = true;
                        dino.setPosition(50, 230);
                        cactus.setPosition(window.getSize().x, 250);
                        dino.setTextureRect({ 0,0,44,47 });
                        pixels = 0;
                        frame = 0;
                        speedgame = 10;
                        jump = false;
                        onGround = true;
                        score = 0;
                    }
                    else if (onGround && state == GameState::Playing)
                    {
                        jump = true;
                        onGround = false;
                        pixels = 0;
                        dino.setTextureRect({ 88,0,44,47 });
                        jumpSound.play(); // Play jump sound
                    }
                }
            }
        }

        if (state == GameState::Menu)
        {
            window.clear(Color::Green);
            window.draw(Background);

            // Draw "Press SPACE to start" text
            Text text("Press SPACE to start", font, 20);
            text.setFillColor(Color::Blue);
            FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width / 8.0f, textRect.top + textRect.height / 2.0f);
            text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
            window.draw(text);

            // Draw "Developed BY HWIC BROTHERS" text
            Text texti("Developed BY HWIC BROTHERS", font, 10);
            texti.setFillColor(Color::Black); // Corrected the object to modify
            FloatRect textiRect = texti.getLocalBounds();
            texti.setOrigin(textiRect.left + textiRect.width / 4.0f, textiRect.top + textiRect.height / 2.0f);
            texti.setPosition(window.getSize().x / 1.7f, window.getSize().y / 1.7f);
            window.draw(texti);

            // Draw digital clock
            std::string dateTime = getCurrentDateTime();
            Text clockText(dateTime, font, 15);
            clockText.setFillColor(Color::Black);
            clockText.setPosition(window.getSize().x - 10 - clockText.getLocalBounds().width, 10);
            window.draw(clockText);

            window.display();
            continue;
        }


        if (!game)
        {
            if (score > highScore) {
                highScore = score;
                std::ofstream highScoreFile("highscore.txt");
                if (highScoreFile.is_open()) {
                    highScoreFile << highScore;
                    highScoreFile.close();
                }
            }
            window.clear(Color::White);
            RectangleShape background(Vector2f(window.getSize().x, window.getSize().y));
            background.setFillColor(currentColor);
            window.draw(background);
            window.draw(dino);
            if (gameover)
            {
                window.draw(cactus);
                window.draw(end);
                window.draw(restart);
            }
            Text scoreText("Score: " + std::to_string(score), font, 20);
            scoreText.setPosition(10, 10);
            window.draw(scoreText);
            Text highScoreText("High Score: " + std::to_string(highScore), font, 20);
            highScoreText.setPosition(10, 40);
            window.draw(highScoreText);
            window.draw(ground); // Draw ground
            window.display();
            continue;
        }

        if (jump) // jumping
        {
            if (jumpPixel > pixels)
            {
                dino.move(0, (-jumpPixel / float(25)));
                pixels += jumpPixel / float(25);
            }
            else
            {
                jump = false;
                pixels = 0;
            }
        }
        else if (!onGround) // falling
        {
            if (jumpPixel > pixels)
            {
                dino.move(0, (jumpPixel / float(25)));
                pixels += jumpPixel / float(25);
            }
            else
            {
                onGround = true;
            }
        }
        else // running animation
        {
            if (frame <= 5)
            {
                dino.setTextureRect({ 88,0,44,47 });
                frame++;
            }
            else if (frame <= 10)
            {
                dino.setTextureRect({ 132,0,44,47 });
                frame++;
            }
            else frame = 0;
        }

        if (cactus.getPosition().x <= -25) // creating a new cactus
        {
            srand(time(NULL));
            cactus.setPosition(window.getSize().x, 250);
            int random = rand() % 5;
            switch (random)
            {
            case 0: cactus.setTextureRect({ 0,0,25,53 }); break;
            case 1: cactus.setTextureRect({ 25,0,25,53 }); break;
            case 2: cactus.setTextureRect({ 50,0,25,53 }); break;
            case 3: cactus.setTextureRect({ 75,0,25,53 }); break;
            case 4: cactus.setTextureRect({ 100,0,28,53 }); break;
            }
        }

        cactus.move(-speedgame, 0);
        speedgame += 0.001;

        // Spawn a new cloud if enough time has passed
        if (cloudSpawnTimer.getElapsedTime().asSeconds() > 3) // Change 3 to desired interval
        {
            Sprite newCloud(CloudTexture);
            newCloud.setScale(1.5, 1.5);
            newCloud.setPosition(800, rand() % 200); // Randomize cloud's y position
            cloud = newCloud;
            cloudSpawnTimer.restart();
        }

        // Move cloud
        cloud.move(-speedgame, 0);

        // Cycle between day and night after some time
        if (timeCycleTimer.getElapsedTime().asSeconds() > 10) // Change 10 to desired interval
        {
            if (isDay)
            {
                currentColor = nightColor;
                isDay = false;
            }
            else
            {
                currentColor = dayColor;
                isDay = true;
            }
            timeCycleTimer.restart();
        }

        // Collision detection with cactus
        if (dino.getGlobalBounds().intersects(cactus.getGlobalBounds()))
        {
            gameover = true;
            game = false;
            dieSound.play(); // Play die sound
        }

        // Increase score every 10 frames
        if (scoreCounter >= 10) {
            score++;
            scoreCounter = 0; // Reset counter
            if (score % 100 == 0) {
                pointSound.play(); // Play point sound
            }
        }
        else {
            scoreCounter++;
        }

        // Draw
        window.clear(Color::White);
        RectangleShape background(Vector2f(window.getSize().x, window.getSize().y));
        background.setFillColor(currentColor);
        window.draw(background);
        window.draw(dino);
        window.draw(cactus);
        window.draw(cloud);
        Text scoreText("Score: " + std::to_string(score), font, 10);
        scoreText.setPosition(10, 10);
        window.draw(scoreText);
        Text highScoreText("High Score: " + std::to_string(highScore), font, 10);
        highScoreText.setPosition(10, 40);
        window.draw(highScoreText);
        window.draw(ground); // Draw ground
        window.display();
    }

    return 0;
}
