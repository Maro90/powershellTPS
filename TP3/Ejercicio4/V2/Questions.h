#ifndef _QUESTIONS_H
#define _QUESTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Protocol.h"

typedef struct question{
    tMessageQuestion questionMsg;
    struct question * next;
} tQuestion;

tQuestion * questions;

void loadQuestions();
tQuestion * getQuestions();
#endif