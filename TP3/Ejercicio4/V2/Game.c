#include "Game.h"

int gameStatus = GAME_STATUS_WAITING;
pthread_mutex_t answer_lock;

int answered = 0;
int respuesCorrecta=3;
int gameActivesUsers;

void answerController(tMessageAnswer msg, tConnection * connection){
   tUserNode * tmpUser = getUser(connection->id);
   	pthread_mutex_lock(&answer_lock);
   	if(!answered){
	   	tmpUser->statistics.count++;
	   	if(msg.id == respuesCorrecta){
	   		printf("El usuario %s respondio correctamente primero \n", tmpUser->user.name );
	   		tmpUser->statistics.correct++;
	   	}else{
	   		printf("El usuario %s respondio incorrectamente primero \n", tmpUser->user.name );
	   		tmpUser->statistics.incorrect++;
	   	}
	}else{
	   	printf("El usuario %s respondio tarde \n", tmpUser->user.name );	
	}
	answered++;
	pthread_mutex_unlock(&answer_lock);
}

void newUser(int socket){
	tConnection connection;
    tUserNode * tmpUser;
    connection.initController = initController;
    connection.acceptController = NULL;
    connection.answerController = answerController;

    connection.descriptor = socket;
    tmpUser = addUser(connection);
    if(gameStatus!=GAME_STATUS_WAITING){
    	tmpUser->status=USER_STATUS_GAMEALREADYSTARTED;
    }
    if(tmpUser && pthread_create( &(tmpUser->threadId), NULL ,  handler_run , (void*) &(tmpUser->connection)) < 0){
        printf("Error al establecer conexion\n");
        tmpUser->status=USER_STATUS_CONNECTIONERROR;
    }

    if(getUserActivesCount() == 2){
    	startGame();
    }
}

void configGame(){
	loadQuestions();
}

void sendQuestion(tQuestion * question){
	answered=0;
	gameActivesUsers = getUserActivesCount();
	tCommand cmd;
	cmd.commandId = COMMAND_QUESTION;
	
	send_all(&cmd,&question->questionMsg);
	while(gameActivesUsers != answered){
		usleep(100);
	}
	printf("Todos los usuarios respondieron\n");
}

void startGame(){
	if (pthread_mutex_init(&answer_lock, NULL) != 0)
    {
        printf("\nFallo al iniciar programa\n");
        exit(1);
    }
	printf("Juego iniciado\n");
	gameStatus = GAME_STATUS_PAYING;
	tQuestion * questions = getQuestions();
	while(questions){
		sendQuestion(questions);
		questions=questions->next;
	}
	printf("Fin del juego\n");
}

void endGame(){
	freeUserList();
}