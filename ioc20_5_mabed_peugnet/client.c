#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
	int i;
	char ip[16];
    	int sockfd, portno, n;
    	struct sockaddr_in serv_addr;
    	struct sockaddr_in sock_addr;
    	struct hostent *server;

    	char buffer[256];

	// Le client doit connaitre l'adresse IP du serveur, et son numero de port
    	if (argc < 3) {
       		fprintf(stderr,"usage %s hostname port\n", argv[0]);
       		exit(0);
    	}

    	portno = atoi(argv[2]);
	// 1) Création de la socket, INTERNET et TCP

    	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    	if (sockfd < 0) 
        	error("ERROR opening socket");
    
	server = gethostbyname(argv[1]);
    	if (server == NULL) {
        	fprintf(stderr,"ERROR, no such host\n");
        	exit(0);
    	}

	printf("\nserver name alias list:\n");
	for (i = 0; server->h_aliases[i]; ++i) {
		printf("%s\n", server->h_aliases[i]);
	}

	printf("\nip address list:\n");
	for (i = 0; server->h_addr_list[i]; ++i) {
		sock_addr.sin_addr = *((struct in_addr*) server->h_addr_list[i]);
		inet_ntop(AF_INET, &sock_addr.sin_addr, ip, sizeof(ip));
		printf("%s\n", ip);
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

	strcpy(buffer,"coucou\n");
        n = write(sockfd,buffer,strlen(buffer));

	// On ferme la socket

    	close(sockfd);
    	return 0;
}
