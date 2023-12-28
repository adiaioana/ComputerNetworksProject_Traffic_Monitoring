#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include "string_messages.h"
#include "stri.h"
#include "reqforcommands.h"
//#include "prereq.h"
#define PORT 3000
#define MAX_SIZE 50
struct sockaddr_in serv_addr;
int server_config();
int ind_of_client=0;
struct info_for_threads{
int cli_sock; int id;
};

int THE_END;

pthread_mutex_t auth_lock;
int IS_AUTH;
bool password_test(char* pass);
void* command_thread(void * arg);
void* main_thread(void * arg);

int main()
{
    int sock_desc=server_config();
    srand(time(NULL));
    
    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("[client] Failed to connect to server\n");
        return -1;
    }
    printf("[client] Connected successfully - Starting... \n");
    
	
    pthread_t cli_thread[4];
    info_for_threads * cli_th = new info_for_threads;
    cli_th->cli_sock=sock_desc;
    cli_th->id=++ind_of_client;
    
    
    printf("[client] Creating thread for handling commands... \n");
    pthread_create(&cli_thread[0],NULL, command_thread,(void *)cli_th);
    pthread_create(&cli_thread[1],NULL, main_thread,(void *)cli_th);
    
    pthread_join(cli_thread[0], NULL); 
    pthread_exit(NULL); 
    pthread_mutex_destroy(&auth_lock);
    close(sock_desc);
    return 0;

}
void* main_thread(void * arg) {
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    int sock_desc=cli_th->cli_sock;
    char mess[250];
    printf("[client][main] Printing speed:\n");
    while(!THE_END) {
      if(IS_AUTH) {
        int nraux=rand()%120; //flag: query to server to be implemented
        snprintf(mess,sizeof(mess),"\n[client][main] Your speed[km/h] is:%d", nraux);
        printf("%s", mess);
        //printf("\r", nraux);
        fflush(stdout);
        sleep(1);
      }
    }
    printf("\n[client][main] Application exited! See you later! \n");
}

void* command_thread(void * arg)
{
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    int sock_desc=cli_th->cli_sock;
    printf("[client][command] Print command:\n");
    while(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
    {
        send(sock_desc,sbuff,strlen(sbuff),0);
        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
           printf("[client][command] Error");
        else {
        	fputs(rbuff,stdout);
        	// Authentification protocol
        	if(strcmp(rbuff,first_response[1])==0) //register
        		REGISTRATION_FORM(sock_desc);
        	else if(strcmp(rbuff, first_response[2])==0) { //login
			LOGIN_REQUEST(sock_desc, &IS_AUTH, &auth_lock);
		        IS_AUTH=1; //debugflag: remove it!!!	
		}
		else if(strcmp(rbuff,first_response[3])==0)
			LOGOUT_REQUEST(sock_desc, &IS_AUTH, &auth_lock);
		else printf("[client][command] Not solved yet");
        }
        bzero(rbuff,MAX_SIZE);
        printf("[client][command] \nPrint command:\n");
    }
    THE_END=1;
}

int server_config() {
    int sock_desc;
    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("[client] Failed creating socket\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);
    return sock_desc;
}
