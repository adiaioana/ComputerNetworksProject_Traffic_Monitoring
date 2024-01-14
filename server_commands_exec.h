#include <cstring>
#include <sqlite3.h>
#include<stdio.h>
#include <string>
#include <time.h>
#include <cstdlib>
#include <iostream>
#include "debugkitchen.h"
info_for_user auxuser; 
struct event{ 
  int code; int id;
  char message[50];
};
event events[250];
struct command_arguments{
	int nr_arg;
	char argv[110][110];
};

struct command{
    int type;
    command_arguments converted;
};
sqlite3* DB; 
int to_int(char * str) {
  int nr=0;
  int lg=strlen(str);
  for(int i=0; i<lg; ++i) {
    nr=nr*10+(str[i]-'0');
  }
  return nr;
}

struct Street{
  int idstreet;
  char Name[60];
  int SpeedLimit;
  int Distance;
  int lft; //intersection
  int rgt; //intersection
};
int speed_of_user[300];
clock_t begin_time_user[300];
int begin_location_user[300];

struct Map {
  int no_locations;
  Street Streets[50];
  int no_nodes; //no_intersections
  int edges[100][100]; //da pun doar (idstreet)
} M;

void string_to_int(char *p, int &opa) {
  opa=0;
  int lgaux=strlen(p);
  for(int i=0; i<lgaux; ++i)
    if(p[i]>='0' && p[i]<='9') {
      for(int j=i; j<lgaux; ++j)
        if(p[j]>='0' && p[j]<='9')
          opa=opa*10+(p[j]-'0');
        else break;
      break;
      }
}
int callback1(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    M.no_locations++;
    Street A;
    for (int i = 0; i < argc; i++) {
        if(strstr(azColName[i],"NAME")) {
        strcpy(A.Name, argv[i]);
        }
        else if(strstr(azColName[i],"NAME")) {
        strcpy(A.Name, argv[i]);
        }
        else if(strstr(azColName[i],"IDLEFT")) {
        string_to_int(argv[i],A.lft);
        }
        else if(strstr(azColName[i],"IDRIGHT")) {
        string_to_int(argv[i],A.rgt);
        }
        else if(strstr(azColName[i],"ID")) {
        string_to_int(argv[i],A.idstreet);
        }
        else if(strstr(azColName[i],"DISTANCE")) {
        string_to_int(argv[i],A.Distance);
        }
        else if(strstr(azColName[i],"SPEEDLIMIT")) {
        string_to_int(argv[i],A.SpeedLimit);
        }
       // printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    M.Streets[M.no_locations]=A;
    printf("\n");
    
    return 0;
}

void getmap() {
  char* messaggeError; int exitC=0;
  
    std::string query = "SELECT * FROM Streets;"; 
    int rc = sqlite3_exec(DB, query.c_str(), callback1, 0, &messaggeError);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", messaggeError);

        sqlite3_free(messaggeError);
    } 
    
    for(int i=1; i<=M.no_locations; ++i) {
    int x=M.Streets[i].lft;
    int y=M.Streets[i].rgt;
    M.edges[x][++M.edges[x][0]]=y;
    M.edges[y][++M.edges[y][0]]=x;
    if(x>M.no_nodes) M.no_nodes=x;
    if(y>M.no_nodes) M.no_nodes=y;
    }
}
void upd_speed_loc(info_for_user* USR) {
  clock_t end = clock();
  double time_spent = (double)(end - begin_time_user[USR->iduser]) / CLOCKS_PER_SEC;
  int new_loc, new_speed;
  
  new_loc=rand()%M.no_locations;
  if((rand()%15)%7==0) {
   new_speed=M.Streets[new_loc].SpeedLimit+(rand()%10);
  }
  else new_speed=M.Streets[new_loc].SpeedLimit-(rand()%10);
  
  speed_of_user[USR->iduser]=new_speed;
  begin_location_user[USR->iduser]=new_loc;
  begin_time_user[USR->iduser]=clock();
}

