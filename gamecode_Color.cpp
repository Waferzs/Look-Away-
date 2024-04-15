#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <ncurses.h>
#endif

using namespace std;

const float EASY_DELAY = 2.0f; // Delay in seconds for easy mode
const float MEDIUM_DELAY = 1.0f; // Delay in seconds for medium mode
const float HARD_DELAY = 0.5f; // Delay in seconds for hard mode
const int LIVES = 3; // Amount of times user input can be wrong or time to count down

// Color constants
const int COLOR_RED = 12;
const int COLOR_GREEN = 10;
const int COLOR_YELLOW = 14;
const int COLOR_BLUE = 9;

// Function to explain game instructions
void displayInstructions() {
    cout << "Welcome to the Look Away Game!" << endl;
    cout << " " << endl;
    cout << "Instructions:" << endl;
    cout << "* Look away from the direction indicated by the arrow. " << endl;
    cout << "* Do this by entering any the direction opposite of the arrow that appears on the screen." << endl;
    cout << " " << endl;
    cout << "Correct response:" << endl;
    cout << "* If the screen displays an up arrow '^' that arrow represents character 'w'" << endl;
    cout << "* The correct response would be either 'a', 's', or 'd' because character 'w' or '^'" << endl;
    cout << "  is what you are being asked to look away from" << endl;
    cout << " " << endl;
    cout << "Controls:" << endl;
    cout << "* Use the keys 'a', 'w', 'd', 's' to look in different directions." << endl;
    cout << "* 'a' - Left, 'w' - Up, 'd' - Right, 's' - Down." << endl;
    cout << " " << endl;
    cout << "Press Enter to start..." << endl;
    cin.get();
}

// Function to generate a random direction for the object along with color
pair<char, int> generateDirectionWithColor(int prevColor) {
    int randNum = rand() % 4;
    char direction;
    int color;
    
    do {
        color = rand() % 15 + 1; // Generate a random color between 1 and 15
    } while (color == prevColor); // Ensure color is not the same as previous one

    switch (randNum) {
    case 0: 
        direction = '<'; // Left
        break;
    case 1: 
        direction = '>'; // Right
        break;
    case 2: 
        direction = '^'; // Up
        break;
    case 3: 
        direction = 'v'; // Down
        break;
    default:
        direction = ' ';
    }
    return make_pair(direction, color);
}

// Function to set text color (Windows only)
void setTextColor(int color) {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#endif
}

// Function to reset text color (Windows only)
void resetTextColor() {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // Default color (white)
#endif
}

// Function to handle user input
char getUserInput() {
#ifdef _WIN32
    if (_kbhit()) {
        return _getch();
    }
    else {
        return '\0';
    }
#else
    return getch();
#endif
}

// Function to handle game over scenarios and losing a life
void handleGameOver(bool& gameRunning, int& score, int& lives, bool lookedInDirection, bool invalidInput, bool sameDirection) {
    if (lives == 0) {
        cout << "Game over! You looked in the direction of the arrow or ran out of lives.\n";
        gameRunning = false;
    }
    else {
        if (invalidInput) {
            cout << "Oops! You entered a character that is not an arrow. You lost one life.\n";
        }
        else if (sameDirection) {
            cout << "Oops! You looked in the same direction as the arrow. You lost one life.\n";
        }
        else {
            cout << "Oops! You didn't press any key in time. You lost one life.\n";
        }
        --lives;
        cout << endl;
        cout << "Press enter to keep going...\n";
        cin.get();
    }
}

// Function to clear the screen (platform-dependent) Without this, garbage characters could be seen
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to simulate the game loop
void gameLoop(float delay, int& score, bool& increasedDifficulty, int& lives) {
    bool gameRunning = true;

    srand(static_cast<unsigned int>(time(nullptr)));

    // Initialize the medium difficulty message flag outside the loop
    bool mediumDifficultyMessageShown = false;
    int prevColor = -1; // Initialize prevColor to an invalid value

    while (gameRunning && lives > 0) {
        // Clear the screen (platform-dependent)
        clearScreen();

        // Draw the game screen
        cout << "Look Away Game\n";
        cout << "Score: " << score << "\n";
        cout << "Lives: " << lives << endl;

        pair<char, int> arrow = generateDirectionWithColor(prevColor); //pair here gives a direction alongside a color to make said arrow to be
        char direction = arrow.first;
        int color = arrow.second;

        // Set text color
        setTextColor(color);
        cout << direction << endl;
        resetTextColor();

        // Update the previous color
        prevColor = color;

        // Sleep for a short duration to control game speed
        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(delay * 1000)));

        // Check if player has pressed a valid key
        char keyPressed = getUserInput();
        
        if (keyPressed != '\0' && (keyPressed == 'a' || keyPressed == 'w' || keyPressed == 's' || keyPressed == 'd')) {
            bool correctDirection = false;
            switch (direction) {
            case '<':
                correctDirection = (keyPressed == 'a');
                break;
            case '>':
                correctDirection = (keyPressed == 'd');
                break;
            case '^':
                correctDirection = (keyPressed == 'w');
                break;
            case 'v':
                correctDirection = (keyPressed == 's');
                break;
            }
            if (correctDirection) {
                handleGameOver(gameRunning, score, lives, false, false, true);
            }
            else {
                ++score;
            }
        }
        else if (keyPressed == '\0') { // No key pressed
            handleGameOver(gameRunning, score, lives, false, false, false); // No input given, timer ran out
        }
        else { // Invalid key pressed
            handleGameOver(gameRunning, score, lives, false, true, false);
        }

        // Inside the game loop
        if (score == 6 && !mediumDifficultyMessageShown) {
            mediumDifficultyMessageShown = true;
            cout << endl;
            cout << "You have entered medium difficulty mode." << endl;
            cout << "The arrows will now appear faster." << endl;
            cout << endl;
            cout << "Press Enter to start..." << endl;
            cin.get(); // Wait for user input
            delay = MEDIUM_DELAY;
        }
        else if (score == 11 && !increasedDifficulty) {
            increasedDifficulty = true;
            cout << endl;
            cout << "Wow! You've reached hard difficulty mode." << endl;
            cout << "Prepare for even faster arrow displays!" << endl;
            cout << endl;
            cout << "Press Enter to start..." << endl;
            cin.get(); // Wait for user input
            delay = HARD_DELAY;
        }
    }

    // Display final score
    cout << "Final score: " << score << endl;
}

int main() {
    char playAgain;

    do {
        // Display game instructions
        displayInstructions();

        // Initialize score, difficulty flag, and lives
        int score = 0;
        bool increasedDifficulty = false;
        int lives = LIVES;

        // Start game loop with easy difficulty
        cout << "You are now starting in easy mode.\n";
        gameLoop(EASY_DELAY, score, increasedDifficulty, lives);

        // Ask user if they want to play again
        do {
            cout << "Do you want to play again? (y/n): \n";
            cin >> playAgain;
            cout << " " << endl;
            if (playAgain != 'y' && playAgain != 'Y' && playAgain != 'n' && playAgain != 'N') {
                cout << "Invalid input! Please enter 'y' or 'n'." << endl;
            }
        } while (playAgain != 'y' && playAgain != 'Y' && playAgain != 'n' && playAgain != 'N');

        cin.ignore(); // Ignore any remaining characters in input buffer

    } while (playAgain == 'y' || playAgain == 'Y');

    cout << "Thanks for playing!" << endl;

    return 0;
}
