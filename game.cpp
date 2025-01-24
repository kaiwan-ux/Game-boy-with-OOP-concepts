#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <cctype>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;

// Constants
const int BLOCK_SIZE = 20;
const int WIDTH = 800 / BLOCK_SIZE;
const int HEIGHT = 600 / BLOCK_SIZE;
const int MAX_SNAKE_LENGTH = WIDTH * HEIGHT;
const int MAX_HIGH_SCORES = 10;

struct SnakeSegment {
    int x, y;
};

class Food;
class Score;

class Snake {
private:
    SnakeSegment snake[MAX_SNAKE_LENGTH];
    int length;
    int direction; // 0 = Up, 1 = Right, 2 = Down, 3 = Left

public:
    Snake() : length(1), direction(1) {
        snake[0] = { WIDTH / 2, HEIGHT / 2 };
    }

    void reset() {
        length = 1;
        direction = 1;
        snake[0] = { WIDTH / 2, HEIGHT / 2 };
    }

    void setDirection(int dir) {
        if ((dir == 0 && direction != 2) ||
            (dir == 1 && direction != 3) ||
            (dir == 2 && direction != 0) ||
            (dir == 3 && direction != 1)) {
            direction = dir;
        }
    }

    void move() {
        SnakeSegment newHead = snake[0];
        if (direction == 0) newHead.y--;
        else if (direction == 1) newHead.x++;
        else if (direction == 2) newHead.y++;
        else if (direction == 3) newHead.x--;

        for (int i = length; i > 0; --i) {
            snake[i] = snake[i - 1];
        }
        snake[0] = newHead;
    }

    bool hasCollision() {
        if (snake[0].x < 0 || snake[0].x >= WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT) {
            return false;
        }

        for (int i = 1; i < length; ++i) {
            if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
                return true;
            }
        }
        return false;
    }

    void grow() { length++; }

    SnakeSegment getHead() { return snake[0]; }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
        block.setFillColor(sf::Color::Green);
        for (int i = 0; i < length; ++i) {
            block.setPosition(200,150);
            window.draw(block);
        }
    }
};

class Food {
private:
    SnakeSegment position;

public:
    Food() { respawn(); }

    void respawn() {
        position.x = rand() % WIDTH;
        position.y = rand() % HEIGHT;
    }

    SnakeSegment getPosition() { return position; }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
        block.setFillColor(sf::Color::Red);
        block.setPosition(position.x * BLOCK_SIZE, position.y * BLOCK_SIZE);
        window.draw(block);
    }
};

class Score {
private:
    int score;
    int highScores[MAX_HIGH_SCORES];
    int highScoreCount;

public:
    Score() : score(0), highScoreCount(0) {
        loadHighScores();
    }

    void reset() { score = 0; }

    void addScore(int points) { score += points; }

    int getScore() { return score; }

    int* getHighScores() { return highScores; } // Getter for highScores array

    int getHighScoreCount() { return highScoreCount; } // Getter for highScoreCount

    void saveHighScore() {
        if (highScoreCount < MAX_HIGH_SCORES) {
            highScores[highScoreCount++] = score;
        }
        else if (score > highScores[highScoreCount - 1]) {
            highScores[highScoreCount - 1] = score;
        }

        for (int i = 0; i < highScoreCount - 1; ++i) {
            for (int j = i + 1; j < highScoreCount; ++j) {
                if (highScores[j] > highScores[i]) {
                    int temp = highScores[i];
                    highScores[i] = highScores[j];
                    highScores[j] = temp;
                }
            }
        }

        ofstream outFile("highscores.txt");
        for (int i = 0; i < highScoreCount; ++i) {
            outFile << highScores[i] << endl;
        }
        outFile.close();
    }

    void loadHighScores() {
        ifstream inFile("highscores.txt");
        if (inFile.is_open()) {
            while (inFile >> highScores[highScoreCount]) {
                ++highScoreCount;
                if (highScoreCount >= MAX_HIGH_SCORES) break;
            }
            inFile.close();
        }
    }

    void draw(sf::RenderWindow& window, sf::Font& font) {
        sf::Text scoreText("Score: " + to_string(score), font, 20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);
        window.draw(scoreText);
    }
};

class DisplayScreen {
private:
    sf::RenderWindow& window;
    sf::Font font;

public:
    DisplayScreen(sf::RenderWindow& win, sf::Font& f) : window(win), font(f) {}

    void showStartScreen() {
        window.clear(sf::Color::Black);

        if (!font.loadFromFile("Roboto-Light.ttf")) {
            cout << "Font loading failed! Ensure 'Roboto-Light.ttf' is in the correct directory." << endl;
            exit(1); // Exit if font loading fails
        }

        // Draw title
        sf::Text title("Snake Game\n", font, 55);
        title.setFillColor(sf::Color::White);
        title.setPosition(window.getSize().x / 2 - 150, 100); // Centered horizontally
        window.draw(title);

        // Draw menu options
        sf::Text startText("1. Start Game\n", font, 40);
        startText.setFillColor(sf::Color(0, 100, 0));
        startText.setPosition(window.getSize().x / 2 - 120, 200);
        window.draw(startText);

        sf::Text highScoreText("2. High Scores\n", font, 40);
        highScoreText.setFillColor(sf::Color::Yellow);
        highScoreText.setPosition(window.getSize().x / 2 - 120, 250);
        window.draw(highScoreText);

        sf::Text quitText("3. Quit\n", font, 40);
        quitText.setFillColor(sf::Color::Red);
        quitText.setPosition(window.getSize().x / 2 - 120, 300);
        window.draw(quitText);

        window.display();
    }


