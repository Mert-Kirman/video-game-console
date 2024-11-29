#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

typedef struct Snake{
    int xCoordinate;
    int yCoordinate;
    int head;
    struct Snake *next;
} Snake;

Snake *snake;

void enableRawMode();
void disableRawMode();
int kbhit();
void handleSignal(int signal);
void printGrid(int baitX, int baitY);
Snake *createNode(int xCoordinate, int yCoordinate, int isHead);
void addNode();
void moveSnake(char direction);
void generateBait(int *baitX, int *baitY);
void checkEat(int *baitX, int *baitY);
void freeResources();

int main(){
    // Connect signals to the signal handler
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    // Initialize snake to start at the middle of the grid
    snake = createNode(7, 7, 1);
    addNode();

    // Initialize bait at a random location on the grid
    int baitX;
    int baitY;
    generateBait(&baitX, &baitY);

    // Direction the snake is headed currently
    char direction = 'a';

    // Variable to store input from the user
    char input;

    enableRawMode();

    // Game loop
    while(1){
        // Print current grid state
        printGrid(baitX, baitY);
        usleep(1.4E5);

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'w' || input == 's' || input == 'a' || input == 'd'){
                // Snake cannot move in the opposite direction
                if(!(direction == 'w' && input == 's') && !(direction == 's' && input == 'w') && !(direction == 'a' && input == 'd') && !(direction == 'd' && input == 'a')) {
                    direction = input;
                }
            }
            else if(input == 'q'){
                break;
            }
        }

        moveSnake(direction);
        checkEat(&baitX, &baitY);
    }

    // Release dynamically allocated memory
    freeResources();

    // Restore terminal settings
    disableRawMode();
    return 0;
}

// Function that free dynamic linked list
void freeResources(){
    Snake *current = snake;
    Snake *nextNode;

    while(current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

// Function that checks if the snake can eat the bait
void checkEat(int *baitX, int *baitY){
    // If snake's head is on bait, eat it and add a new node
    if(snake->xCoordinate == *baitX && snake->yCoordinate == *baitY){
        addNode();
        generateBait(baitX, baitY);
    }
}

// Function that generates a bait at a random point on the grid
void generateBait(int *baitX, int *baitY){
    srand(time(NULL));  // Set the seed for the random function

    while(1){
        int baitSnakeCollision = 0; // Value for checking if bait position collides with snake's position on the grid
        *baitX = rand() % 15;
        *baitY = rand() % 15;

        Snake *current = snake;
        while(current != NULL){
            if(current->xCoordinate == *baitX && current->yCoordinate == *baitY){
                baitSnakeCollision = 1;
                break;
            }

            current = current->next;
        }

        if(!baitSnakeCollision){
            break;
        }
    }
}

// Function that moves snake on the grid in the current direction
void moveSnake(char direction){
    int xChange = 0;
    int yChange = 0;

    if(direction == 'w'){
        xChange = -1;
    }
    else if(direction == 'a'){
        yChange = -1;
    }
    else if(direction == 's'){
        xChange = 1;
    }
    else if(direction == 'd'){
        yChange = 1;
    }

    Snake *current = snake;
    
    // Check if the next position for the snake's head is out of map borders
    int headTargetPositionX = current->xCoordinate + xChange;
    int headTargetPositionY = current->yCoordinate + yChange;
    if(headTargetPositionX < 0 || headTargetPositionX >= 15 || headTargetPositionY < 0 || headTargetPositionY >= 15 ){
        return;
    }
    
    // Check if the next position for the snake's head is a body part
    while(current != NULL){
        if(headTargetPositionX == current->xCoordinate && headTargetPositionY == current->yCoordinate){
            return;
        }

        current = current->next;
    }
    current = snake;

    int oldCoordinateX;
    int oldCoordinateY;

    // Move the snake's head
    oldCoordinateX = current->xCoordinate;
    oldCoordinateY = current->yCoordinate;

    current->xCoordinate += xChange;
    current->yCoordinate += yChange;
    current = current->next;

    // Move the snake's body
    while(current != NULL) {
        int tempX = current->xCoordinate;
        int tempY = current->yCoordinate;

        // Pass the earlier node's coordinates to current node
        current->xCoordinate = oldCoordinateX;
        current->yCoordinate = oldCoordinateY;

        // Save the current node's coordinate to pass it to the next node
        oldCoordinateX = tempX;
        oldCoordinateY = tempY;

        current = current->next;
    }
}

// Function that returns a new node
Snake *createNode(int xCoordinate, int yCoordinate, int isHead){
    Snake *newNode = (Snake*)malloc(sizeof(Snake));
    newNode->xCoordinate = xCoordinate;
    newNode->yCoordinate = yCoordinate;
    newNode->head = isHead;  // New node is always tail
    newNode->next = NULL;

    return newNode;
}

// Function that adds a node to the snake at the tail
void addNode(){
    Snake *current = snake;
    int tempX;
    int tempY;

    if(current->next == NULL){
        tempX = current->xCoordinate;
        tempY = current->yCoordinate;
    }

    while(current->next != NULL) {  // Reach to the last node
        tempX = current->xCoordinate;   // Store the last node's coordinates to give it to the new node
        tempY = current->yCoordinate;
        current = current->next;
    }

    current->next = createNode(tempX, tempY, 0);
}

// Function that renders the current game frame
void printGrid(int baitX, int baitY){
    // Clear the terminal
    system("clear");

    // Initialize new grid to all '.'
    char grid[15][15];
    for(int i=0; i < 15; i++){
        for(int j=0; j < 15; j++){
            grid[i][j] = '.';
        }
    }

    // Add bait to the grid
    grid[baitX][baitY] = 'X';

    // Add snake to the grid
    Snake *current = snake;
    while(current != NULL) {
        if(current->head) {
            grid[current->xCoordinate][current->yCoordinate] = 'O';
        }
        else {
            grid[current->xCoordinate][current->yCoordinate] = '#';
        }

        current = current->next;
    }

    // Print the final grid
    for(int i=0; i < 15; i++){
        for(int j=0; j < 15; j++){
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
