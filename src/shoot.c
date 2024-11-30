#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

typedef struct Bullet{
    int xCoordinate;
    int yCoordinate;
    int velocityX;
    int velocityY;
    int id;
    struct Bullet *next;
} Bullet;

typedef struct Person{
    int xCoordinate;
    int yCoordinate;
    char direction;
    int id;
    struct Person *next;
} Person;

// Map size
const int ROW_SIZE = 20;
const int COL_SIZE = 30;

// Declare player
Person player;

// Store bullet structs in a dynamic linked list
Bullet *bullets;
int bulletIdCount;

// Store enemies in a dynamic linked list
Person *enemies;
int enemyIdCount;

void enableRawMode();
void disableRawMode();
int kbhit();
void handleSignal(int signal);
void freeResources();
Bullet *createBulletNode(int xCoordinate, int yCoordinate, int velocityX, int velocityY);
void addBulletNode(int xCoordinate, int yCoordinate, int velocityX, int velocityY);
void removeBullet(int id);
Person *createEnemyNode(int xCoordinate, int yCoordinate, char direction);
void addEnemyNode(int xCoordinate, int yCoordinate, char direction);
void removeEnemy(int id);
void shoot();
void movePlayer(char input);
void moveBullets(int *playerScore);
void generateEnemy();
void printGrid(int playerScore);

int main(){
    srand(time(NULL));  // Set the seed for the random function

    // Connect signals to the signal handler
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    // Initialize player at the middle of the grid
    player.xCoordinate = COL_SIZE / 2;
    player.yCoordinate = ROW_SIZE / 2;
    player.direction = 'a';

    // Track player score
    int playerScore = 0;

    // Keep track of time passed since game started (in terms of frame count)
    int timePassed = 0;

    // Variable to store input from the user
    char input;

    enableRawMode();

    // Game loop
    while(1){
        if(timePassed % 15 == 0){
            // Create an enemy at a random location on the grid every 10 frames
            generateEnemy();
        }

        // Print current grid state
        printGrid(playerScore);
        usleep(1.4E5);
        timePassed += 1;

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'w' || input == 'a' || input == 's' || input == 'd'){
                movePlayer(input);
            }
            else if(input == ' '){
                shoot();
                playerScore -= 5;   // Score decreases by 5 for every bullet fired
            }
            else if(input == 'q'){  // Exit game
                break;
            }
        }

        moveBullets(&playerScore);
    }

    // Release dynamically allocated memory
    freeResources();

    // Restore terminal settings
    disableRawMode();
    return 0;
}

// Function that returns a new bullet node
Bullet *createBulletNode(int xCoordinate, int yCoordinate, int velocityX, int velocityY){
    Bullet *newNode = (Bullet*)malloc(sizeof(Bullet));
    newNode->xCoordinate = xCoordinate;
    newNode->yCoordinate = yCoordinate;
    newNode->velocityX = velocityX;
    newNode->velocityY = velocityY;
    newNode->id = bulletIdCount;
    newNode->next = NULL;

    return newNode;
}

// Function that adds a node to the bullets linked list at the tail
void addBulletNode(int xCoordinate, int yCoordinate, int velocityX, int velocityY){
    Bullet *current = bullets;

    if(bullets == NULL) {   // No bullet has been created yet
        bullets = createBulletNode(xCoordinate, yCoordinate, velocityX, velocityY);
    }
    else {
        while(current->next != NULL) {  // Reach to the last node
            current = current->next;
        }

        current->next = createBulletNode(xCoordinate, yCoordinate, velocityX, velocityY);
    }
}

// Function that removes a node with a given id from the bullets linked list
void removeBullet(int id) {
    Bullet *currentBullet = bullets;
    Bullet *previousBullet;

    while(currentBullet != NULL) {
        if(currentBullet->id == id) {   // Remove this node
            if(currentBullet->id == bullets->id) {  // First node of the linked list is being deleted
                bullets = currentBullet->next;
            }
            else {
                previousBullet->next = currentBullet->next;
            }

            free(currentBullet);    // Free the removed node
            break;
        }

        previousBullet = currentBullet;
        currentBullet = currentBullet->next;
    }
}