    void showHighScores(int highScores[], int count) {
        window.clear();
        sf::Text title("High Scores", font, 50);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(WIDTH * BLOCK_SIZE / 2 - 150, 50);
        window.draw(title);

        int yOffset = 150;
        for (int i = 0; i < count; ++i) {
            sf::Text scoreText("Score: " + to_string(highScores[i]), font, 20);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(WIDTH * BLOCK_SIZE / 2 - 100, yOffset);
            window.draw(scoreText);
            yOffset += 30;
        }

        sf::Text backText("Press B to go back", font, 20);
        backText.setFillColor(sf::Color::Green);
        backText.setPosition(WIDTH * BLOCK_SIZE / 2 - 100, yOffset + 30);
        window.draw(backText);

        window.display();
    }
};

class SnakeGame : public MainMenu {
private:
    sf::RenderWindow window;
    sf::Font font;
    DisplayScreen displayScreen;
    Snake snake;
    Food food;
    Score score;
    sf::Clock clock;
    sf::Clock levelUpClock; // Track time for level-ups
    sf::Clock messageClock; // Track time for "Level Up!" message
    sf::Time lastUpdate;
    sf::Texture backgroundTexture; // Texture for the background image
    sf::Sprite backgroundSprite;   // Sprite for the background image
    float speed;
    bool gameOver;
    bool showLevelUpMessage; // Flag for showing the level-up message
    const sf::Time messageDisplayTime = sf::seconds(2); // Duration to show the "Level Up!" message
    enum GameState { StartScreen, Playing, HighScores } state;

public:
    SnakeGame(sf::RenderWindow& win)
        : MainMenu(win),
        displayScreen(win, font),
        speed(0.1f),
        gameOver(false),
        state(StartScreen),
        showLevelUpMessage(false)
    {
        if (!font.loadFromFile("Roboto-Light.ttf")) {
            cerr << "Failed to load font!" << endl;
            exit(1);
        }

        if (!backgroundTexture.loadFromFile("background.png")) {
            cerr << "Failed to load background texture!" << endl;
            exit(1);
        }
        backgroundSprite.setTexture(backgroundTexture);
    }

    void resetGame() {
        snake.reset();
        food.respawn();
        score.reset();
        gameOver = false;
        speed = 0.1f;
        levelUpClock.restart(); // Restart level-up clock
        messageClock.restart(); // Restart message clock
        showLevelUpMessage = false; // Reset the level-up message
    }

    void run() override {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            if (state == StartScreen) {
                displayScreen.showStartScreen();

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
                    resetGame();
                    state = Playing;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
                    state = HighScores;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
                    window.close();
                }
            }
            else if (state == HighScores) {
                displayScreen.showHighScores(score.getHighScores(), score.getHighScoreCount());
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
                    state = StartScreen;
                }
            }
            else if (state == Playing) {
                if (!gameOver) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) snake.setDirection(0);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) snake.setDirection(1);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) snake.setDirection(2);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) snake.setDirection(3);

                    sf::Time elapsed = clock.getElapsedTime() - lastUpdate;
                    if (elapsed.asSeconds() > speed) {
                        snake.move();
                        if (snake.hasCollision()) {
                            gameOver = true;
                            score.saveHighScore();
                            state = StartScreen;
                        }

                        if (snake.getHead().x == food.getPosition().x &&
                            snake.getHead().y == food.getPosition().y) {
                            snake.grow();
                            food.respawn();
                            score.addScore(5);
                        }
                        lastUpdate = clock.getElapsedTime();
                    }

                    // Level-Up Speed Logic
                    if (score >=15) {
                        speed = max(speed - 0.01f, 0.05f); // Increase difficulty
                        levelUpClock.restart(); // Reset the level-up clock
                        showLevelUpMessage = true; // Enable message display
                        messageClock.restart(); // Restart message timer
                    }

                    // Turn off message after 2 seconds
                    if (showLevelUpMessage && messageClock.getElapsedTime() >= messageDisplayTime) {
                        showLevelUpMessage = false;
                    }
                }

                window.clear();
                snake.draw(window);
                food.draw(window);
                score.draw(window, font);

                // Draw the level-up message if active
                if (showLevelUpMessage) {
                    sf::Text levelUpText("Level Up!", font, 30);
                    levelUpText.setFillColor(sf::Color::Yellow);
                    levelUpText.setPosition(WIDTH * BLOCK_SIZE / 2 - 80, 10); // Centered at the top
                    window.draw(levelUpText);
                }

                window.display();
            }
        }
    }
    void updateGame() {
        window.clear();
        snake.draw(window);
        food.draw(window);
        score.draw(window, font);
    }
};
////////////////////////////////////////////////CODE FOR WORDLE GAME////////////////////////////////////////////////



class Letter {
private:
    sf::RectangleShape box;
    sf::Text text;
    char value;

public:
    Letter(char val, sf::Font& font, float x, float y) : value(val) {
        box.setSize(sf::Vector2f(50, 50));
        box.setFillColor(sf::Color(200, 200, 200));
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(2);
        box.setPosition(x, y);

        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);
        text.setString(string(1, value));
        text.setPosition(x + 15, y + 10);
    }

    void setColor(const sf::Color& color) { box.setFillColor(color); }

    char getValue() const { return value; }

    void setValue(char newValue) {
        value = newValue;
        text.setString(string(1, value));
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(box);
        window.draw(text);
    }
};

