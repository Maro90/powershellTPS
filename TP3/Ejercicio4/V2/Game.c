#include "Game.h"

int gameStatus = GAME_STATUS_WAITING;
pthread_mutex_t answer_lock;
int answered = 0;
int respuesCorrecta=3;

void answerControlle(tMessageAnswer msg, tConnection * connection){
   tUserNode * tmpUser = getUser(connection->id);
   	pthread_mutex_lock(&answer_lock);
   	if(!answered){
	   	tmpUser->statistics.count++;
	   	if(msg.id == respuesCorrecta){
	   		tmpUser->statistics.correct++;
	   	}else{
	   		tmpUser->statistics.incorrect++;
	   	}
	}
	answered++;
	pthread_mutex_unlock(&answer_lock);
}

void newUser(int socket){
	tConnection connection;
    tUserNode * tmpUser;
    connection.initController = initController;
    connection.acceptController = NULL;

    connection.descriptor = socket;
    tmpUser = addUser(connection);
    if(gameStatus!=GAME_STATUS_WAITING){
    	tmpUser->status=USER_STATUS_GAMEALREADYSTARTED;
    }
    if(tmpUser && pthread_create( &(tmpUser->threadId), NULL ,  handler_run , (void*) &(tmpUser->connection)) < 0){
        printf("Error al establecer conexion\n");
        tmpUser->status=USER_STATUS_CONNECTIONERROR;
    }

    if(getUserCount() == 2){
    	startGame();
    }
}

void sendQuestion(){
	tCommand cmd;
	cmd.commandId = COMMAND_QUESTION;
	tMessageQuestion msg;
	strcpy(msg.title,"PREGUNTA!");
	answered=0;
	send_all(&cmd,&msg);
}

void startGame(){
	if (pthread_mutex_init(&answer_lock, NULL) != 0)
    {
        printf("\nFallo al iniciar programa\n");
        exit(1);
    }
	printf("Juego iniciado\n");
	gameStatus = GAME_STATUS_PAYING;
	sendQuestion();
}