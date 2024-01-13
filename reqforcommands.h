/**
	Functions for command handling such as:
	1. REGISTRATION_FORM: used to get username, pass, full name and sends SQL query to server
	2. LOGIN_REQUEST: reads username, pass => sends SQL query to server 
	3. LOGOUT_REQUEST: logs out if logged in
	4. REPORT_EVENT: reports event
	5. GET_EVENT: get events info
**/
#include <stdio.h>
#include <string.h>
#include <sqlite3.h> 
#include <iterator>
#include "prereq.h"
#include "stri.h"
#include <deque>
#define MAX_CH_ON_LINE 100
int NO_EVENTS;
struct event{
char message[250];
int start;
int idevent;
int lifetime;
};
std:: deque <event> events_list;

struct info_for_user{
	int iduser;	
	char First_Name[110];
	char Surname[110];
	char username[110];
	char password[110];
	int subscriptions[3]; // flag: to be modified later
}; 
 
inline void command_output(char* OUTPUT, int cd, pthread_mutex_t* print_lacatel) {
    pthread_mutex_lock(print_lacatel);
    printf("[client][command] %s\n", OUTPUT);
    fflush(stdout);
    int LG=strlen(OUTPUT);
    if (cd)
      bzero(OUTPUT,LG);
    pthread_mutex_unlock(print_lacatel);
}

inline void REGISTRATION_FORM(int socket_desc, char* response, pthread_mutex_t* response_lacatel, pthread_mutex_t* print_lacatel) {
      //flag> lacatele sunt puse aiurea
        pthread_mutex_lock(response_lacatel);
	char line[MAX_CH_ON_LINE];
	char simplified_args[350];
	info_for_user USR;
	strcpy(response,server_comm_coding[1]);
	strcat(response,"|");
	
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
	/* flag: something doesn't work with the send query*/
	char parameters[510], insert_query[550];
	parameters[0]='\0'; insert_query[0]='\0';
	//memset(parameters, sizeof(parameters)-1, '\0');
	//memset(insert_query, sizeof(insert_query)-1, '\0');
	char id_user_str[20];
	int_to_string(USR.iduser,id_user_str);
	//printf("%d este %s\n", USR.iduser, id_user_str);
	strcat(parameters, id_user_str); strcat (parameters,",'");
	strcat(parameters, USR.First_Name); strcat (parameters,"','");
	strcat(parameters, USR.Surname); strcat (parameters,"','");
	strcat(parameters, USR.username); strcat (parameters,"','");
	strcat(parameters, USR.password); strcat (parameters,"',");
	strcat(parameters, "NULL"); strcat (parameters,","); // flag: to be modified for subscriptions
	strcat(parameters, "NULL"); strcat (parameters,","); // flag: to be modified for subscriptions
	strcat(parameters, "NULL");  // flag: to be modified for subscriptions
	strcat(insert_query, "INSERT INTO Users VALUES(");
	strcat(insert_query,parameters);
	strcat(insert_query,");");
	strcat(response,insert_query);
	
	sprintf(simplified_args,"%s|%s|%s|%s|%s|%s|0|0|0|%s",server_comm_coding[1],id_user_str, USR.First_Name,USR.Surname,USR.username,USR.password, insert_query); /*
	short is_logged;
	int iduser;	
	char First_Name[110];
	char Surname[110];
	char username[110];
	char password[110];
	int subscriptions[3];
	*/
	printf("[client][command] Sending the following response> %s\n",simplified_args);
	//send(socket_desc,insert_query,strlen(insert_query),0);
        pthread_mutex_unlock(response_lacatel);
}

