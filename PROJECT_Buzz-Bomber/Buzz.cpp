#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cfloat>   
#include <cmath> 
#include <fstream>
#include <sstream>
using namespace std;
using namespace sf;

// Constants
const int WINDOW_WIDTH = 960;
const int WINDOW_HEIGHT = 640;
int MAX_BEES = 10;
int MAX_FAST_BEES = 5;
const int MAX_BULLETS = 1000;
const int MAX_HONEYCOMBS = MAX_BEES + MAX_FAST_BEES;
const int MAX_FLOWERS = MAX_BEES + MAX_FAST_BEES;
int BEE_SPEED_HORIZONTAL = 20;
int BEE_SPEED_VERTICAL = 40;
int FAST_BEE_SPEED_HORIZONTAL = 30;
int FAST_BEE_SPEED_VERTICAL = 60;
const int MAX_LIVES = 3;  // Maximum player lives
sf::SoundBuffer bulletFireBuffer, bulletHitBuffer;
sf::Sound bulletFireSound, bulletHitSound;
int currentLevel = 0;   // Starting level
const int MAX_LEVELS = 20; // Maximum number of levels
const int MAX_HIVES = 5;
const int MAX_LEADERBOARD_SIZE = 10;
const int NAME_LENGTH = 50;  // Assuming the name won't exceed 50 characters

// Bird properties
const int BIRD_SPEED = 50;

// Function Prototypes
void shootBullet(Sprite bullets[], bool bulletActive[], const Sprite &playerSprite, Clock &bulletCooldownClock, Texture &bulletTexture);
void moveBullets(Sprite bullets[], bool bulletActive[], Clock &bulletClock);
void moveBees(float beeX[], float beeY[], bool moveRight[], Sprite bees[], Clock &beeClock, bool beeActive[], int beeCount, int horizontalSpeed, int verticalSpeed);
bool checkCollision(const Sprite &bullet, const Sprite &bee);
bool checkBeeHoneycombCollision(const Sprite &bee, const Sprite &honeycomb);
void spawnHoneycomb(float honeycombX[], float honeycombY[], bool honeycombActive[], int honeycombType[], float beeX, float beeY);
void moveBird(Sprite &bird, float &birdX, float &birdY, bool &birdActive, float honeycombX[], float honeycombY[], bool honeycombActive[], int &score, Clock &birdClock, int honeycombType[]);


// Player health
int playerLives = MAX_LIVES;
Texture heartTexture;
Sprite hearts[MAX_LIVES];
void startLevel(RenderWindow &window, int level) {
    // Set background music based on the level
    string musicFile;
    switch (level) {
        case 1: musicFile = "Music/level1.ogg"; break;
        case 2: musicFile = "Music/level2.ogg"; break;
        case 3: musicFile = "Music/level3.ogg"; break;
        case 4: musicFile = "Music/level4.ogg"; break;
    }

    // Load music for the selected level
    Music levelMusic;
    if (!levelMusic.openFromFile(musicFile)) {
        cout << "Error loading music!" << endl;
    } else {
        levelMusic.play();
    }

    // You can implement additional logic here to handle level-specific gameplay
    // For example: enemies, obstacles, objectives for the level, etc.
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        // Level game logic and rendering here
        window.clear();
        // Draw level-specific elements
        window.display();
    }
}


// Function to load the leaderboard from a file
void loadLeaderboard(string leaderboardNames[MAX_LEADERBOARD_SIZE], int leaderboardScores[MAX_LEADERBOARD_SIZE]) {
    ifstream file("leaderboard.txt");

    // Initialize the leaderboard with empty values
    for (int i = 0; i < MAX_LEADERBOARD_SIZE; ++i) {
        leaderboardNames[i] = "";
        leaderboardScores[i] = 0;
    }

    string name;
    int score;
    int index = 0;
    while (file >> name >> score && index < MAX_LEADERBOARD_SIZE) {
        leaderboardNames[index] = name;
        leaderboardScores[index] = score;
        ++index;
    }

    file.close();
}

