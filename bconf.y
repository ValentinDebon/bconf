%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

#include "mkconf.h"

int yylex(void);
void yyerror(struct mkconf *, const char *);
%}

%union{
	struct mkconf_feature *feature;
	char *string;
}

%parse-param {struct mkconf *mkconf}

%token T_CONFIG T_DEFAULTS
%token <string> T_NAME T_STRING
%type <feature> feature features
%type <string> defaults

%%

toplevel: features { mkconf->features = $1; } ;

features: { $$ = NULL; }
	| feature features { $$ = $1; $$->next = $2; } ;

feature: T_CONFIG T_NAME T_STRING defaults {
		$$ = malloc(sizeof *$$);
#ifdef YYNOMEM
		if ($$ == NULL)
			YYNOMEM;
#endif
		$$->name = $2;
		$$->description = $3;
		$$->defaults = $4;
	} ;

defaults: { $$ = NULL; }
	| T_DEFAULTS T_STRING { $$ = $2; } ;

%%
