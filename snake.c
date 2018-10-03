/*! \file snake.c
 * \brief Snake management functions
 *
 * Simple snake implemented on linux terminal
 * Includes a top list record in file for competition
 *
 * Written by Zoltan Gere
 * 1706228
 * 02.10.2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "snake.h"

#define WALLCHAR 'H'
#define SNAKECHAR 'o'
#define APPLECHAR 'b'
#define EMPTYCHAR ' '

void clearBoard(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], size_t lengthX, size_t lengthY)
{
    /*! \var size_t i, j
     *  \brief Loop index variables
     */
    size_t i,j;
    for (i = 0; i< lengthX; i++) {              // Two loop run through all elements on the board...
        for (j = 0; j< lengthY; j++) {
            gameBoard[i][j] = EMPTYCHAR;              // .. and set to empty space character
        }
    }
} // clearBoard function ends

void updateBoard(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], coord * apple, snake * sn, int appleCount) {
    /*! \var size_t i, j
     *  \brief Loop index variables
     */
    size_t i;

    clearBoard(gameBoard, BOARDSIZEX, BOARDSIZEY);                      // clear board

    if (appleCount == 1) {                                              // place apple on board
        gameBoard[apple->x][apple->y] = APPLECHAR;
    }

    if (sn->head >= sn->tail) {                                         // ......tail..>>..head......
        for (i = sn->tail; i <= sn->head; i++) {                        // loop run forward from tail to head...
            gameBoard[sn->position[i].x][sn->position[i].y] = SNAKECHAR;      // ...and place on board
        }
    } else {                                                            // ..>>..head......tail..>>..
        for (i = sn->tail; i < BOARDSIZEX * BOARDSIZEY; i++) {          // loop run forward from tail to end of array
            gameBoard[sn->position[i].x][sn->position[i].y] = SNAKECHAR;      // ...and place on board
        }
        for (i = 0; i <= sn->head; i++) {                               // loop run forward from beginning of array to head
            gameBoard[sn->position[i].x][sn->position[i].y] = SNAKECHAR;      // ...and place on board
        }
    }
} // updateBoard function ends

unsigned char readInput()
{
    /*! \var unsigned char c[3]
     *  \brief Character sequence read from input
     */
    unsigned char c[3];
    /*! \var unsigned char upKey[3]
     *  \brief Predefined character sequence for up arrow key
     */
    unsigned char upKey[3] = {27,91,65};
    /*! \var unsigned char downKey[3]
     *  \brief Predefined character sequence for down arrow key
     */
    unsigned char downKey[3] = {27,91,66};
    /*! \var unsigned char leftKey[3]
     *  \brief Predefined character sequence for left arrow key
     */
    unsigned char leftKey[3] = {27,91,68};
    /*! \var unsigned char rightKey[3]
     *  \brief Predefined character sequence for right arrow key
     */
    unsigned char rightKey[3] = {27,91,67};
    /*! \var unsigned char escKey[3]
     *  \brief Predefined character sequence for escape key
     */
    unsigned char escKey[3] = {27,0,0};
    /*! \var unsigned char spaceKey[3]
     *  \brief Predefined character sequence for space key
     */
    unsigned char spaceKey[3] = {32,0,0};
    /*! \var unsigned char input
     *  \brief 1 char corresponding value for pressed key
     *          l - left arrow, r - right arrow, u - up arrow, d - down arrow, 32 - space key, 27 - escape key
     */
    unsigned char input = '\0';

    // Read user input
    memset(c, 0, sizeof(c));                    // Clear buffer

    if (read(0, &c, 3) != 0) {                  // Read input (0 = stdin, 3 char)
        //printf("You pressed: ");
        if (memcmp(c, upKey, 3) == 0) {         // Input sequence equals to stored up arrow sequence
            //printf("up\n");
            input = 'u';
        } else if (memcmp(c, downKey, 3) == 0) {    // Input sequence equals to stored down arrow sequence
            //printf("down\n");
            input = 'd';
        } else if (memcmp(c, leftKey, 3) == 0) {    // Input sequence equals to stored left arrow sequence
            //printf("left\n");
            input = 'l';
        } else if (memcmp(c, rightKey, 3) == 0) {   // Input sequence equals to stored right arrow sequence
            //printf("right\n");
            input = 'r';
        } else if (memcmp(c, escKey, 3) == 0) {     // Input sequence equals to stored escape key sequence
            input = c[0];
        } else if (memcmp(c, spaceKey, 3) == 0) {   // Input sequence equals to stored space bar key sequence
            input = c[0];
        }
    }
    return input;
} // readInput function ends

void updateSnakeDirection(snake * sn, unsigned char input) {
    if ((input == 'u') || (input == 'd')) {                                     // if input is up or down
        if ((sn->runningDirection == 'l') || (sn->runningDirection == 'r')) {   // snake turns only if running left or right
            sn->runningDirection = input;                                       // otherwise no effect
        }
    } else if ((input == 'l') || (input == 'r')) {                              // if input is left or right
        if ((sn->runningDirection == 'u') || (sn->runningDirection == 'd')) {   // snake turns only if running up or down
            sn->runningDirection = input;                                       // otherwise no effect
        }
    }
} // updateSnakeDirection function ends

