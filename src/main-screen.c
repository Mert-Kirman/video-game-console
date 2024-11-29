#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void enableRawMode();
void disableRawMode();
int kbhit();
void handleSignal(int signal);
void printMainMenu(char *mainMenu, char availableOptions[3][12]);

int gameChosen = 0;
int optionChosen = 0;
char *mainMenu = 
" #####   ##   ##      ##   ##        ######  #####     ##  ##            #####       ##   ##   ##   ######   ######  \n"
"#######  ##   ##    #####  ##        ######  ######    ##  ##           #######    #####  ### ###  #######  #######  \n"
"##   ##  ##   ##    ## ##  ##          ##        ##    ##  ##            #         ## ##  #######  ##       ##       \n"
"##   ##  ##   ##   ##  ##  ##          ##        ##     ####            ##  ###   ##  ##  #######  #######   #####   \n"
"##   ##  ##   ##   ######  ##          ##        ##      ##             ##   ##   ######  ## # ##  ##            ##  \n"
"#######  #######  ##   ##  #######   ######      ##      ##             #######  ##   ##  ##   ##  #######  #######  \n"
" #### ##  #####   ##   ##   ######   ######      ##      ##              # ###   ##   ##  ##   ##   ######  ######   \n\n\n";

int main(){
    // Connect signals to the signal handler
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    // Store available games
    char availableGames[3][12] = {"snake", "ball", "dino"};

    enableRawMode();

    // Main menu loop
    while(1){
        printMainMenu(mainMenu, availableGames);
        usleep(1E5);

        // Variable to store input from the user
        char input;

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'a'){
                optionChosen = optionChosen - 1;
                if(optionChosen < 0){
                    optionChosen += 3;
                }
                optionChosen = optionChosen % 3;
            }
            else if(input == 'd'){
                optionChosen = (optionChosen + 1) % 3;
            }
            else if(input == 'w'){
                if(optionChosen == 1) {
                    gameChosen = gameChosen - 1;
                    if(gameChosen < 0){
                        gameChosen += 3;
                    }
                    gameChosen = gameChosen % 3;
                }
            }
            else if(input == 's'){
                if(optionChosen == 1) {
                    gameChosen = (gameChosen + 1) % 3;
                }
            }
            else if(input == '\n'){
                if(optionChosen == 0){
                    // Prepare the command that will run the chosen game when player presses enter button
                    char command[16] = "./game_";
                    strcat(command, availableGames[gameChosen]);

                    disableRawMode();
                    system(command);
                    enableRawMode();
                }
                else if(optionChosen == 2){
                    break;
                }
            }
            else if(input == 'q'){
                break;
            }
        }
    }

    system("clear");    // Clear the terminal
    disableRawMode();   // Restore terminal settings
    return 0;
}

void printMainMenu(char *mainMenu, char availableGames[3][12]){
    // Clear the terminal
    system("clear");
    printf("%s", mainMenu);
    
    if(optionChosen == 0){
        printf("%30s %28s %29s\n", "[start]", availableGames[gameChosen], "exit");
    }
    else if(optionChosen == 1){
        char gameDisplayed[16] = "[(";
        strcat(gameDisplayed, availableGames[gameChosen]);
        strcat(gameDisplayed, ")]");
        printf("%29s %31s %27s\n", "start", gameDisplayed, "exit");
    }
    else if(optionChosen == 2){
        printf("%29s %29s %30s\n", "start", availableGames[gameChosen], "[exit]");
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

// Function that cleans up and exits gracefully
void handleSignal(int signal) {
    system("clear");    // Clear the terminal
    disableRawMode();   // Reset terminal settings
    exit(0);    // Exit the console
}

