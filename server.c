#include<stdio.h>
#include<string.h>    
#include<stdlib.h>    
#include<sys/socket.h>
#include<arpa/inet.h> 
#include <sqlite3.h> 
#include<unistd.h>    
#include<pthread.h> 
#include "string_messages.h"
#include "server_commands_exec.h"
#include "stri.h"
#define PORT 3000

void *connection_handler(void *);
struct sockaddr_in server;
int server_config();
inline void executioner(char *INPUT, char *OUTPUT);
short server_parse(char* ptr, command_arguments &who);
short assign_commandtype(command *C,command_arguments who);

int main(int argc , char *argv[])
{
    int client_sock , c , *new_sock;
    struct sockaddr_in client;
	int socket_desc=server_config();
	open_DB();
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
    
    close_DB();
    if (client_sock < 0)
    {
        perror("[server] accept failed");
        return 1;
    }
    return 0;
}

inline void executioner(char *INPUT, char *OUTPUT) {
  command Inp_C;
  command_arguments Inp_who;
  server_parse(INPUT,Inp_who);
  assign_commandtype(&Inp_C,Inp_who);
  /* did it for the debug and... It works <3
  sprintf(OUTPUT, "Detected %d with %d args: ", Inp_C.type, Inp_C.converted.nr_arg);
  for(int i=0; i<Inp_C.converted.nr_arg; ++i)
    strcat(OUTPUT,Inp_C.converted.argv[i]);*/
    
  if(Inp_C.type>=1 && Inp_C.type<=3) //Authentification: REG, LOGin, LOGout
  {
    int exec_code;
    switch(Inp_C.type) {
    case 1: exec_code=Register(&Inp_who, OUTPUT); break;   
    case 2: exec_code=LogIn(&Inp_who, OUTPUT); break;
    case 3: exec_code=LogOut(&Inp_who, OUTPUT);  break;
    case 9: exec_code=Author(OUTPUT); break;
    case 10: exec_code=0; break; //exit
    }
    
    if(!exec_code) {
    printf("[server] Failed to execute %s",command_strings[Inp_C.type]);
    }
  }
  else if(Inp_C.type>=4 && Inp_C.type<=5) //Event: REVENT or GEVENT
  {
  
  }
  else if(Inp_C.type==6) //Get Info: Speed, Location
  {
  
  }
  else{
    //flag: To be continued
  }
  
  if(strlen(OUTPUT)==0) {
  strcpy(OUTPUT,"Server is dumb dumb, sorry");
  }
}

void *connection_handler(void *socket_desc)
{
	
    int sock = *(int*)socket_desc;
	//command_arguments argcomm; command Comm;
    int n;
	char sendBuff[100], client_message[2000];
	while((n=recv(sock,client_message,2000,0))>0)
    {
        printf("[server] Received message> %s\n", client_message);
        executioner(client_message, sendBuff);
        send(sock,sendBuff,strlen(sendBuff),0);
        bzero(client_message,2000);
   
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


short assign_commandtype(command *C,command_arguments who)
{
    C->converted=who;
    C->type=-1;
    for(int j=1; j<=number_of_command_strings; j++) {
       // printf("comparing %s with %s\n", who.argv[0], command_strings[j]);

        if(strcmp(who.argv[0],server_comm_coding[j])==0) {
            //printf("Identified command %s type %d\n",who.argv[0],j);
            C->type=j;
            return 1;
        }

    }
    return 0;
}
short server_parse(char* ptr, command_arguments &who)
/* In client parser, the separators are '\t', '\n' or ' '. 
Server receives [server_comm_coding: 1] or [1]|[Log in/register info]|[SQL Query] or
[1][Report event]|[Event-info] 
majorflag: What should logout send to the server? */
{
    char* qtr=ptr;
    int lg=strlen(qtr);
    if(lg==0)
        return 0;

    char *itr=strtok(qtr,"|"); who.nr_arg=0;
    while(itr!=NULL) 
    {
        if(strlen(itr)>0) {
        strcpy(who.argv[who.nr_arg],itr);
        who.nr_arg++;
        }
        itr=strtok(NULL, "|");
    }
    
    return 1;
}


 /*
	int code=parse(client_message,argcomm);
        if(code==0) {
            strcpy(client_message,"Invalid command\n");
            Comm.type=-1;
           }
        else{
            code=assign_commandtype(&Comm,argcomm);
            if(code==0 || Comm.type==-1) {
                strcpy(client_message,"Invalid command\n");
            }C791-0BCE
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
	}*/
