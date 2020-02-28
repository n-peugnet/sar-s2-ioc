/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

#include "client2.h"

int votes[4];
int portpolice;

void reset_votes()
{
	int i;

	for (i=0;i<4;i++)
		votes[i]=0;
}

void print_votes()
{
	int i;

	for (i=0;i<4;i++)
		printf("%d:%4d\n",i,votes[i]);
}

void traiterMessage(char *buffer, char *ip_src)
{
	char com;
	int val;

	sscanf(buffer,"%c %d",&com,&val);
	printf("com=%c val=%d\n",com,val);

	switch (buffer[0])
	{
		case 'V':
			votes[val]++;
			if (val != 1) {
				envoyer_message("localhost", portpolice, ip_src);
			}
			break;
		case 'R':
			reset_votes();
			break;
		case 'P':
			print_votes();
			break;
	}
}


int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	if (argc < 3) 
	{
		fprintf(stderr,"usage %s <dictature-port> <police-port>\n", argv[0]);
		exit(1);
	}

// 1) on crée la socket, SOCK_STREAM signifie TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

// 2) on réclame au noyau l'utilisation du port passé en paramètre 
// INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	portpolice = atoi(argv[2]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	error("ERROR on binding");


// On commence à écouter sur la socket. Le 5 est le nombre max
// de connexions pendantes

	listen(sockfd,5);

	reset_votes();

	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");

		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) 
			error("ERROR reading from socket");

		printf("Received packet from %s:%d\nData: [%s]\n\n",
				inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

		
		traiterMessage(buffer, inet_ntoa(cli_addr.sin_addr));

		close(newsockfd);
	}

	close(sockfd);
	return 0;
}
