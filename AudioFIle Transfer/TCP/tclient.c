#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sndfile.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 10006
#define PACKET_SIZE 1024

#define ARRAY_LEN(x)    ((int) (sizeof (x) / sizeof (x [0])))

SNDFILE *infile;
SF_INFO in_info;
int *buffer;
sf_count_t count;
int file_info[6], n_items;

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

void readAudioFile(){
    char filename[100];
    printf("Enter the audio file name: ");
    scanf("%s",filename);

    memset(&in_info, 0, sizeof(in_info));
    
    // Open the Audio file. 
    if ((infile = sf_open(filename,SFM_READ,&in_info)) == NULL){
        printf("Error: Failed to open the file.\n");
        sf_close(infile);
        exit(-1);
    }

    n_items = in_info.frames*in_info.channels;

    file_info[0] = in_info.frames;
    file_info[1] = in_info.samplerate;
    file_info[2] = in_info.channels;
    file_info[3] = in_info.format;
    file_info[4] = in_info.sections;
    file_info[5] = in_info.seekable;
}

int main(){
    
    int packet[PACKET_SIZE];
    readAudioFile();
    int clientSock = initSocket();   

    buffer = (int *) malloc(n_items*sizeof(int));

    if(write(clientSock,file_info,sizeof(file_info))==-1){
        printf("info sending error!\n");
    }
	
    printf("Audio File Headers Sent.\nSending items.");
    sf_read_int(infile, buffer, n_items);    

    for (int i = 0; i < n_items; i += PACKET_SIZE){
	printf(".");
        memset(packet, 0, sizeof(packet));
        for (int j = 0; j < PACKET_SIZE; ++j){
            packet[j] = buffer[i+j];
        }
        if (write(clientSock, packet, sizeof(packet)) == -1){
            printf("data sending error");
        }   
    }

    printf("\nAudio File Sent.\n");

    sf_close(infile);
    close(clientSock);
    return 0;
}
