#include "common.h"
#include "abuf.h"
#include "editor.h"
#include "rowops.h"

/** EDITOR OPERATIONS **/

void editorInsertChar(int c) {
	if (E.cy == E.numRows) {
		editorInsertRow(E.numRows, "", 0);
	}
	editorRowInsertChar(&E.row[E.cy], E.cx, c);
	E.cx++;
}

void editorInsertNewline() {
	if (E.cx == 0) {
		editorInsertRow(E.cy, "", 0);
		E.cx = 0;		
	} else {
		erow *row = &E.row[E.cy];
		int indentLevel = editorIndentationLevel(row);

		struct abuf ab = ABUF_INIT;
		int rowlen = row->size - E.cx;
		for (int i = 0; i < indentLevel; i++) abAppend(&ab, "\t", 1);	
		abAppend(&ab, &row->chars[E.cx], rowlen);

		editorInsertRow(E.cy + 1, ab.b, ab.len);

		row = &E.row[E.cy];
		row->size = E.cx;
		row->chars[row->size] = '\0';
		editorUpdateRow(row);

		E.cx = ab.len - rowlen;
		abFree(&ab);
	}
	E.cy++;
}

void editorDelChar() {
	if (E.cy == E.numRows) return;
	if (E.cx == 0 && E.cy == 0) return;

	erow *row = &E.row[E.cy];
	if (E.cx > 0) {
		editorRowDelChar(row, E.cx - 1);
		E.cx--;
	} else {
		E.cx = E.row[E.cy - 1].size;
		editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
		editorDelRow(E.cy);
		E.cy--;
	}
}
