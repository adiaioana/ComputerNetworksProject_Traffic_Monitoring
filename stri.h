const char command_strings[110][110]={"","register","login","logout","report","get-events","get-info","subscribe","sub-info","is-auth","exit","help","unkn"};
const char first_response[110][210]={"","Please complete the registration form>","Please provide creditentials","Logged out.","Reporting traffic event...","Printing all recent events...","Subscribing...","Getting subscription info...","","","",""};
const int number_of_command_strings=12;
const char server_comm_coding[110][110]={"INVCOMM","REG","LOGin","LOGout","REVENT","GEVENT","GINFO","SUBSCR","SUBin","AUTHOR","EXIT","HELP","UNKN"};
const char server_response[110][110]={"Invalid command!","Successfully registered user: ","Logged in as: ","Logged out", "Reported event: ","Getting events...","Getting info...","Subscribing to...","Getting data from subscriptions..","Authorized...", "Exiting app immediately...", "Helping..", "Unknown command"};

const char * Invalid_comm="The command entered is invalid. Please provide a valid command or ask for help by typing help :)";
const char* help_comm="Please type one of the following commands>\n1.1 For registration use: register\n1.2 For logging in use: login\n1.3 For logging out use: logout\n2.1 For requesting to subscribe to peco station: subscribe peco [peco_name]\n2.2 For requesting to subscribe to weather information: subscribe weather [today/tommorow]\n2.3 For requesting to subscribe to sports station (Ski, Surf or Chess): subscribe sports [sport_name]\n2.4 For requesting information from subscribed station> sub-info\0";
