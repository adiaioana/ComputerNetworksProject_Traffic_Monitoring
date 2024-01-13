const char command_strings[110][110]={"","register","login","logout","report","get-events","get-info","subscribe","",""};
const char first_response[110][210]={"","Please complete the registration form>","Please provide creditentials","Logged out.","Reporting traffic event...","Printing all recent events...","",""};
const int number_of_command_strings=9;
const char server_comm_coding[110][110]={"INVCOMM","REG","LOGin","LOGout","REVENT","GEVENT","GINFO","SUBSCR","UNKN"};
const char server_response[110][110]={"Invalid command!","Successfully registered user: ","Logged in as: ","Logged out", "Reported event: ","...","...","...","..."};