// WordDictionary class
class WordDictionary {
private:
    vector<string> words;

public:
    WordDictionary() {
        // Populate the dictionary with 5-letter words
        words = { "ABOUT", "ALERT", "ARGUE", "BEACH", "ABOVE", "ALIKE", "ARISE", "BEGAN",
                  "ABUSE", "ALIVE", "ARRAY", "BEGIN", "ACTOR", "ALLOW", "ASIDE", "BEGUN",
                  "ACUTE", "ALONE", "ASSET", "BEING", "ADMIT", "ALONG", "AUDIO", "BELOW",
                  "ADOPT", "ALTER", "AUDIT", "BENCH", "ADULT", "AMONG", "AVOID", "BILLY",
                  "AFTER", "ANGER", "AWARD", "BIRTH", "AGAIN", "ANGLE", "AWARE", "BLACK",
                  "AGENT", "ANGRY", "BADLY", "BLAME", "AGREE", "APART", "BAKER", "BLIND",
                  "AHEAD", "APPLE", "BASES", "BLOCK", "ALARM", "APPLY", "BASIC", "BLOOD",
                  "ALBUM", "ARENA", "BASIS", "BOARD", "BOOST", "BUYER", "CHINA", "COVER",
                  "BOOTH", "CABLE", "CHOSE", "CRAFT", "BOUND", "CALIF", "CIVIL", "CRASH",
                  "BRAIN", "CARRY", "CLAIM", "CREAM", "BRAND", "CATCH", "CLASS", "CRIME",
                  "BREAD", "CAUSE", "CLEAN", "CROSS", "BREAK", "CHAIN", "CLEAR", "CROWD",
                  "BREED", "CHAIR", "CLICK", "CROWN", "BRIEF", "CHART", "CLOCK", "CURVE",
                  "BRING", "CHASE", "CLOSE", "CYCLE", "BROAD", "CHEAP", "COACH", "DAILY",
                  "BROKE", "CHECK", "COAST", "DANCE", "BROWN", "CHEST", "COULD", "DATED",
                  "BUILD", "CHIEF", "COUNT", "DEALT", "BUILT", "CHILD", "COURT", "DEATH",
                  "DEBUT", "ENTRY", "FORTH", "GROUP", "DELAY", "EQUAL", "FORTY", "GROWN",
                  "DEPTH", "ERROR", "FORUM", "GUARD", "DOING", "EVENT", "FOUND", "GUESS",
                  "DOUBT", "EVERY", "FRAME", "GUEST", "DOZEN", "EXACT", "FRANK", "GUIDE",
                  "DRAFT", "EXIST", "FRAUD", "HAPPY", "DRAMA", "EXTRA", "FRESH", "HARRY",
                  "DRAWN", "FAITH", "FRONT", "HEART", "DREAM", "FALSE", "FRUIT", "HEAVY",
                  "DRESS", "FAULT", "FULLY", "HENCE", "DRILL", "FIBRE", "FUNNY", "NIGHT",
                  "DRINK", "FIELD", "GIANT", "HORSE", "DRIVE", "FIFTH", "GIVEN", "HOTEL", "DROVE", "FIFTY", "GLASS", "HOUSE",
                  "DYING", "FIGHT", "GLOBE", "HUMAN", "EAGER", "FINAL", "GOING", "IDEAL",
                  "EARLY", "FIRST", "GRACE", "IMAGE", "EARTH", "FIXED", "GRADE", "INDEX",
                  "EIGHT", "FLASH", "GRAND", "INNER", "ELITE", "FLEET", "GRANT", "INPUT",
                  "EMPTY", "FLOOR", "GRASS", "ISSUE", "ENEMY", "FLUID", "GREEN", "JUICE",
                  "ENJOY", "FOCUS", "GROSS", "JOINT", "ENTER", "FORCE", "GROSS", "JOINT",
                  "JUDGE", "METAL", "MEDIA", "NEWLY", "KNOWN", "LOCAL", "MIGHT", "NOISE",
                  "LABEL", "LOGIC", "MINOR", "NORTH", "LARGE", "LOOSE", "MINUS", "NOTED",
                  "LASER", "LOWER", "MIXED", "NOVEL", "LATER", "LUCKY", "MODEL", "NURSE",
                  "LAUGH", "LUNCH", "MONEY", "OCCUR", "LAYER", "LYING", "MONTH", "OCEAN",
                  "LEARN", "MAGIC", "MORAL", "OFFER", "LEASE", "MAJOR", "MOTOR", "OFTEN",
                  "LEAST", "MAKER", "MOUNT", "ORDER", "LEAVE", "MARCH", "MOUSE", "OTHER",
                  "LEGAL", "MUSIC", "MOUTH", "OUGHT", "LEVEL", "MATCH", "MOVIE", "PAINT",
                  "LIGHT", "MAYOR", "NEEDS", "PAPER", "LIMIT", "MEANT", "NEVER", "PARTY",
                  "PEACE", "POWER", "RADIO", "ROUND", "PANEL", "PRESS", "RAISE", "ROUTE",
                  "PHASE", "PRICE", "RANGE", "ROYAL", "PHONE", "PRIDE", "RAPID", "RURAL",
                  "PHOTO", "PRIME", "RATIO", "SCALE", "PIECE", "PRINT", "REACH", "SCENE",
                  "PILOT", "PRIOR", "READY", "SCOPE", "PITCH", "PRIZE", "REFER", "SCORE",
                  "PLACE", "PROOF", "RIGHT", "SENSE", "PLAIN", "PROUD", "RIVAL", "SERVE",
                  "PLANE", "PROVE", "RIVER", "SEVEN", "PLANT", "QUEEN", "QUICK", "SHALL",
                  "PLATE", "SIXTH", "STAND", "SHAPE", "POINT", "QUIET", "ROMAN", "SHARE",
                  "POUND", "QUITE", "ROUGH", "SHARP", "SHEET", "SPARE", "STYLE", "TIMES",
                  "SHELF", "SPEAK", "SUGAR", "TIRED", "SHELL", "SPEED", "SUITE", "TITLE",
                  "SHIFT", "SPEND", "SUPER", "TODAY", "SHIRT", "SPENT", "SWEET", "TOPIC",
                  "SHOCK", "SPLIT", "TABLE", "TOTAL", "SHOOT", "SPOKE", "TAKEN", "TOUCH",
                  "SHORT", "SPORT", "TASTE", "TOUGH", "SHOWN", "STAFF", "TAXES", "TOWER",
                  "SIGHT", "STAGE", "TEACH", "TRACK", "SINCE", "STAKE", "TEETH", "TRADE",
                  "SIXTY", "START", "TEXAS", "TREAT", "SIZED", "STATE", "THANK", "TREND",
                  "SKILL", "STEAM", "THEFT", "TRIAL", "SLEEP", "STEEL", "THEIR", "TRIED",
                  "SLIDE", "STICK", "THEME", "TRIES", "SMALL", "STILL", "THERE", "TRUCK",
                  "SMART", "STOCK", "THESE", "TRULY", "SMILE", "STONE", "THICK", "TRUST",
                  "SMITH", "STOOD", "THING", "TRUTH", "SMOKE", "STORE", "THINK", "TWICE", "SOLID", "STORM", "THIRD", "UNDER",
                  "SOLVE", "STORY", "THOSE", "UNDUE", "SORRY", "STRIP", "THREE", "UNION", "SOUND", "STUCK", "THREW", "UNITY",
                  "SOUTH", "STUDY", "THROW", "UNTIL", "SPACE", "STUFF", "TIGHT", "UPPER", "UPSET", "WHOLE", "WASTE", "WOUND",
                  "URBAN", "WHOSE", "WATCH", "WRITE", "USAGE", "WOMAN", "WATER", "WRONG", "USUAL", "TRAIN", "WHEEL", "WROTE",
                  "VALID", "WORLD", "WHERE", "YIELD", "VALUE", "WORRY", "WHICH", "YOUNG", "VIDEO", "WORSE", "WHILE", "YOUTH",
                  "VIRUS", "WORST", "WHITE", "WORTH", "VISIT", "WOULD", "VITAL", "VOICE" };
    }
    string getRandomWord() {
        srand(static_cast<unsigned int>(time(nullptr)));
        string word;
        word = words[rand() % words.size()];
        cout << word;
        return word;
    }
};

