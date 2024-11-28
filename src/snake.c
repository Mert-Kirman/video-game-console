#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

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

int main(){
    // Initialize snake to start at the middle of the grid
    Snake *snake = (Snake*)malloc(sizeof(Snake));
    snake->xCoordinate = 7;
    snake->yCoordinate = 7;
    snake->head = 1;
    snake->next = NULL;

    // Buffer to store input from the user
    char input;

    enableRawMode();

    // Game loop
    while(1){
        // Print current grid state
        printGrid(snake);
        printf("%d", rand());
        fflush(stdout);
        sleep(1);

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'w'){
                break;
            }
            else if(input == 's'){
                break;
            }
            else if(input == 'a'){
                break;
            }
            else if(input == 'd'){
                break;
            }
            else if(input == 'q'){
                break;
            }
        }
        

    }

    // Restore terminal settings
    disableRawMode();
    return 0;
}

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
