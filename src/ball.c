#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

typedef struct Ball{
    int xCoordinate;
    int yCoordinate;
    int velocityX;
    int velocityY;
} Ball;

const int ROW_SIZE = 20;
const int COL_SIZE = 30;
const int MAX_VELOCITY = 3;
const int MAX_BALL_COUNT = 4;

// Store ball structs in the array
int currentBallCount = 0;
Ball *balls[MAX_BALL_COUNT];

void enableRawMode();
void disableRawMode();
int kbhit();
void handleSignal(int signal);
void printGrid(int playerBarY);
void freeResources();
void generateBall();
void movePlayerBar(char input, int *playerBarY);
void moveBalls();

int main(){
    // Connect signals to the signal handler
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    // Initialize bar at the middle of the grid
    int playerBarY = COL_SIZE / 2;

    // Create a ball at a random location on the grid
    generateBall();

    // Variable to store input from the user
    char input;

    enableRawMode();

    // Game loop
    while(1){
        // Print current grid state
        printGrid(playerBarY);
        usleep(1.4E5);

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'a' || input == 'd'){
                movePlayerBar(input, &playerBarY);
            }
            else if(input == 'q'){
                break;
            }
        }

        // moveBalls();
    }

    // Release dynamically allocated memory
    freeResources();

    // Restore terminal settings
    disableRawMode();
    return 0;
}

// Function that moves the player bar given it does not exceed grid borders
void movePlayerBar(char input, int *playerBarY){
    if(input == 'a') {
        if(*playerBarY - 1 > 0){
            *playerBarY -= 1;
        }
    }
    else if(input == 'd') {
        if(*playerBarY + 1 < COL_SIZE - 1){
            *playerBarY += 1;
        }
    }
}

void moveBalls(){}

// Function that generates a ball at a random point on the grid
void generateBall(){
    srand(time(NULL));  // Set the seed for the random function

    while(1){
        int ballBallCollision = 0; // Value for checking if ball position collides with any existing balls on the grid
        int newBallY = rand() % COL_SIZE;

        Ball *current;
        for(int i = 0; i < currentBallCount; i++){
            current = balls[i];
            if (current->xCoordinate == 12 && current->yCoordinate == newBallY){
                ballBallCollision = 1;
            }
        }

        if(!ballBallCollision){ // If there are no collisions between balls, create this new ball
            current = (Ball*)malloc(sizeof(Ball));
            current->velocityX = 1;
            current->velocityY = 1;
            current->xCoordinate = 3;
            current->yCoordinate = newBallY;
            balls[currentBallCount] = current;
            currentBallCount += 1;
            break;
        }
    }
}

// Function that free ball structs
void freeResources(){
    Ball *current;
    for(int i = 0; i < currentBallCount; i++){
        current = balls[i];
        free(current);
    }
}

// Function that renders the current game frame
void printGrid(int playerBarY){
    // Clear the terminal
    system("clear");

    // Initialize new grid to all '.'
    char grid[ROW_SIZE][COL_SIZE];
    for(int i=0; i < ROW_SIZE; i++){
        for(int j=0; j < COL_SIZE; j++){
            grid[i][j] = '.';
        }
    }

    // Add the existing balls to the grid
    Ball *current;
    for(int i = 0; i < currentBallCount; i++) {
        current = balls[i];
        grid[current->xCoordinate][current->yCoordinate] = 'O';
    }

    // Add player bar to the grid
    grid[ROW_SIZE-1][playerBarY - 1] = '=';
    grid[ROW_SIZE-1][playerBarY] = '=';
    grid[ROW_SIZE-1][playerBarY + 1] = '=';

    // Print the final grid
    for(int i=0; i < ROW_SIZE; i++){
        for(int j=0; j < COL_SIZE; j++){
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }

    fflush(stdout);
}

void enableRawMode() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);

    tty.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void disableRawMode() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);

    tty.c_lflag |= (ICANON | ECHO); // Re-enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

// Function that checks for non-blocking keyboard press
int kbhit() {
    struct timeval timeout = {0, 0}; // Zero timeout for non-blocking check
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    return select(1, &readfds, NULL, NULL, &timeout);
}

// Function that cleans up and exits gracefully
void handleSignal(int signal) {
    freeResources();    // Free memory
    disableRawMode();   // Reset terminal settings
    exit(0); // Exit the game
}
