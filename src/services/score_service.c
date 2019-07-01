/* Autoren: Karim Echchennouf, Mohammad Kadoura, Florian Kry, Jonathan Trute
 * Klasse: FA12
 * Dateiname: game.c
 * Datum: 25.06.19
 * Beschreibung: In dieser Datei befinden sich alle Funktionen, die
 * etwas mit den Abfragen an die Datenbank zu tun haben, wenn es
 * um den Score bzw. die Zeit der Spieler geht.
*/

#include "../../headers/services/score_service.h"
#include "../../headers/services/connection.h"
#include "../../headers/shared/shared.h"
#include "../../headers/core/view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int iUser_id = 0;
char cSql[200];
char *cpZErrMsg;

sqlite3 *psqlConnection;

int insertScore(int *piUserID, int iScore, int iDifficulty)
/* Fügt die benötigte Zeit des aktuellen Spielers für sein gerade
 * beendetes Spiel in die Datenbank ein.
 * 1. Parameter: Zeiger auf Variable mit dem Wert der UserID des Spielers
 * 2. Parameter: benötigte Zeit
 * 3. Parameter: Schwierigkeitsgrad des gelösten Sudokus
 */
{
    sprintf(cSql, "INSERT INTO `Score` (time, userId, iDifficulty) VALUES(\"%d\", \"%d\", \"%d\");", iScore,
            *piUserID, iDifficulty);
    printf("%s\n", cSql);

    fflush(stdout);
    clear_output();

    int rc = sqlite3_exec(psqlConnection, cSql, NULL, NULL, &cpZErrMsg);
    if (!rc == SQLITE_OK)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

delete_whole_list(struct sScore *scores)
{
    struct sScore *temp;

    while (scores->next != NULL) // skip first element
    {
        temp = scores->next;
        scores = scores->next;
        free(temp);
    }
}


int getScoresCallback(void *pvScores, int iArgc, char **ppcArgv, char **ppcAzColName)
{
    struct sScore *a_head = (struct sScore *) pvScores;

    /* Zum Ende der Liste gehen */
    while (a_head->next != NULL)
    {
        a_head = a_head->next;
    }

    /* Einfügen eines neuen Elements */
    a_head->next = malloc(sizeof(struct sScore));

    for (int i = 0; i < iArgc; i++)
    {
        if (strcmp(ppcAzColName[i], "time") == 0)
        {
            a_head->next->time = atoi(ppcArgv[i]);
        }
        if (strcmp(ppcAzColName[i], "name") == 0)
        {
            strcpy(a_head->next->name, ppcArgv[i]);
        } else if (strcmp(ppcAzColName[i], "userId") == 0)
        {
            a_head->next->userId = atoi(ppcArgv[i]);
        } else if (strcmp(ppcAzColName[i], "iDifficulty") == 0)
        {
            a_head->next->difficulty = atoi(ppcArgv[i]);
        }
    }

    a_head->next->next = NULL;

    return 0;
}

void deleteNode(struct sScore *node)
        {
    struct sScore *temp = node->next;

    node->userId = node->next->userId;
    strcpy(node->name, node->next->name);
    node->time = node->next->time;
    node->next = temp->next;

    free(temp);
}

void getScores(struct sScore *scores, int iDiff)
/* Abfrage der Highscores inklusive der zugehörigen Spielernamen / UserIDs
 * 1. Parameter: Zeiger auf die Struktur, in der die abgefragten Daten ge-
 * speichert werden sollen
 */
{
    sprintf(cSql, "SELECT `Score`.`userId`, `User`.`name`, `Score`.`time`, `Score`.`iDifficulty` FROM `Score` INNER JOIN `User` ON `Score`.`userId` = `User`.`id` ORDER BY `Score`.`time` DESC LIMIT 10;");

    fflush(stdout);
    clear_output();

    int rc = sqlite3_exec(psqlConnection, cSql, getScoresCallback, scores, &cpZErrMsg);

    deleteNode(scores);

    if (rc == SQLITE_OK)
    {
        printf("OK\n");
    }
    else
    {
        printf("NO\n");
    }
}

int bestScoresCallBack(void *pvScores, int iArgc, char **ppcArgv, char **ppcAzColName)
{
    for (int i = 0; i < iArgc; i++)
    {
        if (strcmp(ppcAzColName[i], "time") == 0)
        {
            iUser_id = atoi(ppcArgv[i]);
        }
    }

    return 0;
}

int getBestScoreByUserID(int iUserID)
/* Erfragt den Highscore des aktuellen Nutzers aus der Datenbank.
 * 1. Parameter: UserID des aktuellen Nutzers
 */
{
	sprintf(cSql, "SELECT time FROM `Score` where userId = %d limit 1 sort by time;", iUserID);

    fflush(stdout);
    clear_output();

	int rc = sqlite3_exec(psqlConnection, cSql, bestScoresCallBack, NULL, &cpZErrMsg);
	printf("%s\n", cSql);

	return iUser_id;

}

int bestScoreCallback(void *pcBestScore, int iArgc, char **ppcArgv, char **ppcAzColName)
{
    int *piTmp = (int *) pcBestScore;

    for (int i = 0; i < iArgc; i++)
    {
        if (strcmp(ppcAzColName[i], "time") == 0)
        {
            *piTmp = atoi(ppcArgv[i]);
        }
    }

    return 0;
}


int getBestScore(int *piBestScore, int iDifficulty)
/* Erfragt den Highscore aller Spieler im aktuellen Schwierigkeitsgrad aus der Datenbank.
 * 1. Parameter: Zeiger auf die Variable, in die der Highscore geschrieben werden soll
 * 2. Parameter: aktueller Schwierigkeitsgrad
 */
{
    sprintf(cSql, "SELECT time FROM `Score`  WHERE  iDifficulty=%d ORDER by time desc  LIMIT 1", iDifficulty);

    fflush(stdout);
    clear_output();

    int rc = sqlite3_exec(psqlConnection, cSql, bestScoreCallback, piBestScore, &cpZErrMsg);
    if (!rc == SQLITE_OK)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