// Function that returns a new enemy node
Person *createEnemyNode(int xCoordinate, int yCoordinate, char direction){
    Person *newNode = (Person*)malloc(sizeof(Person));
    newNode->xCoordinate = xCoordinate;
    newNode->yCoordinate = yCoordinate;
    newNode->direction = direction;
    newNode->id = enemyIdCount;
    newNode->next = NULL;

    return newNode;
}

// Function that adds a node to the enemies linked list at the tail
void addEnemyNode(int xCoordinate, int yCoordinate, char direction){
    Person *current = enemies;

    if(enemies == NULL) {
        enemies = createEnemyNode(xCoordinate, yCoordinate, direction);
    }
    else {
        while(current->next != NULL) {  // Reach to the last node
            current = current->next;
        }

        current->next = createEnemyNode(xCoordinate, yCoordinate, direction);
    }
}

// Function that removes a node with a given id from the enemies linked list
void removeEnemy(int id) {
    Person *currentEnemy = enemies;
    Person *previousEnemy;

    while(currentEnemy != NULL) {
        if(currentEnemy->id == id) {   // Remove this node
            if(currentEnemy->id == enemies->id) {  // First node of the linked list is being deleted
                enemies = currentEnemy->next;
            }
            else {
                previousEnemy->next = currentEnemy->next;
            }

            free(currentEnemy);    // Free the removed node
            break;
        }

        previousEnemy = currentEnemy;
        currentEnemy = currentEnemy->next;
    }
}

// Function that generates a new bullet in the direction fired
void shoot(){
    Bullet newBullet;
    if(player.direction == 'w') {
        newBullet.velocityX = 0;
        newBullet.velocityY = -1;
        newBullet.xCoordinate = player.xCoordinate;
        newBullet.yCoordinate = player.yCoordinate - 1;
    }
    else if(player.direction == 'a') {
        newBullet.velocityX = -1;
        newBullet.velocityY = 0;
        newBullet.xCoordinate = player.xCoordinate - 1;
        newBullet.yCoordinate = player.yCoordinate;
    }
    else if(player.direction == 's') {
        newBullet.velocityX = 0;
        newBullet.velocityY = 1;
        newBullet.xCoordinate = player.xCoordinate;
        newBullet.yCoordinate = player.yCoordinate + 1;
    }
    else if(player.direction == 'd') {
        newBullet.velocityX = 1;
        newBullet.velocityY = 0;
        newBullet.xCoordinate = player.xCoordinate + 1;
        newBullet.yCoordinate = player.yCoordinate;
    }

    // Store this new bullet in the linked list
    addBulletNode(newBullet.xCoordinate, newBullet.yCoordinate, newBullet.velocityX, newBullet.velocityY);
    bulletIdCount += 1;
}

// Function that moves the player given it does not exceed grid borders
void movePlayer(char input){
    if(input == 'a') {
        if(player.xCoordinate - 1 > 0){
            player.xCoordinate -= 1;
        }
    }
    else if(input == 'd') {
        if(player.xCoordinate + 1 < COL_SIZE - 1){
            player.xCoordinate += 1;
        }
    }
    else if(input == 'w') {
        if(player.yCoordinate - 1 > 0){
            player.yCoordinate -= 1;
        }
    }
    else if(input == 's') {
        if(player.yCoordinate + 1 < ROW_SIZE - 1){
            player.yCoordinate += 1;
        }
    }

    player.direction = input;
}

// Function that moves all the bullets according to their velocities and checks if bullets hit any enemy
void moveBullets(int *playerScore){
    int targetX;
    int targetY;

    Bullet *currentBullet = bullets;
    while(currentBullet != NULL) {
        int enemyHit = 0;

        // Calculate target position in the next frame
        targetX = currentBullet->xCoordinate + currentBullet->velocityX;
        targetY = currentBullet->yCoordinate + currentBullet->velocityY;

        // Check if target location exceeds grid borders, if so remove the bullet
        if(targetX < 0 || targetX >= COL_SIZE || targetY < 0 || targetY >= ROW_SIZE){
            removeBullet(currentBullet->id);
        }

        // Check if the bullet hits any enemy
        Person *currentEnemy = enemies;
        while(currentEnemy != NULL) {
            if(targetX == currentEnemy->xCoordinate && targetY == currentEnemy->yCoordinate){
                enemyHit = 1;
                Bullet *tmp = currentBullet->next;
                removeBullet(currentBullet->id);
                removeEnemy(currentEnemy->id);
                currentBullet = tmp;
                *playerScore += 10;
                break;
            }

            currentEnemy = currentEnemy->next;
        }

        if(!enemyHit) { // If the bullet did not hit any enemy, update its location
            currentBullet->xCoordinate = targetX;
            currentBullet->yCoordinate = targetY;
            currentBullet = currentBullet->next;
        }
    }
}

