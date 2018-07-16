#include "Questions.h"

tQuestion * getQuestions(){ return questions;}

void loadQuestions() {
    int count = 0;
    int ansPos = 0;
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    tQuestion * lastQ = NULL;
    tQuestion * tmpQ = NULL;
    fp = fopen("preguntas.txt", "r");
    if (fp == NULL) {
        printf("No se encontro el archivo de preguntas \n");
        exit(1);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        char type = line[0];
        if( type == 'P'){
        	if(tmpQ!=NULL){
        		if(lastQ==NULL){
        		questions = tmpQ;
	        	}else{
	        		lastQ->next = tmpQ;
	        	}
	        	lastQ = tmpQ;
        	}
            ansPos = 0;
            count++;
            tmpQ = (tQuestion *)malloc(sizeof(tQuestion));
            tmpQ->next=NULL;
            tmpQ->questionMsg.ans = -1;
            if(tmpQ== NULL){
            	printf("Archivo de preguntas muy grande, memoria insuficiente\n");
        		exit(1);
            }
            strcpy(tmpQ->questionMsg.title,line+2);
        } else if ( type == 'R' ) {
        	if(line[1]=='C'){
        		if(tmpQ->questionMsg.ans!=-1){
	        		printf("Archivo de preguntas invalido, una pregunta tiene mas de una respuesta\n");
	        		exit(1);	
	        	}
	        	tmpQ->questionMsg.ans = ansPos;
        	}
        	strcpy(tmpQ->questionMsg.answers[ansPos],line+3);
        	ansPos++;
        }
    }
	if(tmpQ && lastQ==NULL){
        questions = tmpQ;
    }else{
        lastQ->next = tmpQ;
    }
    fclose(fp);
    printf("Se cargaron %d preguntas\n", count);
 }