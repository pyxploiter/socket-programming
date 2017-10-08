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

    SNDFILE *outfile;
    SF_INFO in_info;
    int recvPacket[PACKET_SIZE];
    
    sf_count_t count;

    memset(&in_info, 0, sizeof(in_info));

    int serverSock = initSocket();
    int file_info[6]={0};

    int clientSock = accept(serverSock,NULL,NULL);
    
    if(read(clientSock,file_info,sizeof(file_info))==-1){
        printf("read error.\n");
    }
    in_info.frames = file_info[0] ;
    in_info.samplerate = file_info[1];
    in_info.channels = file_info[2];
    in_info.format = file_info[3];
    in_info.sections = file_info[4];
    in_info.seekable = file_info[5];
    
    int n_items = in_info.frames*in_info.channels; 
    int nbytes = n_items/PACKET_SIZE;
    int counter = 0;

    int *baraBuf = (int *) malloc(n_items*sizeof(int));

    if ((outfile = sf_open("received.wav", SFM_WRITE, &in_info)) == NULL){
        printf("Error: Failed to open output file.\n");
        sf_close(outfile);
        exit(-1);
    }

    while (counter < n_items){
        if(read(clientSock,recvPacket,sizeof(recvPacket))==-1){
            printf("read error.\n");
            exit(-1);
        }

        if (counter < (n_items-(n_items%PACKET_SIZE))){
            for (int i = 0; i < ARRAY_LEN(recvPacket); i++){
                baraBuf[counter] = recvPacket[i];
                counter++;
            }
        }
        else{
            for (int i = 0; i < (n_items%PACKET_SIZE); i++){
                baraBuf[counter] = recvPacket[i];
                counter++;
            }

        }
    }

    sf_write_int(outfile, baraBuf, n_items);
    printf("Audio File Received.\n");

    close(clientSock);
    close(serverSock);
    return 0;
}
