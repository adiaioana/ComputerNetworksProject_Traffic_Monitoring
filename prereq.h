/**
	For basic functions such as int_to_string, etc.
**/
#include <stdio.h>
#include <sqlite3.h> 
#include <string.h>
#define MAX_CH_ON_LINE 100

 char* UPPER_ALPHABET="QWERTYUIOPASDFGHJKLZXCVBNM";
 char* LOWER_ALPHABET="qwertyuiopasdfghjklzxcvbnm";
 char* ALL_DIGITS="0123456789";
 char*  ALL_SIGNS="!@#$%^&*()`~':;|[]<>,.+=_-";

void read_line(char *buffer)
{
	int ch, i;
	for ( i = 0; ( i  < MAX_CH_ON_LINE ) && (( ch = getchar()) != EOF) && ( ch !='\n' ); ++i )
    		buffer[i] = ch;
	buffer[i]='\0';
}
int notyesorno(char *str_source){
  char straux[220];
  strcpy(straux,str_source);
  if(strchr(straux,'Y')==NULL or strchr(straux,'n')==NULL)
    return 0;
  int nY=0, nn=0; int lgaux=strlen(straux);
  for(int j=0; j<lgaux; ++j)
    if(straux[j]=='Y')
      nY++;
    else if(straux[j]=='n')
      nn++;
    else if(isalpha(straux[j]) or isdigit(straux[j]))
      return 0;
    if(nn+nY!=1)
      return 0;
    return 1;
}
//debug?? maybe ca returneaza un pointer poate da aiurea
int Contains_Any_Chars_From(char * str_source, char *str_tokens) {
	int how_many_tokens=strlen(str_tokens);
	int hm=0;
	
	for(int i=0; i<how_many_tokens; ++i)
		if(strchr(str_source,str_tokens[i])!=NULL)
			hm++;
	return hm;
}

bool password_test(char* pass) {
	char aux[110];
	strcpy(aux,pass);
	if(!Contains_Any_Chars_From(aux, ALL_DIGITS)) {
		printf("\n Your pass does not contain digits!");
		return 1;
	}
	if(!Contains_Any_Chars_From(aux, ALL_SIGNS)) {
		printf("\n Your pass does not contain signs!");
		return 1;
	}
		
	if(!Contains_Any_Chars_From(aux, UPPER_ALPHABET)) {
		printf("\n Your pass does not upper letters!");
		return 1;
	}
		
	if(!Contains_Any_Chars_From(aux, LOWER_ALPHABET)) {
		printf("\n Your pass does not lower letters!");
		return 1;
	}
	
	return 0;
}
char* int_to_string(int x) {

	char p[20];
	memset(p,sizeof(p),'\0');
	int lg=0;
	if(x==0) {
	p[0]='0';
	return p;
	}
	
	while(x) {
	p[lg++]=x%10+'0';
	p[lg]='\0';
	}
	
	int i=0, j=lg-1;
	while(i<j) {
		char aux=p[i];
		p[i]=p[j];
		p[j]=aux;
		i++; j--;
	}
	
	return p;
}
