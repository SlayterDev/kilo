#include "common.h"
#include "fileio.h"
#include "syntax.h"
#include "terminal.h"
#include "rowops.h"

/** FILE I/O **/

char *editorRowsToString(int *buflen) {
	int totlen = 0;
	int j;
	for (j = 0; j < E.numRows; j++)
		totlen += E.row[j].size + 1;
	*buflen = totlen;

	char *buf = (char *)malloc(totlen);
	char *p = buf;
	for (j = 0; j < E.numRows; j++) {
		memcpy(p, E.row[j].chars, E.row[j].size);
		p += E.row[j].size;
		*p = '\n';
		p++;
	}

	return buf;
}

void editorOpen(const char *filename) {
	free(E.filename);
	E.filename = strdup(filename);

	editorSelectSyntaxHighlight();

	FILE *fp = fopen(filename, "r");
	if (!fp) {
		E.dirty = 0;
		return;
	}

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, fp)) != -1) {	
		while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
			linelen--;
		
		editorInsertRow(E.numRows, line, linelen);
	}
	free(line);
	fclose(fp);
	E.dirty = 0;
}

void editorSave() {
	if (E.filename == NULL) {
		E.filename = editorPrompt("Save as (ESC to cancel): %s", NULL);
		if (E.filename == NULL) {
			editorSetStatusMessage("Save aborted");
			return;
		}
		
		editorSelectSyntaxHighlight();
	}

	int len;
	char *buf = editorRowsToString(&len);

	int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
	if (fd != -1) {
		if (ftruncate(fd, len) != -1) {
			if (write(fd, buf, len) == len) {
				close(fd);
				free(buf);
				E.dirty = 0;
				editorSetStatusMessage("%d bytes written to %s", len, E.filename);
				return;
			}
		}
		close(fd);
	}

	free(buf);
	editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

void editorReadConfigFile() {
	char configPath[4096];
	snprintf(configPath, 4096, "%s/.kilorc", getenv("HOME"));

	FILE *fp = fopen(configPath, "r");
	if (!fp) {
		return;
	}

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, fp)) != -1) {
		char *tok = strtok(line, "=");
		char *val = strtok(NULL, "=");
		int valInt = atoi(val);
		if (!strcmp(tok, "tab_stop") && valInt != 0) {
			E.tab_stop = valInt;
		} else if (!strcmp(tok, "quit_times") && valInt != 0) {
			E.quit_times = valInt;
		}
	}
	free(line);
	fclose(fp);
}
