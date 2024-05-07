#define _XOPEN_SOURCE 700
#include <locale.h>
#include <string.h>
#include <ncursesw/curses.h>

// Define the sizes of the various border panes
#define LEFT_WIDTH 15
#define RIGHT_WIDTH 15
#define CENTER_WIDTH (term_props.width - LEFT_WIDTH - RIGHT_WIDTH)
#define TOP_HEIGHT 2
#define BOTTOM_HEIGHT 2
#define CENTER_HEIGHT (term_props.height - TOP_HEIGHT - BOTTOM_HEIGHT)

#define MSG_AREA_WIDTH (CENTER_WIDTH - 1)
#define MSG_AREA_HEIGHT CENTER_HEIGHT

// Declare color pair numbers for UI
enum {
	COLPAIR_DEFAULT = 0,
	COLPAIR_MSG_BG,
	COLPAIR_SERVER_INFO,
	COLPAIR_SERVER_INFO_ONLINE,
	COLPAIR_SERVER_INFO_OFFLINE,
	COLPAIR_SERVER_INFO_NUM,
	COLPAIR_STATUS_BOX,
	COLPAIR_STATUS_BOX_ICON,
	COLPAIR_CONNECTED,

	COLPAIR_STATUS_OFFLINE,
	COLPAIR_STATUS_DO_NOT_DISTURB,
	COLPAIR_STATUS_IDLE,
	COLPAIR_STATUS_ONLINE,

	COLPAIR_TEST_USER_PINK,
	COLPAIR_TEST_USER_PURPLE,
	COLPAIR_TEST_USER_RED,
};

struct {
	int width, height;
	bool color;
	bool utf_8;
} term_props;

WINDOW *msg_area_win, *scroll_bar_win, *server_info_win, *channel_win,
       *status_win, *tab_win, *msg_box_win, *search_box_win, *user_win,
       *connect_win;

/*
 * Channel setup stuff
 */

struct channel {
	char *name;
	char *trailing_chars;
};

struct channel *galnet_dev_cat_channels[] = {
	&(struct channel) {"dev-main", "*"},
	&(struct channel) {"dev-backend", NULL},
	&(struct channel) {"dev-frontend", NULL},
	&(struct channel) {"dev-design", NULL},
	&(struct channel) {"dev-ports", NULL},
	&(struct channel) {"galnet-public", NULL},
};

struct channel *meowmeow_cat_channels[] = {
	&(struct channel) {"meow", NULL},
	&(struct channel) {"meowmeow", NULL},
	&(struct channel) {"meowmeowmeow", NULL},
};

struct channel *open_salami_cat_channels[] = {
	&(struct channel) {"hehehehe", NULL},
};

