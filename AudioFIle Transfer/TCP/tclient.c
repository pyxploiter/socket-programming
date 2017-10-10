#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 10006
#define PACKET_SIZE 1024

int *buffer;

int initSocket(){
    struct sockaddr_in serverAddr;
    int clientSock=socket(AF_INET,SOCK_STREAM,0);
    
    if(clientSock<0){
        printf("socket creation failed\n");
        exit(-1);
    }

    printf("socket create successfully.\n");

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons((u_short) PORT);
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    
    if(connect(clientSock,(struct sockaddr *)&serverAddr,sizeof(serverAddr))<0){
        printf("Connection error.\n");
        exit(-1);
    }

    printf("Connected successfully.\n");

    return clientSock;
}

int main(){
    int count = 1;
    char filename[50];
    printf("Enter the audio file name(e.g. music.mp3): ");
    scanf("%s", filename);
	
    FILE *music;
    if((music = fopen(filename,"rb")) == NULL){
	exit(0);
    }
    fseek(music, 0, SEEK_END);
    int size = ftell(music);
    fseek(music, 0, SEEK_SET);
    char packet[PACKET_SIZE];
    int clientSock = initSocket();
    int no_packets = size/PACKET_SIZE;
    
    char chr_size[4];
    *((int*)chr_size) = size;   // typecasting int to char[]
    
    write(clientSock, chr_size, sizeof(chr_size));  // sending file size
    write(clientSock, filename, sizeof(filename));
    printf("sending\n");
    //sending packets of 1024 bits
    while(count < (no_packets+1)){
        fread(packet, sizeof(char),sizeof(packet),music);
        if(write(clientSock, packet, sizeof(packet)) == -1){
            printf("data sending error\n");
        }
        memset(packet, 0, sizeof(packet));
        count++;
    }
    //sending remaining bits
    fread(packet, sizeof(char),(size-(no_packets*PACKET_SIZE)),music);
    write(clientSock, packet, sizeof(packet));
    printf("\nAudio File Sent.\n");
    fclose(music);
    close(clientSock);
    return 0;
}
