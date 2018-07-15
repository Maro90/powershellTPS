#include "Protocol.h"

void * handler_run(void *args){
	int readedLen = 0;
	tConnection * connection = args;
	tCommand command;
	tMessageInit msg_init;
	tMessageAccept msg_accept;
	tMessageQuit msg_quit;
	tMessageStatistics msg_statistics;
	tMessageQuestion msg_question;
	tMessageAnswer msg_answer;

	while(readedLen!= -1){
		readedLen = read_socket(&command, sizeof(tCommand),connection);

		if(readedLen == 0){
			usleep (100);
			continue;
		}

		switch(command.commandId){
			case COMMAND_INIT :
				read_socket(&msg_init, sizeof(tMessageInit),connection);
				connection->initController(msg_init,connection);
				break;
			case COMMAND_ACCEPT :
				read_socket(&msg_accept, sizeof(tMessageAccept),connection);
				connection->acceptController(msg_accept,connection);
				break;
			case COMMAND_QUIT :
				read_socket(&msg_quit, sizeof(tMessageQuit),connection);
				connection->quitController(msg_quit,connection);
				break;
			case COMMAND_ANSWER :
				read_socket(&msg_answer, sizeof(tMessageAnswer),connection);
				connection->answerController(msg_answer,connection);
				break;
			case COMMAND_QUESTION :
				read_socket(&msg_question, sizeof(tMessageQuestion),connection);
				connection->questionController(msg_question,connection);
				break;
			case COMMAND_STATISTICS :
				read_socket(&msg_statistics, sizeof(tMessageStatistics),connection);
				connection->statisticsController(msg_statistics,connection);
				break;
			default:
				printf("Mensaje desconocido!\n");
				exit(1);
			break;
		}
	}

	free(args);
}

int read_socket (void *data, int len, tConnection * connection) {
	int readed = 0;
	int aux = 0;
	if ((connection->descriptor == -1) || (data == NULL) || (len < 1)){
		return -1;
	}
	while (readed < len) {
		aux = recv(connection->descriptor, data + readed, len - readed,MSG_WAITALL);
		if (aux > 0) {
			readed = readed + aux;
		} else {
			if (aux == 0) 
				return readed;
			if (aux == -1) {
				switch (errno) {
					case EINTR:
					case EAGAIN:
						usleep (100);
						break;
					default:
						return -1;
				}
			}
		}
	}
	return readed;
}

int send_command(tCommand * cmd,void * data, tConnection * connection){
	int len;
	int total;
	total = send_socket(cmd,sizeof(tCommand),connection);
	switch(cmd->commandId){
		case COMMAND_INIT :
			len = sizeof(tMessageInit);
			break;
		case COMMAND_ACCEPT :
			len = sizeof(tMessageAccept);
			break;
		case COMMAND_QUIT:
			len = sizeof(tMessageQuit);
			break;
		case COMMAND_QUESTION:
			len = sizeof(tMessageQuestion);
			break;
		case COMMAND_ANSWER:
			len = sizeof(tMessageAnswer);
			break;
		case COMMAND_STATISTICS:
			len = sizeof(tMessageStatistics);
			break;
	}
	total += send_socket(data,len,connection);
	return total;
}

int send_socket (void * data,int len , tConnection * connection) {
	int sended = 0;
	int aux = 0;

	if ((connection->descriptor == -1) || (data == NULL) || (len < 1))
		return -1;
	while (sended < len) {
		aux = send(connection->descriptor, data + sended, len - sended,0);
		if (aux > 0) {
			sended = sended + aux;
		} else {
			if (aux == 0)
				return sended;
			else
				return -1;
		}
	}
	return sended;
}
