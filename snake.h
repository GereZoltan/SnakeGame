/*! \file snake.h
 * \brief Snake management functions header file
 *
 * Simple snake implemented on linux terminal
 * Includes a top list record in file for competition
 *
 * Written by Zoltan Gere
 * 1706228
 * 02.10.2018
 */

/*! \def BOARDSIZEX
 *  \brief Horizontal size of the game board
 */
#define BOARDSIZEX 20
/*! \def BOARDSIZEY
 *  \brief Vertical size of the game board
 */
#define BOARDSIZEY 14

#ifndef SNAKEGAME_SNAKE_H
#define SNAKEGAME_SNAKE_H

/*! \typedef struct coord
 *  \brief Contains 1 coordinate on game board
 *
 * \var int x Horizontal position
 * \var int y Vertical position
 */
typedef struct coord_t {
    int x;
    int y;
} coord;

/*! \typedef struct snake
 *  \brief Contains 1 snake
 *
 * \var coord position[BOARDSIZEX * BOARDSIZEY] Snake segment poitions
 * \var size_t head The head segment on the list
 * \var size_t tail The tail segment on the list
 * \var size_t snakeCurrentLength Length of the snake
 * \var size_t snakeSupposedLength Length the snake should be
 * \var unsigned char runningDirection The direction the snake is facing and sliding
 */
typedef struct snake_t {
    coord position[BOARDSIZEX * BOARDSIZEY];
    size_t head;
    size_t tail;
    size_t snakeCurrentLength;
    size_t snakeSupposedLength;
    unsigned char runningDirection;           // u, d, l, r
} snake;

/*! \fn void clearBoard(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], size_t lengthX, size_t lengthY)
 * \brief Clear the game board
 *
 * Fills the game boad with 0 values
 *
 * \param gameBoard Pointer to board
 * \param lengthX Number of columns
 * \param lengthY Number of rows
 * \return void No values returned
 */
void clearBoard(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], size_t lengthX, size_t lengthY);

/*! \fn void updateBoard(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], coord * apple, snake * sn, int appleCount)
 * \brief Place the snake and the apples on board
 *
 * Place the snake and the apples on board
 *
 * \param gameBoard Pointer to board
 * \param apple Pointer to apple
 * \param sn Pointer to snake
 * \param appleCount Number of apples on board
 * \return void No values returned
 */
void updateBoard(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], coord * apple, snake * sn, int appleCount);

/*! \fn unsigned char readInput()
 * \brief Reads keyboard input
 *
 * Reads character input sequence from input
 * In raw mode keys generate many character long sequences
 * Reading compared the predefined character sequences
 *
 * l - left arrow, r - right arrow, u - up arrow, d - down arrow, 32 - space key, 27 - escape key
 *
 * \return unsigned char Letter equivalent for input key l, r, u, d, 32, 27
 */
unsigned char readInput();

/*! \fn void updateSnakeDirection(snake * sn, unsigned char input)
 * \brief Update snake facing direction information
 *
 * Update snake running direction / bearing in the snake structure
 *
 * \param sn Snake
 * \param input New direction for snake
 * \return void No values returned
 */
void updateSnakeDirection(snake * sn, unsigned char input);

/*! \fn void updateSnake(snake * sn)
 * \brief Update snake structure
 *
 * Update snake position when snake moves forward
 * Actual and supposed length updated
 *
 * \param sn Snake
 * \return void No values returned
 */
void updateSnake(snake * sn);

/*! \fn int placeApple(coord * apple, snake * sn)
 * \brief Place an apple on the board if it is possible
 *
 * Find a new position for apple
 * Snake position is considered and not placed on it
 * If new place is found returns 1
 * otherrwise returns 0
 *
 * \param apple Pointer to apple
 * \param sn Pointer to nake
 * \return int No. of placed apples returned
 */
int placeApple(coord * apple, snake * sn);

/*! \fn void drawScreen(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], int score, int rowNumber)
 * \brief Render the game screen on terminal window
 *
 * Render the game screen on terminal window
 * Screen line feed writing adjusted to terminal size, therefore the board appears at the same position
 *
 * \param gameBoard The game board with snake and apple on it
 * \param score Players current score
 * \param Number of rows on terminal
 * \return void No values returned
 */
void drawScreen(unsigned char gameBoard[BOARDSIZEX][BOARDSIZEY], int score, int rowNumber);

#endif //SNAKEGAME_SNAKE_H

// End of snake.h