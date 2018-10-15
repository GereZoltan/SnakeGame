/*! \file main.c
 * \brief Snake game on terminal
 *
 * Simple snake implemented on linux terminal
 * Includes a top list record in file for competition
 *
 * Written by Zoltan Gere
 * 1706228
 * 02.10.2018
 */

//#define DEBUG

#include <stdio.h>
#include <stdlib.h> // for random number
#include <string.h> // memory operations
#include <sys/ioctl.h> // for ioctl
#include <unistd.h> // for ioctl and read
#include <termios.h> // for reading and writing terminal parameters
#include <time.h> // for timing functions
#include "snake.h"

/*! \struct entry
 *  \brief Contains 1 entry for top list
 *
 * \var char name[40] Name of the player
 * \var int score The score the player has achieved
 */
struct entry {
    char name[40];
    int score;
};

/*! \fn void sortToplist(struct entry *toplist, size_t num)
 * \brief Sort the top list entries on scores descending order
 *
 * Receive a pointer to the top list array and the number of elements.
 * Sort the array by score values descending order, highest score is first entry
 *
 * \param toplist Pointer to array with entries
 * \param num Number of entries in array
 * \return void No values returned
 */
void sortToplist(struct entry *toplist, size_t num);

void sortToplist(struct entry *toplist, size_t num) {
    /*! \var size_t i, j
     *  \brief Loop index variables
     */
    size_t i, j;
    /*! \var size_t largest
     *  \brief Index of the largest entry
     */
    size_t largest;
    /*! \var struct entry temp
     *  \brief Helper for swapping 2 entries
     */
    struct entry temp;

    for (i = 0; i < num-1; i++) {               // Loop through sorted section
        largest = i;                            // Suppose sorted section member is in position
        for (j = i+1; j < num; j++) {           // Loop through unsorted section
            if ((toplist+j)->score > (toplist+largest)->score) { // If member in unsorted section larger
                largest = j;                    // Remember its index
            }
        }
        if (largest != i) {                     // If largest entry is not in sorted section...
            memcpy(temp.name, (toplist+i)->name, sizeof((toplist+i)->name));    // ...swap it with the entry...
            temp.score = (toplist+i)->score;                                    // ...in unsorted section.
            memcpy((toplist+i)->name, (toplist+largest)->name, sizeof((toplist+largest)->name)); // Both the name and the score...
            (toplist+i)->score = (toplist+largest)->score;                      // ...must be swapped.
            memcpy((toplist+largest)->name, temp.name, sizeof(temp.name));
            (toplist+largest)->score = temp.score;
        }
    }
    /*
    printf("    Toplist after sort\n");                                         // DEBUG: Print the toplist
        for (i = 0; i < 10; i++) {
            printf("%4ld %30s %6d\n", i+1, (toplist+i)->name, (toplist+i)->score);
        }
        */
}

/*! \fn main(int argc, char **argv)
 * \brief The main entry point of the program
 *
 * Declares and initialize all used variables
 * Reads terminal window size
 * Configures terminal environment for interactive use, disables waiting for keyboard entry
 * Starts the main event loop
 * Runs the snake game
 * When main loop ended, processses the top list file
 * Add player name and score if player earned sufficient point to enter Top 10
 * Finally present the toplist
 *
 * \param argc Number of parameters
 * \param argv Pointer pointing to array of parameters
 * \return int Exit code
 */
int main(int argc, char **argv) {

    /*! \var clock_t previousTime
     *  \brief Time of the beginning of interval
     */
    clock_t previousTime = clock();
    /*! \var clock_t currentTime
     *  \brief Time of the end of interval
     */
    clock_t currentTime;
    /*! \var size_t i
     *  \brief Loop index variable
     */
    size_t i;
    /*! \var unsigned char input
     *  \brief Input character after processing
     */
    unsigned char input;
    /*! \var int gameRun
     *  \brief True while the game runs, when player lose it is set to false
     */
    int gameRun = 1;
    /*! \var int score
     *  \brief Score achieved by player
     */
    int score = 0;

#ifdef DEBUG
    score = 0;
#endif

    /*! \var unsigned char board
     *  \brief The game board
     *
     *  Snake is rendered into the board
     */
    unsigned char board[BOARDSIZEX][BOARDSIZEY];
    /*! \var snake player
     *  \brief The players snake
     */
    snake player;
    /*! \var int appleCount
     *  \brief Number of apples currently on board
     *
     *  Count is always keept at 1
     */
    int appleCount = 0;
    /*! \var coord apple
     *  \brief Coordinate of the apple
     */
    coord apple;
    /*! \var FILE * datafile
     *  \brief Pointer to file containing the Top 10 list
     */
    FILE * datafile;
    /*! \var char name[40]
     *  \brief Players name input
     */
    char name[40];
    /*! \var struct entry toplist[10]
     *  \brief Top 10 list read from file / written to file
     */
    struct entry toplist[10];
    /*! \var struct termios cooked, raw
     *  \brief Contains the state of terminal
     *
     *  cooked is the original condition, restored after the game
     *  raw is the altered condition suitable for game
     */
    struct termios cooked, raw;
    /*! \var struct winsize w
     *  \brief Struct containing the terminal window size
     *
     *  The actual window size is required for proper rendering
     */
    struct winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);       // Query the terminal window size

