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

int initSocket(){
    struct sockaddr_in serverAddr;

    int serverSock=socket(AF_INET,SOCK_STREAM,0);
    if(serverSock<0){
        printf("socket creation failed\n");
        exit(-1);
    }

    printf("socket create successfully.\n");

    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons((u_short) PORT);
    serverAddr.sin_addr.s_addr=htons(INADDR_ANY);
    
    if(bind(serverSock,(struct sockaddr *)&serverAddr,sizeof(serverAddr))==-1){
        printf("Binding error.\n");
        close(serverSock);
        exit(-1);
    }

    printf("Bind successful.\n");

    if(listen(serverSock,10)<0){
        printf("Listening error!\n");
    }

    printf("Listening...\n");
    return serverSock;
}

int main(){
    int count = 1;
    char filename[50];
    char received[10] = "received_";
    FILE *receiving_file;
    char recvPacket[PACKET_SIZE];
    int serverSock = initSocket();
    int clientSock = accept(serverSock,NULL,NULL);
    
    char size[4]; int file_size;
    read(clientSock, size, sizeof(size));   // receiving file size
    file_size = *((int*)size);              // typecasting from char[] to int
    int no_packets = file_size/PACKET_SIZE; 
    read(clientSock, filename, sizeof(filename));
    strcat(received,filename);
    receiving_file = fopen(received,"wb");

    // receiving packets of 1024 bits
    while(count < (no_packets+1)){
	if (read(clientSock,recvPacket,sizeof(recvPacket)) == -1){
		printf("Reading error\n");
		exit(0);
	}       
	// writing data to file 
	fwrite(recvPacket, sizeof(char),sizeof(recvPacket),receiving_file);
        memset(&recvPacket, 0, sizeof(recvPacket));
	count++;
    }
    // receiving remaining bits
    read(clientSock, recvPacket, (file_size-(no_packets*PACKET_SIZE)));
    fwrite(recvPacket, sizeof(char), (file_size-(no_packets*PACKET_SIZE)), receiving_file);

    printf("receiving_file File Received.\n");
    fclose(receiving_file);
    close(clientSock);
    close(serverSock);
    return 0;
}
