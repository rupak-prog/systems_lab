#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>

int main(int argc,char *argv[]){
	struct sockaddr_in serverAddr;
	struct hostent *server;
	char *hostname;
	int socketfd;

	if(argc < 3){
		fprintf(stderr,"use %s <hostname> <port>\n",argv[0]);
		exit(-1);
	}

	hostname = argv[1];
	int portno = atoi(argv[2]);

	socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(!socketfd){
		perror("Error in opening socket");
		exit(1);
	}

	server = gethostbyname(hostname);
	if(server == NULL){
		fprintf(stderr,"Can't resolver hostname\n");
		exit(2);
	}

	bzero((char*)&serverAddr,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serverAddr.sin_addr.s_addr,server->h_length);
	serverAddr.sin_port = htons(portno);

	printf("Try to connect to server...\n");
	if(connect(socketfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0){
		perror("connect");
		exit(3);
	}
	printf("Connected to server ip =  %s with port = %d\n",inet_ntoa(serverAddr.sin_addr),ntohs(serverAddr.sin_port));
	while(1){
		char *buf = "Message-1";
		int bufLen = strlen(buf);
		if(strcmp(buf,":exit") == 0) break;
		
		printf("Try to send Message-1\n");
		if(send(socketfd,buf,bufLen+1,0)<0){
			perror("error in sending");
			exit(4);
		}
		printf("Message-1 is sent successfully\n");

		char recvBuf[2048];
		bufLen = sizeof(recvBuf);
		int recvLen = 0;
		int serLen = sizeof(serverAddr);
		
		printf("waiting to receive response from server....");
		if((recvLen = recv(socketfd,recvBuf,bufLen,0))<0){
			perror("error in receiving");
			exit(5);
		}

		recvBuf[recvLen] = 0;
		printf("recved UDP port %s\n",recvBuf);

		close(socketfd);

		struct sockaddr_in serverAddr1;
		struct hostent *server1;
		int socketfd1;
		
		socketfd1 = socket(AF_INET,SOCK_DGRAM,0);
		if(!socketfd1){
			perror("Error in opening socket");
			exit(1);
		}

		server1 = gethostbyname(hostname);
		if(server == NULL){
			fprintf(stderr,"Can't resolver hostname\n");
			exit(2);
		}

		bzero((char*)&serverAddr1,sizeof(serverAddr1));
		serverAddr1.sin_family = AF_INET;
		bcopy((char*)server1->h_addr, (char*)&serverAddr1.sin_addr.s_addr,server1->h_length);
		serverAddr1.sin_port = htons(atoi(recvBuf));
		
		sleep(2);
		while(1){
			printf("type text:\t");
			char buf1[100];
			scanf("%s",buf1);
			int bufLen1 = strlen(buf1);
			printf("send data to server...Message-3\n");
			if( sendto(socketfd1,buf1,bufLen1+1,0,(struct sockaddr*)&serverAddr1,sizeof(serverAddr1)) < 0){
				perror("error in sending");
				exit(3);
			}
			printf("Data is sent successfully\n");

			char recvBuf1[2048];
			bufLen1 = sizeof(recvBuf1);
			int recvLen1 = 0;
			int serLen1 = sizeof(serverAddr1);
			if(strcmp(buf1,"exit") == 0){
				printf("connection finished..\n");
				break;
			}

			printf("waiting to receive data from server....\n");
			if((recvLen1 = recvfrom(socketfd1,recvBuf1,bufLen1,0,(struct sockaddr*)&serverAddr1,&serLen1)) < 0){
				perror("error in receiving the data");
				exit(4);
			}

			recvBuf1[recvLen1] = 0;
			printf("Recv from%s:%d: ",inet_ntoa(serverAddr1.sin_addr),ntohs(serverAddr1.sin_port));
			printf("%s\n",recvBuf1);
		}
	}
	return 0;
}
