#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "client2.h"

#define WIDTH 30
#define HEIGHT 10 

int startx = 0;
int starty = 0;

char *choices[] = {
	"Choix 1",
	"Choix 2",
	"Choix 3",
	"Choix 4",
};
int n_choices = sizeof(choices) / sizeof(char *);
void print_menu(WINDOW *menu_win, int highlight);

int main(int argc, char *argv[])
{
	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;

	char *hostname;
	int portno;

	// Le client doit connaitre l'adresse IP du serveur, et son numero de port
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	hostname = argv[1];
	portno = atoi(argv[2]);

	initscr();
	clear();
	noecho();
	cbreak();
	/* Line buffering disabled. pass on everything */
	startx = (80 - WIDTH) / 2;
	starty = (24 - HEIGHT) / 2;
		
	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
	refresh();
	print_menu(menu_win, highlight);
	while(1)
	{
		c = wgetch(menu_win);
		switch(c)
		{	case KEY_UP:
				if(highlight == 1)
					highlight = n_choices;
				else
					--highlight;
				break;
			case KEY_DOWN:
				if(highlight == n_choices)
					highlight = 1;
				else
					++highlight;
				break;
			case 10:
				choice = highlight;
				char buff[5];
				sprintf(buff, "V %d\n", choice);
				envoyer_message(hostname, portno, buff);
				break;
			default:
				mvprintw(24, 0, "Character pressed is = %3d Hopefully it can be printed as '%c'", c, c);
				refresh();
				break;
		}
		print_menu(menu_win, highlight);
		//if(choice != 0)	/* User did a choice come out of the infinite loop */
		//	break;
	}
	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
	clrtoeol();
	refresh();
	endwin();
	return 0;
}


void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;

	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < n_choices; ++i)
	{
		if(highlight == i + 1) /* High light the present choice */
		{	wattron(menu_win, A_REVERSE); 
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}
