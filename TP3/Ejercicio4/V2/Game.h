#ifndef _GAME_H
#define _GAME_H

#define GAME_STATUS_WAITING 0
#define GAME_STATUS_PAYING 1

#include<pthread.h>

#include "Protocol.h"
#include "Users.h"
#include "Questions.h"


void newUser(int);
void configGame();
void startGame();
void endGame();

#endif