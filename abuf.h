#ifndef ABUF_H
#define ABUF_H

#include <string.h>

struct abuf {
	char *b;
	int len;
};

#define ABUF_INIT {NULL, 0};

void abAppend(struct abuf *, const char *, int);
void abFree(struct abuf *);

#endif