// Game grid class
class Grid {
private:
    vector<vector<Letter>> grid;
    sf::Font font;
    int currentRow;
    int currentCol;

public:
    Grid(const string& fontPath) : currentRow(0), currentCol(0) {
        if (!font.loadFromFile(fontPath)) {
            cerr << "Error: Could not load font!" << endl;
            exit(1);
        }

        // Center the grid
        float gridX = (800 - 5 * 60) / 2;
        float gridY = (500 - 6 * 60) / 2;

        for (int i = 0; i < 6; ++i) {
            vector<Letter> row;
            for (int j = 0; j < 5; ++j) {
                row.emplace_back(' ', font, gridX + j * 60, gridY + i * 60);
            }
            grid.push_back(row);
        }
    }

    int getCurrentCol() const { return currentCol; }

    vector<Letter>& getCurrentRow() { return grid[currentRow]; }

    void addLetter(char letter) {
        if (currentRow < 6 && currentCol < 5) {
            grid[currentRow][currentCol].setValue(toupper(letter));
            currentCol++;
        }
    }

    void removeLetter() {
        if (currentCol > 0) {
            currentCol--;
            grid[currentRow][currentCol].setValue(' ');
        }
    }

    void submitGuess(const string& correctWord) {
        if (currentCol == 5) {
            string guess;
            for (int i = 0; i < 5; ++i) {
                guess += grid[currentRow][i].getValue();
            }

            for (int i = 0; i < 5; ++i) {
                if (guess[i] == correctWord[i]) {
                    grid[currentRow][i].setColor(sf::Color::Green);
                }
                else if (correctWord.find(guess[i]) != string::npos) {
                    grid[currentRow][i].setColor(sf::Color::Yellow);
                }
                else {
                    grid[currentRow][i].setColor(sf::Color::Red);
                }
            }
            currentRow++;
            currentCol = 0;
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& row : grid) {
            for (auto& letter : row) {
                letter.draw(window);
            }
        }
    }

    bool isGameOver(const string& correctWord) {
        if (currentRow == 6) return true;
        string lastGuess;
        for (int i = 0; i < 5; ++i) {
            lastGuess += grid[currentRow - 1][i].getValue();
        }
        return lastGuess == correctWord;
    }

    bool isMaxTriesReached() { return currentRow == 6; }

    void resetGrid() {
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 5; ++j) {
                grid[i][j].setValue(' ');
                grid[i][j].setColor(sf::Color(200, 200, 200));
            }
        }
        currentRow = 0;
        currentCol = 0;
    }
};

// Keyboard class
class Keyboard {
private:
    vector<Letter> keys;
    sf::Font font;

public:
    Keyboard(const string& fontPath) {
        if (!font.loadFromFile(fontPath)) {
            cerr << "Error: Could not load font!" << endl;
            exit(1);
        }

        vector<string> rows = { "QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM" };
        int x = (800 - 10 * 55) / 2; // Center the keyboard
        int y = 500;

        for (const auto& row : rows) {
            for (char letter : row) {
                keys.emplace_back(letter, font, x, y);
                x += 55;
            }
            x = (800 - 10 * 55) / 2; // Reset x for the next row
            y += 60;
        }
    }

    void draw(sf::RenderWindow& window) const {
        for (const auto& key : keys) {
            key.draw(window);
        }
    }
};

// Main WordleGame class
class WordleGame : public MainMenu {
private:
    sf::RenderWindow& window;
    Grid grid;
    Keyboard keyboard;
    WordDictionary wordDictionary; // Include WordDictionary
    string correctWord;
    bool gameOver;

