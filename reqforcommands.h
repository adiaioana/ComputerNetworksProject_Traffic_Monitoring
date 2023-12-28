/**
	Functions for command handling such as:
	1. REGISTRATION_FORM: used to get username, pass, full name and sends SQL query to server
	2. LOGIN_REQUEST: reads username, pass => sends SQL query to server 
	3. LOGOUT_REQUEST: logs out if logged in
**/
#include <stdio.h>
#include <string.h>
#include <sqlite3.h> 
#include "prereq.h"
#define MAX_CH_ON_LINE 100


struct info_for_user{
	int iduser;	
	char First_Name[110];
	char Surname[110];
	char username[110];
	char password[110];
	int subscriptions[3]; // flag: to be modified later
}; 
 
inline void REGISTRATION_FORM(int socket_desc) {
	char line[MAX_CH_ON_LINE];
	info_for_user USR;
	printf("\n[client][command] Your First Name: ");
	read_line(line);
	strcpy(USR.First_Name,line);
	char *pdebug;
	while(strlen(USR.First_Name)==0 || Contains_Any_Chars_From(USR.First_Name, "1234567890!@#$%^&*()`~':;|[]<>,.+=_") ) {//
	    printf("\n[client][command] Please re-enter first name (no digits or signs beside -): ");
	    read_line(line);
	    strcpy(USR.First_Name,line);
	}
	
	printf("\n[client][command] Your Surname: ");
	read_line(line);
	strcpy(USR.Surname,line);
	while(strlen(USR.Surname)==0 || Contains_Any_Chars_From(USR.Surname, "1234567890!@#$%^&*()`~':;|[]<>,.+=_") ){
		printf("\n[client][command] Please re-enter surname (no digits or signs beside -): ");
		read_line(line);
	        strcpy(USR.Surname,line);
	}
	
	printf("\n[client][command] Your username: ");
	read_line(line);
	strcpy(USR.username,line);
	while(strlen(USR.username)==0 || Contains_Any_Chars_From(USR.username,"!@#$%^&*()`~':;|[]<>,.+=") || !isalpha(USR.username[0]) ) {
		printf("\n[client][command] Please re-enter username (Username has to start with a letter and the only signs allowed are - and _): ");
		read_line(line);
	        strcpy(USR.username,line);
	}
	
	printf("\n[client][command] Your password: ");
	read_line(line);
	strcpy(USR.password,line);
	while(password_test(USR.password)) {
		printf("\n[client][command] Please think of another password: ");
		read_line(line);
		strcpy(USR.password,line);
	}
	char promt[4][120]={"[client][command] Your subscription preference for weather information [Y/n]",
	"[client][command] Your subscription preference for sports channel [Y/n]",
	"[client][command] Your subscription preference for peco information [Y/n]"};
	
	// flag: to modify subscriptions
	for(int ind_for_promt=0; ind_for_promt<3; ++ind_for_promt) {
	printf("\n%s", promt[ind_for_promt]);
	read_line(line);
	while(notyesorno(line)) { //flag: notyesorno doesn't work
	  printf("\n[client][command]Please provide a [Y/n]: ");
	  read_line(line);
	}
	if(strchr(line,'Y')!=NULL or strchr(line,'y')!=NULL) 
	  USR.subscriptions[ind_for_promt]=1;
	else USR.subscriptions[ind_for_promt]=0;
	}
	
	USR.iduser=rand()%1000000;
	printf("[client][command] %d este ID", USR.iduser);
	/* flag: something doesn't work with the send query
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
	//send(socket_desc,insert_query,strlen(insert_query),0);*/
}

inline void LOGIN_REQUEST(int socket_desc, int * token, pthread_mutex_t * lacatel) {

	char line[MAX_CH_ON_LINE];
	info_for_user USR;
	pthread_mutex_lock(lacatel);
        if(*token) {
          printf("[client][command] User already logged in, for logging in another user, log out first\n");
        }
        else {
        printf("[client][command] For logging in, your username and password are required...\n");
        printf("\nY[client][command] our username: ");
	read_line(line);
	strcpy(USR.username,line);
	printf("\n[client][command] Your password: ");
	read_line(line);
	strcpy(USR.password,line);
	
	*token=0;
	
	if(strlen(USR.username)==0 || 
	Contains_Any_Chars_From(USR.username,"!@#$%^&*()`~':;|[]<>,.+=") || 
	!isalpha(USR.username[0]) ) {
	      printf("[client][command] Not logged in! (found invalid characters in username)\n");
	      *token=0;
	}
	
	else if(password_test(USR.password)) {
		printf("[client][command] Not logged in! (found invalid characters in username)\n");
		*token=0;
	}
	else{
        //flag: SQL query
        }
        pthread_mutex_unlock(lacatel);
	}
        //flag: to implement SQL query + sending to server
}

inline void LOGOUT_REQUEST(int socket_desc, int * token, pthread_mutex_t * lacatel) {
      
      pthread_mutex_lock(lacatel);
      *token=0;
      if(*token) {
        printf("[client][command] Logging out.\n");
      }
      else
        printf("[client][command] Not logged in: Error\n");
      pthread_mutex_unlock(lacatel);
}