// Function to save the leaderboard to a file
void saveLeaderboard(string leaderboardNames[MAX_LEADERBOARD_SIZE], int leaderboardScores[MAX_LEADERBOARD_SIZE]) {
    ofstream file("leaderboard.txt");

    for (int i = 0; i < MAX_LEADERBOARD_SIZE; ++i) {
        if (leaderboardNames[i] != "") {
            file << leaderboardNames[i] << " " << leaderboardScores[i] << endl;
        }
    }

    file.close();
}

// Function to add a new score to the leaderboard
void addScoreToLeaderboard(int score, string leaderboardNames[MAX_LEADERBOARD_SIZE], int leaderboardScores[MAX_LEADERBOARD_SIZE]) {
    // Check if the new score is high enough to be in the top 10
    for (int i = 0; i < MAX_LEADERBOARD_SIZE; ++i) {
        if (score > leaderboardScores[i]) {
            // Shift the leaderboard entries down to make space for the new score
            for (int j = MAX_LEADERBOARD_SIZE - 1; j > i; --j) {
                leaderboardNames[j] = leaderboardNames[j - 1];
                leaderboardScores[j] = leaderboardScores[j - 1];
            }

            // Prompt for the player's name and insert the new score
            string playerName;
            cout << "Enter your name: ";
            getline(cin, playerName);
            leaderboardNames[i] = playerName;
            leaderboardScores[i] = score;
            break;
        }
    }
}

// Function to display the leaderboard
void displayLeaderboard(RenderWindow &window, Font &font, string leaderboardNames[MAX_LEADERBOARD_SIZE], int leaderboardScores[MAX_LEADERBOARD_SIZE]) {
    window.clear();

    // Leaderboard title
    Text titleText;
    titleText.setFont(font);
    titleText.setString("Leaderboard");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color::White);
    titleText.setPosition(window.getSize().x / 2 - titleText.getLocalBounds().width / 2, 50);
    window.draw(titleText);

    // Display the leaderboard
    for (int i = 0; i < MAX_LEADERBOARD_SIZE; ++i) {
        if (leaderboardNames[i] != "") {
            Text leaderboardText;
            leaderboardText.setFont(font);
            stringstream ss;
            ss << (i + 1) << ". " << leaderboardNames[i] << " - " << leaderboardScores[i];
            leaderboardText.setString(ss.str());
            leaderboardText.setCharacterSize(30);
            leaderboardText.setFillColor(Color::White);
            leaderboardText.setPosition(window.getSize().x / 2 - leaderboardText.getLocalBounds().width / 2, 150 + i * 40);
            window.draw(leaderboardText);
        }
    }

    // Option to go back to main menu
    Text backText;
    backText.setFont(font);
    backText.setString("Press Esc to Return");
    backText.setCharacterSize(30);
    backText.setFillColor(Color::White);
    backText.setPosition(window.getSize().x / 2 - backText.getLocalBounds().width / 2, 400);
    window.draw(backText);

    window.display();

    // Wait for user input to go back to main menu
    bool backToMenu = false;
    while (!backToMenu && window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            } else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                backToMenu = true; // Go back to the main menu
            }
        }
    }
}
void displayScore(RenderWindow &window, int score, Font &font) {
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(WINDOW_WIDTH - 150, 10);  // Position it on the top-right corner

    window.draw(scoreText);
}

int highScore = 0; // Global variable for high score

// Function to load high score from file
void loadHighScore() {
    ifstream inputFile("highscore.txt");
    if (inputFile) {
        inputFile >> highScore;
    }
    inputFile.close();
}

// Function to save high score to file
void saveHighScore() {
    ofstream outputFile("highscore.txt");
    if (outputFile) {
        outputFile << highScore;
    }
    outputFile.close();
}

