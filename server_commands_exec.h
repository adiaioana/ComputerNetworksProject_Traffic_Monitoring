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
int speed_of_user[300100];
int progress_user[300100];
clock_t begin_time_user[300100];
int begin_location_user[300100];

struct Map {
  int no_locations;
  Street Streets[50];
  int no_nodes; //no_intersections
  int edges[100][100]; //da pun celelalte strazi cu care se intersect.
  int streets_to_int[100][100]; //[i - strada][j- intersectie]=1 daca se conecteaza
} M;
char PecoInfo_out[300], WeatherInfo_out[300],SportsInfo_out[300];

static int callbackpeco(void *NotUsed, int argc, char **argv, char **azColName){
    memset(PecoInfo_out,sizeof(PecoInfo_out),'\0');
    for (int i = 0; i < argc; i++) {
        char rowy[150];
        sprintf(rowy,"%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        strcat(PecoInfo_out,rowy);
    }
    printf("\n");
    return 0;
}

static int callbackweather(void *NotUsed, int argc, char **argv, char **azColName){
    memset(WeatherInfo_out,sizeof(WeatherInfo_out),'\0');
    for (int i = 0; i < argc; i++) {
        char rowy[150];
        sprintf(rowy,"%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        strcat(WeatherInfo_out,rowy);
    }
    printf("\n");
    return 0;
}

static int callbacksports(void *NotUsed, int argc, char **argv, char **azColName){
    memset(SportsInfo_out,sizeof(PecoInfo_out),'\0');
    for (int i = 0; i < argc; i++) {
        char rowy[150];
        sprintf(rowy,"%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        strcat(SportsInfo_out,rowy);
    }
    printf("\n");
    return 0;
}

void PecoInfo() {
  char *err_msg = 0;
    const char *sql = "SELECT * FROM Peco";
    if (sqlite3_exec(DB, sql, callbackpeco, 0, &err_msg) != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}
void WeatherInfo() {
char *err_msg = 0;
    const char *sql = "SELECT * FROM Weather";
    if (sqlite3_exec(DB, sql, callbackweather, 0, &err_msg) != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}
void SportsInfo() {
char *err_msg = 0;
    const char *sql = "SELECT * FROM Sports";
    if (sqlite3_exec(DB, sql, callbacksports, 0, &err_msg) != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}
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
    M.streets_to_int[M.Streets[i].idstreet][x]=1;
    M.streets_to_int[M.Streets[i].idstreet][y]=1;
    }
    for(int i=1; i<=M.no_locations; ++i)
       for(int j=i+1; j<=M.no_locations; ++j)
          for(int k=1; k<=20; ++k)
            if(M.streets_to_int[j][k]==1 && M.streets_to_int[i][k]==1) {
              M.edges[i][++M.edges[i][0]]=j;
              M.edges[j][++M.edges[j][0]]=i;
            }
}
void upd_speed_loc(info_for_user* USR) {
  clock_t end = clock();
  double time_spent = (double)(end - begin_time_user[USR->iduser]) / CLOCKS_PER_SEC;
  
  int new_loc, new_speed=speed_of_user[USR->iduser];
  new_loc=begin_location_user[USR->iduser];
  int new_dir=M.edges[begin_location_user[USR->iduser]][rand()%(M.edges[begin_location_user[USR->iduser]][0]+1)+1];
  if(new_dir==0) {
    new_dir=rand()%M.no_locations;
  }
  if((rand()%15)%3!=0 || new_speed<M.Streets[new_loc].SpeedLimit/2) {
   new_speed+=(rand()%15);
  }
  else new_speed-=(rand()%15);
  if (new_speed>M.Streets[new_loc].SpeedLimit) {
    new_speed-=5;
  }
  if(new_speed<15)
    new_speed= 17;
  //progress_user[USR->iduser]+=new_speed*time_spent;
  
  if((rand()%15)%7==0|| progress_user[USR->iduser]>=M.Streets[begin_location_user[USR->iduser]].Distance) {
    new_loc=new_dir;
    if(progress_user[USR->iduser]>=M.Streets[begin_location_user[USR->iduser]].Distance)
      progress_user[USR->iduser]-=M.Streets[begin_location_user[USR->iduser]].Distance;
  }
  new_loc=rand()%M.no_locations;
  
  if (new_speed>M.Streets[new_loc].SpeedLimit) {
    new_speed-=5;
  }
  
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
        //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
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
    progress_user[USR->iduser]=0;
    speed_of_user[USR->iduser]=15;
    begin_location_user[USR->iduser]=rand()%M.no_locations+1;
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
int ModSubscr(command_arguments* Data, char* Mess, info_for_user* USR){
  if (USR->auth_key!=1)
  {
  strcpy(Mess,"[AppError] Not authorised to modify subscriptions.");
  return 0;
  }
  if(sql_query_for_users(Data->argv[Data->nr_arg-1],&auxuser,1,USR)) {
    strcpy(Mess,"Updated subscriptions.");
    USR->subscriptions[0]=Data->argv[1][0]-'0';
    USR->subscriptions[1]=Data->argv[2][0]-'0';
    USR->subscriptions[2]=Data->argv[3][0]-'0';
    return 1;
  }
  strcpy(Mess,"[AppError] Error handling SQL.");
  return 0;
}
int DataSub(int key, char* p) {
  if(key>2) return 0;
  if(key<0) return 0;
  
  switch(key) {
    case 0: PecoInfo(); break;
    case 1: WeatherInfo(); break;
    case 2: SportsInfo(); break;
  }
  
  switch(key) {
    case 0: strcpy(p,PecoInfo_out); break;
    case 1: strcpy(p,WeatherInfo_out); break;
    case 2: strcpy(p,SportsInfo_out); break;
  }
  
  return 1;
}

int GetSubscr(command_arguments* Data, char* Mess, info_for_user* USR){
  if (USR->auth_key!=1)
  {
  strcpy(Mess,"[AppError] Not authorised to get subscriptions.");
  return 0;
  }
  char promt[5][220]={"News from your peco subscriptions>",
	"News from your weather subscriptions>",
	"News from your sports subscriptions>"};
  char data[250];
  for(int i=0; i<=2; ++i)
    if(USR->subscriptions[i]) {
    if(!DataSub(i,data)) {
      sprintf(Mess,"[AppError] SQL in subscriptions database");
      return 0;
    }
    strcat(Mess,promt[i]);
    strcat(Mess,"\n");
    strcat(Mess,data);
    strcat(Mess,"\n");
    }
  
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
void subscriptionstableinit() {
 char *err_msg = 0;
 int rc;
const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS Peco (" \
                                 "id INT PRIMARY KEY, " \
                                 "name TEXT, " \
                                 "street TEXT, " \
                                 "benzina_price INT, " \
                                 "motorina_price INT);";
    
    rc = sqlite3_exec(DB, sqlCreateTable, 0, 0, &err_msg);
  if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(DB);
    }

    const char *sqlInsertData = "INSERT INTO Peco (id, name, street, benzina_price,   motorina_price) VALUES (1, 'Station A', 'Street A', 100, 90), " \
                                "(2, 'Station B', 'Street B', 102, 92), " \
                                "(3, 'Station C', 'Street C', 101, 91);";

    rc = sqlite3_exec(DB, sqlInsertData, callback, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error during insert: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    PecoInfo();
    const char *sqlCreateWeather = "CREATE TABLE IF NOT EXISTS Weather (" \
                                   "date TEXT, " \
                                   "morning INT, " \
                                   "noon INT, " \
                                   "night INT);";
    rc = sqlite3_exec(DB, sqlCreateWeather, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // Create Sports table
    const char *sqlCreateSports = "CREATE TABLE IF NOT EXISTS Sports (" \
                                  "date TEXT, " \
                                  "upcoming_matches TEXT);";
    rc = sqlite3_exec(DB, sqlCreateSports, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // Insert data into Weather table
    const char *sqlInsertWeather = "INSERT INTO Weather (date, morning, noon, night) VALUES " \
                                   "('2024-01-18', 15, 20, 10), " \
                                   "('2024-01-19', 16, 21, 11);";
    rc = sqlite3_exec(DB, sqlInsertWeather, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error during insert into Weather: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    // Insert data into Sports table
    const char *sqlInsertSports = "INSERT INTO Sports (date, upcoming_matches) VALUES " \
                                  "('2024-01-18', 'Match A vs B; Match C vs D'), " \
                                  "('2024-01-19', 'Match E vs F; Match G vs H');";
    rc = sqlite3_exec(DB, sqlInsertSports, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error during insert into Sports: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    WeatherInfo();
    SportsInfo();
}