    sf::RectangleShape yesButton;
    sf::RectangleShape noButton;
    sf::Text yesText;
    sf::Text noText;
    sf::Font font;

public:
    WordleGame(sf::RenderWindow& win)
        : MainMenu(win),              // Pass window to MainMenu
        window(win),
        grid("Roboto-Light.ttf"),
        keyboard("Roboto-Light.ttf"),
        wordDictionary(),           // Initialize WordDictionary
        gameOver(false)
    {
        correctWord = wordDictionary.getRandomWord();  // Use WordDictionary to get a random word

        if (!font.loadFromFile("Roboto-Light.ttf")) {
            cerr << "Error: Could not load font!" << endl;
            exit(1);
        }

        yesButton.setSize(sf::Vector2f(100, 50));
        yesButton.setFillColor(sf::Color::Green);
        yesButton.setPosition(150, 500);
        yesText.setFont(font);
        yesText.setString("Yes");
        yesText.setCharacterSize(20);
        yesText.setFillColor(sf::Color::White);
        yesText.setPosition(175, 515);

        noButton.setSize(sf::Vector2f(100, 50));
        noButton.setFillColor(sf::Color::Red);
        noButton.setPosition(350, 500);
        noText.setFont(font);
        noText.setString("No");
        noText.setCharacterSize(20);
        noText.setFillColor(sf::Color::White);
        noText.setPosition(375, 515);
    }

    void run() override {
        while (window.isOpen()) {
            processEvents();
            render();
        }
    }

private:
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (!gameOver && event.type == sf::Event::TextEntered) {
                if (std::isalpha(event.text.unicode) && grid.getCurrentCol() < 5) {
                    grid.addLetter(static_cast<char>(event.text.unicode));
                }
                else if (event.text.unicode == '\b') {
                    grid.removeLetter();
                }
                else if (event.text.unicode == '\r') {
                    // Only allow submission if 5 letters have been entered
                    if (grid.getCurrentCol() == 5) {
                        grid.submitGuess(correctWord);
                        if (grid.isMaxTriesReached()) {
                            gameOver = true;
                            displayMessage("You Lost! The word was " + correctWord);
                        }
                        else if (grid.isGameOver(correctWord)) {
                            gameOver = true;
                            displayMessage("Congratulations, you win!");
                        }
                    }
                }
            }
            else if (gameOver && event.type == sf::Event::MouseButtonPressed) {
                handleRestart(event.mouseButton.x, event.mouseButton.y);
            }
        }
    }

    void render() {
        window.clear(sf::Color(50, 50, 50)); // Set a dark gray background
        if (!gameOver) {
            grid.draw(window);
            keyboard.draw(window);
        }
        else {
            drawRestartScreen();
        }
        window.display();
    }

    void drawRestartScreen() {
        sf::Text message("Do you want to play again?", font, 30);
        message.setFillColor(sf::Color::Black);
        message.setPosition(100, 400);

        window.draw(message);
        window.draw(yesButton);
        window.draw(noButton);
        window.draw(yesText);
        window.draw(noText);
    }

    void displayMessage(const string& message) {
        sf::Text text(message, font, 30);
        text.setFillColor(sf::Color::Black);
        text.setPosition(150, 300);

        window.clear(sf::Color::White);
        window.draw(text);
        window.display();
        sf::sleep(sf::seconds(2));
    }

    void handleRestart(int mouseX, int mouseY) {
        if (yesButton.getGlobalBounds().contains(mouseX, mouseY)) {
            grid.resetGrid();
            correctWord = wordDictionary.getRandomWord();
            gameOver = false;
        }
        else if (noButton.getGlobalBounds().contains(mouseX, mouseY)) {
            window.close();
        }
    }
};


////////////////////////////////////////////////CODE FOR HANGMAN GAME////////////////////////////////////////////////




class User {
private:
    string userName;
    int winsCount;
    int lossesCount;

public:
    User(string name = "player") : userName(name), winsCount(0), lossesCount(0) {}

    string getUserName() const {
        return userName;
    }

    void increaseWins() {
        winsCount++;
    }

    void increaseLosses() {
        lossesCount++;
    }

    int getWinsCount() const {
        return winsCount;
    }

    int getLossesCount() const {
        return lossesCount;
    }

    string getSummary() const {
        return "Player: " + userName + "\nWins: " + to_string(winsCount) + "\nLosses: " + to_string(lossesCount);
    }

    void saveUserProfile() const {
        ofstream out("user_profile.txt");
        if (out.is_open()) {
            out << userName << endl;
            out << winsCount << endl;
            out << lossesCount << endl;
            out.close();
        }
    }

    bool loadUserProfile() {
        ifstream in("user_profile.txt");
        if (in.is_open()) {
            getline(in, userName);
            in >> winsCount;
            in >> lossesCount;
            in.close();
            return true;
        }
        return false;
    }
};

class Hangman {
private:
    string targetWord;
    string currentGuess;
    int remainingTries;
    string clue;
    string attemptedLetters;

    const string wordList[5] = { "NEBULA", "QUANTUM", "CATASTROPHIC", "CRYPTID", "PHANTASMAGORIA" };
    const string clues[5] = {
        "A massive cloud in space, where stars are born",
        "A scientific theory involving particles at subatomic scales",
        "An event causing major destruction, often irreversible",
        "A mythical creature whose existence is debated",
        "A surreal experience with strange or hallucinatory images"
    };

public:
    Hangman() {
        srand(time(0));
        startNewGame();
    }

    void startNewGame() {
        int index = rand() % 5;
        targetWord = wordList[index];
        clue = clues[index];
        currentGuess = string(targetWord.length(), '_');
        remainingTries = 0;
        attemptedLetters.clear();
    }