int Info(char* Mess, info_for_user* USR) {
  if(USR->auth_key==0) {
    strcpy(Mess,"[AppError] Not authorised to access speed/loc data");
    return 0;
  }
  //Message format is"%Speed km/h in %Location"
  upd_speed_loc(USR);
  if(speed_of_user[USR->iduser]<=M.Streets[begin_location_user[USR->iduser]].SpeedLimit)
    sprintf(Mess,"%dkm/h in %s",speed_of_user[USR->iduser],M.Streets[begin_location_user[USR->iduser]].Name);
  else sprintf(Mess,"(WARNING: SPEEDLIMIT) %dkm/h in %s",speed_of_user[USR->iduser],M.Streets[begin_location_user[USR->iduser]].Name);
  return 1;
}
int userdatacomplete(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {
        switch(i) {
          case 0: auxuser.iduser=to_int(argv[i]); break;
          case 1: strcpy(auxuser.First_Name, argv[i]); break;
          case 2: strcpy(auxuser.Surname, argv[i]); break;
          case 3: strcpy(auxuser.username, argv[i]); break;
          case 4: strcpy(auxuser.password, argv[i]); break;
          case 5: auxuser.subscriptions[0]=to_int(argv[i]); break;
          case 6: auxuser.subscriptions[1]=to_int(argv[i]); break;
          case 7: auxuser.subscriptions[2]=to_int(argv[i]); break;
        
        }
        //debugflag: remove it!!!	
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    //print_user_data(USR); //debugflag: remove it!!!	
    printf("\n");
    
    return 0;
}
pthread_mutex_t auth_lock=  PTHREAD_MUTEX_INITIALIZER;
int sql_query_for_users(char * sql_stmt, info_for_user* user_data, int purpose,info_for_user* USR) {
    char* errMesg = 0;
    int ret = 0;
     
    ret = sqlite3_exec(DB, sql_stmt, NULL, 0, &errMesg);
    if (ret != SQLITE_OK) {

        printf("Error in SQL statement: %s\n", errMesg);
        sqlite3_free(errMesg);

        return 0;
    }
    
    if(purpose==1) //Log in
    {
      
        pthread_mutex_lock(&auth_lock);
      ret = sqlite3_exec(DB, sql_stmt, userdatacomplete, 0, &errMesg);
      *USR=auxuser;
      USR->auth_key=1;
        pthread_mutex_unlock(&auth_lock);
      
    }
  
    return 1;
}

int Register(command_arguments* Data, char* Mess,info_for_user* USR) {
  if(sql_query_for_users(Data->argv[Data->nr_arg-1], &auxuser,0, USR)){
    strcpy(Mess,"Registered new user.");
    return 1;
  }
  strcpy(Mess,"[AppError] Can't register user!");
  return 0;
}
int Author(char *Mess, info_for_user* USR) {
  if(USR->auth_key==0) {
    strcpy(Mess,"NO");
    return 0;
  }
  sprintf(Mess,"YES:%s",USR->username);
  return 1;
}

pthread_mutex_t event_lock=  PTHREAD_MUTEX_INITIALIZER;
/*
    case 4: exec_code=ReportEvent(&Inp_who,OUTPUT); break;
    case 5: exec_code=GetEvents(OUTPUT);break;
*/
int no_events=0;
int ReportEvent(command_arguments* Data, char* Mess, int isrep[250], info_for_user* USR) {
        pthread_mutex_lock(&event_lock);
        ++no_events;
        printf("There are %d\n", no_events);
        event auxev;
        auxev.id=no_events;
        isrep[auxev.id]=0;
        
        strcpy(auxev.message,Data->argv[Data->nr_arg-1]); 
        if(strstr(auxev.message,"accident")) {
        auxev.code=1;
        }
        else if(strstr(auxev.message,"police")) {
        auxev.code=2;
        }
        else if(strstr(auxev.message,"traffic jam")) {
        auxev.code=3;
        }
        events[no_events]=auxev;
        strcpy(Mess, "Event reported.");
        
        pthread_mutex_unlock(&event_lock);
        return 1;
}
int GetEvents(char* Mess, int isreported[250], info_for_user* USR) {
    pthread_mutex_lock(&event_lock);
    Mess[0]='\0';
    int ok=0;
    for(int i=0; i<no_events; ++i)
      if(isreported[i]==0) {
      strcat(Mess, events[i].message);
      strcat(Mess,"\n");
      isreported[i]=1;
      ok=1;
      }
    if(!ok)
      strcpy(Mess,"No events to report\n");
    pthread_mutex_unlock(&event_lock);
      return 1;
}

int LogIn(command_arguments* Data, char* Mess, info_for_user* USR) {
  if(sql_query_for_users(Data->argv[Data->nr_arg-1],&auxuser,1,USR)) {
    sprintf(Mess,"Logged in as: %s", USR->username);
    USR->auth_key=1;
    begin_time_user[USR->iduser]=clock();
    begin_location_user[USR->iduser]=rand()%14+1;
    return 1;
  }
  strcpy(Mess,"[AppError] Can't login! Wrong username-password combination.");
  return 0;
}

int LogOut(command_arguments* Data, char* Mess, info_for_user* USR) {
  if(USR->auth_key==0) {
    strcpy(Mess,"[AppError] Not logged in, can't log out!");
    return 0;
  }
  strcpy(Mess,"Logged out!");
  USR->auth_key=0;
  return 1;
}

///SQL

int callback(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}

inline void test_ops_on_DB() {

    char* messaggeError; int exitC=0;
  
    std::string query = "SELECT * FROM Users;"; 
    int rc = sqlite3_exec(DB, query.c_str(), callback, 0, &messaggeError);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", messaggeError);

        sqlite3_free(messaggeError);
    } 
}
inline void open_DB() { //flag> to add the rest DBs
  int exitC = 0; 
  exitC = sqlite3_open("Iashington.db", &DB); 
  if (exitC) { 
        printf("Error open DB %d\n",sqlite3_errmsg(DB));
  }
  else
        printf("Opened Database Successfully!\n"); 
    //test_ops_on_DB();
}
inline void close_DB() { //flag> to add the rest DBs
  sqlite3_close(DB); 
}