// Function that generates an enemy at a random point on the grid
void generateEnemy(){
    while(1){
        int enemyCollision = 0; // Value for checking if new enemy position collides with any existing enemies or the player on the grid
        int newEnemyX = rand() % COL_SIZE;
        int newEnemyY = rand() % ROW_SIZE;

        // New enemy should not collide with an existing enemy
        Person *currentEnemy = enemies;
        while(currentEnemy != NULL){
            if(currentEnemy->xCoordinate == newEnemyX && currentEnemy->yCoordinate == newEnemyY){
                enemyCollision = 1;
                break;
            }

            currentEnemy = currentEnemy->next;
        }

        // New enemy should not collide with an existing bullet
        Bullet *currentBullet = bullets;
        while(currentBullet != NULL) {
            if(currentBullet->xCoordinate == newEnemyX && currentBullet->yCoordinate == newEnemyY) {
                enemyCollision = 1;
                break;
            }

            currentBullet = currentBullet->next;
        }

        int enemyPlayerDistance = (newEnemyX - player.xCoordinate) * (newEnemyX - player.xCoordinate) + (newEnemyY - player.yCoordinate) * (newEnemyY - player.yCoordinate);
        if(enemyPlayerDistance < 4){   // New enemy cannot be closer to player less than 2 units
            enemyCollision = 1;
        }

        if(!enemyCollision){ // If there are no collisions, create this new enemy
            addEnemyNode(newEnemyX, newEnemyY, 'a');
            enemyIdCount += 1;
            break;
        }
    }
}

// Function that free enemies and bullets linked lists
void freeResources(){
    // Free bullets
    Bullet *currentBullet = bullets;
    Bullet *nextBullet;

    while(currentBullet != NULL) {
        nextBullet = currentBullet->next;
        free(currentBullet);
        currentBullet = nextBullet;
    }
    
    // Free enemies
    Person *currentEnemy = enemies;
    Person *nextEnemy;

    while(currentEnemy != NULL) {
        nextEnemy = currentEnemy->next;
        free(currentEnemy);
        currentEnemy = nextEnemy;
    }
}

// Function that renders the current game frame
void printGrid(int playerScore){
    // Clear the terminal
    system("clear");

    // Initialize new grid to all '.'
    char grid[ROW_SIZE][COL_SIZE];
    for(int i=0; i < ROW_SIZE; i++){
        for(int j=0; j < COL_SIZE; j++){
            grid[i][j] = '.';
        }
    }

    // Add the existing bullets to the grid
    Bullet *currentBullet = bullets;
    while(currentBullet != NULL) {
        grid[currentBullet->yCoordinate][currentBullet->xCoordinate] = '*';
        currentBullet = currentBullet->next;
    }
    
    // Add the existing enemies to the grid
    Person *currentEnemy = enemies;
    while(currentEnemy != NULL) {
        grid[currentEnemy->yCoordinate][currentEnemy->xCoordinate] = 'X';
        currentEnemy = currentEnemy->next;
    }

    // Add the player to the grid
    grid[player.yCoordinate][player.xCoordinate] = 'O';
    if(player.direction == 'w') {
        grid[player.yCoordinate - 1][player.xCoordinate] = '|';
    }
    else if(player.direction == 'a') {
        grid[player.yCoordinate][player.xCoordinate - 1] = '-';
    }
    else if(player.direction == 's') {
        grid[player.yCoordinate + 1][player.xCoordinate] = '|';
    }
    else if(player.direction == 'd') {
        grid[player.yCoordinate][player.xCoordinate + 1] = '-';
    }

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
