#define _XOPEN_SOURCE 700
#include <locale.h>
#include <ncursesw/curses.h>

#define CHANNELS_PANE_WIDTH 15
#define USERS_PANE_WIDTH 15
#define MSG_AREA_WIDTH (max_x - CHANNELS_PANE_WIDTH - USERS_PANE_WIDTH)

#define COLPAIR_X_BUTTON 1
#define COLPAIR_UL_SYMBOL 2
#define COLPAIR_SERVER_INFO 3
#define COLPAIR_MSG_BG 4
#define COLPAIR_SERVER_INFO_ONLINE 5
#define COLPAIR_SERVER_INFO_OFFLINE 6
#define COLPAIR_SERVER_INFO_NUM 7

int main(void) {
	setlocale(LC_ALL, ""); // Enables UTF-8
	initscr(); // Start ncurses

	// Setup color (assumes color is supported)
	start_color();
	use_default_colors();
	init_pair(COLPAIR_X_BUTTON, -1, COLOR_RED); // white on red
	// magenta on blue
	init_pair(COLPAIR_UL_SYMBOL, COLOR_MAGENTA, COLOR_BLUE);
	init_pair(COLPAIR_SERVER_INFO, -1, COLOR_MAGENTA);
	init_pair(COLPAIR_MSG_BG, COLOR_WHITE, -1);
	init_pair(COLPAIR_SERVER_INFO_ONLINE, COLOR_GREEN, COLOR_MAGENTA);
	init_pair(COLPAIR_SERVER_INFO_OFFLINE, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(COLPAIR_SERVER_INFO_NUM, COLOR_WHITE, COLOR_MAGENTA);

	curs_set(0); // Hide the cursor

	// Get terminal size
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	// Set up server name/active users
	WINDOW *server_info_win = newwin(2, CHANNELS_PANE_WIDTH, 0, 0);
	wcolor_set(server_info_win, COLPAIR_SERVER_INFO, NULL);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < CHANNELS_PANE_WIDTH; j++) {
			waddch(server_info_win, ACS_CKBOARD);
		}
		waddch(server_info_win, '\n');
	}
	mvwaddstr(server_info_win, 0, (CHANNELS_PANE_WIDTH - 11) / 2,
			"Test Server");
	mvwaddch(server_info_win, 1, 3,
			ACS_DIAMOND | COLOR_PAIR(COLPAIR_SERVER_INFO_ONLINE));
	wcolor_set(server_info_win, COLPAIR_SERVER_INFO_NUM, NULL);
	wprintw(server_info_win, "%d", 42);
	mvwaddch(server_info_win, 1, 9,
			ACS_DIAMOND | COLOR_PAIR(COLPAIR_SERVER_INFO_OFFLINE));
	wprintw(server_info_win, "%d", 69);
	wcolor_set(server_info_win, COLPAIR_SERVER_INFO, NULL);

	// Set up message area
	WINDOW *msg_win = newwin(max_y - 4, MSG_AREA_WIDTH, 2,
			CHANNELS_PANE_WIDTH);
	for (int i = 0; i < max_y - 4; i++) {
		wmove(msg_win, i, 0);
		for (int j = 0; j < MSG_AREA_WIDTH; j++) {
			waddch(msg_win, '`' | COLOR_PAIR(COLPAIR_MSG_BG));
		}
	}

	// Draw the rest of the screen
	mvprintw(max_y / 2, (max_x - 17) / 2, "%d cols X %d rows", max_x,
			max_y);
	//box(stdscr, 0, 0);
	/*border_set(WACS_D_VLINE, WACS_D_VLINE, WACS_D_HLINE, WACS_D_HLINE,
			WACS_D_ULCORNER, WACS_D_URCORNER, WACS_D_LLCORNER,
			WACS_D_LRCORNER);*/
	//mvadd_wch(max_y - 1, 0, WACS_D_LLCORNER);

	// Draw everything
	refresh();
	wrefresh(msg_win);
	wrefresh(server_info_win);

	getch(); // Wait for user input to terminate
	endwin();

	return 0;
}
