#include <sys/types.h>  /* for type definitions */
#include <sys/socket.h> /* for socket API calls */
#include <netinet/in.h> /* for address structs */
#include <arpa/inet.h>  /* for sockaddr_in */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for strlen() */
#include <unistd.h>     /* for close() */
#include <time.h>	/* for time */
#include <pthread.h> 	/* for threading , link with lpthread */
#include <string.h>     /* for comparing strings */

struct sockaddr_in sa;        /* socket address structure */
static int time_toggle=1, echo_toggle=1;	/* service flags */
int port8000=1, port8001=1, port8002=1;	/* port status flags */	

int ready(int);
void *at_port_8000(void *);
void *at_port_8001(void *);
void *at_port_8002(void *);
void update_port_status();

int ready(int port){
  	int sockfd;                     /* socket descriptor */
  	sockfd=socket(AF_INET,SOCK_STREAM,0);
  	if(sockfd<0){
     		printf("error in socket\n");
     		exit(0);
 	}
  
  	//else 
		//printf("----------------\nport %d opened\n----------------\n",port);
  	int optval = 1;
  	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
     	(const void *)&optval , sizeof(int));
  	bzero(&sa,sizeof(sa));
  	sa.sin_port=htons(port);
  	sa.sin_addr.s_addr=htonl(0);
  	if(bind(sockfd,(struct sockaddr*)&sa,sizeof(sa))<0){	
     		printf("Error in binding\n");
  	}
  	else
     		//printf("Binded Successfully\n");
     	if(listen(sockfd,5)<0)
		printf("Error in listening\n");
  	return sockfd;
}

int main(int argc, char *argv[]) {
	int sock[3];          	      /* socket descriptor */
	struct ip_mreq mc_req;        /* multicast request structure */
	char* mc_addr_str;            /* multicast IP address */
	unsigned short mc_port;       /* multicast port */
	struct sockaddr_in from_addr; /* packet source */

  	/* validate number of arguments */
  	if (argc != 2){
    		fprintf(stderr, "Usage: %s <Server IP>\n", argv[0]);
    		exit(1);
   	}

  	mc_addr_str = argv[1];  
  	sock[0] = ready(8000);
  	sock[1] = ready(8001);
  	sock[2] = ready(8002);
	update_port_status();
	pthread_t showtime, echo, settings;
	int *new_sock = malloc(1);
        *new_sock = sock[0];
	if( pthread_create( &showtime,NULL,at_port_8000,(void*) new_sock) < 0){
            perror("could not create thread-1");
            return 1;
        }
	int *new_sock2 = malloc(1);
        *new_sock2 = sock[1];
	if( pthread_create( &echo,NULL,at_port_8001,(void*) new_sock2) < 0){
            perror("could not create thread-2");
            return 1;
        }
	int *new_sock3 = malloc(1);
        *new_sock3 = sock[2];
	if( pthread_create( &settings,NULL,at_port_8002,(void*) new_sock3) < 0){
            perror("could not create thread-3");
            return 1;
        }
	pthread_exit(NULL);
 	 
  	close(sock[0]);
  	close(sock[1]);
  	close(sock[2]);
}

void *at_port_8000(void *socket_desc){
	while(1){
		int sock = *(int*)socket_desc;		//typecasting the socket descriptor to int
		struct sockaddr_in cli;
  		char str[100];
		int len, ch, conntfd;
  		time_t tick;
        	len=sizeof(ch);
		//printf("PORT     8000     LISTENING\n");
		port8000 = 1;
		update_port_status();
		conntfd=accept(sock,(struct sockaddr*)&cli,&len);	//accepting connection
		if (time_toggle){
        		//printf("PORT     8000     CONNECTED\n");
			port8000 = 0;
			update_port_status();
        		tick=time(NULL);
        		snprintf(str,sizeof(str),"------------------------\n%s------------------------\n",ctime(&tick));
        		//printf("%s",str);
			write(conntfd,str,strlen(str));
		}
		else 
			write(conntfd,"Time service is disabled.\n",26);
		close(conntfd);
	}
	//Free the socket pointer
    	free(socket_desc);
	
    	return 0;
}

