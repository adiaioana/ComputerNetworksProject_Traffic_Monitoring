#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include <sqlite3.h> 
#include<unistd.h>    
#include<pthread.h> 
#include "string_messages.h"
#include "stri.h"
#define PORT 3000

sqlite3* USERSDB; 
struct command_arguments{
	int nr_arg;
	char argv[110][110];
};
struct command{
    int type;
    command_arguments converted;
};

int sql_query_for_users();
inline void open_DBs();
inline void close_DBs();
void *connection_handler(void *);
struct sockaddr_in server;
int server_config();
short parse(char* ptr, command_arguments &who);
short assign_commandtype(command *C,command_arguments who);

int main(int argc , char *argv[])
{
    int client_sock , c , *new_sock;
    struct sockaddr_in client;
	int socket_desc=server_config();
	open_DBs();
    int optval=1; 
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0) {
        //print the error message
        perror("[server] bind failed. Error");
        return 1;
    }
    puts("[server]bind done");

    listen(socket_desc , 3);

    puts("[server] Waiting for incoming connections...");
    c=sizeof(struct sockaddr_in);
    while(client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c))
    {
        puts("[server] Connection accepted");

        pthread_t sniffer_thread;
        new_sock = (int*)malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("[server]could not create thread");
            return 1;
        }

        puts("[server] Handler assigned");
    }
    
    close_DBs();
    if (client_sock < 0)
    {
        perror("[server] accept failed");
        return 1;
    }
    return 0;
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
            return 1;
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

void *connection_handler(void *socket_desc)
{
	
    int sock = *(int*)socket_desc;
	command_arguments argcomm; command Comm;
    int n;
	char sendBuff[100], client_message[2000];
	while((n=recv(sock,client_message,2000,0))>0)
    {
		int code=parse(client_message,argcomm);
        if(code==0) {
            strcpy(client_message,"Invalid command\n");
            Comm.type=-1;
           }
        else{
            code=assign_commandtype(&Comm,argcomm);
            if(code==0 || Comm.type==-1) {
                strcpy(client_message,"Invalid command\n");
            }
            else{
                memset(client_message,sizeof(client_message),'\0');
            //strcpy(client_message,"Identified command: ");
            //printf("ooooo %s\n",Comm.converted.argv[0]);
            strcpy(client_message, first_response[Comm.type]);
            }
            printf("[server] Identified type %d> %s\n",Comm.type,Comm.converted.argv[0]);
            
            //printf("ooooo %s\n",client_message);
        }
	send(sock,client_message,strlen(client_message),0);
	if(Comm.type==1 || Comm.type==2) // register: Server has to send SQL query to the database
	{
		if((n=recv(sock,client_message,2000,0))>0) {
			// received SQL query
			printf("[server] Received SQL query> %s\n", client_message);
		}
		else{
		printf("[server][error] Didn't receive SQL query from client!!!");
		// didn't receive anything > weird behaviour
		}
	}/**/
    }
    close(sock);

    if(n==0) {
        puts("[server] Client Disconnected");
    }
    else {
        perror("[server] recv failed");
    }
    return 0;
}
int server_config(){
	int socket_desc;
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("[server] Could not create socket");
		exit(0);
    }
    puts("[server] Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
	return socket_desc;
}

int sql_query_for_users() {
     
}

inline void open_DBs() { //flag> to add the rest DBs
  int exit = 0; 
  exit = sqlite3_open("users.csv", &USERSDB); 
  if (exit) { 
        printf("Error open DB %d\n",sqlite3_errmsg(USERSDB));
       // return (-1); 
  }
}
inline void close_DBs() { //flag> to add the rest DBs
  sqlite3_close(USERSDB); 
}