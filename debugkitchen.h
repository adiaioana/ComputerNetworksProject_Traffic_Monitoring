#include <time.h>
#include <sqlite3.h>
sqlite3* DB; 
struct info_for_user{
        short auth_key;
	int iduser;	
	char First_Name[110];
	char Surname[110];
	char username[110];
	char password[110];
	int subscriptions[3]; 
	
};
void print_user_data(info_for_user* A) {
  printf("[server][debug] Printing user data>\n");
  printf("Is logged?? %d; ", A->auth_key);
  printf("ID= %d; ", A->iduser);
  printf("Name is (F) %s (S) %s ; Username= %s & Password= %s ;\n", A->First_Name, A->Surname, A->username, A->password);
  printf("Subscriptions are (sports) %d (weather) %d (peco) %d\n", A->subscriptions[1], A->subscriptions[2], A->subscriptions[0]);
}

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


//Peco, Weather, Sports
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
