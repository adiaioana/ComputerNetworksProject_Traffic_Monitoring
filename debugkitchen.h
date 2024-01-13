struct info_for_user{
        short auth_key;
	int iduser;	
	char First_Name[110];
	char Surname[110];
	char username[110];
	char password[110];
	int subscriptions[3]; // flag: to be modified later
	//IMP: subscr are indexed by alphabetic order: Peco, Sports, Weather
};

void print_user_data(info_for_user* A) {
  printf("[server][debug] Printing user data>\n");
  printf("Is logged?? %d; ", A->auth_key);
  printf("ID= %d; ", A->iduser);
  printf("Name is (F) %s (S) %s ; Username= %s & Password= %s ;\n", A->First_Name, A->Surname, A->username, A->password);
  printf("Subscriptions are (sports) %d (weather) %d (peco) %d\n", A->subscriptions[1], A->subscriptions[2], A->subscriptions[0]);
  
}
