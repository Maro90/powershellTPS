#include "Users.h"

int userCount = 0;

tUserNode * userList = NULL;

tUserNode * getUser(int connectionId){
    tUserNode * tmpUser = userList;
    while(tmpUser){
        if(tmpUser->connection.id == connectionId){
            return tmpUser;
        }
        tmpUser = tmpUser->prev;
    }
    return NULL;
}

tUserNode * addUser(tConnection connection){
    tUserNode * tmpUser = (tUserNode *)malloc(sizeof(tUserNode));
    if(!tmpUser)
        return 0;
    tmpUser->connection = connection;
    if(userList){
        tmpUser->prev = userList;
    }else{
        tmpUser->prev = NULL;
    }
    userCount++;
    tmpUser->connection.id=userCount;
    tmpUser->status=1;
    userList = tmpUser;
    return tmpUser;
}

void freeUserList(){
    tUserNode * tmpUser;
    while(userList){
    	close(userList->connection.descriptor);
        tmpUser = userList;
        userList = userList->prev;
        free(tmpUser);
    }
}

void userQuit(int connectionId, int status){
    tUserNode * tmpUser;
    tmpUser = getUser(connectionId);
    printf("Usuario desconectado id: %d nombre: %s \n",connectionId,tmpUser->user.name );
    if(tmpUser){
        tmpUser->status=status;
    }
}

void initController(tMessageInit m, tConnection * connection){
    tUserNode * tmpUser = getUser(connection->id);
    tCommand cmd;
    strcpy(tmpUser->user.name,m.user);
    if(tmpUser->status == USER_STATUS_OK){
    	printf("Usuario logueado id: %d  nombre: %s \n",connection->id,tmpUser->user.name);
	    cmd.commandId = COMMAND_ACCEPT;
	    tMessageAccept msg_accept;
	    msg_accept.id = connection->id;
	    strcpy(msg_accept.message,"Conectado - El juego iniciara en breve");
	    if(send_command(&cmd,&msg_accept,connection)==0){
	    	userQuit(connection->id,USER_STATUS_CONNECTIONERROR);
	    }
	}else{
    	printf("Usuario rechazado id: %d  nombre: %s \n",connection->id,tmpUser->user.name);
    	cmd.commandId=COMMAND_QUIT;
    	tMessageQuit msg_quit;
    	strcpy(msg_quit.message,"El juego ya inicio, intente mas tarde");
    	send_command(&cmd,&msg_quit,connection);
	}
}

int send_all(tCommand * cmd,void * data){
	int count = 0;
	tUserNode * tmpUser = userList;
    while(tmpUser){
        if(tmpUser->status == USER_STATUS_OK && send_command(cmd,data,&(tmpUser->connection)) == 0){
        	tmpUser->status=USER_STATUS_CONNECTIONERROR;
        }else{
        	count ++;
        }
        tmpUser = tmpUser->prev;
    }
    return count;
}


int getUserCount(){ return userCount;}