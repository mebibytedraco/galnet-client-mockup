#include <curses.h>

int main(void) {
	initscr();
	printw("Hello world!! :3");
	refresh();
	getch();
	endwin();

	return 0;
}
