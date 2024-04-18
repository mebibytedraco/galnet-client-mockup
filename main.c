#define _XOPEN_SOURCE 700
#include <locale.h>
#include <ncursesw/curses.h>

int main(void) {
	setlocale(LC_ALL, "");
	initscr();
	curs_set(0);

	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);
	mvprintw(max_y / 2, (max_x - 17) / 2, "%d cols X %d rows", max_x,
			max_y);
	box(stdscr, 0, 0);
	mvadd_wch(0, 0, WACS_D_ULCORNER);

	refresh();
	getch();
	endwin();

	return 0;
}
