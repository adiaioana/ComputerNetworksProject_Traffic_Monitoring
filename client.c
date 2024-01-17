#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <chrono>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#include "reqforcommands.h"

#define PORT 3000
#define MAXSIZE 350
/* Note: "///" comments are meant for verification, only verified 
functions/variables/code sections have these comments */

struct sockaddr_in serv_addr;
int ind_of_client=0;
struct info_for_threads{
int cli_sock; int id;
};

struct command_arguments{
	int nr_arg;
	char argv[110][110];
};
struct command{
    int type;
    command_arguments converted;
};


int THE_END;
int IS_AUTH;

pthread_mutex_t auth_lock=  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t notif_lock=  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_lock=  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_lock=  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t comm_lock=  PTHREAD_MUTEX_INITIALIZER;

void* command_thread(void * arg);
void* warnings_thread(void * arg);
void* events_thread(void * arg);
void* main_thread(void* arg);

void  comm_send_receive(int sd,char * messsage_sent,char * message_recv);

inline void command_output(char* OUTPUT, int cd,pthread_mutex_t* print_lacatel); /// i think so
//cd is 0 if output doesn't have to be erased after printing as in for constant strings
int server_config();///
short parse(char* ptr, command_arguments &who); ///
short assign_commandtype(command *C,command_arguments who); ///

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
    pthread_create(&cli_thread[1],NULL, warnings_thread,(void *)cli_th);
    //pthread_create(&cli_thread[2],NULL, events_thread,(void *)cli_th);
    
    // bonus: Figure out if you should detach instead of join
    pthread_join(cli_thread[0], NULL); 
    pthread_join(cli_thread[1], NULL); 
    //pthread_join(cli_thread[2], NULL); 
    
    pthread_exit(NULL); 
    
    pthread_mutex_destroy(&auth_lock);
    pthread_mutex_destroy(&print_lock);
    pthread_mutex_destroy(&send_lock);
    pthread_mutex_destroy(&comm_lock);
    
    close(sock_desc);
    return 0;

}
void* main_thread(void * arg) {

}
void* warnings_thread(void * arg) { 
//Fun fact: main thread 🤝 warnings_thread
    info_for_threads * cli_th=(info_for_threads *)arg;
    int sock_desc=cli_th->cli_sock;
    
    char message_sent[50];
    char message_recv[MAXSIZE];
    while(1) {
    
      sleep(1);
      strcpy(message_sent,"AUTHOR");
      comm_send_receive(sock_desc, message_sent, message_recv);
      
      if(strstr(message_recv,"YES:")!=NULL) {
        pthread_mutex_lock(&notif_lock);
        strcpy(message_sent,"GINFO");
        comm_send_receive(sock_desc, message_sent, message_recv);
        fflush(stdout);
       usleep(190);
       printf("\033[s");
        printf("\033[2A\033[K\033[0;36m\r[client][notif] Your speed and location: %s\n\033[0m", message_recv);
        printf("\033[u");
        fflush(stdout);
        usleep(190);
        bzero(message_recv,MAXSIZE);
        pthread_mutex_unlock(&notif_lock);
      }
      
      if(THE_END)
        break;
    }
}
void  comm_send_receive(int sd,char * messsage_sent,char * message_recv)
{
        pthread_mutex_lock(&comm_lock);
        
        send(sd,messsage_sent,strlen(messsage_sent),0);
        if(recv(sd,message_recv,MAXSIZE,0)==0)
           printf("[client][?] Error");
          
	pthread_mutex_unlock(&comm_lock);
}

void* events_thread(void * arg)
{
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAXSIZE],rbuff[MAXSIZE];
    int sock_desc=cli_th->cli_sock;
    char message_sent[50];
    char message_recv[MAXSIZE];
    while(1) {
      
      sleep(1);
      strcpy(message_sent,"AUTHOR");
      comm_send_receive(sock_desc, message_sent, message_recv);
      
      if(strstr(message_recv,"YES:")!=NULL) {
        strcpy(message_sent,"GEVENT");
        comm_send_receive(sock_desc, message_sent, message_recv);
        
        if(!strcmp(message_recv,"No data")) {
        bzero(message_recv,MAXSIZE);
        continue;
        }
        
        pthread_mutex_lock(&notif_lock);
        usleep(390);
        fflush(stdout);
        printf("\033[s");
        printf("\033[2A\033[K\033[0;36m\r[client][event]  New notification! %s\n\033[0m", message_recv);
        printf("\033[u");
        fflush(stdout);
        usleep(390);
        bzero(message_recv,MAXSIZE);
        pthread_mutex_unlock(&notif_lock);
      }
      
      if(THE_END)
        break;
    }
}