    string getTargetWord() const {
        return targetWord;
    }

    string getCurrentGuess() const {
        return currentGuess;
    }

    string getClue() const {
        return clue;
    }

    int getRemainingTries() const {
        return remainingTries;
    }

    void processInput(char letter, sf::Text& feedbackText) {
        letter = toupper(letter);
        if (attemptedLetters.find(letter) != string::npos) {
            feedbackText.setString("Letter '" + string(1, letter) + "' already guessed!");
            return;
        }

        attemptedLetters += letter;

        bool correctGuess = false;
        for (size_t i = 0; i < targetWord.length(); i++) {
            if (letter == targetWord[i] && currentGuess[i] == '_') {
                currentGuess[i] = targetWord[i];
                correctGuess = true;
            }
        }

        if (!correctGuess) {
            remainingTries++;
        }
    }

    bool isWon() const {
        return currentGuess == targetWord;
    }

    bool isLost() const {
        return remainingTries >= 6;
    }
};

class GameManager : public MainMenu {
private:
    sf::RenderWindow& window;
    User currentUser;
    Hangman hangman;
    bool nameInputActive;
    bool gameEnd;
    string enteredName;

    // SFML Objects
    sf::Font gameFont;
    sf::Text titleText, hintText, guessedText, statusMessage, userProfile, namePrompt, nameDisplay;
    sf::CircleShape head;
    sf::RectangleShape body, leftArm, rightArm, leftLeg, rightLeg;

public:
    // Corrected Constructor
    GameManager(sf::RenderWindow& win)
        : MainMenu(win), window(win), nameInputActive(true), gameEnd(false) {
        if (!gameFont.loadFromFile("calibri.ttf")) {
            cout << "Error loading font!" << endl;
            exit(-1);
        }

        if (currentUser.loadUserProfile()) {
            cout << "User profile loaded successfully.\n";
        }
        else {
            cout << "No profile found. Creating a new one.\n";
        }

        setupUI();
    }

    void setupUI() {
        titleText = sf::Text("Hangman Game", gameFont, 50);
        titleText.setFillColor(sf::Color(255, 182, 193));
        titleText.setPosition(250, 20);

        hintText = sf::Text("Hint: " + hangman.getClue(), gameFont, 20);
        hintText.setFillColor(sf::Color(255, 105, 180));
        hintText.setPosition(50, 100);

        guessedText = sf::Text(hangman.getCurrentGuess(), gameFont, 40);
        guessedText.setFillColor(sf::Color(0, 128, 255));
        guessedText.setPosition(250, 200);

        statusMessage = sf::Text("", gameFont, 30);
        statusMessage.setFillColor(sf::Color(255, 105, 180));
        statusMessage.setPosition(250, 500);

        userProfile = sf::Text(currentUser.getSummary(), gameFont, 20);
        userProfile.setFillColor(sf::Color(255, 105, 180));
        userProfile.setPosition(50, 500);

        namePrompt = sf::Text("Enter your name: ", gameFont, 20);
        namePrompt.setFillColor(sf::Color(255, 105, 180));
        namePrompt.setPosition(50, 100);

        nameDisplay = sf::Text("", gameFont, 20);
        nameDisplay.setFillColor(sf::Color(255, 105, 180));
        nameDisplay.setPosition(250, 100);

        // Hangman parts setup
        head = sf::CircleShape(30);
        head.setFillColor(sf::Color(255, 223, 186));
        head.setOutlineThickness(3);
        head.setOutlineColor(sf::Color::Black);
        head.setPosition(100, 150);

        body = sf::RectangleShape(sf::Vector2f(5, 100));
        body.setFillColor(sf::Color(255, 105, 180));
        body.setPosition(130, 210);

        leftArm = sf::RectangleShape(sf::Vector2f(5, 50));
        leftArm.setFillColor(sf::Color(255, 105, 180));
        leftArm.setRotation(45);
        leftArm.setPosition(130, 210);

        rightArm = sf::RectangleShape(sf::Vector2f(5, 50));
        rightArm.setFillColor(sf::Color(255, 105, 180));
        rightArm.setRotation(-45);
        rightArm.setPosition(130, 210);

        leftLeg = sf::RectangleShape(sf::Vector2f(5, 50));
        leftLeg.setFillColor(sf::Color(255, 105, 180));
        leftLeg.setRotation(45);
        leftLeg.setPosition(130, 310);

        rightLeg = sf::RectangleShape(sf::Vector2f(5, 50));
        rightLeg.setFillColor(sf::Color(255, 105, 180));
        rightLeg.setRotation(-45);
        rightLeg.setPosition(130, 310);
    }

