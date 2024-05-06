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
	bool utf_8;
} term_props;

WINDOW *server_info_win, *msg_win, *channel_win;

struct channel {
	char *name;
};

struct channel *galnet_dev_cat_channels[] = {
	&(struct channel) {"dev-main"},
	&(struct channel) {"dev-backend"},
	&(struct channel) {"dev-frontend"},
	&(struct channel) {"dev-design"},
	&(struct channel) {"dev-ports"},
	&(struct channel) {"galnet-public"},
};

struct channel *meowmeow_cat_channels[] = {
	&(struct channel) {"meow"},
	&(struct channel) {"meowmeow"},
	&(struct channel) {"meowmeowmeow"},
};

struct channel *open_salami_cat_channels[] = {
	&(struct channel) {"hehehehe"},
};

struct channel_category {
	char *name;
	struct channel **channels;
	int length;
	bool opened;
} galnet_dev_cat = {
	.name = "GalNet Dev",
	.channels = galnet_dev_cat_channels,
	.length = 6,
	.opened = true,
}, meowmeow_cat = {
	.name = "MeowMeow",
	.channels = meowmeow_cat_channels,
	.length = 3,
	.opened = true,
}, closed_cat = {
	.name = "Closed",
	.channels = NULL,
	.length = 0,
	.opened = false,
}, also_closed_cat = {
	.name = "Also closed",
	.channels = NULL,
	.length = 0,
	.opened = false,
}, open_salami_cat = {
	.name = "OPEN SALAMI",
	.channels = open_salami_cat_channels,
	.length = 1,
	.opened = true,
};

struct channel_category *test_categories[] = {
	&galnet_dev_cat,
	&meowmeow_cat,
	&closed_cat,
	&also_closed_cat,
	&open_salami_cat,
};

struct server_channels {
	struct channel_category **cats;
	int length;
} channel_cats = {
	.cats = test_categories,
	.length = 5,
};

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

	term_props.utf_8 = true; // TODO: Guess if terminal has UTF-8

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

void try_add_double_hline(WINDOW *win) {
	if (term_props.utf_8) {
		wadd_wch(win, WACS_D_HLINE);
	} else {
		addch('=');
	}
}

void msg_area_draw(void) {
	msg_win = newwin(CENTER_HEIGHT, CENTER_WIDTH, TOP_HEIGHT, LEFT_WIDTH);
	// Fill the background of the message area
	const chtype msg_bg_char = '`' | TRY_COLPAIR(COLPAIR_MSG_BG);
	for (int i = 0; i < CENTER_HEIGHT; i++) {
		wmove(msg_win, i, 0);
		for (int j = 0; j < CENTER_WIDTH; j++) {
			waddch(msg_win, msg_bg_char);
		}
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
	// Draw icon for offline users (inverted if color is not enabled)
	chtype offline_icon_attr = term_props.color
		? TRY_COLPAIR(COLPAIR_SERVER_INFO_OFFLINE) : A_REVERSE;
	mvwaddch(server_info_win, 1, 9, ACS_DIAMOND | offline_icon_attr);
	// Write number of offline users
	wprintw(server_info_win, "%d", 69);
	// Reset color to normal for server info
	try_color_set(server_info_win, COLPAIR_SERVER_INFO, NULL);
}

void channel_list_draw(void) {
	channel_win = newwin(CENTER_HEIGHT, LEFT_WIDTH, TOP_HEIGHT, 0);
	// Draw "Hide Channels" label
	mvwaddch(channel_win, 0, 0, 'v' | A_REVERSE);
	waddstr(channel_win, "Hide Channels\n");
	// Draw separator
	for (int i = 0; i < LEFT_WIDTH; i++) {
		try_add_double_hline(channel_win);
	}

	for (int i = 0; i < channel_cats.length; i++) {
		struct channel_category *cat = channel_cats.cats[i];
		// Add dropdown icon (opened or closed)
		char dropdown_char = cat->opened ? 'v' : '>';
		waddch(channel_win, dropdown_char | A_REVERSE);
		// Write category name
		waddstr(channel_win, cat->name);
		waddch(channel_win, '\n');
		if (cat->opened) {
			for (int j = 0; j < cat->length; j++) {
				struct channel *channel = cat->channels[j];
				// Add tree lines
				chtype tree_char = ACS_LTEE;
				if (j == cat->length - 1) {
					tree_char = ACS_LLCORNER;
				}
				waddch(channel_win, tree_char);
				// Write channel name
				waddstr(channel_win, channel->name);
				waddch(channel_win, '\n');
			}
		}
	}
}

int main(void) {
	setup_term();

	server_info_draw();
	channel_list_draw();
	msg_area_draw();

	// Refresh everything
	refresh();
	wrefresh(server_info_win);
	wrefresh(channel_win);
	wrefresh(msg_win);

	getch(); // Wait for user input to terminate
	endwin();

	return 0;
}
