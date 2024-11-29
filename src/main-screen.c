#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

void enableRawMode();
void disableRawMode();
int kbhit();
void handleSignal(int signal);
void printMainMenu(char *mainMenu);

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

    enableRawMode();

    // Main menu loop
    while(1){
        printMainMenu(mainMenu);
        usleep(1.5E5);

        // Variable to store input from the user
        char input;

        if(kbhit()){
            // Take input from player if any
            input = getchar();

            if(input == 'a'){
                break;
            }
            else if(input == 'd'){
                break;
            }
            else if(input == 'w'){
                break;
            }
            else if(input == 's'){
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

void printMainMenu(char *mainMenu){
    // Clear the terminal
    system("clear");
    printf("%s", mainMenu);
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
    disableRawMode();   // Reset terminal settings
    exit(0); // Exit the game
}