// Main Menu Function
// Function to display the main menu
void displayMainMenu(RenderWindow &window, Font &font, int &selectedOption) {
    window.clear();

    // Title Text
    Text titleText;
    titleText.setFont(font);
    titleText.setString("Buzz Bombers");
    titleText.setCharacterSize(50);
    titleText.setFillColor(Color::White);
    titleText.setPosition(
        window.getSize().x / 2 - titleText.getLocalBounds().width / 2, 
        100
    );
    window.draw(titleText);

    // High Score Display (Placeholder for actual high score)
    int highScore = 1000; // Replace with actual high score logic if available
    Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setString("High Score: " + std::to_string(highScore));
    highScoreText.setCharacterSize(30);
    highScoreText.setFillColor(Color::White);
    highScoreText.setPosition(
        window.getSize().x / 2 - highScoreText.getLocalBounds().width / 2, 
        180
    );
    window.draw(highScoreText);

    // Menu Options
    string menuOptions[] = {
        "Start Game (Level 1)",
        "Start Game (Level 2)",
        "Start Game (Level 3)",
        "Start Game (Level 4)",
        "Leaderboard",
        "Quit"
    };

    for (int i = 0; i < 6; ++i) {  // Changed from 5 to 6 options now
        Text menuText;
        menuText.setFont(font);
        menuText.setString(menuOptions[i]);
        menuText.setCharacterSize(30);
        menuText.setFillColor(selectedOption == i ? Color::Red : Color::White);
        menuText.setPosition(
            window.getSize().x / 2 - menuText.getLocalBounds().width / 2, 
            240 + i * 40 // Space out menu options
        );
        window.draw(menuText);
    }

    window.display();
}

// Function to display the current level
void displayLevel(RenderWindow &window, int level, Font &font) {
    // Create a text object for the level display
    Text levelText;
    levelText.setFont(font);
    levelText.setString("Level: " + std::to_string(level));
    levelText.setCharacterSize(30);
    levelText.setFillColor(Color::White);

    // Position the level text in the top-right corner
    levelText.setPosition(window.getSize().x - levelText.getLocalBounds().width - 20, 10);
    window.draw(levelText);
}

void loadHivesFromFile(const std::string& filename, float x[], float y[], int type[], int& hiveCount) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error opening file!" << std::endl;
        return;
    }

    hiveCount = 0;
    while (file.good() && hiveCount < MAX_HIVES) {
        file >> x[hiveCount] >> y[hiveCount] >> type[hiveCount];

        if (file.good()) {
            ++hiveCount;
        }
    }

    file.close();
}

// Function to draw the hives
void drawHives(RenderWindow& window, float x[], float y[], int type[], int hiveCount, Texture& hiveTexture) {
    for (int i = 0; i < hiveCount; ++i) {
        Sprite hiveSprite(hiveTexture);
        hiveSprite.setPosition(x[i], y[i]);
        window.draw(hiveSprite);
    }
}