void* command_thread(void * arg)
{
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAXSIZE],rbuff[MAXSIZE], input[MAXSIZE];
    int sock_desc=cli_th->cli_sock;
    
    command_arguments argcomm; command Comm;
    char Printing_command_mess[50]="Print command:";
    char Invalid_command_mess[50]="Invalid command";
    char Not_solved_mess[50]="Not solved yet";
    
    command_output(Printing_command_mess,0,&print_lock);
    while(1)
    {
        printf("\033[34m>>\033[0m ");
        fflush(stdout);
        if(!(fgets(input, MAXSIZE , stdin)!=NULL))
          break;
        
        int code=parse(input,argcomm);
        if(code==11) { //help;
          pthread_mutex_lock(&print_lock);
          printf("[client][command] %s\n", help_comm);
          fflush(stdout);
          pthread_mutex_unlock(&print_lock);
            command_output(Printing_command_mess,0,&print_lock);
	    continue;
        }
        if(code==0) {
            command_output(Invalid_command_mess,0,&print_lock);
            command_output(Printing_command_mess,0,&print_lock);
	    continue;
        }
        code=assign_commandtype(&Comm,argcomm);
        if(code==0) {
            command_output(Invalid_command_mess,0,&print_lock);
            command_output(Printing_command_mess,0,&print_lock);
	    continue;
        }
        // Authentification protocol
        if(code>=1 && code<=3) {
        switch(code) {
        case 1: REGISTRATION_FORM(sock_desc, sbuff, &send_lock,&print_lock); break;
        case 2: LOGIN_REQUEST(sock_desc, &IS_AUTH, &auth_lock, sbuff, &send_lock,&print_lock);break;
        case 3: LOGOUT_REQUEST(sock_desc, &IS_AUTH, &auth_lock, sbuff, &send_lock);break;
	}
	// Event handling
	else if(code>=4 && code<=5) {
	switch (code) {
	case 4: REPORT_EVENT(sock_desc, &print_lock, sbuff, &send_lock,&print_lock);break;
	case 5:GET_EVENTS(sock_desc, &print_lock, sbuff, &send_lock);break;
	}
	else if(code==6) { //get-info: useless
	pthread_mutex_lock(&send_lock);
        pthread_mutex_lock(&print_lock);
        strcpy(sbuff,server_comm_coding[6]);
        pthread_mutex_unlock(&send_lock);
        pthread_mutex_unlock(&print_lock);
	}
	else if(code>=7 && code<=8){
          switch(code) {
          case 7: SUBSCRIBE_REQ(sock_desc, &IS_AUTH, &auth_lock, sbuff, &send_lock,&print_lock); break;
          case 8:SUBSCR_INFO(sock_desc, &IS_AUTH, &auth_lock, sbuff, &send_lock); break;
          }
        }
	else if(code==9){ //is-auth
	pthread_mutex_lock(&send_lock);
        pthread_mutex_lock(&print_lock);
        strcpy(sbuff,server_comm_coding[9]);
        pthread_mutex_unlock(&send_lock);
        pthread_mutex_unlock(&print_lock);
        }
        else if(code==10) { //exit
          printf("Exiting app..");
          sleep(1);
          exit(0);
        }
        else {
	    strcpy(sbuff,"UNKN");
            printf("[client][V]Sending: %s\n", sbuff);
            comm_send_receive(sock_desc,sbuff,rbuff);
	    command_output(Not_solved_mess,0,&print_lock);
            command_output(Printing_command_mess,0,&print_lock);
	    continue;
        }
        printf("[client][V]Sending: %s\n", sbuff);
        comm_send_receive(sock_desc,sbuff,rbuff);
        
        command_output(rbuff,1,&print_lock);
        
        bzero(input,MAXSIZE);
        bzero(sbuff,MAXSIZE);
        command_output(Printing_command_mess,0,&print_lock);
    }
    THE_END=1; //flag> should have lock, it's a shared resource + makes no sense
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


short assign_commandtype(command *C,command_arguments who)
{
    C->converted=who;
    C->type=-1;
    for(int j=1; j<=number_of_command_strings; j++) {
       // printf("comparing %s with %s\n", who.argv[0], command_strings[j]);

        if(strcmp(who.argv[0],command_strings[j])==0) {
            //printf("Identified command %s type %d\n",who.argv[0],j);
            C->type=j;
            return C->type;
        }

    }
    return 0;
}
short parse(char* ptr, command_arguments &who)
{
    char* qtr=ptr;
    int lg=strlen(qtr);
    if(lg==0)
        return 0;

    char *itr=strtok(qtr," \t\n"); who.nr_arg=0;
    while(itr!=NULL) 
    {
        if(strlen(itr)>0) {
        strcpy(who.argv[who.nr_arg],itr);
        who.nr_arg++;
        }
        itr=strtok(NULL, " \t\n");
    }
    
    return 1;
}