    void run() override {
        while (window.isOpen()) {
            processEvents();
            update();
            render();
        }
    }
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                currentUser.saveUserProfile();
                window.close();
            }

            if (nameInputActive && event.type == sf::Event::TextEntered) {
                char typedChar = event.text.unicode;
                if (typedChar == '\b' && !enteredName.empty()) {
                    enteredName.pop_back();
                }
                else if (isalnum(typedChar)) {
                    enteredName += typedChar;
                }

                nameDisplay.setString(enteredName);
            }

            if (nameInputActive && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                currentUser = User(enteredName);
                nameInputActive = false;
            }

            if (!nameInputActive && event.type == sf::Event::KeyPressed) {
                if (isalpha(event.key.code + 'A')) {
                    hangman.processInput(event.key.code + 'A', statusMessage);
                    guessedText.setString(hangman.getCurrentGuess());
                }
            }

            if (!nameInputActive && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && gameEnd) {
                hangman.startNewGame();
                guessedText.setString(hangman.getCurrentGuess());
                hintText.setString("Hint: " + hangman.getClue());
                statusMessage.setString("");
                gameEnd = false;
            }
        }
    }

    void update() {
        if (!nameInputActive && !gameEnd) {
            if (hangman.isWon()) {
                currentUser.increaseWins();
                currentUser.saveUserProfile();
                statusMessage.setString("You Win! Press SPACE to play again.");
                gameEnd = true;
            }
            else if (hangman.isLost()) {
                currentUser.increaseLosses();
                currentUser.saveUserProfile();
                statusMessage.setString("You Lose! The word was: " + hangman.getTargetWord());
                gameEnd = true;
            }

            userProfile.setString(currentUser.getSummary());
        }
    }

    void render() {
        window.clear(sf::Color(255, 239, 255));

        if (hangman.getRemainingTries() > 0) window.draw(head);
        if (hangman.getRemainingTries() > 1) window.draw(body);
        if (hangman.getRemainingTries() > 2) window.draw(leftArm);
        if (hangman.getRemainingTries() > 3) window.draw(rightArm);
        if (hangman.getRemainingTries() > 4) window.draw(leftLeg);
        if (hangman.getRemainingTries() > 5) window.draw(rightLeg);

        if (nameInputActive) {
            window.draw(namePrompt);
            window.draw(nameDisplay);
        }
        else {
            window.draw(titleText);
            window.draw(hintText);
            window.draw(guessedText);
            window.draw(statusMessage);
            window.draw(userProfile);
        }

        window.display();
    }
};



class MainMenu {
protected:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text menuText1, menuText2;
    sf::Texture backgroundTexture;  // Texture now a member variable
    sf::Sprite backgroundSprite;    // Sprite to use the texture

public:
    MainMenu(sf::RenderWindow& win) : window(win) {
        if (!font.loadFromFile("Roboto-Light.ttf")) {  
            cout << "Failed to load font!" << endl;
            exit(1);
        }

        // Load background texture
        if (!backgroundTexture.loadFromFile("background.png")) {
            cerr << "Failed to load background texture!" << endl;
            exit(1);
        }
        // Assign the texture to the sprite
        backgroundSprite.setTexture(backgroundTexture);
    }

    // Pure virtual function - must be overridden by derived classes
    virtual void run() = 0;

    // Rename the second function to avoid overloading conflict
    int showMenu() {
        sf::Event event;
        while (window.isOpen()) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        return 1;  // Snake Game selected
                    }
                    else if (event.key.code == sf::Keyboard::Num2) {
                        return 2;  // Wordle Game selected
                    }
                    else if (event.key.code == sf::Keyboard::Num3) {
                        return 3;  // Hangman Game selected
                    }
                }
            }

            window.clear();
            window.draw(backgroundSprite);  // Draw the background first
            window.draw(menuText1);
            window.draw(menuText2);
            window.display();
        }
        return 0;
    }

    virtual ~MainMenu() {}  // Virtual destructor for safety
};

class MainMenu1 : public MainMenu {
private:
    sf::Texture backgroundTexture;  // Background texture
    sf::Sprite backgroundSprite;    // Background sprite

public:
    // Constructor with base class initialization
    MainMenu1(sf::RenderWindow& win)
        : MainMenu(win), backgroundTexture(), backgroundSprite() {

        if (!backgroundTexture.loadFromFile("firstmenu.png")) {
            cout << "Error loading background image\n";
        }
        backgroundSprite.setTexture(backgroundTexture);
    }

    // Override the pure virtual run() method
    void run() override {
        int choice = showMenu();  // Display menu from the base class

        sf::Font font;
        if (!font.loadFromFile("Roboto-Light.ttf")) {
            cout << "Error loading font\n";
            return;
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    case sf::Keyboard::Num1: choice = 1; break;  // Games
                    case sf::Keyboard::Num2: choice = 2; break;  // How to Play
                    case sf::Keyboard::Num3: choice = 3; break;  // Player Profile
                    case sf::Keyboard::Num4: window.close(); return;  // Quit
                    }
                }
            }

            window.clear();
            window.draw(backgroundSprite);  // Draw background

            // Example placeholder text drawing
            sf::Text menuText("Main Menu", font, 60);
            menuText.setFillColor(sf::Color::White);
            menuText.setPosition(window.getSize().x / 2 - 150, 100);
            window.draw(menuText);

            window.display();
        }
    }
};

