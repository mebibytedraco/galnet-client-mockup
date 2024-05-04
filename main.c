#define _XOPEN_SOURCE 700
#include <locale.h>
#include <ncursesw/curses.h>

// Define the sizes of the various border panes
#define LEFT_WIDTH 15
#define RIGHT_WIDTH 15
#define CENTER_WIDTH (term_props.width - LEFT_WIDTH - RIGHT_WIDTH)
#define TOP_HEIGHT 2
#define BOTTOM_HEIGHT 2
#define CENTER_HEIGHT (term_props.height - TOP_HEIGHT - BOTTOM_HEIGHT)

// Declare color pair numbers for UI
enum {
	COLPAIR_MSG_BG = 1,
	COLPAIR_SERVER_INFO,
	COLPAIR_SERVER_INFO_ONLINE,
	COLPAIR_SERVER_INFO_OFFLINE,
	COLPAIR_SERVER_INFO_NUM,
};

struct {
	int width, height;
	bool color;
} term_props;

WINDOW *server_info_win, *msg_win;

void setup_color(void) {
	if (has_colors()) {
		term_props.color = true;
		start_color(); // Instruct ncurses to initialize colors
		use_default_colors(); // Enable -1 as default color

		// Define color pairs for UI
		init_pair(COLPAIR_MSG_BG,
				COLOR_WHITE, -1);
		init_pair(COLPAIR_SERVER_INFO,
				-1, COLOR_MAGENTA);
		init_pair(COLPAIR_SERVER_INFO_ONLINE,
				COLOR_GREEN, COLOR_MAGENTA);
		init_pair(COLPAIR_SERVER_INFO_OFFLINE,
				COLOR_WHITE, COLOR_MAGENTA);
		init_pair(COLPAIR_SERVER_INFO_NUM,
				COLOR_WHITE, COLOR_MAGENTA);
	} else {
		term_props.color = false;
	}
}

void setup_term(void) {
	setlocale(LC_ALL, ""); // Enables UTF-8 (if present)
	initscr(); // Start ncurses
	setup_color(); // Attempts to initialize colors

	curs_set(0); // Hide the cursor

	// Get terminal size
	getmaxyx(stdscr, term_props.height, term_props.width);
}

#define TRY_COLPAIR(pair) (term_props.color ? COLOR_PAIR(pair) : 0)

void try_color_set(WINDOW *win, short pair, void *opts) {
	if (term_props.color) {
		wcolor_set(win, pair, opts);
	}
}

void server_info_draw(void) {
	server_info_win = newwin(2, LEFT_WIDTH, 0, 0);

	try_color_set(server_info_win, COLPAIR_SERVER_INFO, NULL);
	// Draw shaded background
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < LEFT_WIDTH; j++) {
			waddch(server_info_win, ACS_CKBOARD);
		}
		waddch(server_info_win, '\n');
	}
	// Write the name of the server, centered
	mvwaddstr(server_info_win, 0, (LEFT_WIDTH - 11) / 2,
			"Test Server");
	// Draw icon for online users
	mvwaddch(server_info_win, 1, 3,
			ACS_DIAMOND | TRY_COLPAIR(COLPAIR_SERVER_INFO_ONLINE)
			| A_BOLD);
	// Write number of online users
	try_color_set(server_info_win, COLPAIR_SERVER_INFO_NUM, NULL);
	wprintw(server_info_win, "%d", 42);
	// Draw icon for offline users
	mvwaddch(server_info_win, 1, 9, ACS_DIAMOND
			| TRY_COLPAIR(COLPAIR_SERVER_INFO_OFFLINE));
	// Write number of offline users
	wprintw(server_info_win, "%d", 69);
	// Reset color to normal for server info
	try_color_set(server_info_win, COLPAIR_SERVER_INFO, NULL);
}

void msg_area_draw(void) {
	msg_win = newwin(CENTER_HEIGHT, CENTER_WIDTH, 2, LEFT_WIDTH);
	// Fill the background of the message area
	const chtype msg_bg_char = '`' | TRY_COLPAIR(COLPAIR_MSG_BG);
	for (int i = 0; i < CENTER_HEIGHT; i++) {
		wmove(msg_win, i, 0);
		for (int j = 0; j < CENTER_WIDTH; j++) {
			waddch(msg_win, msg_bg_char);
		}
	}
}

int main(void) {
	setup_term();

	server_info_draw();
	msg_area_draw();

	// Refresh everything
	refresh();
	wrefresh(msg_win);
	wrefresh(server_info_win);

	getch(); // Wait for user input to terminate
	endwin();

	return 0;
}
