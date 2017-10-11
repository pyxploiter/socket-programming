#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 11006
#define PACKET_SIZE 1024

int initSocket(){
	int serverSock=socket(AF_INET,SOCK_DGRAM,0);
	if(serverSock==-1){
		perror("socket creation error!\n");
		exit(-1);
	}

	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr=inet_addr("127.0.0.1");

	if(bind(serverSock,(struct sockaddr*)&addr,sizeof(addr))==-1){
		printf("Binding error!\n");
		close(serverSock);
		exit(-1);
	}

	printf("Bind successfully.\n");
	return serverSock;
}

int main(){
    int count = 1;
    char filename[50];
    char received[10] = "received_";
    FILE *receiving_file;

    struct sockaddr_in from;
    socklen_t len = sizeof(from);

    char recvPacket[PACKET_SIZE];
    int serverSock = initSocket();
    
    char size[4]; int file_size;

    if(recvfrom(serverSock,size,sizeof(size),0,(struct sockaddr *) &from, &len)==-1){
        printf("reading error.\n");
    }

    file_size = *((int*)size);              // typecasting from char[] to int
    
    int no_packets = file_size/PACKET_SIZE; 
    
    if(recvfrom(serverSock,filename,sizeof(filename),0,(struct sockaddr *) &from, &len)==-1){
        printf("reading error.\n");
    }
    
    strcat(received,filename);
    receiving_file = fopen(received,"wb");

    // receiving packets of 1024 bits
    while(count < (no_packets+1)){
        if(recvfrom(serverSock,recvPacket,sizeof(recvPacket), 0,(struct sockaddr *) &from, &len)==-1){
            printf("read error.\n");
            exit(-1);
        }      
        // writing data to file 
        fwrite(recvPacket, sizeof(char),sizeof(recvPacket),receiving_file);
        memset(&recvPacket, 0, sizeof(recvPacket));
        printf("%d\n",count++);
    }
    // receiving remaining bits
    recvfrom(serverSock,recvPacket,sizeof(recvPacket), 0,(struct sockaddr *) &from, &len);
    fwrite(recvPacket, sizeof(char), (file_size-(no_packets*PACKET_SIZE)), receiving_file);

    printf("receiving_file File Received.\n");
    fclose(receiving_file);
    close(serverSock);
    return 0;
    
}