int main() {
    srand(time(0));

    // Create the game window
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Buzz Bombers");

    // Load font
    Font font;
        Text scoreText;
    if (!font.loadFromFile("Textures/arial.ttf")) {
        return -1; // Handle error loading font
    }
    // Load leaderboard from file
    string leaderboardNames[MAX_LEADERBOARD_SIZE];
    int leaderboardScores[MAX_LEADERBOARD_SIZE];
    loadLeaderboard(leaderboardNames, leaderboardScores);

    int selectedOption = 0; // Current menu selection
while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
        if (event.type == Event::Closed) {
            window.close();
        } else if (event.type == Event::KeyPressed) {
            // Handle Up/Down arrow keys for navigation
            if (event.key.code == Keyboard::Up) {
                selectedOption = (selectedOption > 0) ? selectedOption - 1 : 5; // Wrap-around to bottom
            } else if (event.key.code == Keyboard::Down) {
                selectedOption = (selectedOption < 5) ? selectedOption + 1 : 0; // Wrap-around to top
            } else if (event.key.code == Keyboard::Enter) {
                if (selectedOption == 5) { // Quit option
                    window.close();
                } else if (selectedOption == 4) { // Leaderboard option
                    // Display the leaderboard (or transition to leaderboard screen)
                    displayLeaderboard(window, font, leaderboardNames, leaderboardScores);
                } else {
                    // Start game logic for selected level (1-4)
                    int level = selectedOption + 1;
                    std::cout << "Starting Level " << level << "!" << std::endl;
                    // Transition to game logic here
                }
            }
        }
    }

    displayMainMenu(window, font, selectedOption);
}

    bool isInMenu = true;
    
    loadHighScore(); // Load the high score from file

	if (!bulletFireBuffer.loadFromFile("fire.wav")) {
	    cout << "Error loading bullet fire sound!" << endl;
	    return -1;
	}
	bulletFireSound.setBuffer(bulletFireBuffer);

	if (!bulletHitBuffer.loadFromFile("hit.wav")) {
	    cout << "Error loading bullet hit sound!" << endl;
	    return -1;
	}
	bulletHitSound.setBuffer(bulletHitBuffer);  

    // Main game loop
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            
            // Handle menu navigation
            if (isInMenu) {
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Up) {
                        selectedOption = (selectedOption == 0) ? 1 : 0;  // Toggle between Start and Quit
                    }
                    if (event.key.code == Keyboard::Down) {
                        selectedOption = (selectedOption == 1) ? 0 : 1;
                    }
                    if (event.key.code == Keyboard::Enter) {
                        if (selectedOption == 0) {
                            // Start the game
                            isInMenu = false;
                        } else if (selectedOption == 1) {
                            // Quit the game
                            window.close();
                        }
                    }
                }
            }
        }

        // Display main menu
        if (isInMenu) {
            displayMainMenu(window, font, selectedOption);
        } else {
            // Proceed to game loop here (your existing game code)
            // For now, we just simulate a delay before starting the game loop
            cout << "Starting the game..." << endl;
            break;
        }
    }

    // Game code starts here (your current game logic)
   // Setup score text properties
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(650, 10); // Position it at the top-right of the screen

    // Load background texture
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Textures/gal.jpg")) {
        cout << "Error loading background texture!" << endl;
        return -1;
    }
    Sprite background(backgroundTexture);

	Texture flowerTexture;
	if (!flowerTexture.loadFromFile("Textures/obstacles.png")) {  // Add your flower image path here
	    cout << "Error loading flower texture!" << endl;
	    return -1;
	}
	Sprite flowers[MAX_BEES];  // Using MAX_BEES for flowers, but you can adjust this
	float flowerX[MAX_BEES], flowerY[MAX_BEES];
	bool flowerActive[MAX_BEES] = {false};

      if (!heartTexture.loadFromFile("Textures/heart.png")) {
      cout << "Error loading heart texture!" << endl;
      return -1;
      }

