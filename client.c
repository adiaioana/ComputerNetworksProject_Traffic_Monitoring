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
#include "prereq.h"
#define PORT 3000
#define MAX_SIZE 50
struct sockaddr_in serv_addr;
int server_config();
struct info_for_threads{
int cli_sock; int id;
};
struct info_for_user{
	int iduser;	
	char First_Name[110];
	char Surname[110];
	char username[110];
	char password[110];
	int subscriptions[3]; // flag: to be modified later
};

bool IS_AUTH;
inline void REGISTRATION_FORM(int socket_desc);
inline void LOGIN_REQUEST();
inline void LOGOUT_REQUEST();
bool password_test(char* pass);
void* command_thread(void * arg);

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
    cli_th->id=1;
    
    
    printf("[client] Creating thread for handling commands... \n");
    pthread_create(&cli_thread[0],NULL, command_thread,(void *)cli_th);
    
    pthread_join(cli_thread[0], NULL); 
    pthread_exit(NULL); 
    close(sock_desc);
    return 0;

}

inline void REGISTRATION_FORM(int socket_desc) {
	info_for_user USR;
	printf("\nYour First Name: ");
	scanf("%s", USR.First_Name);
	char *pdebug;
	while(strlen(USR.First_Name)==0 || Contains_Any_Chars_From(USR.First_Name, "1234567890!@#$%^&*()`~':;|[]<>,.+=_") ) {//
		printf("\nPlease re-enter first name (no digits or signs beside -): ");
		scanf("%s", USR.First_Name);
	}
	
	printf("\nYour Surname: ");
	scanf("%s", USR.Surname);
	while(strlen(USR.Surname)==0 || Contains_Any_Chars_From(USR.Surname, "1234567890!@#$%^&*()`~':;|[]<>,.+=_") ){
		printf("\nPlease re-enter surname (no digits or signs beside -): ");
		scanf("%s", USR.Surname);
	}
	
	printf("\nYour username: ");
	scanf("%s", USR.username);
	while(strlen(USR.username)==0 || Contains_Any_Chars_From(USR.username,"!@#$%^&*()`~':;|[]<>,.+=") || !isalpha(USR.username[0]) ) {
		printf("\nPlease re-enter username (Username has to start with a letter and the only signs allowed are - and _): ");
		scanf("%s", USR.username);
	}
	
	char line[MAX_CH_ON_LINE];
	printf("Your password: ");
	read_line(line);
	strcpy(USR.password,line);
	while(password_test(USR.password)) {
		printf("\nPlease think of another password: ");
		read_line(line);
		strcpy(USR.password,line);
		printf("It worked> %s and %d\n", USR.password, password_test(USR.password));
	}
	printf("[client] It works?\n");
	 // flag: to add subscriptions
	//srand(time(NULL));
	USR.iduser=1;
	printf("[client] %d este ID", USR.iduser);
	
	char parameters[210], insert_query[350];
	memset(parameters, sizeof(parameters), '\0');
	memset(insert_query, sizeof(insert_query), '\0');
	printf("%d este %s\n", USR.iduser, int_to_string(USR.iduser));
	strcat(parameters, int_to_string(USR.iduser)); strcat (parameters,",");
	strcat(parameters, USR.First_Name); strcat (parameters,",");
	strcat(parameters, USR.Surname); strcat (parameters,",");
	strcat(parameters, USR.username); strcat (parameters,",");
	strcat(parameters, USR.password); strcat (parameters,",");
	strcat(parameters, "NULL"); strcat (parameters,","); // flag: to be modified for subscriptions
	strcat(parameters, "NULL"); strcat (parameters,","); // flag: to be modified for subscriptions
	strcat(parameters, "NULL");  // flag: to be modified for subscriptions
	sprintf(insert_query, "INSERT INTO Users VALUES('%s')", parameters);
	printf("[client] Sending the following query> %s\n",insert_query);
	//send(socket_desc,insert_query,strlen(insert_query),0);
}

void* command_thread(void * arg)
{
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    int sock_desc=cli_th->cli_sock;
    printf("[client] Print command:\n");
    while(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
    {
        send(sock_desc,sbuff,strlen(sbuff),0);
        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
           printf("[client] Error");
        else {
        	fputs(rbuff,stdout);
        	// Authentification protocol
        	if(strcmp(rbuff,first_response[1])==0) //register
        		REGISTRATION_FORM(sock_desc);
        	else if(strcmp(rbuff, first_response[2])==0) //login
			LOGIN_REQUEST();
		else if(strcmp(rbuff,first_response[3])==0)
			LOGOUT_REQUEST();
		else printf("[client] Not solved yet");
        }
        bzero(rbuff,MAX_SIZE);
 
    }
    pthread_exit(NULL);
}

inline void LOGIN_REQUEST() {

}
inline void LOGOUT_REQUEST() {

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
