/* Autoren: Karim Echchennouf, Mohammad Kadoura, Florian Kry, Jonathan Trute
 * Klasse: FA12
 * Dateiname: main.c
 * Datum:
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>


#include "headers/core/game.h"
#include "headers/services/user_service.h"
#include "headers/services/score_service.h"
#include "headers/services/connection.h"
#include "libs/sqlite3.h"


#define TRUE 1
#define FALSE 0

#ifdef __unix__

#include <termios.h>
#include <unistd.h>
#include <headers/core/view.h>

static struct termios new_io;
static struct termios old_io;


int cbreak(int fd) {
    if ((tcgetattr(fd, &old_io)) == -1)
        return -1;
    new_io = old_io;
    new_io.c_lflag = new_io.c_lflag & ~(ECHO | ICANON);
    new_io.c_cc[VMIN] = 1;
    new_io.c_cc[VTIME] = 0;

    if ((tcsetattr(fd, TCSAFLUSH, &new_io)) == -1)
        return -1;
    return 1;
}

int getch(void) {
    int c;

    if (cbreak(STDIN_FILENO) == -1) {
        printf("Fehler bei der Funktion cbreak ... \n");
        exit(EXIT_FAILURE);
    }
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &old_io);
    return c;
}

#elif __WIN32__
#define HEIGHT 805
#define WIDTH 370
#include <Windows.h>
#include <conio.h>
#endif

int exitTheGame = 0;
int windows = 0;

sqlite3 *connection;

void resizeWindow() {
#ifdef __unix__
    system("resize -s 48 48");
#endif

#ifdef __WIN32__
    HWND hwnd = FindWindow("ConsoleWindowClass", NULL);
    MoveWindow(hwnd, 550, 15, WIDTH, HEIGHT, TRUE);
    setvbuf(stdout, NULL, _IOFBF, 10000);
#endif
}


int gameData[9][9];
int deletedCells[9][9];
int userCells[9][9];
int difficulty;
int isGameActive;
int isSolvedAutomatic;
int currentPosition;
char username[50] = "Name eingeben...\0";
int *userID = 0;
int *bestScore = 0;
int remaining = 0;
int marks[9][9][MAX_MARKS];


int b = 0;

struct score *scores = NULL;
void checkGameState();
void renderGame();

int anzahlDerTipps=0;
int anzahlDerHilfe = 0;


int erlaubteAnzahlDerTipps=0;
int erlaubteAnzahlDerHilfe = 0;

void setConfig() {
    anzahlDerTipps=0;
    anzahlDerHilfe = 0;
    switch (difficulty){
        case EASY:
            erlaubteAnzahlDerHilfe=5;
            erlaubteAnzahlDerTipps=8;
            break;
        case MEDIUM:
            erlaubteAnzahlDerHilfe=4;
            erlaubteAnzahlDerTipps=6;
            break;
        case HARD:
            erlaubteAnzahlDerHilfe=3;
            erlaubteAnzahlDerTipps=4;
            break;
    }
}

int main() {

    if (createDatabaseIfNotExist()) {
        int test = createTables();
        printf("test: %d", test);
        fflush(stdout);
        system("Pause");
    }

    resizeWindow();
    initColors();

    userID = malloc(sizeof(int));
    bestScore = malloc(sizeof(int));

    int rc = sqlite3_open("./sudoku.db", &connection);

    if (rc != SQLITE_OK) {
        printf("Failed to open the sudoku.db\n");
        return 1;
    }

    srand(time(NULL));
    scores = malloc(sizeof(score));
    scores->next = NULL;


    currentPosition = MENU;
    difficulty = EASY;

    renderGame();

    printf("Ciao");
    return 0;
}

void checkGameState(){
        if (isGameActive && getGameStatus(gameData) == FILLED) {
            int solveState = solveGame(gameData);
            if(solveState == 1){
                if(*userID != 0 && isSolvedAutomatic == 0 ){
                    int score = timer(0);
                    insertScore(userID, score, difficulty);
                }
                currentPosition = SOLVED_GAME;
                isGameActive=0;
            } else{
                strcpy(gameMessage, "Das Spiel ist nicht korrekt geloest.");
            }
        }
}

void navigateTo(int pos) {
    switch (pos) { // the real value
        case UP:
            // code for arrow up
            x_coordinate--;
            break;
        case DOWN:
            // code for arrow down
            x_coordinate++;
            break;
        case RIGHT:
            // code for arrow right
            y_coordinate++;
            break;
        case LEFT:
            // code for arrow left
            y_coordinate--;
            break;
        case UP_LINUX:
            // code for arrow up
            x_coordinate--;
            break;
        case DOWN_LINUX:
            // code for arrow down
            x_coordinate++;
            break;
        case RIGHT_LINUX:
            // code for arrow right
            y_coordinate++;
            break;
        case LEFT_LINUX:
            // code for arrow left
            y_coordinate--;
            break;
    }

    if (x_coordinate > 8) {
        x_coordinate = 0;
    }
    if (x_coordinate < 0) {
        x_coordinate = 8;
    }
    if (y_coordinate > 8) {
        y_coordinate = 0;
    }
    if (y_coordinate < 0) {
        y_coordinate = 8;
    }
}

void handleUserInput() {
    int userInput;
    char ch;

    if (currentPosition == USER_NAME) {
        ch = getch();

        if (ch != 13 && ch != '\n' && ch != EOF) {
            if (strcmp(username, "Name eingeben...") == 0) resetArray(username);
            if (ch == 27) { // escape
                strcpy(username, "anonym");
                currentPosition = DIFFICULTY_DIALOG;
            } else if (ch == 127 || ch == 8) {
                b--;
                if (b < 0) {
                    b = 0;
                }
                username[b] = 0;

            } else if (b < 30) {
                username[b] = ch;
                b++;
            }
        } else if (strlen(username) > 0) {
            username[b] = '\0';
            if (strcmp(username, "Name eingeben...") == 0) {
                strcpy(username, "anonym");
            } else {
                registerUser(username, userID);
                insertScore(userID, 0, difficulty);
            }
            fflush(stdout);
            clear_output();
            currentPosition = DIFFICULTY_DIALOG;
        }
    } else {

        if ((userInput = getch()) == 224) {
            navigateTo(getch()); // windows
        } else {
            navigateTo(userInput); // linux
            switch (currentPosition) {
                case DIFFICULTY_DIALOG:
//				if (userInput == 10) // enter pressed
//				{
//					currentPosition = IN_GAME;
//				}
                    if (isalpha(userInput)) {
                        switch (userInput) {
                            case 'a':
                                difficulty = EASY;
                                currentPosition = IN_GAME;
                                setConfig();
                                break;

                            case 'b':
                                difficulty = MEDIUM;
                                currentPosition = IN_GAME;
                                setConfig();
                                break;

                            case 'c':
                                difficulty = HARD;
                                currentPosition = IN_GAME;
                                setConfig();
                                break;
                        }
                    }
                    break;

                case MENU:
                    if (isalpha(userInput)) {
                        switch (userInput) {
                            case 's':
                                if (isGameActive > 0) {
                                    break;
                                }

                                currentPosition = USER_NAME;
                                break;

                            case 'r':
                                if (isGameActive == 0) {
                                    break;
                                }
                                timer(TIMER_PAUSE);
                                currentPosition = IN_GAME;
                                break;

                            case 'b':
                                currentPosition = DETAILS_DIALOG;
                                break;

                            case 'k':
                                currentPosition = HELP;
                                break;

                            case 'q':
                                exitTheGame = 1;
                                break;
                        }
                    }
                    break;

                case IN_GAME:
                    if (isdigit(userInput)) {
                        if (userCells[x_coordinate][y_coordinate] == 1) {
                            gameData[x_coordinate][y_coordinate] = userInput - '0';
                        }
                    } else if (isalpha(userInput)) {
                        switch (userInput) {
                            case 'h':
                                if (deletedCells[x_coordinate][y_coordinate] > 0) {
                                    if (anzahlDerHilfe == erlaubteAnzahlDerHilfe){
                                        strcpy(gameMessage, "Anzahl der Hilfen verbraucht.");
                                        break;
                                    }
                                    anzahlDerHilfe++;
                                    solveCell(gameData, x_coordinate, y_coordinate);
                                    timer(HELP_USED);
                                } else{
                                    strcpy(gameMessage, "Zelle ist nicht leer.");
                                }


                                break;
                            case 't':
                                if (deletedCells[x_coordinate][y_coordinate] > 0) {
                                    if (anzahlDerTipps == erlaubteAnzahlDerTipps){
                                        strcpy(gameMessage, "Anzahl der Tipps verbraucht.");
                                        break;
                                    }
                                    anzahlDerTipps++;
                                    fillNotesForCell(x_coordinate, y_coordinate);
                                    timer(TIPP_USED);
                                } else {
                                    strcpy(gameMessage, "Tipp ist nicht verfuegbar");
                                }

                                break;
                            case 'q':
                                exitTheGame = 1;
                            case 'a':
                                resetGameData(gameData);
                                isGameActive = 0;
                                timer(RESET_TIMER);

                            case 'p':
                                timer(TIMER_PAUSE);
                                currentPosition = MENU;
                                break;
                            case 'k':
                                timer(TIMER_PAUSE);
                                currentPosition = HELP;
                                break;
                            case 's':
                                solveAll(gameData, deletedCells);
                                isSolvedAutomatic = 1;
                                break;
                            case 'm':
                                if (gameData[x_coordinate][y_coordinate] == 0){
                                    currentPosition = SET_MARK;
                                } else{
                                    strcpy(gameMessage, "Markiere-Mous nicht verfuegbar.");
                                }
                                break;
                        }
                    }
                    break;
                case SOLVED_GAME:
                    if (isalpha(userInput)) {
                        switch (userInput) {
                            case 'z':
                                currentPosition = MENU;
                                break;
                            case 'q':
                                exitTheGame = 1;
                                break;
                        }
                    }
                    break;
                case SET_MARK :
                    if (isdigit(userInput)) {
                        makeNote(x_coordinate, y_coordinate, userInput - '0');
                    }
                    if (isalpha(userInput)) {
                        switch (userInput) {
                            case 'm':
                                currentPosition = IN_GAME;
                                break;
                            case 'q':
                                exitTheGame = 1;
                        }
                    }
                    break;
                case DETAILS:
                    if (isalpha(userInput)) {
                        switch (userInput) {
                            case 'z':
                                currentPosition = MENU;

                                break;
                        }
                    }
                    break;
                case DETAILS_DIALOG:
                    if (isalpha(userInput)) {
                        switch (userInput) {
                            case 'e':
                                difficulty = EASY;
                                currentPosition = DETAILS;
                                break;
                            case 'm':
                                difficulty = MEDIUM;
                                currentPosition = DETAILS;
                                break;
                            case 's':
                                difficulty = HARD;
                                currentPosition = DETAILS;
                                break;
                            case 'z':
                                currentPosition = MENU;
                                break;
                        }
                        getScores(scores, difficulty);
                    }
                    break;
                case HELP:
                    if (isalpha(userInput)) {
                        switch (userInput) {
                            case 'z':
                                if (isGameActive > 0) {
                                    timer(TIMER_PAUSE);
                                    currentPosition = IN_GAME;
                                } else {
                                    currentPosition = MENU;
                                }
                                break;
                        }
                    }
                    break;
            }
        }
    }
}


void renderGame(){
    while (!exitTheGame) {
        clear_output();
        switch (currentPosition) {
            case MENU:
                renderMenu();
                fflush(stdout);
                break;

            case USER_NAME:
                renderUsernameDialog(username);
                fflush(stdout);
                break;

            case IN_GAME:

                if (isGameActive == 0) {
                    resetGameData(gameData);
                    resetGameData(deletedCells);
                    resetGameData(userCells);
                    generateGameData(gameData);
                    deleteCells(gameData, difficulty);
                    resetArray(marks[x_coordinate][y_coordinate]);
                    isGameActive = 1;
                    timer(TIMER_START);
                    isSolvedAutomatic=0;
                }

                // int s = getBestScoreByUserID(userID, p);
                getBestScore(bestScore);
                remaining = getRemainingCells(gameData);
                renderInfoBox(username, bestScore, difficulty, remaining);
                renderNotesBox(x_coordinate, y_coordinate);
                renderCourt(gameData, userCells, x_coordinate, y_coordinate);
                renderGameMenu();
                sprintf(gameMessage, "%s", "");
                fflush(stdout);
                break;

            case SET_MARK:
                getBestScore(bestScore);
                remaining = getRemainingCells(gameData);
                renderInfoBox(username, bestScore, difficulty, remaining);
                renderMarkModeMessage();
                renderCourt(gameData, userCells, x_coordinate, y_coordinate);
                renderGameMenu();
                sprintf(gameMessage, "%s", "");
                fflush(stdout);
                break;

            case DIFFICULTY_DIALOG:
                renderDifficultyDialog();
                fflush(stdout);
                break;

            case DETAILS_DIALOG:
                renderDBestScoreDialog();
                fflush(stdout);
                break;

            case DETAILS:
                renderDetails(scores, difficulty);
                fflush(stdout);
                break;
            case HELP:
                renderHelpDialog();
                fflush(stdout);
                break;
            case SOLVED_GAME:
                printf("%d      %d", anzahlDerTipps, anzahlDerHilfe);
                renderSolvedGame(isSolvedAutomatic, anzahlDerTipps, anzahlDerHilfe);
                renderCourt(gameData, userCells, x_coordinate, y_coordinate);
                fflush(stdout);
                break;
        }

        handleUserInput();
        checkGameState();

    }
}


