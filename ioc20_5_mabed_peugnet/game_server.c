#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define NB_CARDS 100

struct dist_card {
	int card_val;
	int player_sock;
};

int cards[NB_CARDS];

void cards_init(int *cards) {
	int i;
	for (i = 0; i < NB_CARDS; i++)
	{
		cards[i] = i + 1;
	}
}

void cards_shuffle(int *cards, int iter) {
	int i, a, b, tmp_card;
	srand(time (NULL));
	for (i = 0; i < iter; i++)
	{
		a = rand() % NB_CARDS;
		b = rand() % NB_CARDS;
		tmp_card = cards[a];
		cards[a] = cards[b];
		cards[b] = tmp_card;
	}
}

void cards_print(int *cards) {
	int i;
	for (i = 0; i < NB_CARDS; i++)
	{
		printf("%d ", cards[i]);
	}
	printf("\n");
}

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
			/* Connection request */
			send_message(sockfd, "to change");
			break;
		case 'P':
			/* Card played request */
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
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	cards_init(cards);
	cards_shuffle(cards, 1000);
	cards_print(cards);

	// Le client doit connaitre l'adresse IP du serveur, et son numero de port
	if (argc < 2) {
		fprintf(stderr,"usage %s port\n", argv[0]);
		exit(0);
	}

	// 1) on crée la socket, SOCK_STREAM signifie TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	// 2) on réclame au noyau l'utilisation du port passé en paramètre 
	// INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");


	// On commence à écouter sur la socket. Le 5 est le nombre max
	// de connexions pendantes

	listen(sockfd,5);
	while (1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");

		server_thread(newsockfd);

		close(newsockfd);
	}

	// On ferme la socket

	close(sockfd);
	return n;
}