#ifdef DEBUG
    printf ("Lines: %d\n", w.ws_row);
    printf ("Columns: %d\n", w.ws_col);
#endif

    (void) tcgetattr(0, &cooked);               // Get the state of the terminal
    (void) memcpy(&raw, &cooked, sizeof(struct termios));       // Make a copy of it
    raw.c_lflag &= ~ (ICANON | ECHO);           // Turn off echoing, linebuffering, and special-character processing
    raw.c_cc[VMIN] = 0;                         // No waiting time for input, so program is not stopped
    raw.c_cc[VTIME] = 0;                        // If no data availale READ returns 0
    (void) tcsetattr(0, TCSANOW, &raw);         // Write back the changed state


    clearBoard(board, BOARDSIZEX, BOARDSIZEY);  // Clear game board

    for (i = 0; i< BOARDSIZEX * BOARDSIZEY; i++) {      // Clear snake data.
        player.position[i].x = 0;
        player.position[i].y = 0;
    };
    player.head = 0;
    player.tail = 0;
    player.position[player.head].x = BOARDSIZEX / 2;    // Positon snake at the center of the board
    player.position[player.head].y = BOARDSIZEY / 2;

    player.snakeCurrentLength = 1;                      // Initial snake length
    player.snakeSupposedLength = 4;                     // The snake "flows in"
    player.runningDirection = 'l';                      // Snake runs to the left

    srand (time(NULL));                                 // Init random number generator

    while (gameRun) {                                   // main event loop starts here
        input = readInput();                            // Read user input
        if (input == 27) {                              // Game stops when ESC key pressed
                gameRun = 0;
        }

#ifdef DEBUG
        if (input == ' ') {                             // In debug mode it is possible to pause the game
            do {
                input = readInput();
            } while (input != ' ');
            input = readInput();
        }
#endif

        updateSnakeDirection(&player, input);           // update snake direction according to input

        currentTime = clock();
        if ((currentTime - previousTime) > (CLOCKS_PER_SEC / 6) ) {     // timed part, runs only n times / second
            previousTime = currentTime;

            updateSnake(&player);                       // update snake position and length

            // check snake collision
            if (board[player.position[player.head].x][player.position[player.head].y] == 'o') {     // Snake hits itself
                // game over
                gameRun = 0;
            }
            if ((player.position[player.head].x < 0) || (player.position[player.head].x == BOARDSIZEX)) {   // Snake hits vetical wall
                // game over
                gameRun = 0;
            }
            if ((player.position[player.head].y < 0) || (player.position[player.head].y == BOARDSIZEY)) {   // Snake hits hoizontal wall
                // game over
                gameRun = 0;
            }
            if (board[player.position[player.head].x][player.position[player.head].y] == 'b') {             // Snake eats apple
                // ate apple
                score += 10;
                appleCount = 0;
                player.snakeSupposedLength++;
            }

            if (appleCount == 0) {                                  // if no apple, find position for apple
                appleCount = placeApple(&apple, &player);
                if (appleCount == 0) {                              // no more apple possible...
                    gameRun = 0;                                    // ..so the game ends
                }
            }

            updateBoard(board, &apple, &player, appleCount);        // update board

            drawScreen(board, score, w.ws_row);                     // draw board

#ifdef DEBUG
            // DEBUG: print some variable values
            printf ("SnakeCurrentLength: %ld SnakeSupposedLength: %ld Head: %ld Tail: %ld Time: %ld Input %c", player.snakeCurrentLength, player.snakeSupposedLength, player.head, player.tail,  currentTime, input);
#endif
            printf("\n");
        } // timed part ends

    } // main event loop ends

    (void) tcsetattr(0, TCSANOW, &cooked);                          // Restore the original cooked state of terminal

    printf("\n\nGAME OVER!\n\n");
    printf("Your final score is: %d\n", score);

    for (i = 0; i < 10; i++) {                                      // Empty Top 10 list
        memset(toplist[i].name, 0, sizeof(toplist[i].name));
        toplist[i].score = 0;
    }

    datafile = fopen("snakegame.dat", "ab+");                       // Open data file, if it is not exist, it is created
    if (datafile != NULL) {
        rewind(datafile);                                           // Go to the beginning

        fread(toplist, sizeof(toplist[0]), 10, datafile);           // Read in the top list
        fclose(datafile);                                           // ...and close

        if (toplist[9].score < score) {                             // Player earned a place on top list
            printf("You earned a place on the top list! Enter your name: ");
            fgets(name, sizeof(name)-1, stdin);                     // Ask for players name
            name[strlen(name)-1] = '\0';
            printf("\n");

            memcpy(toplist[9].name, name, sizeof(name));            // Put player at the last poition
            toplist[9].score = score;

            sortToplist(toplist, 10);                               // Sort the list

            datafile = fopen("snakegame.dat", "wb");                // Write the top list back to file
            if (datafile != NULL) {
                fwrite(toplist, sizeof(toplist[0]), 10, datafile);
                fclose(datafile);
            }
        }

        printf("    Hall of fame\n");                               // print top list
        for (i = 0; i < 10; i++) {
            printf("%4ld %30s %6d\n", i+1, toplist[i].name, toplist[i].score);
        }
        printf("\n");
    }

    return 0;
} // main function ends

// End of main.c
