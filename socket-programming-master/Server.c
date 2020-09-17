#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

int main(int argc,char* argv[]){
	struct sockaddr_in serverAddr;
	int socketfd;

	if(argc < 2){
		fprintf(stderr,"use %s <port>\n",argv[0]);
		exit(-1);
	}

	int portno = atoi(argv[1]);

	socketfd = socket(AF_INET,SOCK_STREAM,0);
	if(!socketfd){
		perror("Error in opening socket");
		exit(1);
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //address from local host
	serverAddr.sin_port = htons(portno);

	printf("Try to bind...\n");
	if( bind(socketfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0 ){
		perror("failed to bind");
		exit(2);
	}
	printf("bind successful...\nNow server is in active state...Listening\n");

	if(listen(socketfd,5)){  //5 how much connection get backlog
		perror("error in listen: ");
		exit(4);
	}

	int optval = 1;
	setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval,sizeof(int));
	struct sockaddr_in clientAddr;
	char recvBuf[2048];
	int bufLen = sizeof(recvBuf);
	int recvLen = 0;
	int cliLen = sizeof(clientAddr);

	while(1){
		cliLen = sizeof(clientAddr);

		printf("Waiting to accept connection...\n");
		int childfd = accept(socketfd,(struct sockaddr*)&clientAddr,&cliLen);
		if(childfd < 0){
			perror("error in accept");
			continue;
		}
		printf("server accepted connection request...\n");
		printf("New connection from %s:%d: \n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port) );
		int forkRet = fork();
		if(forkRet == 0){
			printf("Port: %d Try to accept data from client...\n",ntohs(clientAddr.sin_port));
			if((recvLen = recv(childfd,recvBuf,bufLen,0))<0){
				perror("Error in recving");
				break;
			}
			if(recvLen == 0){
				printf("buff is empty...closing childfd\n");
				close(childfd);
				break;
			}
			recvBuf[recvLen] = 0;
			printf("recv from: %s:%s\n",inet_ntoa(clientAddr.sin_addr),recvBuf);

			if(strcmp(recvBuf,"Message-1") == 0){
				srand((unsigned) time(0));
				int port = (rand() % 20000);
				char udp[10];
    			int myInteger = 4711;
    			sprintf(udp, "%d", port);

				printf("Port = %d Try to send udp port to client...\n",ntohs(clientAddr.sin_port));
				if(send(childfd,udp,sizeof(udp)+1,0)<0){
					perror("error in sending");
					continue;
				}
				printf("Port = %d successfully sent udp port number\n",ntohs(clientAddr.sin_port));

				if(fork() == 0){
					struct sockaddr_in serverAddr1; //defining a socket
					int socketfd1;  //defining a socket identifier
					socketfd1 = socket(AF_INET,SOCK_DGRAM,0);
					if(!socketfd1){
						perror("Error in opening socket");
						exit(1);
					}
					serverAddr1.sin_family = AF_INET;
					serverAddr1.sin_addr.s_addr = htonl(INADDR_ANY);
					serverAddr1.sin_port = htons(atoi(udp));

					printf("Try to bind with new udp port...\n");
					if( bind(socketfd1,(struct sockaddr *)&serverAddr1,sizeof(serverAddr1)) < 0 ){
						perror("failed to bind");
						exit(2);
					}
					printf("bind with udp port successful\n");
					
					int optval1 = 1;
					setsockopt(socketfd1,SOL_SOCKET,SO_REUSEADDR,(const void *)&optval1,sizeof(int));
					struct sockaddr_in clientAddr1;
					char recvBuf1[2048];
					int bufLen1 = sizeof(recvBuf1);
					int recvLen1 = 0;
					int cliLen1 = sizeof(clientAddr1);
					
					while(1){
						cliLen1 = sizeof(clientAddr1);

						printf("Try to receive data from client...\n");
						if( (recvLen1 = recvfrom(socketfd1,recvBuf1,bufLen1,0,(struct sockaddr*)&clientAddr1,&cliLen1)) < 0 ){
							perror("Errot in recving");
							exit(4);
						}
						recvBuf1[recvLen1] = 0;
						printf("Recv from %s:%d:",inet_ntoa(clientAddr1.sin_addr),ntohs(clientAddr1.sin_port));
						printf("%s\n",recvBuf1);

						if(strcmp(recvBuf1,"exit")==0){
							printf("%d break the connection\n",ntohs(clientAddr1.sin_port));
							break;
						}
						printf("Port = %d Try to send data back to client...\n",ntohs(clientAddr.sin_port));
						if(sendto(socketfd1,recvBuf1,recvLen1,0,(struct sockaddr*)&clientAddr1,sizeof(clientAddr1)) < 0){
							perror("error in sending");
							exit(3);
						}
						printf("Port = %d data is sent successfully\n",ntohs(clientAddr.sin_port));
					}
				}
			}
			exit(0);
		}
	}
	return 0;
}
