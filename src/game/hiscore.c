#include <stdio.h> // FILE, fopen, fprintf, perror, fscanf, fclose

#include "../util.h"
#include "hiscore.h"
#include "paddle.h"

// Constants
static const char HISCORE_FILE[] = "hiscore.txt";

// Variables
static int  hiscore;
static bool isHiscore;

// Function definitions

void hiscore_load(void)
{
    FILE *fp = fopen(HISCORE_FILE, READ_ONLY_TEXT);
    if (!fp)
    {
        fprintf(stderr, "Could not open file %s\n", HISCORE_FILE);
        perror("fopen() error");
	return;
    }

    if (fscanf(fp, "%i", &hiscore) != 1)
    {
        fprintf(stderr, "Could not read hiscore from file %s\n", HISCORE_FILE);
	return;
    }

    if (fclose(fp) == EOF)
    {
        fprintf(stderr, "Error on closing file %s\n", HISCORE_FILE);
        perror("fclose() error");
    }
}

void hiscore_save(void)
{
    FILE *fp = fopen(HISCORE_FILE, WRITE_ONLY_TEXT);
    if (!fp)
    {
        fprintf(stderr, "Could not open file %s.\n", HISCORE_FILE);
        perror("fopen() failed");
	return;
    }

    if (fprintf(fp, "%i\n", hiscore) < 0)
    {
        perror("fprintf failed");
	return;
    }

    fclose(fp);
}

void hiscore_check(void)
{
    int score = paddle_getScore();
    if (score > hiscore)
    {
        hiscore = score;
        isHiscore = true;
    }
}

int hiscore_getHi(void)
{
    return hiscore;
}

bool hiscore_isHi(void)
{
    return isHiscore;
}

void hiscore_resetIsHi(void)
{
    isHiscore = false;
}
