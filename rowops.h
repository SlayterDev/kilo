#ifndef ROWOPS_H
#define ROWOPS_H

int editorRowCxToRx(erow *, int);
int editorRowRxToCx(erow *, int);
int editorIndentationLevel(erow *);
void editorUpdateRow(erow *);
void editorInsertRow(int, char *, size_t);
void editorDelRow(int);
void editorRowInsertChar(erow *, int, int);
void editorRowAppendString(erow *, char *, size_t);
void editorRowDelChar(erow *, int);

#endif
