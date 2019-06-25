//
// Created by karim on 25.06.19.
//
#include "../../libs/sqlite3.h"

typedef struct Score{
    char *username;
    int score;
    char *time;
    int difficulty;
};

/**
 * \brief Inserts score in the database.
 *
 * @param username[]
 * @param score
 * @param _time
 * @param difficulty
 * @return Returns the id of the last user if insered, otherwise -1.
 */
int insertScore(char username[], int score, char *_time, int difficulty);


/**
 * \brief Retrieves score from the database.
 *
 * @param scores is an array of Score where the scores put into.
 */
void getScores(Score scores[]);