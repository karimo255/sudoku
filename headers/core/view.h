//
// Created by karim on 24.06.19.
//

#ifndef SUDOKU_VIEW_H
#define SUDOKU_VIEW_H

#endif //SUDOKU_VIEW_H

#define KNRM  "\x1B[0m" ///< default condole color
#define KRED  "\x1B[31m" ///< color red
#define KGRN  "\x1B[32m" ///< color green
#define KYEL  "\x1B[33m" ///< color yellow
#define KBLU  "\x1B[34m" ///< color blue
#define KMAG  "\x1B[35m" ///< color magenta
#define KCYN  "\x1B[36m" ///< color cay
#define KWHT "\x1B[37m" ///< color white

/**
 * \brief function to render menu
 *
 * It show the game menu in the console
 */
void renderMenu();

/**
 * \brief Function to render best-score scrren
 *
 * It shows infos about users and scores
 */
void renderDetails();

/**
 * \brief Function to render the game grid
 *
 * It shows the game grid
 */
void renderCourt();

/**
 * \brief Function to render infos about current session
 *
 * It shows the current session information
 */
void renderInfoBox();

/**
 * \brief Function to render the game menu
 *
 * It shows the game menu
 */
void renderGameMenu();

/**
 * \brief Function to render the difficult settings dialog
 *
 * It shows difficult dialog
 */
void renderDifficultyDialog();

/**
 * \brief Function to get the length of integers (aka strlen for strings)
 *
 * @param input an integer to get length of
 * @return the length of the given integer
 *
 * It return the length of integer number (for optimal printing)
 */
int lenHelper(int input);

/**
* \brief Get count of not filled cells
*
* \param array gameData
* \return count of not filled cells
*/

int getRemainingCells(int array[][9]);