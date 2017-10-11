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
    
    int clientSock=socket(AF_INET,SOCK_DGRAM,0);
	if(clientSock==-1){
		perror("socket creation error!\n");
		exit(-1);
	}
	
	struct sockaddr_in addr_to;
	addr_to.sin_family=AF_INET;
	addr_to.sin_port=htons(PORT);
	addr_to.sin_addr.s_addr=inet_addr("127.0.0.1");

	struct sockaddr_in addr_from;
	addr_from.sin_family=AF_INET;
	addr_from.sin_port=htons(0);
	addr_from.sin_addr.s_addr=htons(INADDR_ANY);

	if(bind(clientSock,(struct sockaddr*)&addr_from,sizeof(addr_from))==-1){
		printf("Binding error!\n");
		close(clientSock);
		exit(-1);
	}

	printf("Bind successfully.\n");

    int no_packets = size/PACKET_SIZE;
    
    char chr_size[4];
    *((int*)chr_size) = size;   // typecasting int to char[]
    
    if(sendto(clientSock,chr_size,sizeof(chr_size),0,(struct sockaddr*)&addr_to,sizeof(addr_to))==-1){
			printf("sending failed!\n");
	}

	if(sendto(clientSock,filename,sizeof(filename),0,(struct sockaddr*)&addr_to,sizeof(addr_to))==-1){
			printf("sending failed!\n");
	}

    printf("sending\n");
    //sending packets of 1024 bits
    while(count < (no_packets+1)){
        fread(packet, sizeof(char),sizeof(packet),music);
        if (sendto(clientSock, packet, sizeof(packet),0,(struct sockaddr*)&addr_to,sizeof(addr_to)) == -1){
            printf("data sending error");
        }  
        memset(packet, 0, sizeof(packet));
        printf("%d",count++);
    }
    //sending remaining bits
    fread(packet, sizeof(char),(size-(no_packets*PACKET_SIZE)),music);
    sendto(clientSock, packet, sizeof(packet),0,(struct sockaddr*)&addr_to,sizeof(addr_to));
    printf("\nAudio File Sent.\n");
    fclose(music);
    close(clientSock);
    return 0;
}