void showHowToPlay(sf::RenderWindow& window, sf::Font& font) {
    
    font.loadFromFile("Roboto-Light.ttf");

    sf::Text text(
        "How to Play:\n\n"
        "- **Snake Game**:\n"
        "  Control a snake to eat food, growing in size and increasing your score by 5 points per food.\n"
        "  Every 15 seconds, the snake’s speed increases. Avoid colliding with the walls or yourself .\n"
        "  Use the arrow keys to control the snake. The game tests your reflexes and strategy as it\n gets faster over time.\n\n"
        "- **Wordle Game**:\n"
        "  Guess a 5-letter word in six attempts. Letters are highlighted to show correctness:\n"
        "  Green (correct and in position),Yellow(correct but wrong position),and Red(not in the word).\n"
        "  Win by guessing the word within the attempts. It's a fun challenge to improve your logic.\n\n"
        "- **Hangman Game**:\n"
        "  Guess a hidden word letter by letter.For each wrong guess,a part of a stick-figure is drawn.\n"
        "  You lose if the figure is fully drawn after 6 incorrect guesses. Hints are provided to help\n you guess the word."
        "  The game combines problem-solving and word knowledge in a\n classic format.",
        font, 25);

    text.setFillColor(sf::Color::Black);
    text.setPosition(50, 50);

    // Main event loop for the "How to Play" screen
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B) {
                return;  
            }
        }

        window.clear(sf::Color(185, 173, 150));
        window.draw(text);
        window.display();
    }
}
void managePlayerProfile(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("Roboto-Light.ttf")) {
        cout << "Failed to load font!" << endl;
        return;
    }

    // Load All Previous Records from File
    string previousRecords = "Previous Records:\n";
    ifstream inFile("player_profile.txt");

    if (inFile.is_open()) {
        string name, game;
        int recordCount = 1;
        while (getline(inFile, name) && getline(inFile, game)) {
            previousRecords += to_string(recordCount++) + ". Player: " + name + " - Last Game: " + game + "\n";
        }
        inFile.close();
    }
    else {
        previousRecords += "No records found.\n";
    }

    // Prepare Previous Records Display Text
    sf::Text previousRecordText(previousRecords, font, 30);
    previousRecordText.setFillColor(sf::Color::Yellow);
    previousRecordText.setPosition(100, 50);

    // Name Input Text
    sf::Text prompt("Enter your name: ", font, 40);
    prompt.setFillColor(sf::Color::White);
    prompt.setPosition(100, 300);

    string playerName;
    sf::Text playerInput("", font, 40);
    playerInput.setFillColor(sf::Color::Black);
    playerInput.setPosition(100, 380);

    // Game Choice Input
    sf::Text gamePrompt("Which game do you want to play?", font, 40);
    gamePrompt.setFillColor(sf::Color::White);
    gamePrompt.setPosition(100, 450);

    string gameChoice;
    sf::Text gameInput("", font, 40);
    gameInput.setFillColor(sf::Color::Black);
    gameInput.setPosition(100, 530);

    bool askingForGame = false;

    // Main Event Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle Name Input
            if (!askingForGame && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !playerName.empty()) {
                    playerName.pop_back();  // Handle backspace
                }
                else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    playerName += static_cast<char>(event.text.unicode);
                }
            }

            // Handle Game Choice Input
            if (askingForGame && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !gameChoice.empty()) {
                    gameChoice.pop_back();  // Handle backspace
                }
                else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    gameChoice += static_cast<char>(event.text.unicode);
                }
            }

            // Handle Enter Key
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                if (!askingForGame && !playerName.empty()) {
                    askingForGame = true;  // Proceed to Game Choice
                }
                else if (askingForGame && !gameChoice.empty()) {
                    // Save the New Record
                    ofstream outFile("player_profile.txt", ios::app);  // Append to file
                    outFile << playerName << endl;
                    outFile << gameChoice << endl;
                    outFile.close();

                    cout << "Record Saved: " << playerName << " - " << gameChoice << endl;
                    return;  // Exit the function
                }
            }
        }

        // Update Dynamic Texts
        playerInput.setString(playerName);
        gameInput.setString(gameChoice);

        // Draw UI
        window.clear(sf::Color(185, 173, 150));
        window.draw(previousRecordText);  // Show All Previous Records
        window.draw(prompt);              // Show Name Prompt
        window.draw(playerInput);         // Show Entered Name

        if (askingForGame) {
            window.draw(gamePrompt);   // Show Game Choice Prompt
            window.draw(gameInput);    // Show Entered Game Choice
        }

        window.display();
    }
}


void showCredits(sf::RenderWindow& window) {
    sf::Font font;
    if (!font.loadFromFile("Roboto-Light.ttf")) {
        cerr << "Failed to load font!" << endl;
        return;
    }


    string creditsText =
        "Game Credits:\n\n"
        "Snake Game:\n"
        "Kaiwan Shahid\n"
        "Roll Number: 23i-2513\n"
        "Class: AI-A\n\n"

        "Wordle Game:\n"
        "Bilal Naseer\n"
        "Roll Number: 23i-0128\n"
        "Class: AI-A\n\n"

        "Hangman Game:\n"
        "Laiba Iftikhar\n"
        "Roll Number: 23i-0099\n"
        "Class: AI-A\n\n"

        "\t\t\t\t\t\t\t" "\"Ever tried. Ever failed. No matter.\n"
        "\t\t\t\t\t\t\tTry again. Fail again. Fail better.\n"
        "\t\t\t\t\t\t\tThe world is yours.\"\n";

    sf::Text credits(creditsText, font, 30);
    credits.setFillColor(sf::Color::Black);
    credits.setPosition(100, 50);

    // Main Loop for Display
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                return;  // Exit the credits screen when closed or Escape is pressed
            }
        }

        // Clear and Draw Credits
        window.clear(sf::Color(185, 173, 150));
        window.draw(credits);
        window.display();
    }
}


int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 900), "GameBoy Emulator");
    sf::Font font;
    // Load background music
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("chupke chupke.ogg")) {
        cerr << "Error loading music file!" << endl;
        return -1;
    }

    backgroundMusic.setLoop(true);
    backgroundMusic.setVolume(50);
    backgroundMusic.play();

    // Main Menu Loop
    while (window.isOpen()) {
        MainMenu* currentGame = nullptr;
        MainMenu1 mainMenu(window);
        
        // Correctly initialized
        mainMenu.run();  // Show main menu

        int choice = mainMenu.showMenu();  // Fetch selected option

        // Handle Game Selection with Polymorphism
        switch (choice) {
        case 1:
            currentGame = new SnakeGame(window);  // Correct initialization
            break;
        case 2:
            currentGame = new WordleGame(window);  // Correct initialization
            break;
        case 3:
            currentGame = new GameManager(window);  // Correct initialization
            break;
        default:
            window.close();  // Close the window on invalid choice
            break;
        }

        // Run the selected game
        if (currentGame) {
            currentGame->run();  // Polymorphic behavior
            delete currentGame;  // Cleanup
            currentGame = nullptr;
        }
    }

    return 0;
}






