#include "common.h"
#include "render.h"
#include "rowops.h"
#include "abuf.h"
#include "syntax.h"

/** OUTPUT **/

void editorScroll() {
	E.rx = 0;
	if (E.cy < E.numRows)
		E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);

	if (E.cy < E.rowoff) {
		E.rowoff = E.cy;
	}
	if (E.cy >= E.rowoff + E.screenRows) {
		E.rowoff = E.cy - E.screenRows + 1;
	}
	if (E.rx < E.coloff) {
		E.coloff = E.rx;
	}
	if (E.rx >= E.coloff + E.screenCols) {
		E.coloff = E.rx - E.screenCols + 1;
	}
}

void editorDrawRows(struct abuf *ab) {
	for (int y = 0; y < E.screenRows; y++) {
		int filerow = y + E.rowoff;
		if (filerow >= E.numRows) {
			if (E.numRows == 0 && y == E.screenRows / 3) {
				char welcome[80];
				int welcomeLen = snprintf(welcome, sizeof(welcome), "Kilo Editor --- version %s", KILO_VERSION);
				if (welcomeLen > E.screenCols) welcomeLen = E.screenCols;
				int padding = (E.screenCols - welcomeLen) / 2;
				if (padding) {
					abAppend(ab, "~", 1);
					padding--;
				}
				while (padding--) abAppend(ab, " ", 1);
				abAppend(ab, welcome, welcomeLen);
			} else {
				abAppend(ab, "~", 1);
			}
		} else {
			int len = E.row[filerow].rsize - E.coloff;
			if (len < 0) len = 0;
			if (len > E.screenCols) len = E.screenCols;
			
			char *c = &E.row[filerow].render[E.coloff];
			unsigned char *hl = &E.row[filerow].hl[E.coloff];
			int current_color = -1;
			for (int j = 0; j < len; j++) {
				if (iscntrl(c[j])) {
					char sym = (c[j] <= 26) ? '@' + c[j] : '?';
					abAppend(ab, "\x1b[7m", 4);
					abAppend(ab, &sym, 1);
					abAppend(ab, "\x1b[m", 3);
					if (current_color != -1) {
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
						abAppend(ab, buf, clen);
					}
				} else if (hl[j] == HL_NORMAL) {
					if (current_color != -1) {
						abAppend(ab, "\x1b[39m", 5);
						current_color = -1;	
					}
					
					abAppend(ab, &c[j], 1);
				} else {
					int color = editorSyntaxToColor(hl[j]);
					if (color != current_color) {
						current_color = color;
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
						abAppend(ab, buf, clen);
					}

					abAppend(ab, &c[j], 1);
				}
			}
			abAppend(ab, "\x1b[39m", 5);
		}

		abAppend(ab, "\x1b[K", 3);
		abAppend(ab, "\r\n", 2);
	}
}

void editorDrawStatusBar(struct abuf *ab) {
	abAppend(ab, "\x1b[7m", 4); // invert colors

	char status[80], rStatus[80];
	
	int len = snprintf(status, sizeof(status), "%.20s - %d lines %s", (E.filename) ? E.filename : "[No Name]", 
		E.numRows, (E.dirty) ? "(modified)" : "");
	int rlen = snprintf(rStatus, sizeof(rStatus), "%s | %d/%d", (E.syntax) ? E.syntax->filetype : "no ft",
		E.cy + 1, E.numRows);

	if (len > E.screenCols) len = E.screenCols;
	abAppend(ab, status, len);

	while (len < E.screenCols) {
		if (E.screenCols - len == rlen) {
			abAppend(ab, rStatus, rlen);
			break;
		} else {
			abAppend(ab, " ", 1);
			len++;
		}
	}
	abAppend(ab, "\x1b[m", 3); // revert colors
	abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
	abAppend(ab, "\x1b[K", 3);
	int msglen = strlen(E.statusMessage);
	if (msglen > E.screenCols) msglen = E.screenCols;
	if (msglen && time(NULL) - E.statusmsg_time < 5)
		abAppend(ab, E.statusMessage, msglen);
}

void editorRefreshScreen() {
	editorScroll();

	struct abuf ab = ABUF_INIT;

	abAppend(&ab, "\x1b[?25l", 6);
	abAppend(&ab, "\x1b[H", 4);

	editorDrawRows(&ab);
	editorDrawStatusBar(&ab);
	editorDrawMessageBar(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.rx - E.coloff) + 1);
	abAppend(&ab, buf, strlen(buf));

	abAppend(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, ab.len);
	abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(E.statusMessage, sizeof(E.statusMessage), fmt, ap);
	va_end(ap);
	E.statusmsg_time = time(NULL);
}