void updateSnake(snake * sn)
{
    // update snake position
    if (sn->head < BOARDSIZEX * BOARDSIZEY - 1) {                               // head segment is not the last element
                                                                                // so the head pointer is increased
        switch (sn->runningDirection) {                                         // which way the snake moves
            case 'u':
                sn->position[sn->head+1].x = sn->position[sn->head].x;          // head moves up on the board
                sn->position[sn->head+1].y = sn->position[sn->head].y-1;
                sn->head++;
                break;
            case 'd':
                sn->position[sn->head+1].x = sn->position[sn->head].x;          // head moves down on the board
                sn->position[sn->head+1].y = sn->position[sn->head].y+1;
                sn->head++;
                break;
            case 'l':
                sn->position[sn->head+1].x = sn->position[sn->head].x-1;        // head moves left on the board
                sn->position[sn->head+1].y = sn->position[sn->head].y;
                sn->head++;
                break;
            case 'r':
                sn->position[sn->head+1].x = sn->position[sn->head].x+1;        // head moves right on the board
                sn->position[sn->head+1].y = sn->position[sn->head].y;
                sn->head++;
                break;
            default:
                break;
        }
    } else {                                                                    // head segment is the last element
                                                                                // so the head pointer is moved to 0 position
        switch (sn->runningDirection) {
            case 'u':
                sn->position[0].x = sn->position[sn->head].x;                   // head moves up on the board
                sn->position[0].y = sn->position[sn->head].y-1;
                sn->head = 0;
                break;
            case 'd':
                sn->position[0].x = sn->position[sn->head].x;                   // head moves down on the board
                sn->position[0].y = sn->position[sn->head].y+1;
                sn->head = 0;
                break;
            case 'l':
                sn->position[0].x = sn->position[sn->head].x-1;                 // head moves left on the board
                sn->position[0].y = sn->position[sn->head].y;
                sn->head = 0;
                break;
            case 'r':
                sn->position[0].x = sn->position[sn->head].x+1;                 // head moves right on the board
                sn->position[0].y = sn->position[sn->head].y;
                sn->head = 0;
                break;
            default:
                break;
        }
    }

    sn->snakeCurrentLength++;                                                   // because head moved forward, length is inceased
    if (sn->snakeCurrentLength > sn->snakeSupposedLength) {                     // if length is longer than it should
        sn->tail++;                                                             // tail is moved forward, after the head
        if (sn->tail == BOARDSIZEX * BOARDSIZEY) {                              // tail segment is after the last element
            sn->tail = 0;                                                       // so the tail pointer is moved to 0 position
        }
        sn->snakeCurrentLength--;                                               // because tail moved forward, length is decreased
    }
} // updateSnake function ends

int placeApple(coord * apple, snake * sn) {
    /*! \var int foundApplePosition
     *  \brief 1 is apple position successfully found
     */
    int foundApplePosition = 0;
    /*! \var size_t i
     *  \brief Loop index variables
     */
    size_t i;

    if (sn->snakeSupposedLength != BOARDSIZEX * BOARDSIZEY) {                   // if snake not covering the whole board
        while (foundApplePosition == 0) {                                       // find a random position for apple
            apple->x = rand() % BOARDSIZEX;
            apple->y = rand() % BOARDSIZEY;
            foundApplePosition = 1;                                             // found one position

                                                                                // compare apple position with snake position
            if (sn->head >= sn->tail) {                                         // ......tail..>>..head......
                for (i = sn->tail; i <= sn->head; i++) {
                    if ((sn->position[i].x == apple->x) && (sn->position[i].y == apple->y)) {   // compare position
                        foundApplePosition = 0;                                 // positions overlap
                    }                                                           // position not found
                }
            } else {                                                            // ..>>..head......tail..>>..
                for (i = sn->tail; i < BOARDSIZEX * BOARDSIZEY; i++) {
                    if ((sn->position[i].x == apple->x) && (sn->position[i].y == apple->y)) {   // compare position
                        foundApplePosition = 0;                                 // positions overlap
                    }                                                           // position not found
                }
                for (i = 0; i <= sn->head; i++) {
                    if ((sn->position[i].x == apple->x) && (sn->position[i].y == apple->y)) {   // compare position
                        foundApplePosition = 0;                                 // positions overlap
                    }                                                           // position not found
                }
            }
        }
    }
    return foundApplePosition;
} // placeApple function ends

void drawScreen(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], int score, int rowNumber) {
    /*! \var size_t i, j
     *  \brief Loop index variables
     */
    size_t i,j;
    char wall = WALLCHAR;

    // draw board on the screen
    printf ("Snake game\n                                    Your score: %d\n", score);     // Title line + player score
    for (j = 0; j < BOARDSIZEX + 2; j++) {                                                  // Top edge of board
        printf ("%c", wall);
    }
    printf("\n");

    for (i = 0; i< BOARDSIZEY; i++) {                                                       // Game board
        printf ("%c", wall);                                                                       // Left edge of board


        for (j = 0; j < BOARDSIZEX; j++) {                                                  // Game board
            printf ("%c", gameBoard[j][i]);
        }


        printf ("%c\n", wall);                                                                     // Right edge of board
    }

    for (j = 0; j < BOARDSIZEX + 2; j++) {                                                  // Bottom edge of board
        printf ("%c", wall);
    }

    printf ("\n\n    Use arrow keys to turn snake. Press ESC to quit game.\n");             // Print instructions

    for (i = BOARDSIZEY + 8; i< rowNumber; i++) {                                           // Fill remaining space
        printf("\n");                                                                       // so board is always
    }                                                                                       // at the same position
    //(void) fflush(stdout);
} // drawScreen function ends

// End of snake.c