struct channel_category {
	char *name;
	char *trailing_chars;
	struct channel **channels;
	int length;
	bool opened;
} galnet_dev_cat = {
	.name = "GalNet Dev",
	.trailing_chars = "*+",
	.channels = galnet_dev_cat_channels,
	.length = 6,
	.opened = true,
}, meowmeow_cat = {
	.name = "MeowMeow",
	.trailing_chars = "+",
	.channels = meowmeow_cat_channels,
	.length = 3,
	.opened = true,
}, closed_cat = {
	.name = "Closed",
	.trailing_chars = NULL,
	.channels = NULL,
	.length = 0,
	.opened = false,
}, also_closed_cat = {
	.name = "Also closed",
	.trailing_chars = NULL,
	.channels = NULL,
	.length = 0,
	.opened = false,
}, open_salami_cat = {
	.name = "OPEN SALAMI",
	.trailing_chars = NULL,
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

/*
 * User list setup stuff
 */

enum user_status {
	STATUS_OFFLINE,
	STATUS_DO_NOT_DISTURB,
	STATUS_IDLE,
	STATUS_ONLINE,
};

const short status_colors[] = {
	COLPAIR_STATUS_OFFLINE,
	COLPAIR_STATUS_DO_NOT_DISTURB,
	COLPAIR_STATUS_IDLE,
	COLPAIR_STATUS_ONLINE,
};

struct user {
	char *name;
	enum user_status status;
	short color;
};

struct user *meowmeowmeow_cat_users[] = {
	&(struct user) {"ashie", STATUS_DO_NOT_DISTURB,
		COLPAIR_TEST_USER_PINK},
	&(struct user) {"Zenith", STATUS_ONLINE, COLPAIR_TEST_USER_PINK},
};

struct user *developers_cat_users[] = {
	&(struct user) {"Maya", STATUS_ONLINE, COLPAIR_TEST_USER_RED},
	&(struct user) {"gay", STATUS_IDLE, COLPAIR_TEST_USER_PURPLE},
	&(struct user) {"*Apples*", STATUS_IDLE, COLPAIR_TEST_USER_PURPLE},
};

struct user *offline_cat_users[] = {
	&(struct user) {"Eda", STATUS_OFFLINE, COLPAIR_TEST_USER_PINK},
	&(struct user) {"ryfox", STATUS_OFFLINE, COLPAIR_TEST_USER_PURPLE},
};

struct user_category {
	char *name;
	struct user **users;
	int length;
	short color;
} meowmeowmeow_cat = {
	"MEOWMEOWMEOW", meowmeowmeow_cat_users, 2, COLPAIR_TEST_USER_PINK
}, developers_cat = {
	"Developers", developers_cat_users, 3, COLPAIR_TEST_USER_PURPLE
}, offline_cat = {
	"Offline", offline_cat_users, 2, COLPAIR_DEFAULT
};

struct user_category *test_users[] = {
	&meowmeowmeow_cat,
	&developers_cat,
	&offline_cat,
};

struct server_users {
	struct user_category **cats;
	int length;
} user_cats = {test_users, 3};

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
		init_pair(COLPAIR_STATUS_BOX,
			-1, -1);
		init_pair(COLPAIR_STATUS_BOX_ICON,
			COLOR_GREEN, -1);
		init_pair(COLPAIR_CONNECTED,
			COLOR_GREEN, -1);

		init_pair(COLPAIR_STATUS_OFFLINE,
			-1, -1);
		init_pair(COLPAIR_STATUS_DO_NOT_DISTURB,
			COLOR_RED, -1);
		init_pair(COLPAIR_STATUS_IDLE,
			COLOR_YELLOW, -1);
		init_pair(COLPAIR_STATUS_ONLINE,
			COLOR_GREEN, -1);

		init_pair(COLPAIR_TEST_USER_PINK,
			COLOR_MAGENTA, -1);
		init_pair(COLPAIR_TEST_USER_PURPLE,
			COLOR_BLUE, -1);
		init_pair(COLPAIR_TEST_USER_RED,
			COLOR_RED, -1);
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
		waddch(win, ACS_HLINE);
	}
}

