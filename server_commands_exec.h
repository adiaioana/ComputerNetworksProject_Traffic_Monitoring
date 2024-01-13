#include <cstring>
#include <sqlite3.h>
#include<stdio.h>
#include <string>
#include <iostream>

struct info_for_user{
        short auth_key;
	int iduser;	
	char First_Name[110];
	char Surname[110];
	char username[110];
	char password[110];
	int subscriptions[3]; // flag: to be modified later
}USR,auxuser; 

struct command_arguments{
	int nr_arg;
	char argv[110][110];
};

struct command{
    int type;
    command_arguments converted;
};
sqlite3* DB; 

int sql_query_for_users(char * query, info_for_user* user_data) {
     return 1;
}

int Register(command_arguments* Data) {
  if(sql_query_for_users(Data->argv[Data->nr_arg-1], &auxuser))
    return 1;
  return 0;
}

int LogIn(command_arguments* Data) {
  if(sql_query_for_users(Data->argv[Data->nr_arg-1],&USR)) {
    USR.auth_key=1;
    return 1;
  }
  return 0;
}

int LogOut(command_arguments* Data) {
  if(USR.auth_key==0)
    return 0;
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