// Initialize heart sprites
for (int i = 0; i < MAX_LIVES; ++i) {
    hearts[i].setTexture(heartTexture);
    hearts[i].setPosition(10 + i * 40, 10);  // Position hearts horizontally
}

    // Player sprite
    Texture playerTexture;
    if (!playerTexture.loadFromFile("Textures/spray.png")) {
        cout << "Error loading player texture!" << endl;
        return -1;
    }
    Sprite playerSprite(playerTexture);
    playerSprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 64);

    // Bullet handling
    Texture bulletTexture;
    if (!bulletTexture.loadFromFile("Textures/bullet.png")) {
        cout << "Error loading bullet texture!" << endl;
        return -1;
    }
    Sprite bullets[MAX_BULLETS];
    bool bulletActive[MAX_BULLETS] = {false};
    Clock bulletClock;
    Clock bulletCooldownClock;


    Texture hiveTexture;
    if (!hiveTexture.loadFromFile("Textures/hive.png")) {
            cout << "Error loading hive texture!" << std::endl;
        return -1;
    }

    // Arrays to store hive data
    float x[MAX_HIVES], y[MAX_HIVES];
    int type[MAX_HIVES];
    int hiveCount = 5;
    
    
    // Regular Bee handling
    Texture beeTexture;
    if (!beeTexture.loadFromFile("Textures/Regular_bee.png")) {
        cout << "Error loading bee texture!" << endl;
        return -1;
    }
   
    Sprite bees[MAX_BEES];
    float beeX[MAX_BEES], beeY[MAX_BEES];
    bool beeActive[MAX_BEES] = {false}, moveRight[MAX_BEES] = {false};

    for (int i = 0; i < MAX_BEES; ++i) {
        bees[i].setTexture(beeTexture);
        beeX[i] = rand() % (WINDOW_WIDTH - 32);
        beeY[i] = rand() % (WINDOW_HEIGHT / 2);
        beeActive[i] = true;
        moveRight[i] = rand() % 2 == 0;
    }
    Clock beeClock;

    // Fast Bee handling
    Texture fastBeeTexture;
    if (!fastBeeTexture.loadFromFile("Textures/Fast_bee.png")) {
        cout << "Error loading fast bee texture!" << endl;
        return -1;
    }
    Sprite fastBees[MAX_FAST_BEES];
    float fastBeeX[MAX_FAST_BEES], fastBeeY[MAX_FAST_BEES];
    bool fastBeeActive[MAX_FAST_BEES] = {false}, fastMoveRight[MAX_FAST_BEES] = {false};

    for (int i = 0; i < MAX_FAST_BEES; ++i) {
        fastBees[i].setTexture(fastBeeTexture);
        fastBeeX[i] = rand() % (WINDOW_WIDTH - 32);
        fastBeeY[i] = rand() % (WINDOW_HEIGHT / 2);
        fastBeeActive[i] = true;
        fastMoveRight[i] = rand() % 2 == 0;
    }
    Clock fastBeeClock;

    // Honeycomb handling
    Texture honeycombTexture, redHoneycombTexture;
    if (!honeycombTexture.loadFromFile("Textures/honeycomb.png") ||
        !redHoneycombTexture.loadFromFile("Textures/honeycomb_red.png")) {
        cout << "Error loading honeycomb textures!" << endl;
        return -1;
    }
    Sprite honeycombs[MAX_HONEYCOMBS];
    float honeycombX[MAX_HONEYCOMBS] = {0}, honeycombY[MAX_HONEYCOMBS] = {0};
    bool honeycombActive[MAX_HONEYCOMBS] = {false};
    int honeycombType[MAX_HONEYCOMBS] = {0};

    // Bird handling
    Texture birdTexture;
    if (!birdTexture.loadFromFile("Textures/bird.png")) {
        cout << "Error loading bird texture!" << endl;
        return -1;
    }
    Sprite bird(birdTexture);
    float birdX = -50, birdY = 100;
    bool birdActive = true;
    int score = 0;
    Clock birdClock;

    bool spacebarPressed = false;

	while (window.isOpen()) {
	    Event event;
	    while (window.pollEvent(event)) {
		if (event.type == Event::Closed)
		    window.close();
	    }

	    // Player movement
	    if (Keyboard::isKeyPressed(Keyboard::Left) && playerSprite.getPosition().x > 0)
		playerSprite.move(-2, 0);
	    if (Keyboard::isKeyPressed(Keyboard::Right) && playerSprite.getPosition().x < WINDOW_WIDTH - 32)
		playerSprite.move(2, 0);
		
	// Player movement with flower blocking
	for (int i = 0; i < MAX_FLOWERS; ++i) {
	    if (flowerActive[i]) {
		// Check if the player is blocked by a flower on the left
		if (Keyboard::isKeyPressed(Keyboard::Left) && playerSprite.getPosition().x > 0) {
		    if (playerSprite.getPosition().x > flowerX[i]) {  // If player is not blocked by a flower on the left
		        playerSprite.move(-2, 0);
		    }
		}

		// Check if the player is blocked by a flower on the right
		if (Keyboard::isKeyPressed(Keyboard::Right) && playerSprite.getPosition().x < WINDOW_WIDTH - 32) {
		    if (playerSprite.getPosition().x < flowerX[i]) {  // If player is not blocked by a flower on the right
		        playerSprite.move(2, 0);
		    }
		}
	    }
	}

	    // Shooting bullets - Only fire when space is pressed and not continuously
	    if (Keyboard::isKeyPressed(Keyboard::Space) && !spacebarPressed) {
		shootBullet(bullets, bulletActive, playerSprite, bulletCooldownClock, bulletTexture);
		spacebarPressed = true;  // Set flag to prevent continuous firing
	    } else if (!Keyboard::isKeyPressed(Keyboard::Space)) {
		spacebarPressed = false;  // Reset flag when spacebar is released
	    }

	    // Update bullet positions
	    moveBullets(bullets, bulletActive, bulletClock);

	    // Move bees
	    moveBees(beeX, beeY, moveRight, bees, beeClock, beeActive, MAX_BEES, BEE_SPEED_HORIZONTAL, BEE_SPEED_VERTICAL);
	    moveBees(fastBeeX, fastBeeY, fastMoveRight, fastBees, fastBeeClock, fastBeeActive, MAX_FAST_BEES, FAST_BEE_SPEED_HORIZONTAL, FAST_BEE_SPEED_VERTICAL);

	    // Move bird
	    moveBird(bird, birdX, birdY, birdActive, honeycombX, honeycombY, honeycombActive, score, birdClock, honeycombType);

	    // Check collisions
	    for (int i = 0; i < MAX_BEES; ++i) {
		if (beeActive[i]) {
		    for (int j = 0; j < MAX_BULLETS; ++j) {
		        if (bulletActive[j] && checkCollision(bullets[j], bees[i])) {
		            beeActive[i] = false;
		            bulletActive[j] = false;
		            spawnHoneycomb(honeycombX, honeycombY, honeycombActive, honeycombType, beeX[i], beeY[i]);
		        }
		    }
		}
	    }

	    for (int i = 0; i < MAX_FAST_BEES; ++i) {
		if (fastBeeActive[i]) {
		    for (int j = 0; j < MAX_BULLETS; ++j) {
		        if (bulletActive[j] && checkCollision(bullets[j], fastBees[i])) {
		            fastBeeActive[i] = false;
		            bulletActive[j] = false;
		            spawnHoneycomb(honeycombX, honeycombY, honeycombActive, honeycombType, fastBeeX[i], fastBeeY[i]);
		        }
		    }
		}
	    }
	    
		for (int i = 0; i < MAX_HONEYCOMBS; ++i) {
		    if (honeycombActive[i]) {  // Check if honeycomb is active
			for (int j = 0; j < MAX_BULLETS; ++j) {
			    if (bulletActive[j] && checkCollision(bullets[j], honeycombs[i])) { // Check if the bullet collides with the honeycomb
				honeycombActive[i] = false;  // Deactivate the honeycomb after collision
				bulletActive[j] = false;     // Deactivate the bullet after collision
				break; // No need to check further bullets for this honeycomb
			    }
			}
		    }
		}

	    for (int i = 0; i < MAX_BEES; ++i) {
    if (beeActive[i]) {
        if (checkCollision(playerSprite, bees[i])) {
            // Spawn flower at the collision position
            flowerX[i] = playerSprite.getPosition().x;
            flowerY[i] = playerSprite.getPosition().y;
            flowerActive[i] = true;

            // Make the bee inactive
            beeActive[i] = false;
            
            // Reduce the player's movement area
            if (playerSprite.getPosition().x < flowerX[i]) {
                // Restrict player to the left side of the flower
                playerSprite.setPosition(flowerX[i] + 32, playerSprite.getPosition().y);
            } else {
                // Restrict player to the right side of the flower
                playerSprite.setPosition(flowerX[i] - 32, playerSprite.getPosition().y);
            }
        }
    }
            }

		// Check for bee collisions with the player
		for (int i = 0; i < MAX_BEES; ++i) {
		    if (beeActive[i] && playerSprite.getGlobalBounds().intersects(bees[i].getGlobalBounds())) {
			// Decrease player's lives
			playerLives--;
			beeActive[i] = false; // Deactivate the bee
			
			// Game Over condition
			if (playerLives <= 0) {
			    cout << "Game Over!" << endl;
			    window.close();
			}
		    }
		}

		for (int i = 0; i < MAX_FAST_BEES; ++i) {
		    if (fastBeeActive[i] && playerSprite.getGlobalBounds().intersects(fastBees[i].getGlobalBounds())) {
			// Decrease player's lives
			playerLives--;
			fastBeeActive[i] = false; // Deactivate the fast bee

			// Game Over condition
			if (playerLives <= 0) {
			    cout << "Game Over!" << endl;
			    window.close();
			}
		    }
		}

	    window.clear();

	    // Draw background
	    window.draw(background);

           displayScore(window, score, font);
        
	    // Draw player
	    window.draw(playerSprite);
	    displayLevel(window, currentLevel, font);

	    // Draw bullets
	    for (int i = 0; i < MAX_BULLETS; ++i)
		if (bulletActive[i]) window.draw(bullets[i]);

	    // Draw bees
	    for (int i = 0; i < MAX_BEES; ++i)
		if (beeActive[i]) window.draw(bees[i]);
	    for (int i = 0; i < MAX_FAST_BEES; ++i)
		if (fastBeeActive[i]) window.draw(fastBees[i]);

        drawHives(window, x, y, type, hiveCount, hiveTexture);
	    // Draw honeycombs
	    for (int i = 0; i < MAX_HONEYCOMBS; ++i) {
		if (honeycombActive[i]) {
		    honeycombs[i].setTexture(honeycombType[i] == 1 ? redHoneycombTexture : honeycombTexture);
		    honeycombs[i].setPosition(honeycombX[i], honeycombY[i]);
		    window.draw(honeycombs[i]);
		}
	    }
	    bool allHoneycombsCollected = true;

// Check if all honeycombs are collected
for (int i = 0; i < MAX_HONEYCOMBS; ++i) {
    if (honeycombActive[i]) {
        allHoneycombsCollected = false;
        break;
    }
}

if (allHoneycombsCollected) {
    currentLevel++;
    if (currentLevel > MAX_LEVELS) {
        cout << "You Win! Game Completed!" << endl;
        window.close(); // End the game
    } else {
        cout << "Level Up! Welcome to Level " << currentLevel << "!" << endl;

        // Reactivate honeycombs for the next level
        for (int i = 0; i < MAX_HONEYCOMBS; ++i) {
            honeycombActive[i] = true; // Activate all honeycombs
            honeycombX[i] = rand() % (WINDOW_WIDTH - 32); // Randomize positions
            honeycombY[i] = rand() % (WINDOW_HEIGHT - 32);
        }


        // Reset Bees
        for (int i = 0; i < MAX_BEES; ++i) {
            beeX[i] = rand() % (WINDOW_WIDTH - 32);
            beeY[i] = rand() % (WINDOW_HEIGHT / 2);
            beeActive[i] = true;
            moveRight[i] = rand() % 2 == 0;
        }

        for (int i = 0; i < MAX_FAST_BEES; ++i) {
            fastBeeX[i] = rand() % (WINDOW_WIDTH - 32);
            fastBeeY[i] = rand() % (WINDOW_HEIGHT / 2);
            fastBeeActive[i] = true;
            fastMoveRight[i] = rand() % 2 == 0;
        }

        // Increase difficulty
        BEE_SPEED_HORIZONTAL += 5;
        BEE_SPEED_VERTICAL += 10;
        FAST_BEE_SPEED_HORIZONTAL += 5;
        FAST_BEE_SPEED_VERTICAL += 10;
        
    }
}

	    // Draw bird
	    if (birdActive)
		window.draw(bird);
		
			
		    // Draw flowers
	for (int i = 0; i < MAX_BEES; ++i) {
	    if (flowerActive[i]) {
		flowers[i].setTexture(flowerTexture);
		flowers[i].setPosition(flowerX[i], flowerY[i]);
		window.draw(flowers[i]);
	    }
	}
	
	for (int i = 0; i < MAX_BEES; ++i) {
             if (beeActive[i] && playerSprite.getGlobalBounds().intersects(bees[i].getGlobalBounds())) {
             playerLives--;  // Decrease player's lives
             beeActive[i] = false;  // Remove the bee
                if (playerLives <= 0) {
                  cout << "Game Over!" << endl;
                  window.close();  // Close the game window
                 }
              }
         }

        
        // Draw hearts based on remaining lives
	for (int i = 0; i < playerLives; ++i) {
    	window.draw(hearts[i]);
	}

            window.draw(scoreText); // Draw the score text
            
            if (score > highScore) {
    highScore = score;
    saveHighScore();  // Save the new high score to the file
}

	    window.display();
	}
	 int playerScore = 1500;  // Replace with actual score from the game
    addScoreToLeaderboard(playerScore, leaderboardNames, leaderboardScores);

    // Save the updated leaderboard
    saveLeaderboard(leaderboardNames, leaderboardScores);

    // Display leaderboard
    displayLeaderboard(window, font, leaderboardNames, leaderboardScores);
    return 0;
}