void *at_port_8001(void *socket_desc){
	while(1){
		int sock = *(int*)socket_desc;		//typecasting the socket descriptor to int
		struct sockaddr_in cli;
		int len, ch, conntfd;
        	len=sizeof(ch);
		//printf("PORT     8001     LISTENING\n");
		port8001 = 1;
		update_port_status();
		conntfd=accept(sock,(struct sockaddr*)&cli,&len);	//accepting connection
		if (echo_toggle){	
			port8001 = 0;
			update_port_status();
        		//printf("PORT     8001     CONNECTED\n");
			int read_size;
    			char *message , client_message[1000];
    			message = "Type a message to server and server will echo back your message;\nEnter close to terminate the conncetion\n\n> ";
    			write(conntfd , message , strlen(message));
     
    			//Receive a message from client
    			while( (read_size = recv(conntfd , client_message , 1000 , 0)) > 0 ){
				if (strncmp(client_message,"close",5) != 0){  //if client message is not "close", keep connection alive
					message = "Server: ";
    					write(conntfd , message , strlen(message));
        				//Send the message back to client
        				write(conntfd , client_message , strlen(client_message));
					memset(client_message, 0, 1000);
    				}
				else
					break;	//break the loop if user enters "close"
			}
     	
			if(read_size == 0){
    				puts("Client disconnected");
    				fflush(stdout);
    			}
    			else if(read_size == -1){
    		    		perror("recv failed");
    			}
		}
		else
			write(conntfd,"Echo service is disabled.\n",26);
		close(conntfd);
	}
	//Free the socket pointer
    	free(socket_desc);
    	return 0;
}
void *at_port_8002(void *socket_desc){
	while(1){
		int sock = *(int*)socket_desc;		//typecasting the socket descriptor to int
		struct sockaddr_in cli;
  		char *message;
		int read_size;
    		char client_message[1000];
		int len, ch, conntfd;
  		time_t tick;
        	len=sizeof(ch);
		port8002 = 1;
		update_port_status();
		//printf("PORT     8002     LISTENING\n");
		conntfd=accept(sock,(struct sockaddr*)&cli,&len);	//accepting connection
		port8002 = 0;
		update_port_status();
        	//printf("PORT     8002     CONNECTED\n");
		message = "\n--------------------------------------------";
		write(conntfd , message , strlen(message));
		message = "\nWelcome to myserver.\n\n";
		write(conntfd , message , strlen(message));
	
		if (time_toggle)
			message = "The Time service is currently active.\n\n";
		else	
			message = "The Time service is currently inactive.\n\n";	
		write(conntfd , message , strlen(message));
		if (echo_toggle)	
			message = "The Echo service is currently active.\n\n";
		else
			message = "The Echo service is currently inactive.\n\n";
		write(conntfd , message , strlen(message));
		message = "Input 1 to toggle the time service on/off.\n\n";
		write(conntfd , message , strlen(message));
		message = "Input 2 to toggle the echo service on/off.\n\n";
		write(conntfd , message , strlen(message));
		message = "Input q to shutdown the server.\n\n";
		write(conntfd , message , strlen(message));
		message = "Input r to refresh the server.\n\n";
		write(conntfd , message , strlen(message));
		message = "Input any other key to terminate connection.\n";
		write(conntfd , message , strlen(message));
		message = "---------------------------------------------\n> ";
		write(conntfd , message , strlen(message));
	
		//Receive a message from client
	    	while( (read_size = recv(conntfd , client_message , 1000 , 0)) > 0 ){
			char* num_ptr = client_message;
			char* shutdown_var = malloc(1);
			char* refresh_var = malloc(1);
			*shutdown_var = 'q';
			*refresh_var = 'r';
	     		while(*num_ptr < '0' || *num_ptr > '9') //skip anything until there is digit from 0-9
	          		++num_ptr;
	     		int number = atoi(num_ptr);		//convert char* to int
			if (strncmp(shutdown_var,client_message,1) == 0){	//shutdown server if 'q' is entered
				printf("Shutting down the server...\n");
				shutdown(conntfd, SHUT_RDWR);
			}
			else if (strncmp(refresh_var,client_message,1) == 0){	//refresh server if 'r' is entered
				printf("Server Refreshed...\n");
			}
			else if(number == 1){
				time_toggle = !time_toggle;	//port 8000; time toggle
			}
			else if (number == 2)
				echo_toggle = !echo_toggle;	//port 80001; echo back toggle
			
			else{
				printf("connection terminated...\n");	//terminate server on any other key
				close(conntfd);
				break;
			}
			message = "---------------------------------------------\n";
			write(conntfd , message , strlen(message));
			if (time_toggle)
				message = "The Time service is currently active.\n\n";
			else	
				message = "The Time service is currently inactive.\n\n";	
			write(conntfd , message , strlen(message));
			if (echo_toggle)	
				message = "The Echo service is currently active.\n\n";
			else
				message = "The Echo service is currently inactive.\n\n";
			write(conntfd , message , strlen(message));
			message = "Input 1 to toggle the time service on/off.\n\n";
			write(conntfd , message , strlen(message));
			message = "Input 2 to toggle the echo service on/off.\n\n";
			write(conntfd , message , strlen(message));
			message = "Input q to shutdown the server.\n\n";
			write(conntfd , message , strlen(message));
			message = "Input r to refresh the server.\n\n";
			write(conntfd , message , strlen(message));
			message = "Input any other key to terminate connection.\n";
			write(conntfd , message , strlen(message));
			message = "---------------------------------------------\nInput > ";
			write(conntfd , message , strlen(message));
			
		}
		if(read_size == 0){	
	    		puts("Client disconnected");
    			fflush(stdout);
    		}
 		else if(read_size == -1){
    		    	perror("recv failed");
    		}
	}
	//Free the socket pointer
    	free(socket_desc);
    	return 0;
}

void update_port_status(){
	system("clear");
	printf("------------------------------------------------------\n");
	printf("Local Address        State           Service\n");
	printf("------------------------------------------------------\n");
	if (port8000)
		printf("127.0.0.1:8000       LISTENING       TIME_SERVICE\n");
	else
		printf("127.0.0.1:8000       CONNECTED       TIME_SERVICE\n");
	if (port8001)
		printf("127.0.0.1:8001       LISTENING       ECHO_BACK_SERVICE\n");
	else
		printf("127.0.0.1:8001       CONNECTED       ECHO_BACK_SERVICE\n");
	if (port8002)
		printf("127.0.0.1:8002       LISTENING       CONTROL_PANEL\n");
	else
		printf("127.0.0.1:8002       CONNECTED       CONTROL_PANEL\n");
	printf("------------------------------------------------------\n");
}
