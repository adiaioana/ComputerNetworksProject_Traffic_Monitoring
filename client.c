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
#include "reqforcommands.h"
//#include "prereq.h"
#define PORT 3000
#define MAXSIZE 100
struct sockaddr_in serv_addr;
int server_config();
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

pthread_mutex_t auth_lock=  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t print_lock=  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t send_lock=  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t comm_lock=  PTHREAD_MUTEX_INITIALIZER;

int IS_AUTH;
bool password_test(char* pass);
void* command_thread(void * arg);
void* main_thread(void * arg);
void* events_thread(void * arg);
void  comm_send_receive(int sd,char * messsage_sent,char * message_recv);

short parse(char* ptr, command_arguments &who);
short assign_commandtype(command *C,command_arguments who);

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
    //pthread_create(&cli_thread[1],NULL, main_thread,(void *)cli_th);
    //pthread_create(&cli_thread[2],NULL, events_thread,(void *)cli_th);
    
    pthread_join(cli_thread[0], NULL); 
    //pthread_join(cli_thread[1], NULL); 
    //pthread_join(cli_thread[2], NULL); 
    pthread_exit(NULL); 
    pthread_mutex_destroy(&auth_lock);
    close(sock_desc);
    return 0;

}
void* main_thread(void * arg) {
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAXSIZE],rbuff[MAXSIZE];
    int sock_desc=cli_th->cli_sock;
    char mess[250];
    printf("[client][main] Printing speed:\n");
    while(!THE_END) {
      sleep(1);
      if(IS_AUTH) {
        int nraux=rand()%120; //flag: query to server to be implemented
        snprintf(mess,sizeof(mess),"\n[client][main] Your speed[km/h] is:%d", nraux);
        printf("%s", mess);
        fflush(stdout);
        sleep(1);
      }
    }
    printf("\n[client][main] Application exited! See you later! \n");
}
void  comm_send_receive(int sd,char * messsage_sent,char * message_recv)
{
        pthread_mutex_lock(&comm_lock);
        
        send(sd,messsage_sent,strlen(messsage_sent),0);
        if(recv(sd,message_recv,MAXSIZE,0)==0)
           printf("[client][command] Error");
          
	pthread_mutex_unlock(&comm_lock);
}

void* events_thread(void * arg)
{
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAXSIZE],rbuff[MAXSIZE];
    int sock_desc=cli_th->cli_sock;
    /*
    while(1) {
      sleep(1);
      char message_sent[250];
      char message_recv[250];
      strcpy(message_sent,"get-events");
      comm_send_receive(sock_desc, messsage_sent, message_recv);
      
      if(!strcmp(message_recv,NoData)) {
      pthread_mutex_lock(&print_lock);
      printf("[client][events] New notification!\n%s", message_recv);
      pthread_mutex_lock(&print_unlock);
      }
      
      if(THE_END)
        break;
    }*/
}

void* command_thread(void * arg)
{
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAXSIZE],rbuff[MAXSIZE], input[MAXSIZE];
    int sock_desc=cli_th->cli_sock;
    
    command_arguments argcomm; command Comm;
    printf("[client][command] Print command:\n");
    while(fgets(input, MAXSIZE , stdin)!=NULL)
    {
        int code=parse(input,argcomm);
        //comm_send_receive(sock_desc,sbuff,rbuff);
        if(code==0) {
	    printf("[client][command] Invalid command");
            printf("\n[client][command] Print command:\n");
	    continue;
        }
        code=assign_commandtype(&Comm,argcomm);
        if(code==0) {
	    printf("[client][command] Invalid command");
            printf("\n[client][command] Print command:\n");
	    continue;
        }
        // Authentification protocol
        if(code>=1 && code<=3) {
                if(code==1) //register
        	  REGISTRATION_FORM(sock_desc, sbuff, &send_lock);
                else if(code==2) { //login
		  LOGIN_REQUEST(sock_desc, &IS_AUTH, &auth_lock, sbuff, &send_lock);
		//IS_AUTH=1; //debugflag: remove it!!!	
	        }
	        else if(code==3)
	            LOGOUT_REQUEST(sock_desc, &IS_AUTH, &auth_lock, sbuff, &send_lock);
	}
	// Event handling
	else if(code>=4 && code<=5) {
	      if(code==4)//report event
		REPORT_EVENT(sock_desc, &print_lock, sbuff, &send_lock); //flag: re-verify 1st lock
	      else if(code==5) //get-events
		GET_EVENTS(sock_desc, &print_lock, sbuff, &send_lock); //flag: re-verify 1st lock
	}
	else{
	    strcpy(sbuff,"UNKN");
	    printf("[client][command] Not solved yet");
            printf("\n[client][command] Print command:\n");
	    continue;
        }
        send(sock_desc,sbuff,strlen(sbuff),0);
        bzero(input,MAXSIZE);
        bzero(sbuff,MAXSIZE);
        printf("\n[client][command] Print command:\n");
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
