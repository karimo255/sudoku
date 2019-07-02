/* Autoren: Karim Echchennouf, Mohammad Kadoura, Florian Kry, Jonathan Trute
 * Klasse: FA12
 * Dateiname: game.c
 * Datum: 24.06.19
 * Beschreibung: Hier werden im Wesentlichen alle für das Spiel
 * nötigen Berechnungen durchgeführt. Z.B. das Generieren des Spiel-
 * feldes oder der Timer, wie lange ein Spieler für das Lösen
 * ein Spiel benötigt.
*/

#include <stdlib.h>
#include "../../headers/core/view.h"
#include "../../headers/core/game.h"
#include "../../headers/shared/shared.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <unistd.h>
#include <headers/services/score_service.h>

int iX_coordinate = 0;
int iY_coordinate = 0;
int iDifficulty = EASY;
char cGameMessage[200] = {0};
int iIsGameActive;

int iGameData[9][9] = {0};
int iMarks[9][9][MAX_MARKS];
int iDeletedCells[9][9] = {0};
int iUserCells[9][9] = {0};

int iElementsInSomeColumn[9] = {0};

time_t start, end, _pause;

void fillNotesForCell(int iX_coordinate, int iY_coordinate)
{
    int iRandomIndexForSolution = rand() % 2;
    iMarks[iX_coordinate][iY_coordinate][iRandomIndexForSolution] = iDeletedCells[iX_coordinate][iY_coordinate];
    for (int i = 0; i < MAX_MARKS; i++)
    {
        if (iMarks[iX_coordinate][iY_coordinate][i] == 0)
        {
            int iNumber = generateRandomNumber();
            if (isElementInArray(iMarks[iX_coordinate][iY_coordinate], iNumber, MAX_MARKS) > 0)
            {
                i--;
                continue;
            }
            iMarks[iX_coordinate][iY_coordinate][i] = iNumber;
        }
    }
}

void solveCell(int iGameData[][9], int iX, int iY)
{
    if (iDeletedCells[iX][iY] > 0)
    {
        iUserCells[iX][iY] = 0;
        iGameData[iX][iY] = iDeletedCells[iX][iY];
    }
}

void makeNote(int iX, int iY, int iSuggestion)
{
    for (int i = 0; i <= MAX_MARKS; i++)
    {
        if (iMarks[iX][iY][i] == 0)
        {
            iMarks[iX][iY][i] = iSuggestion;
            break;
        }
        else if (i == MAX_MARKS)
        {
            iMarks[iX][iY][0] = iSuggestion;
            for (int j = 1; j < MAX_MARKS; j++)
            {
                iMarks[iX][iY][j] = 0;
            }
        }
    }
}

void solveAll(int iGameData[][9], int iDeletedCells[][9])
{
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            if (iDeletedCells[x][y] != 0)
            {
                iGameData[x][y] = iDeletedCells[x][y];
            }
        }
    }
}

int solveGame(int iGameData[][9])
{
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            int iNumber = iGameData[x][y];
            iGameData[x][y] = 0;
            if (isElementInArray(iGameData[x], iNumber, 9) >= 0 || iNumber == 0)
            {
                iGameData[x][y] = iNumber;
                return 0;
            }

            resetArray(iElementsInSomeColumn, 9);
            for (int l = 0; l < sizeof(iElementsInSomeColumn); l++)
            {
                iElementsInSomeColumn[l] = iGameData[l][y];
            }

            if (isElementInArray(iElementsInSomeColumn, iNumber, 9) >= 0 ||
                isElementInBox(iGameData, x - x % 3, y - y % 3, iNumber) >= 0)
            {
                iGameData[x][y] = iNumber;
                return 0;
            }
            iGameData[x][y] = iNumber;
        }
    }
    return 1;
}

int generateRandomNumber()
{
    return 1 + rand() % 9;
}

void resetArray(int iArray[], int iSize)
{
    for (int l = 0; l < 9; l++)
    {
        iArray[l] = 0;
    }
}

void resetGameData(int iGmaeData[][9])
{
    for (int _x = 0; _x < 9; _x++)
    {
        for (int _y = 0; _y < 9; _y++)
        {
            iGmaeData[_x][_y] = 0;
        }
    }
}

int isElementInArray(int iArray[], int iNumber, int iSize)
{
    for (int x = 0; x < iSize; x++)
    {
        if (iArray[x] == iNumber && iNumber != 0)
        {
            return x;
        }
    }
    return -1;
}

int isElementInBox(int iArr[][9], int iBox_start_row, int iBox_start_col, int ele)
{
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
        {

            if (iArr[row + iBox_start_row][col + iBox_start_col] == ele && ele != 0)
            {
                return 1;
            }
        }
    return -1;
}

