#include "keyInput.h"

#include <assert.h>
#include <unistd.h>

#define NIL (0)

int main () {
	//Ncurses Attempt
	/*initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	notimeout(stdscr, TRUE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	scrollok(stdscr, TRUE);*/

	Display * dpy = XOpenDisplay(NIL);
	assert(dpy);

	int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
	int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));


	int ch;
	while (TRUE){
		if ((ch = getch()) == ERR){
			// User hasn't hit a key yet
		}else{
			// User has hit a key
			waddch(stdscr, ch | A_UNDERLINE | COLOR_PAIR(3));
			wrefresh(stdscr);
		}
	}


	endwin();
}