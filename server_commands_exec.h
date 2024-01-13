#include <cstring>
#include <sqlite3.h>
#include<stdio.h>
#include <string>
#include <iostream>
#include "debugkitchen.h"
info_for_user USR,auxuser; 
struct event{ 
  int code;
  char* message;
};

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
int Info(char* Mess) {
  if(USR.auth_key==0) {
    strcpy(Mess,"[AppError] Not authorised to access speed/loc data");
    return 0;
  }
  strcpy(Mess, "1km/h in Pascani");
  return 1;
}

int userdatacomple(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {
        switch(i) {
          case 0: USR.iduser=to_int(argv[i]); break;
          case 1: strcpy(USR.First_Name, argv[i]); break;
          case 2: strcpy(USR.Surname, argv[i]); break;
          case 3: strcpy(USR.username, argv[i]); break;
          case 4: strcpy(USR.password, argv[i]); break;
          case 5: USR.subscriptions[0]=to_int(argv[i]); break;
          case 6: USR.subscriptions[1]=to_int(argv[i]); break;
          case 7: USR.subscriptions[2]=to_int(argv[i]); break;
        
        }
        //debugflag: remove it!!!	
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    print_user_data(&USR); //debugflag: remove it!!!	
    printf("\n");
    
    return 0;
}

int sql_query_for_users(char * sql_stmt, info_for_user* user_data, int purpose) {
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
      user_data->auth_key=1;
      ret = sqlite3_exec(DB, sql_stmt, userdatacomple, 0, &errMesg);
    }
  
    return 1;
}

int Register(command_arguments* Data, char* Mess) {
  if(sql_query_for_users(Data->argv[Data->nr_arg-1], &auxuser,0)){
    strcpy(Mess,"Registered new user.");
    return 1;
  }
  strcpy(Mess,"[AppError] Can't register user!");
  return 0;
}
int Author(char *Mess) {
  if(USR.auth_key==0) {
    strcpy(Mess,"NO");
    return 0;
  }
  sprintf(Mess,"YES:%s",USR.username);
  return 1;
}
int LogIn(command_arguments* Data, char* Mess) {
  if(sql_query_for_users(Data->argv[Data->nr_arg-1],&USR,1)) {
    sprintf(Mess,"Logged in as: %s", USR.username);
    USR.auth_key=1;
    return 1;
  }
  strcpy(Mess,"[AppError] Can't login! Wrong username-password combination.");
  return 0;
}

int LogOut(command_arguments* Data, char* Mess) {
  if(USR.auth_key==0) {
    strcpy(Mess,"[AppError] Not logged in, can't log out!");
    return 0;
  }
  strcpy(Mess,"Logged out!");
  USR.auth_key=0;
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
   /* std::string sql1 = "CREATE TABLE IF NOT EXISTS 'Users' Users("
                      "ID INT PRIMARY KEY     NOT NULL, "
                      "NAME           TEXT    NOT NULL, "
                      "SURNAME          TEXT     NOT NULL, "
                      "USERNAME            VARCHAR(50)     NOT NULL, "
                      "PASSWORD        VARCHAR(50) NOT NULL, "
                      "PECO_SUB         INT,"
                      "WEATHER_SUB         INT,"
                      "SPORT_SUB         INT );";
    exitC = sqlite3_exec(DB, sql1.c_str(), NULL, 0, &messaggeError); 
    if (exitC != SQLITE_OK) { 
        printf("Error Create\n"); 
        sqlite3_free(messaggeError); 
    } 
    else
        printf("Table created Successfully!\n"); */
   /* std::string sql(".schema Users;");*/
  /*  std::string sql("INSERT INTO Users VALUES(1, 'BOGDAN', 'GATES', 'admin', 'a1A!',0,0,0);"
               "INSERT INTO Users VALUES(2, 'SERBAN', 'WOZNIAK', 'boyer', 'b2B!',0,0,0);"
               "INSERT INTO Users VALUES(3, 'SILVIU', 'JOBS', 'moore', 'c3C!',0,0,0);"); 
    
    exitC = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError); 
    if (exitC != SQLITE_OK) { 
        printf("Error Ins\n"); 
        sqlite3_free(messaggeError); 
    } 
    else
        printf("Records created Successfully!\n"); */
        
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
    test_ops_on_DB();
}
inline void close_DB() { //flag> to add the rest DBs
  sqlite3_close(DB); 
}

