#include "common.h"
#include "syntax.h"

/** FILETYPES **/

char *C_HL_extensions[] = {".c", ".h", ".cpp", NULL};
char *C_HL_keywords[] = {
	"switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union", "typedef",
	"static", "enum", "class", "case",

	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|",
	NULL
};

char *PY_HL_extensions[] = {".py", NULL};
char *PY_HL_keywords[] = {
	"and", "as", "assert", "break", "class", "finally", "for", "from", "global", "if", "elif", "else",
	"import", "in", "is", "lambda", "nonlocal", "not", "or", "pass", "raise", "return", "try", "while", 
	"with", "yield", "def", "print", "del",

	"False|", "None|", "True|", "int|", "float|", "str|", "isinstance|", "list|",
	NULL
};

char *JS_HL_exstensions[] = {".js", ".json", NULL};
char *JS_HL_keywords[] = {
	"break", "case", "catch", "class", "continue", "debugger", "default", "delete", "do", "else", "export",
	"extends", "finally", "for", "function", "if", "import", "in", "instanceof", "new", "return", "switch",
	"throw", "try", "while", "with", "get", "implements", "interface", "set",

	"const|", "false|", "new|", "null|", "protected|", "super|", "this|", "true|", "typeof|", "var|", "abstract|",
	"boolean|", "byte|", "char|", "decimal|", "double|", "enum|", "final|", "float|", "int|", "internal|", "long|",
	"package|", "private|", "public|", "sbyte|", "short|", "static|", "uint|", "ulong|", "ushort|", "void|",
	NULL
};

struct editorSyntax HLDB[] = {
	{
		"c/c++",
		C_HL_extensions,
		C_HL_keywords,
		"//", "/*", "*/",
		HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_LIBSTRINGS
	},
	{
		"python",
		PY_HL_extensions,
		PY_HL_keywords,
		"#", NULL, NULL,
		HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
	},
	{
		"javascript",
		JS_HL_exstensions,
		JS_HL_keywords,
		"//", "/*", "*/",
		HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
	}
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

/** SYNTAX HIGHLIGHTING **/

int is_separator(int c) {
	return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];:", c) != NULL;
}

void editorUpdateSyntax(erow *row) {
	row->hl = realloc(row->hl, row->rsize);
	memset(row->hl, HL_NORMAL, row->rsize);

	if (E.syntax == NULL) return;

	char **keywords = E.syntax->keywords;

	char *scs = E.syntax->singleline_comment_start;
	char *mcs = E.syntax->multiline_comment_start;
	char *mce = E.syntax->multiline_comment_end;

	int scs_len = (scs) ? strlen(scs) : 0;
	int mcs_len = (mcs) ? strlen(mcs) : 0;
	int mce_len = (mce) ? strlen(mce) : 0;

	int prev_sep = 1;
	int in_string = 0;
	int in_comment = (row->idx > 0 && E.row[row->idx - 1].hl_open_comment);

	int i = 0;
	while (i < row->rsize) {
		char c = row->render[i];
		unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

		if (scs_len && !in_string && !in_comment) {
			if (!strncmp(&row->render[i], scs, scs_len)) {
				memset(&row->hl[i], HL_COMMENT, row->rsize - i);
				break;
			}
		}

		if (mcs_len && mce_len && !in_string) {
			if (in_comment) {
				row->hl[i] = HL_MLCOMMENT;
				if (!strncmp(&row->render[i], mce, mce_len)) {
					memset(&row->hl[i], HL_MLCOMMENT, mce_len);
					i += mce_len;
					in_comment = 0;
					prev_sep = 1;
					continue;
				} else {
					i++;
					continue;
				}
			} else if (!strncmp(&row->render[i], mcs, mcs_len)) {
				memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
				i += mcs_len;
				in_comment = 1;
				continue;
			}
		}

		if (E.syntax->flags & HL_HIGHLIGHT_STRINGS) {
			if (in_string) {
				row->hl[i] = HL_STRING;
				if (c == '\\' && i + 1 < row->rsize) {
					row->hl[i + 1] = HL_STRING;
					i += 2;
					continue;
				}
				if (c == in_string) in_string = 0;
				i++;
				prev_sep = 1;
				continue;
			} else {
				if ((c == '"' || c == '\'') || (E.syntax->flags & HL_HIGHLIGHT_LIBSTRINGS && c == '<' && 
				!is_separator(row->render[i + 1]))) {
					in_string = (c == '<') ? '>' : c;
					row->hl[i] = HL_STRING;
					i++;
					continue;
				}
			}
		}

		if (E.syntax->flags & HL_HIGHLIGHT_NUMBERS) {
			if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)) {
				row->hl[i] = HL_NUMBER;
				i++;
				prev_sep = 0;
				continue;
			}
		}

		if (prev_sep) {
			int j;
			for (j = 0; keywords[j]; j++) {
				int klen = strlen(keywords[j]);
				int kw2 = keywords[j][klen - 1] == '|';
				if (kw2) klen--;

				if (!strncmp(&row->render[i], keywords[j], klen) && is_separator(row->render[i + klen])) {
					memset(&row->hl[i], (kw2) ? HL_KEYWORD2 : HL_KEYWORD1, klen);
					i += klen;
					break;
				}
			}
			if (keywords[j] != NULL) {
				prev_sep = 0;
				continue;
			}
		}

		prev_sep = is_separator(c);
		i++;
	}

	int changed = (row->hl_open_comment != in_comment);
	row->hl_open_comment = in_comment;
	if (changed && row->idx + 1 < E.numRows)
		editorUpdateSyntax(&E.row[row->idx + 1]);
}

int editorSyntaxToColor(int hl) {
	switch (hl) {
		case HL_COMMENT:
		case HL_MLCOMMENT: return 36;
		case HL_KEYWORD1: return 33;
		case HL_KEYWORD2: return 32;
		case HL_STRING: return 35;
		case HL_NUMBER: return 31;
		case HL_MATCH: return 34;
		default: return 37;
	}
}

void editorSelectSyntaxHighlight() {
	E.syntax = NULL;
	if (E.filename == NULL) return;

	char *ext = strrchr(E.filename, '.');

	for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
		struct editorSyntax *s = &HLDB[j];
		unsigned int i = 0;
		while (s->filematch[i]) {
			int is_ext = (s->filematch[i][0] == '.');
			if ((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(E.filename, s->filematch[i]))) {
				E.syntax = s;

				for (int filerow = 0; filerow < E.numRows; filerow++)
					editorUpdateSyntax(&E.row[filerow]);

				return;
			}
			i++;
		}
	}
}