// Function Definitions
void shootBullet(Sprite bullets[], bool bulletActive[], const Sprite &playerSprite, Clock &bulletCooldownClock, Texture &bulletTexture) {
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (!bulletActive[i]) {
            bullets[i].setTexture(bulletTexture);
            bullets[i].setPosition(playerSprite.getPosition().x + playerSprite.getLocalBounds().width / 2, playerSprite.getPosition().y);
            bulletActive[i] = true;
            bulletCooldownClock.restart();

            // Play bullet firing sound
            bulletFireSound.play();
            break;
        }
    }
}

void moveBullets(Sprite bullets[], bool bulletActive[], Clock &bulletClock) {
    for (int i = 0; i < MAX_BULLETS; ++i) {
        if (bulletActive[i]) {
            bullets[i].move(0, -8);  // Move the bullet upwards
            if (bullets[i].getPosition().y < 0) {  // If it goes off-screen, deactivate the bullet
                bulletActive[i] = false;
            }
        }
    }
}

void moveBees(float beeX[], float beeY[], bool moveRight[], Sprite bees[], Clock &beeClock, bool beeActive[], int beeCount, int horizontalSpeed, int verticalSpeed) {
    if (beeClock.getElapsedTime().asSeconds() > 0.1) {
        for (int i = 0; i < beeCount; ++i) {
            if (beeActive[i]) {
                if (moveRight[i]) {
                    beeX[i] += horizontalSpeed;
                    if (beeX[i] > WINDOW_WIDTH - 32) {
                        moveRight[i] = false;
                        beeY[i] += verticalSpeed;
                    }
                } else {
                    beeX[i] -= horizontalSpeed;
                    if (beeX[i] < 0) {
                        moveRight[i] = true;
                        beeY[i] += verticalSpeed;
                    }
                }
                bees[i].setPosition(beeX[i], beeY[i]);
            }
        }
        beeClock.restart();
    }
}

