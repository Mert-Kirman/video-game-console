#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

typedef struct Snake{
    int xCoordinate;
    int yCoordinate;
    int head;
    struct Snake *next;
} Snake;

void enableRawMode();
void disableRawMode();
int kbhit();
void printGrid(Snake *snake);
Snake *createNode(int xCoordinate, int yCoordinate, int isHead);
void addNode(Snake *snake, int xCoordinate, int yCoordinate);
void moveSnake(Snake *snake, char direction);
void generateBait(Snake *snake, int *baitX, int *baitY);

int main(){
    // Initialize snake to start at the middle of the grid
    Snake *snake = createNode(7, 7, 1);

    // Initialize bait at a random location on the grid
    int baitX;
    int baitY;
    generateBait(snake, &baitX, &baitY);

    // Direction the snake is headed currently
    char direction = 'a';

    // Buffer to store input from the user
    char input;

    enableRawMode();

    // Game loop
    while(1){
        // Print current grid state
        printGrid(snake);
        fflush(stdout);
        usleep(1.5E5);

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'w' || input == 's' || input == 'a' || input == 'd'){
                direction = input;
            }
            else if(input == 'q'){
                break;
            }
        }

        moveSnake(snake, direction);
    }

    // Restore terminal settings
    disableRawMode();
    return 0;
}

// Function that generates a bait at a random point on the grid
void generateBait(Snake *snake, int *baitX, int *baitY){
    srand(time(NULL));  // Set the seed for the random function

    while(1){
        int baitSnakeCollision = 0; // Value for checking if bait position collides with snake's position on the grid
        *baitX = rand() % 15;
        *baitY = rand() % 15;

        Snake *current = snake;
        while(current != NULL){
            if(current->xCoordinate == *baitX || current->yCoordinate == *baitY){
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
void moveSnake(Snake *snake, char direction){
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
    while(current != NULL) {
        current->xCoordinate += xChange;
        current->yCoordinate += yChange;

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
void addNode(Snake *snake, int xCoordinate, int yCoordinate){
    Snake *current = snake;
    while(current->next != NULL) {  // Reach to the last node
        current = current->next;
    }

    current->next = createNode(xCoordinate, yCoordinate, 0);
}

// Function that renders the current game frame
void printGrid(Snake *snake){
    // Clear the terminal
    system("clear");

    // Initialize new grid to all '.'
    char grid[15][15];
    for(int i=0; i < 15; i++){
        for(int j=0; j < 15; j++){
            grid[i][j] = '.';
        }
    }

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

int kbhit() {
    struct timeval timeout = {0, 0}; // Zero timeout for non-blocking check
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    return select(1, &readfds, NULL, NULL, &timeout);
}
