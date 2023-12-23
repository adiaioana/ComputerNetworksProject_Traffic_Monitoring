#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#define PORT 3000
#define MAX_SIZE 50
struct sockaddr_in serv_addr;
int server_config();

int main()
{
    char sbuff[MAX_SIZE],rbuff[MAX_SIZE];
    int sock_desc=server_config();
    
    if (connect(sock_desc, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("[client] Failed to connect to server\n");
        return -1;
    }

    printf("[client] Connected successfully - Please enter string\n");
    while(fgets(sbuff, MAX_SIZE , stdin)!=NULL)
    {
        send(sock_desc,sbuff,strlen(sbuff),0);
        if(recv(sock_desc,rbuff,MAX_SIZE,0)==0)
           printf("[client] Error");
        else
           fputs(rbuff,stdout);
        bzero(rbuff,MAX_SIZE);
    }
    close(sock_desc);
    return 0;

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