bool checkCollision(const Sprite &bullet, const Sprite &bee) {
    return bullet.getGlobalBounds().intersects(bee.getGlobalBounds());
}

bool checkBeeHoneycombCollision(const Sprite &bee, const Sprite &honeycomb) {
    return bee.getGlobalBounds().intersects(honeycomb.getGlobalBounds());
}

void spawnHoneycomb(float honeycombX[], float honeycombY[], bool honeycombActive[], int honeycombType[], float beeX, float beeY) {
    for (int i = 0; i < MAX_HONEYCOMBS; ++i) {
        if (!honeycombActive[i]) {
            honeycombX[i] = beeX;
            honeycombY[i] = beeY;
            honeycombActive[i] = true;
            honeycombType[i] = rand() % 2; // 0 for regular, 1 for red
            break;
        }
    }
}

void moveBird(sf::Sprite &bird, float &birdX, float &birdY, bool &birdActive, float honeycombX[], float honeycombY[], bool honeycombActive[], int &score, sf::Clock &birdClock, int honeycombType[]) {
    if (birdClock.getElapsedTime().asSeconds() > 0.05) {
        if (birdActive) {
            // Find the nearest honeycomb
            int nearestHoneycombIndex = -1;
            float minDistance = FLT_MAX;

            // Calculate the distance to each active honeycomb
            for (int i = 0; i < MAX_HONEYCOMBS; ++i) {
                if (honeycombActive[i]) {
                    float dx = honeycombX[i] - birdX;
                    float dy = honeycombY[i] - birdY;
                    float distance = sqrt(dx * dx + dy * dy);

                    if (distance < minDistance) {
                        minDistance = distance;
                        nearestHoneycombIndex = i;
                    }
                }
            }

            // If a honeycomb is found, move the bird towards it
            if (nearestHoneycombIndex != -1) {
                float dx = honeycombX[nearestHoneycombIndex] - birdX;
                float dy = honeycombY[nearestHoneycombIndex] - birdY;
                float distance = sqrt(dx * dx + dy * dy);

                // Normalize the direction
                float moveX = (dx / distance) * 3;  // Speed at which bird moves (adjust as needed)
                float moveY = (dy / distance) * 3;

                birdX += moveX;
                birdY += moveY;

                bird.setPosition(birdX, birdY);

                // Check if bird has reached the honeycomb
                if (distance < 5) {  // Threshold to determine when the bird has reached the honeycomb
                    score++;
                    honeycombActive[nearestHoneycombIndex] = false;  // Remove the honeycomb
                }
            }
        }

        birdClock.restart();
    }
}
