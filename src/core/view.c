//
// Created by karim on 24.06.19.
//
#include <stdio.h>
#include <string.h>
#include "../../headers/core/view.h"
#include "../../headers/shared/shared.h"
#include "../../headers/core/game.h"

int deletedCells[9][9];
int userCells[9][9];
int difficulty;
int isGameActive;

void renderCourt()
{
    printf("%s+---+---+---+---+---+---+---+---+---+\n", KCYN);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            int number = arr[i][j];
            if (j % 3 == 0)
            {
                printf("%s| ", KCYN);
            }
            else
            {
                printf("%s| ", KGRN);
            }

            if (i == x && j == y)
            {
                if (number > 0)
                {
                    printf("%s%d ", KRED, number);
                }
                else
                {
                    printf("%s| ", KRED);
                }
            }
            else
            {
                if (arr[i][j] > 0)
                {
                    if (userCells[i][j] == 1)
                    {
                        printf("%s%d ", KMAG, number);
                    }
                    else
                    {
                        printf("%s%d ", KWHT, number);
                    }
                }
                else
                {
                    printf("%s  ", KGRN );
                }
            }
        }
        printf("%s|\n", KCYN);
        if ((i + 1) % 3 == 0)
        {
            printf("%s+---+---+---+---+---+---+---+---+---+\n", KCYN);
        }
        else
        {
            printf("%s+---+---+---+---+---+---+---+---+---+\n", KGRN );
        }
    }
    printf("%s \n", gameMessage);
    printf("\n");
}
void renderInfoBox()
{
    char difficultyText[20] = "";
    switch (difficulty)
    {
        case EASY:
            sprintf(difficultyText, "%s", "Einfach");
            break;
        case MEDIUM:
            sprintf(difficultyText, "%s", "Mittel");
            break;
        case HARD:
            sprintf(difficultyText, "%s", "Schwer");
            break;
        default:
            break;
    }
    int difficultyBoxWith = 8;
    int userBoxWith = 10;
    int remainingBoxWith = 5;
    int remaining = getRemainingCells(arr);
    printf("%s+---------------------------------------+\n", KCYN);
    printf("%s| %sUser: %s%*s%s| %sBestscore: %d      %s|\n", KCYN, KWHT, "Otto", userBoxWith - strlen("Otto"), "", KCYN, KWHT, 257, KCYN);
    printf("%s| %sTime: %s     %s| %sDifficulty: %s%*s%s|\n", KCYN, KWHT, "01:30", KCYN, KWHT, difficultyText, difficultyBoxWith - strlen(difficultyText), "", KCYN);
    printf("%s| %sRemaining: %d%*s%s|                     |\n", KCYN, KWHT, remaining, remainingBoxWith - lenHelper(remaining), "", KCYN);
    printf("%s+---------------------------------------+\n\n", KCYN);
}

void renderGameMenu()
{
    printf("%s Movement        Commands\n\n", KCYN);
    printf("%s %c - Right       h - Give a hint\n\n", KWHT,'>');
    printf("%s %c - Left        %ss - Solve \n\n", KWHT, '<', getGameStatus(arr) == FILLED ? KWHT : KRED);
    printf("%s %c - Top         a - Abbrechen \n\n", KWHT, '^');
    printf("%s %c - Down        z - Zurueck \n\n", KWHT, 'v');
	printf("                 q - Beenden \n\n", KWHT);
}

void renderMenu()
{
    printf("%s+----------- Menu ------------+\n",KCYN);
    if (isGameActive > 0)
    {
        printf("| %sr - Spiel Fortsetzen        %s|\n",KWHT,KCYN);
    }
    else
    {
        printf("| %ss - Spiel Starten           %s|\n",KWHT,KCYN);
    }

    printf("| %sb - BestenListe             %s|\n",KWHT,KCYN);
    printf("| %sq - Beenden                 %s|\n",KWHT,KCYN);
    printf("%s+-----------------------------+\n",KCYN);
}

void renderDetails()
{
    printf("%s+---------- Details  ----------+\n",KCYN);
    printf("| %sz - Zurueck                  %s|\n",KWHT,KCYN);
    printf("+------------------------------+\n");
}

void renderDifficultyDialog()
{
    printf("%s+----- Schwierigkeiteinstellungen ------+%s\n",KCYN,KWHT);
    printf("%s| %sa - Einfach                           %s|\n",KCYN,KWHT,KCYN);
    printf("%s| %sb - Mittel                            %s|\n",KCYN,KWHT,KCYN);
    printf("%s| %sc - Schwer                            %s|\n",KCYN,KWHT,KCYN);
	printf("%s|                                       |\n",KCYN);
    printf("%s| %sWaehle die gewuenschte                %s|\n",KCYN,KWHT,KCYN);
    printf("%s| %sSchwierigkeitsstufe aus.              %s|\n",KCYN,KWHT,KCYN);
    printf("%s+---------------------------------------+\n",KCYN);
}



int getRemainingCells(int array[][9])
{
    int counter = 0;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (array[i][j] == 0)
            {
                counter++;
            }
        }
    }
    return counter;
}

int lenHelper(int x) {
    if (x >= 100000)     return 6;
    if (x >= 10000)      return 5;
    if (x >= 1000)       return 4;
    if (x >= 100)        return 3;
    if (x >= 10)         return 2;
    return 1;
}