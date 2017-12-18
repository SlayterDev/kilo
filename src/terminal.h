#ifndef TERMINAL_H
#define TERMINAL_H

void die(const char *);
void enableRawMode();
void disableRawMode();
int editorReadKey();
int getWindowSize(int *, int *);

#endif