void try_add_double_vline(WINDOW *win) {
	if (term_props.utf_8) {
		wadd_wch(win, WACS_D_VLINE);
	} else {
		waddch(win, ACS_VLINE);
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
		// Write trailing characters
		if (cat->trailing_chars != NULL) {
			int x, y;
			getyx(channel_win, y, x);
			mvwaddstr(channel_win, y,
				LEFT_WIDTH - strlen(cat->trailing_chars),
				cat->trailing_chars);
		} else {
			waddch(channel_win, '\n');
		}
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

void status_box_draw(void) {
	status_win = newwin(BOTTOM_HEIGHT, LEFT_WIDTH,
		term_props.height - BOTTOM_HEIGHT, 0);
	// Fill background of box
	wmove(status_win, 0, 0);
	for (int i = 0; i < BOTTOM_HEIGHT; i++) {
		for (int j = 0; j < LEFT_WIDTH; j++) {
			waddch(status_win, ACS_CKBOARD);
		}
	}
	// Draw top half of status icon
	try_color_set(status_win, COLPAIR_STATUS_BOX, NULL);
	mvwaddch(status_win, 0, 0, ACS_ULCORNER | A_BOLD
		| TRY_COLPAIR(COLPAIR_STATUS_BOX_ICON));
	waddch(status_win, ACS_URCORNER | A_BOLD
		| TRY_COLPAIR(COLPAIR_STATUS_BOX_ICON));
	// Display username
	waddstr(status_win, "Zenith#0302");
	// Draw bottom half of status icon
	mvwaddch(status_win, 1, 0, ACS_LLCORNER | A_BOLD
		| TRY_COLPAIR(COLPAIR_STATUS_BOX_ICON));
	waddch(status_win, ACS_LRCORNER | A_BOLD
		| TRY_COLPAIR(COLPAIR_STATUS_BOX_ICON));
	// Display status
	waddstr(status_win, "Status lol");
}

void tab_bar_draw(void) {
	tab_win = newwin(TOP_HEIGHT, CENTER_WIDTH, 0, LEFT_WIDTH);
	// Draw tabs
	const int num_tabs = 2;
	const int active_tab = 1;
	for (int i = 0; i < num_tabs; i++) {
		if (i == active_tab) {
			wattron(tab_win, A_REVERSE);
		}
		waddch(tab_win, ACS_ULCORNER);
		waddstr(tab_win, "TAB ");
		waddch(tab_win, '1' + i);
		waddch(tab_win, ACS_URCORNER);
		if (i == active_tab) {
			wattroff(tab_win, A_REVERSE);
		}
	}
	waddch(tab_win, '\n');
	// Draw channel name
	const char *channel_name = "Channel name lol";
	waddstr(tab_win, channel_name);
	// Draw channel description
	const char *channel_desc = "Channel Desc.";
	int y, x;
	getyx(tab_win, y, x);
	mvwaddstr(tab_win, y, CENTER_WIDTH - strlen(channel_desc),
		channel_desc);
}

void msg_area_draw(void) {
	msg_area_win = newwin(MSG_AREA_HEIGHT, MSG_AREA_WIDTH,
		TOP_HEIGHT, LEFT_WIDTH);
	// Fill the background of the message area
	const chtype msg_bg_char = '`' | TRY_COLPAIR(COLPAIR_MSG_BG);
	for (int i = 0; i < MSG_AREA_HEIGHT; i++) {
		wmove(msg_area_win, i, 0);
		for (int j = 0; j < MSG_AREA_WIDTH; j++) {
			waddch(msg_area_win, msg_bg_char);
		}
	}
	// Write a test message
	const char *test_username = "Zenith";
	const char *test_msg = "lol imagine progress happening";
	try_color_set(msg_area_win, COLPAIR_TEST_USER_PINK, NULL);
	wattron(msg_area_win, A_BOLD);
	mvwaddstr(msg_area_win, MSG_AREA_HEIGHT - 1, 0, test_username);
	waddstr(msg_area_win, ": ");
	try_color_set(msg_area_win, COLPAIR_DEFAULT, NULL);
	wattroff(msg_area_win, A_BOLD);
	waddstr(msg_area_win, test_msg);
}

void scroll_bar_draw(void) {
	scroll_bar_win = newwin(MSG_AREA_HEIGHT, 1, TOP_HEIGHT,
		LEFT_WIDTH + MSG_AREA_WIDTH);
	// Draw upper part
	const int bar_size = 5;
	for (int i = 0; i < MSG_AREA_HEIGHT - bar_size; i++) {
		try_add_double_vline(scroll_bar_win);
	}
	// Draw lower part
	for (int i = 0; i < bar_size; i++) {
		waddch(scroll_bar_win, ' ' | A_REVERSE);
	}
}

void msg_box_draw(void) {
	msg_box_win = newwin(BOTTOM_HEIGHT, CENTER_WIDTH,
		term_props.height - BOTTOM_HEIGHT, LEFT_WIDTH);
	// Draw first line
	waddch(msg_box_win, ACS_LTEE);
	waddstr(msg_box_win, "Message...");
	int y, x;
	getyx(msg_box_win, y, x);
	for (int i = x; i < CENTER_WIDTH - 1; i++) {
		waddch(msg_box_win, ACS_S9);
	}
	waddch(msg_box_win, ACS_RTEE);
	// Draw second line
	waddch(msg_box_win, ACS_VLINE);
	waddch(msg_box_win, '+');
	waddch(msg_box_win, ACS_PI);
	waddch(msg_box_win, ACS_DARROW);
	waddch(msg_box_win, ACS_DIAMOND);
	waddch(msg_box_win, ACS_VLINE);
	const char *typing_msg = "*Apples* is typing";
	waddstr(msg_box_win, typing_msg);
	mvwaddch(msg_box_win, 1, CENTER_WIDTH - 3, ACS_VLINE);
	waddch(msg_box_win, '>' | A_REVERSE);
	waddch(msg_box_win, ACS_VLINE);
}

void search_box_draw(void) {
	search_box_win = newwin(TOP_HEIGHT, LEFT_WIDTH,
		0, term_props.width - LEFT_WIDTH);
	// Draw first line
	waddch(search_box_win, ACS_CKBOARD);
	waddstr(search_box_win, "Search...");
	mvwaddch(search_box_win, 0, RIGHT_WIDTH - 1, ACS_CKBOARD);
	// Draw second line
	chtype search_box_icons[] = {
		'A' | A_BOLD,
		ACS_UARROW | A_BOLD | A_UNDERLINE,
		ACS_PI | A_BOLD,
		ACS_BLOCK,
	};
	for (int i = 0; i < 4; i++) {
		waddch(search_box_win, ACS_CKBOARD);
		waddch(search_box_win, ACS_CKBOARD);
		waddch(search_box_win, search_box_icons[i]);
	}
	int y, x;
	getyx(search_box_win, y, x);
	for (int i = x; i < LEFT_WIDTH; i++) {
		waddch(search_box_win, ACS_CKBOARD);
	}
}

void user_list_draw(void) {
	user_win = newwin(CENTER_HEIGHT, RIGHT_WIDTH, TOP_HEIGHT,
		term_props.width - RIGHT_WIDTH);
	const chtype status_icons[] = {ACS_BULLET, '-', '*', ACS_DIAMOND};
	for (int i = 0; i < user_cats.length; i++) {
		struct user_category *cat = user_cats.cats[i];
		waddch(user_win, ACS_HLINE);
		// Write category name (in correct color)
		try_color_set(user_win, cat->color, NULL);
		wattron(user_win, A_BOLD);
		waddstr(user_win, cat->name);
		try_color_set(user_win, COLPAIR_DEFAULT, NULL);
		wattroff(user_win, A_BOLD);
		// Draw the line following the category name
		int y, x;
		getyx(user_win, y, x);
		for (int i = x; i < RIGHT_WIDTH; i++) {
			waddch(user_win, ACS_HLINE);
		}

		// Write users in category
		for (int j = 0; j < cat->length; j++) {
			struct user *user = cat->users[j];
			// Write status icon
			waddch(user_win, ' ');
			waddch(user_win, status_icons[user->status] | A_BOLD
				| TRY_COLPAIR(status_colors[user->status]));
			waddch(user_win, ' ');
			// Write username
			try_color_set(user_win, user->color, NULL);
			wattron(user_win, A_BOLD);
			waddstr(user_win, user->name);
			try_color_set(user_win, COLPAIR_DEFAULT, NULL);
			wattroff(user_win, A_BOLD);
			waddch(user_win, '\n');
		}
	}
}

void connect_info_draw(void) {
	connect_win = newwin(BOTTOM_HEIGHT, RIGHT_WIDTH,
		term_props.height - BOTTOM_HEIGHT,
		term_props.width - RIGHT_WIDTH);
	// Draw first line
	waddch(connect_win, ACS_CKBOARD);
	try_color_set(connect_win, COLPAIR_CONNECTED, NULL);
	wattron(connect_win, A_BOLD);
	waddstr(connect_win, "=Connected!");
	try_color_set(connect_win, COLPAIR_DEFAULT, NULL);
	wattroff(connect_win, A_BOLD);
	int y, x;
	getyx(connect_win, y, x);
	for (int i = x; i < RIGHT_WIDTH; i++) {
		waddch(connect_win, ACS_CKBOARD);
	}
	// Draw second line
	waddch(connect_win, ACS_CKBOARD);
	waddstr(connect_win, "Serv.er24ms");
	waddch(connect_win, ACS_CKBOARD);
	waddch(connect_win, '*');
	getyx(connect_win, y, x);
	for (int i = x; i < RIGHT_WIDTH; i++) {
		waddch(connect_win, ACS_CKBOARD);
	}
}

int main(void) {
	setup_term();

	server_info_draw();
	channel_list_draw();
	status_box_draw();
	tab_bar_draw();
	msg_area_draw();
	scroll_bar_draw();
	msg_box_draw();
	search_box_draw();
	user_list_draw();
	connect_info_draw();

	// Refresh everything
	refresh();
	wrefresh(server_info_win);
	wrefresh(channel_win);
	wrefresh(status_win);
	wrefresh(tab_win);
	wrefresh(msg_area_win);
	wrefresh(scroll_bar_win);
	wrefresh(msg_box_win);
	wrefresh(search_box_win);
	wrefresh(user_win);
	wrefresh(connect_win);

	getch(); // Wait for user input to terminate
	endwin();

	return 0;
}
