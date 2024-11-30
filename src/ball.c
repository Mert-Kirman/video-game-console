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
Ball *balls;
int ballsArrayCapacity = 1;

void enableRawMode();
void disableRawMode();
int kbhit();
void handleSignal(int signal);
void printGrid(int playerBarX, int playerScore);
void freeResources();
void generateBall();
void movePlayerBar(char input, int *playerBarX);
void moveBalls(int playerBarX, int *playerScore);

int main(){
    // Connect signals to the signal handler
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    balls = (Ball*)malloc((currentBallCount + 1) * sizeof(Ball));

    // Initialize bar at the middle of the grid
    int playerBarX = COL_SIZE / 2;

    // Track player score
    int playerScore = 0;

    // Variable to store input from the user
    char input;

    enableRawMode();

    // Game loop
    while(1){
        if(playerScore >= 50 * currentBallCount && currentBallCount <= MAX_BALL_COUNT){
            // Create a ball at a random location on the grid
            generateBall();
        }

        // Print current grid state
        printGrid(playerBarX, playerScore);
        usleep(1.4E5);

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'a' || input == 'd'){
                movePlayerBar(input, &playerBarX);
            }
            else if(input == 'q'){
                break;
            }
        }

        moveBalls(playerBarX, &playerScore);
    }

    // Release dynamically allocated memory
    freeResources();

    // Restore terminal settings
    disableRawMode();
    return 0;
}

// Function that moves the player bar given it does not exceed grid borders
void movePlayerBar(char input, int *playerBarX){
    if(input == 'a') {
        if(*playerBarX - 1 > 0){
            *playerBarX -= 1;
        }
    }
    else if(input == 'd') {
        if(*playerBarX + 1 < COL_SIZE - 1){
            *playerBarX += 1;
        }
    }
}

void moveBalls(int playerBarX, int *playerScore){
    int targetX;
    int targetY;
    Ball current;
    for(int i = 0; i < currentBallCount; i++) {
        current = balls[i];

        // Calculate target position in the next frame
        targetX = current.xCoordinate + current.velocityX;
        targetY = current.yCoordinate + current.velocityY;

        // Check if target location collide with grid borders, if so bounce them
        if(targetX < 0 || targetX >= COL_SIZE){    // Ball hits left or right border
            current.velocityX = current.velocityX * -1;
            targetX += 2 * current.velocityX;
        }

        if(targetY < 0 || targetY >= ROW_SIZE){    // Ball hits ceiling or floor
            current.velocityY = current.velocityY * -1;
            targetY += 2 * current.velocityY;
        }

        // Check if the ball hits the player bar
        if(targetY == ROW_SIZE -1 && (targetX == playerBarX - 1 || targetX == playerBarX || targetX == playerBarX + 1)){
            current.velocityY = current.velocityY * -1;
            targetY += 2 * current.velocityY;
            *playerScore += 10;
        }

        current.xCoordinate = targetX;
        current.yCoordinate = targetY;
        balls[i] = current;
    }
}

// Function that generates a ball at a random point on the grid
void generateBall(){
    srand(time(NULL));  // Set the seed for the random function

    while(1){
        int ballBallCollision = 0; // Value for checking if ball position collides with any existing balls on the grid
        int newBallX = rand() % COL_SIZE;

        Ball current;
        for(int i = 0; i < currentBallCount; i++){
            current = balls[i];
            if (current.yCoordinate == 3 && current.xCoordinate == newBallX){
                ballBallCollision = 1;
                break;
            }
        }

        if(!ballBallCollision){ // If there are no collisions between balls, create this new ball
            if(ballsArrayCapacity <= currentBallCount){ // If dynamic array is full, resize it
                ballsArrayCapacity *= 2;
                balls = realloc(balls, ballsArrayCapacity * sizeof(Ball));
            }
            current.velocityX = 1;
            current.velocityY = 1;
            current.xCoordinate = newBallX;
            current.yCoordinate = 3;
            balls[currentBallCount] = current;
            currentBallCount += 1;
            break;
        }
    }
}

// Function that free ball structs
void freeResources(){
    free(balls);
}

// Function that renders the current game frame
void printGrid(int playerBarX, int playerScore){
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
    Ball current;
    for(int i = 0; i < currentBallCount; i++) {
        current = balls[i];
        grid[current.yCoordinate][current.xCoordinate] = 'O';
    }

    // Add player bar to the grid
    grid[ROW_SIZE-1][playerBarX - 1] = '=';
    grid[ROW_SIZE-1][playerBarX] = '=';
    grid[ROW_SIZE-1][playerBarX + 1] = '=';

    // Print the final grid
    printf("Player Score: %d\n", playerScore);
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
