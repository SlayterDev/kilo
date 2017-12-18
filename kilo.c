#include "common.h"
#include "terminal.h"
#include "fileio.h"
#include "input.h"

/** INIT **/

void initEditor() {
	E.tab_stop = KILO_TAB_STOP;
	E.quit_times = KILO_QUIT_TIMES;

	E.cx = 0;
	E.cy = 0;
	E.rx = 0;
	E.rowoff = 0;
	E.coloff = 0;
	E.numRows = 0;
	E.row = NULL;
	E.dirty = 0;
	E.insert = 0;
	E.filename = NULL;
	E.statusMessage[0] = '\0';
	E.statusmsg_time = 0;
	E.syntax = NULL;

	if (getWindowSize(&E.screenRows, &E.screenCols) == -1) die("getWindowSize");
	E.screenRows -= 2;
}

int main(int argc, char const *argv[]) {	
	enableRawMode();
	initEditor();
	editorReadConfigFile();
	if (argc >= 2) {
		editorOpen(argv[1]);
	}

	editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-F = find | Ctrl-Q = quit");

	while (1) {
		editorRefreshScreen();
		editorProcessKeypress();
	}

	return 0;
}