inline void LOGIN_REQUEST(int socket_desc, int * token, pthread_mutex_t * lacatel, char* response, pthread_mutex_t* response_lacatel, pthread_mutex_t* print_lacatel) {
      
      //flag> lacatele sunt puse aiurea
	char query[310];
	char simplified_args[350];
	query[0]='\0';
	char line[MAX_CH_ON_LINE];
	info_for_user USR;
        printf("[client][command] For logging in, your username and password are required...\n");
        printf("\n[client][command] Your username: ");
	read_line(line);
	strcpy(USR.username,line);
	printf("\n[client][command] Your password: ");
	read_line(line);
	strcpy(USR.password,line);
	
	if(strlen(USR.username)==0 || 
	Contains_Any_Chars_From(USR.username,"!@#$%^&*()`~':;|[]<>,.+=") || 
	!isalpha(USR.username[0]) ) {
	      printf("[client][command] Not logged in! (found invalid characters in username)\n");
	strcpy(query,"Invalid");
	
	}
	else if(password_test(USR.password)) {
		printf("[client][command] Not logged in! (found invalid characters in username)\n");
	strcpy(query,"Invalid");
	
	}
	else{
	strcpy(query,"SELECT * FROM Users WHERE username='");
	strcat(query, USR.username);
	strcat(query,"' AND password='");
	strcat(query, USR.password);
	strcat(query,"';");
	printf("[client][command] Sending query %s\n",query);
        }
	
        pthread_mutex_lock(response_lacatel);
        sprintf(simplified_args,"%s|%s|%s|%s",server_comm_coding[2],USR.username,USR.password, query);
        
	strcpy(response,server_comm_coding[2]);
	strcat(response,"|");
	strcat(response,USR.username);
	strcat(response,"|");
	strcat(response,query);
        pthread_mutex_unlock(response_lacatel);
	
        //flag: to implement SQL query + sending to server
}

void update_events(int rightnow) {
while(!events_list.empty() && (events_list.front().start+events_list.front().lifetime)>rightnow)
    events_list.pop_front();
}

void add_event(event A){
  events_list.push_back(A);
 //bonus: update_events(A.start);
}

inline void REPORT_EVENT(int socket_desc,  pthread_mutex_t * lacatel, char* response, pthread_mutex_t* response_lacatel, pthread_mutex_t* print_lacatel) {
    
      //flag> lacatele sunt puse aiurea
      char line[MAX_CH_ON_LINE];
      line[0]='\0';
      memset(line, MAX_CH_ON_LINE-1, '\0');
      
      pthread_mutex_lock(lacatel);
      printf("[client][command] What type of event do you want to report?\n\t1 - accident\n\t2 - police control\n\t3 - traffic jam\n");
      int nr;
      read_line(line);
      string_to_int(line, nr);
      while(nr<1 || nr>3) {
      printf("\n[client][command]Please enter an input from 1 to 3.\n"); 
      printf("[client][command] What type of event do you want to report?\n\t1 - accident\n\t2-police control\n\t3 - traffic jam\n");
      
      read_line(line);
      string_to_int(line, nr);
      }
      //printf("[client][command] The nr is %d\n", nr);
      
      event auxevent;
      char auxmes[200], loc[200];
      auxmes[0]='\0'; loc[0]='\0';
      switch(nr) {
      case 1: strcat(auxmes, "Detected an accident on "); break;
      case 2: strcat(auxmes, "Detected a police control on ");break;
      case 3: strcat(auxmes, "Detected a traffic jam on ");break;
      }
      ///
      printf("\n[client][command]Please enter the location of the traffic event>");
      //scanf("%[^\n]s", line);
      read_line(line);
      strcpy(loc,line);
      strcat(auxmes,loc);
      strcpy(auxevent.message,auxmes);
      auxevent.idevent=++NO_EVENTS;
      auxevent.start=0; //bonus: to be modified
      auxevent.lifetime=100000000; //bonus: to be modified
      printf("\n[client][command] Final message is...%s", auxmes);
      
      pthread_mutex_lock(response_lacatel);
      strcpy(response,server_comm_coding[4]);
      strcat(response,"|");
      strcat(response,auxmes);
      pthread_mutex_unlock(response_lacatel);
      //add_event(auxevent);
      pthread_mutex_unlock(lacatel);
}
inline void LOGOUT_REQUEST(int socket_desc, int * token, pthread_mutex_t * lacatel, char* response, pthread_mutex_t* response_lacatel) {
      
      pthread_mutex_lock(lacatel);
      pthread_mutex_lock(response_lacatel);
      strcpy(response,server_comm_coding[3]);
      pthread_mutex_unlock(response_lacatel);
      pthread_mutex_unlock(lacatel);
}
inline void GET_EVENTS(int socket_desc, pthread_mutex_t * lacatel,char* response, pthread_mutex_t* response_lacatel) {
//flag> de pus lacatelu
      pthread_mutex_lock(lacatel);
      pthread_mutex_lock(response_lacatel);
      strcpy(response,server_comm_coding[5]);
      pthread_mutex_unlock(response_lacatel);
      pthread_mutex_unlock(lacatel);

}
