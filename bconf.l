/* SPDX-License-Identifier: BSD-3-Clause */
%{
#include "mkconf.h"
#include "y.tab.h"

static char *
strdupesc(char *esc, size_t esclen) {
	char buf[esclen + 1];
	char *src = esc;
	char *dst = buf;

	while (*++src != '"') {
		if (*src == '\\') {
			switch (*++src) {
			case '\n':
				continue;
			case 't':
				*dst = '\t';
				break;
			case 'r':
				*dst = '\r';
				break;
			case 'n':
				*dst = '\n';
				break;
			default:
				*dst = *src;
				break;
			}
		} else {
			*dst = *src;
		}
		dst++;
	}
	*dst = '\0';

	return strdup(buf);
}
%}

%%
"config"   return T_CONFIG;
"defaults" return T_DEFAULTS;

[A-Z][A-Z0-9_]* {
	yylval.string = strdup(yytext);
	return T_NAME;
}

\".*\" {
	yylval.string = strdupesc(yytext, yyleng);
	return T_STRING;
}

\n { yylineno++; }
[ \t\r]+
%%
int
yywrap(void) {
	return feof(yyin);
}

void
yyerror(struct mkconf *mkconf, const char *message) {
	mkconf_err(mkconf, "%d: %s near '%s'", yylineno, message, yytext);
}
