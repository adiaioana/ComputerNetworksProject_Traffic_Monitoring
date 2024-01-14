
int no_locations=0;
int speed_of_user[300];
clock_t begin_time_user[300];
int start_location_user[300];

char Street_Name[30][100];


int callback_streets(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    int ind=1;
    for (int i = 0; i < argc; i++) {
      strcpy(Street_Name[ind],argv[i]);
      no_locations++;
      ind++;
      if(ind>15)
      {
      printf("data base seems to be wrong\n");
      return 0;
      }
    }
    
    //printf("\n");
    
    return 0;
}

void getmap() {
  char* messaggeError; int exitC=0;
  
    std::string query = "SELECT Name FROM Streets;"; 
    int rc = sqlite3_exec(DB, query.c_str(), callback_streets, 0, &messaggeError);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", messaggeError);

        sqlite3_free(messaggeError);
    } 

}
