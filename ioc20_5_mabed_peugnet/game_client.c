#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int send_message(int sockfd, char *message)
{
	char buffer[256];
	sprintf(buffer,"%s\n", message);
	return write(sockfd,buffer,strlen(buffer));
}


void treat_message(int sockfd, char *buffer)
{
	char com;
	int val;

	sscanf(buffer,"%c %d",&com,&val);
	printf("com=%c val=%d\n",com,val);

	switch (buffer[0])
	{
		case 'C':
			/* Connected */
			break;
		case 'D':
			/* Card distributed */
			send_message(sockfd, "to change");
			break;
		case 'P':
			/* Card played */
			break;
	}
}

void server_thread(int sockfd) {
	char buffer[256];
	int n;
	while (1)
	{
		n = read(sockfd, buffer, 255);
		if (n < 0)
			error("ERROR reading from socket");

		printf("Received packet %s\n", buffer);
		treat_message(sockfd, buffer);
	}
}

int main(int argc, char *argv[])
{
	char *hostname;
	int portno;
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	// Le client doit connaitre l'adresse IP du serveur, et son numero de port
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	hostname = argv[1];
	portno = atoi(argv[2]);


	// 1) Création de la socket, INTERNET et TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
		error("ERROR opening socket");
	
	server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		return 0;
	}

	// On donne toutes les infos sur le serveur
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	// On se connecte. L'OS local nous trouve un numéro de port, grâce auquel le serveur
	// peut nous renvoyer des réponses

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");

	server_thread(sockfd);

	// On ferme la socket

	close(sockfd);
	return n;
}