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

#define PORT 11006
#define PACKET_SIZE 1024

#define ARRAY_LEN(x)    ((int) (sizeof (x) / sizeof (x [0])))

SNDFILE *infile;
SF_INFO in_info;
int *buffer;
sf_count_t count;
int file_info[6], n_items;

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

	buffer = (int *) malloc(n_items*sizeof(int));

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

	if(sendto(clientSock,file_info,sizeof(file_info),0,(struct sockaddr*)&addr_to,sizeof(addr_to))==-1){
			printf("sending failed!\n");
	}

	sf_read_int(infile, buffer, n_items); 

	for (int i = 0; i < n_items; i += PACKET_SIZE){
        memset(packet, 0, sizeof(packet));
        for (int j = 0; j < PACKET_SIZE; ++j){
            packet[j] = buffer[i+j];
        }
        if (sendto(clientSock, packet, sizeof(packet),0,(struct sockaddr*)&addr_to,sizeof(addr_to)) == -1){
            printf("data sending error");
        }   
    }

    printf("Audio File Sent.\n");

    sf_close(infile);
	close(clientSock);
	return 0;
}
