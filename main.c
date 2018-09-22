/*
 * main.c
 * Snake game on terminal
 *
 * Written by Zoltan Gere
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define BOARDSIZEX 20
#define BOARDSIZEY 14

typedef struct coord_t {
    int x;
    int y;
} coord;


int main(int argc, char **argv) {

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    printf ("Lines: %d\n", w.ws_row);
    printf ("Columns: %d\n", w.ws_col);

 


    struct termios cooked, raw;

    // Get the state of the tty
    (void) tcgetattr(0, &cooked);
    // Make a copy we can mess with
    (void) memcpy(&raw, &cooked, sizeof(struct termios));
    // Turn off echoing, linebuffering, and special-character processing,
    // but not the SIGINT or SIGQUIT keys.
    raw.c_lflag &= ~ (ICANON | ECHO);
    raw.c_cc[VMIN] = 0;                 // If no data availale READ returns 0.
    raw.c_cc[VTIME] = 0;
    // Ship the raw control blts
    (void) tcsetattr(0, TCSANOW, &raw);

    unsigned char c[3];
    int gameRun = 1;
    unsigned char upKey[3] = {27,91,65};
    unsigned char downKey[3] = {27,91,66};
    unsigned char leftKey[3] = {27,91,68};
    unsigned char rightKey[3] = {27,91,67};
    unsigned char escKey[3] = {27,0,0};

    int score = 0;
    unsigned char runningDirection = 'l';           // u, d, l, r
    unsigned char input = 'l';
    unsigned char board[BOARDSIZEX][BOARDSIZEY];
    coord snake[BOARDSIZEX * BOARDSIZEY];
    size_t head = 0;
    size_t tail = 0;

    coord apple;
    int appleCount = 0;
    int foundApplePosition = 0;

    size_t snakeCurrentLength = 1;
    size_t snakeSupposedLength = 4;

    clock_t previousTime = clock();
    clock_t currentTime = clock();

    int i, j;

    for (i = 0; i< BOARDSIZEY; i++) {
        for (j = 0; j < BOARDSIZEX; j++) {
            board[j][i] = ' ';
        }
    }

    for (i = 0; i< BOARDSIZEX * BOARDSIZEY; i++) {
        snake[i].x = 0;
        snake[i].y = 0;
    };
    snake[0].x = BOARDSIZEX / 2;
    snake[0].y = BOARDSIZEY / 2;

    srand (time(NULL));

    // main event loop
    while (gameRun) {
        // Read user input
        memset(c, 0, sizeof(c));        // Clear buffer

        if (read(0, &c, 3) != 0) {      // If input read
            //printf("You pressed: ");
            if (memcmp(c, upKey, 3) == 0) {
                //printf("up\n");
                input = 'u';
            } else if (memcmp(c, downKey, 3) == 0) {
                //printf("down\n");
                input = 'd';
            } else if (memcmp(c, leftKey, 3) == 0) {
                //printf("left\n");
                input = 'l';
            } else if (memcmp(c, rightKey, 3) == 0) {
                //printf("right\n");
                input = 'r';
            } else if (memcmp(c, escKey, 3) == 0) {
                gameRun = 0;
            }
            
        }

        // update snake direction according to input
        if ((input == 'u') || (input == 'd')) {
            if ((runningDirection == 'l') || (runningDirection == 'r')) {
                runningDirection = input;
            }
        } else if ((input == 'l') || (input == 'r')) {
            if ((runningDirection == 'u') || (runningDirection == 'd')) {
                runningDirection = input;
            }
        }

        // timed part, runs only 2x / second
        currentTime = clock();
        if ((currentTime - previousTime) > (CLOCKS_PER_SEC / 2) ) {
            previousTime = currentTime;

            // update snake position
            if (head < BOARDSIZEX * BOARDSIZEY - 1) {
                switch (runningDirection) {
                    case 'u':
                        snake[head+1].x = snake[head].x;
                        snake[head+1].y = snake[head].y-1;
                        head++;
                        break;
                    case 'd':
                        snake[head+1].x = snake[head].x;
                        snake[head+1].y = snake[head].y+1;
                        head++;
                        break;
                    case 'l':
                        snake[head+1].x = snake[head].x-1;
                        snake[head+1].y = snake[head].y;
                        head++;
                        break;
                    case 'r':
                        snake[head+1].x = snake[head].x+1;
                        snake[head+1].y = snake[head].y;
                        head++;
                        break;
                }
            }
            if (head == BOARDSIZEX * BOARDSIZEY - 1) {
                switch (runningDirection) {
                    case 'u':
                        snake[0].x = snake[head].x;
                        snake[0].y = snake[head].y-1;
                        head = 0;
                        break;
                    case 'd':
                        snake[0].x = snake[head].x;
                        snake[0].y = snake[head].y+1;
                        head = 0;
                        break;
                    case 'l':
                        snake[0].x = snake[head].x-1;
                        snake[0].y = snake[head].y;
                        head = 0;
                        break;
                    case 'r':
                        snake[0].x = snake[head].x+1;
                        snake[0].y = snake[head].y;
                        head = 0;
                        break;
                }
            }

            // update snake length
            snakeCurrentLength++;
            if (snakeCurrentLength > snakeSupposedLength) {
                tail++;
                if (tail == BOARDSIZEX * BOARDSIZEY) {
                    tail = 0;
                }
                snakeCurrentLength--;
            }
            
            // check snake collision
            if (board[snake[head].x][snake[head].y] == 'o') {
                // game over
                gameRun = 0;
            }
            if ((snake[head].x < 0) || (snake[head].x == BOARDSIZEX)) {
                // game over
                gameRun = 0;
            }
            if ((snake[head].y < 0) || (snake[head].y == BOARDSIZEY)) {
                // game over
                gameRun = 0;
            }
            if (board[snake[head].x][snake[head].y] == 'b') {
                // eate apple
                score += 10;
                appleCount = 0;
                snakeSupposedLength++;
            }

            // if no apple, find position for apple
            if (appleCount == 0) {
                foundApplePosition = 0;
                while (foundApplePosition == 0) {
                    apple.x = rand() % BOARDSIZEX;
                    apple.y = rand() % BOARDSIZEY;
                    foundApplePosition = 1;

                    if (head >= tail) {
                        for (i = tail; i <= head; i++) {
                            if ((snake[i].x == apple.x) && (snake[i].y == apple.y)) {
                                foundApplePosition = 0;
                            }
                        }
                    } else {
                        for (i = tail; i < BOARDSIZEX * BOARDSIZEY; i++) {
                            if ((snake[i].x == apple.x) && (snake[i].y == apple.y)) {
                                foundApplePosition = 0;
                            }
                        }
                        for (i = 0; i <= head; i++) {
                            if ((snake[i].x == apple.x) && (snake[i].y == apple.y)) {
                                foundApplePosition = 0;
                            }
                        }
                    }
                }
                appleCount = 1;
            }

            // clear board
            for (i = 0; i< BOARDSIZEY; i++) {
                for (j = 0; j < BOARDSIZEX; j++) {
                    board[j][i] = ' ';
                }
            }
            
            // put snake and apple on board
            if (appleCount == 1) {
                board[apple.x][apple.y] = 'b';
            }

            if (head >= tail) {
                for (i = tail; i <= head; i++) {
                    board[snake[i].x][snake[i].y] = 'o';
                }
            } else {
                for (i = tail; i < BOARDSIZEX * BOARDSIZEY; i++) {
                    board[snake[i].x][snake[i].y] = 'o';
                }
                for (i = 0; i <= head; i++) {
                    board[snake[i].x][snake[i].y] = 'o';
                }
            }

            // draw board
            printf ("    Your score: %d\n", score);
            for (j = 0; j < BOARDSIZEX + 2; j++) {
                printf ("H");
            }
            printf("\n");

            for (i = 0; i< BOARDSIZEY; i++) {
                printf ("H");
                
                    
                for (j = 0; j < BOARDSIZEX; j++) {
                    printf ("%c", board[j][i]);
                }
                
                
                printf ("H\n");
            }

            for (j = 0; j < BOARDSIZEX + 2; j++) {
                printf ("H");
            }
            printf("\n");

            printf ("\n    Use arrow keys to turn snake. Press ESC to quit game.\n");
            printf ("SnakeCurrentLength: %ld SnakeSupposedLength: %ld Head: %ld Tail: %ld Time: %ld Input %c\n", snakeCurrentLength, snakeSupposedLength, head, tail,  currentTime, input);
            for (i = BOARDSIZEY + 7; i< w.ws_row; i++) {
                printf("\n");
            }

            //(void) fflush(stdout);

        } // timed part ends

        //(void) fflush(stdout);
    } // main event loop ends

    printf("GAME OVER!\n");
    printf("Your score is: %d\n", score);

    // Restore the cooked state
    (void) tcsetattr(0, TCSANOW, &cooked);

    return 0;
} // main function ends

// End of main.c