void generateGameData(int iGameData[][9])
{
    time_t start_t, end_t;
    double iDiff_t;
    time(&start_t);
    resetGameData(iGameData);
    srand(time(NULL));
    clear_output();
    printf("          Generieren von Spieldaten....\n");
    fflush(stdout);
    usleep(500000);

    for (int _x = 0; _x < 9; _x++)
    {
        for (int _y = 0; _y < 9; _y++)
        {
            time(&end_t);
            iDiff_t = difftime(end_t, start_t);
            if (iDiff_t > 2)
            {
                generateGameData(iGameData);
                break; // das ist der fix;
            }
            int number = generateRandomNumber();

            if (isElementInArray(iGameData[_x], number, 9) >= 0)
            { // number darf nur einmal in row vorkommen.
                _y--;
                resetArray(iElementsInSomeColumn, 9);
                continue;
            }

            resetArray(iElementsInSomeColumn, 9);
            for (int l = 0; l < 9; l++)
            {
                iElementsInSomeColumn[l] = iGameData[l][_y];
            }

            // number darf nur einmal in column und box vorkommen.
            if (isElementInArray(iElementsInSomeColumn, number, 9) >= 0 ||
                isElementInBox(iGameData, _x - _x % 3, _y - _y % 3, number) >= 0)
            {
                resetArray(iGameData[_x], 9);
                _x--;
                break;
            }
            iGameData[_x][_y] = number;
        }
    }
    printf("\n");
    fflush(stdout);
    clear_output();
}

int generateNumberByInterval(int iX, int iY)
{
    return iX + rand() % (iY - iX + 1);
}

void deleteCells(int iArray[][9], int iDifficulty)
{
    for (int x = 1; x <= 3; x++)
    {
        for (int y = 1; y <= 3; y++)
        {
            int tmp = iDifficulty;
            while (tmp > 0)
            {
                int r = generateNumberByInterval(3 * (x - 1), 3 * x - 1);
                int c = generateNumberByInterval(3 * (y - 1), 3 * y - 1);
                if (iArray[r][c] > 0)
                { // not already deleted
                    iDeletedCells[r][c] = iArray[r][c];
                    iUserCells[r][c] = 1;
                    iArray[r][c] = 0;
                }
                tmp--;
            }
        }
    }
}

int getGameStatus(int iArray[][9])
{
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 9; y++)
        {
            if (iArray[x][y] == 0)
            {
                return NOT_FILLED;
            }
        }
    }
    return FILLED;
}

int timer(int iAction)
{

    static int iFirst = 0, iPaused = 0;
    static int iTimer = 0, zwErg = 0;

    switch (iAction)
    {

    /** Keine Aktion, da nur aktuelle Zeit ausgegeben werden soll */
    case TIMER_STATE:
        break;

    /** Stoppuhr pausieren. Beim ersten Durchlauf, wird ein aktueller
     * Zeitstempel genommen, beim zweiten Durchlauf, wird die
     * pausierte Zeit zum Zwischenergebnis aufaddiert */
    case TIMER_PAUSE:
        if (iPaused == 0)
        {
            _pause = time(NULL);
            iPaused++;
        }
        else
        {
            end = time(NULL);
            zwErg += (end - _pause);
            iPaused--;
        }
        break;

        /** iFirst auf 1 setzen, um Timer zu starten (erster Duchlauf) */
    case TIMER_START:
        iFirst = 1;
        zwErg = 0;
        iPaused = 0;
        break;

        /** Stoppuhr bzw. alle zugehörigen Werte zurücksetzen. */
    case RESET_TIMER:
        iFirst = 1;
        zwErg = 0;
        iPaused = 0;
        iTimer = 0;
        break;

        /** Vom Startzeitstempel 15 Sekunden abziehen, die als zusätzliche
         * 15 Strafsekungen gerechnet werden. */
    case TIPP_USED:
        start -= 15;
        break;

        /** Vom Startzeitstempel 30 Sekunden abziehen, die als zusätzliche
         * 30 Strafsekungen gerechnet werden. */
    case HELP_USED:
        start -= 30;
    default:
        break;
    }

    if (iFirst)
    {
        start = time(NULL);
        iFirst = 0;
    }

    end = time(NULL);

    iTimer = end - start;
    iTimer -= zwErg;

    return iTimer;
}

void timeToString(int iUserTime, char cStringTime[])
{
    int seconds = iUserTime % 60;
    int minutes = iUserTime / 60;

    char cS[2] = {0};
    char cM[2] = {0};

    if (seconds < 10)
    {
        cS[0] = '0';
        cS[1] = '\0';
    }

    if (minutes < 10)
    {
        cM[0] = '0';
        cM[1] = '\0';
    }
    sprintf(cStringTime, "%s%d:%s%d", cM, minutes, cS, seconds);
    cStringTime[5] = '\0';
}

void checkGameState() {
    if (iIsGameActive && getGameStatus(iGameData) == FILLED) {
        int solveState = solveGame(iGameData);
        if (solveState) {
            if (iPUserID != NULL && isSolvedAutomatic == 0) {
                strcpy(cGameMessage, "insert.");

                int _score = timer(TIMER_STATE);
                insertScore(iPUserID, _score, iDifficulty);
            }
            iIsGameActive = 0;
            iCurrentPosition = SOLVED_GAME;
        } else {
            strcpy(cGameMessage, "Das Spiel ist nicht korrekt geloest.");
        }
    }
}