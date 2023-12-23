#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 3000
#define MAX_SIZE 50
struct sockaddr_in serv_addr;
int server_config();
struct info_for_threads{
int cli_sock; int id;
};

void* command_thread(void * arg);
int main()
{
    int sock_desc=server_config();
    
    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("[client] Failed to connect to server\n");
        return -1;
    }
     printf("[client] Connected successfully - Starting... \n");
    pthread_t cli_thread[4];
    info_for_threads * cli_th;
    cli_th->cli_sock=sock_desc;
    cli_th->id=1;
	
    
    pthread_create(&cli_thread[0],NULL, &command_thread,cli_th);
    
    
    pthread_exit(NULL); 
    close(sock_desc);
    return 0;

}
void* command_thread(void * arg)
{
    info_for_threads * cli_th=(info_for_threads *)arg;
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    int sock_desc=cli_th->cli_sock;
    printf("[client] Print command:\n");
    while(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
    {
        send(sock_desc,sbuff,strlen(sbuff),0);
        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
           printf("[client] Error");
        else {
        	fputs(rbuff,stdout);
        }
        bzero(rbuff,MAX_SIZE);
 
    }

}

int server_config() {
    int sock_desc;
    if((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("[client] Failed creating socket\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);
    return sock_desc;
}
