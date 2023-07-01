/* SPDX-License-Identifier: BSD-3-Clause */
#include "mkconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <err.h>

#include "y.tab.h"

struct mkconf_args {
	const char *bconf;
	const char *output;
	const char *output_template;
	const char *makefile_template;
};

void
mkconf_err(struct mkconf *mkconf, const char *format, ...) {
	struct mkconf_error *error = malloc(sizeof (*error));
	va_list ap;
	int err;

	if (error == NULL) {
		return;
	}

	va_start(ap, format);
	err = vasprintf(&error->message, format, ap);
	va_end(ap);

	if (err > 0) {
		error->next = NULL;
		if (mkconf->errors.last != NULL) {
			mkconf->errors.last->next = error;
		} else {
			mkconf->errors.first = error;
		}
		mkconf->errors.last = error;
	} else {
		free(error);
	}
}

static void
mkconf_init_from_name_and_version(struct mkconf *mkconf, char *name, char *version) {

	if (*name == '\0') {
		errx(EXIT_FAILURE, "Package name cannot be empty");
	}

	if (*name == '.') {
		errx(EXIT_FAILURE, "Package name '%s' cannot begin with a '.'", name);
	}

	for (unsigned int i = 0; name[i] != '\0'; i++) {
		if (name[i] == '/' || name[i] == '-') {
			errx(EXIT_FAILURE, "Unexpected '%c' in package name '%s'", name[i], name);
		}
	}

	if (*version == 'v') {
		version++;
	}

	if (*version == '\0') {
		version = "none";
	}

	for (unsigned int i = 0; version[i] != '\0'; i++) {
		if (version[i] == '/' || version[i] == '-') {
			errx(EXIT_FAILURE, "Unexpected '%c' in package version '%s'", version[i], version);
		}
	}

	mkconf->package.name = name;
	mkconf->package.version = version;

	mkconf->errors.first = mkconf->errors.last = NULL;
}

static void
mkconf_init_from_name(struct mkconf *mkconf, char *namever) {
	char *separator = strchr(namever, '-');
	char *name, *version;

	name = namever;
	if (separator == NULL) {
		version = "";
	} else {
		version = separator + 1;
	}

	mkconf_init_from_name_and_version(mkconf, name, version);
}

static void
mkconf_init(struct mkconf *mkconf) {
	static char workdir[PATH_MAX];
	char *namever;

	if (getcwd(workdir, sizeof (workdir)) == NULL) {
		err(EXIT_FAILURE, "getcwd");
	}

	namever = basename(workdir);
	if (namever == NULL) {
		errx(EXIT_FAILURE, "basename %s", workdir);
	}

	mkconf_init_from_name(mkconf, namever);
}

static void
mkconf_print_errors(struct mkconf *mkconf, const char *prefix) {
	struct mkconf_error *error = mkconf->errors.first;

	do {
		fprintf(stderr, "%s:%s\n", prefix, error->message);
		error = error->next;
	} while (error != NULL);
}

static void
mkconf_parse_features(struct mkconf *mkconf, const char *path) {
	extern int yyparse(struct mkconf *);
	extern FILE *yyin;

	yyin = fopen(path, "r");
	if (yyin == NULL) {
		if (errno == ENOENT) {
			mkconf->features = NULL;
			return;
		}
		err(EXIT_FAILURE, "fopen '%s'", path);
	}

	if (yyparse(mkconf) != 0) {
		mkconf_print_errors(mkconf, path);
		errx(EXIT_FAILURE, "Unable to parse feature file '%s'", path);
	}
}

static void
mkconf_open_files(struct mkconf *mkconf, const struct mkconf_args *args) {

	mkconf->makefile_template = fopen(args->makefile_template, "r");
	if (mkconf->makefile_template == NULL) {
		err(EXIT_FAILURE, "fopen '%s'", args->makefile_template);
	}

	mkconf->output_template = fopen(args->output_template, "r");
	if (mkconf->output_template == NULL) {
		err(EXIT_FAILURE, "fopen '%s'", args->output_template);
	}

	mkconf->output = fopen(args->output, "w");
	if (mkconf->output == NULL) {
		err(EXIT_FAILURE, "fopen '%s'", args->output);
	}
}

static void
strntolower(char *dst, const char *src, size_t n) {
	size_t i = 0;

	while (src[i] != '\0') {
		dst[i] = tolower((unsigned char)src[i]);
		i++;
	}
	memset(dst + i, 0, n - i);
}

static void
und2dashes(char *dst, char *src, size_t n) {
	size_t i = 0;

	while (src[i] != '\0') {
		if (src[i] != '_') {
			dst[i] = src[i];
		} else {
			dst[i] = '-';
		}
		i++;
	}
	memset(dst + i, 0, n - i);
}

static void
fputs_escape_sh(const char *str, FILE *out) {
	static const char accept[] = "$\t\r\n\"";
	const char *prev = str, *curr;

	while (curr = strpbrk(prev, accept), curr != NULL) {
		fwrite(prev, 1, curr - prev, out);
		switch (*curr) {
		case '\t':
			fputs("\\t", out);
			break;
		case '\r':
			fputs("\\r", out);
			break;
		case '\n':
			fputs("\\n", out);
			break;
		default:
			fputc('\\', out);
			fputc(*curr, out);
			break;
		}
		prev = curr + 1;
	}
	fputs(prev, out);
}

static void
mkconf_preprocess_usage(struct mkconf *mkconf) {
	struct mkconf_feature *feature = mkconf->features;
	size_t longest = 0;

	while (feature != NULL) {
		const size_t length = strlen(feature->name);
		if (length > longest) {
			longest = length;
		}
		feature = feature->next;
	}
	feature = mkconf->features;

	while (feature != NULL) {
		const size_t length = strlen(feature->name);
		const int padding = (int)(longest - length);
		char opt[length + 1];

		strntolower(opt, feature->name, sizeof (opt));
		und2dashes(opt, opt, sizeof (opt));

		fprintf(mkconf->output, "\t--[enable|disable|without]-%s %*s", opt, padding, "");
		fputs_escape_sh(feature->description, mkconf->output);
		fprintf(mkconf->output, ".\n\t--with-%s=<value>             %*s", opt, padding + 4, "");
		if (feature->defaults != NULL) {
			fputs("where <value> defaults to '", mkconf->output);
			fputs_escape_sh(feature->defaults, mkconf->output);
			fputs("'.\n", mkconf->output);
		} else {
			fputs("where <value> is not set by default.\n", mkconf->output);
		}

		feature = feature->next;
	}
}

static void
mkconf_preprocess_defaults(struct mkconf *mkconf) {
	struct mkconf_feature *feature = mkconf->features;

	while (feature != NULL) {

		if (feature->defaults != NULL) {
			char lwr[strlen(feature->name) + 1];

			strntolower(lwr, feature->name, sizeof (lwr));

			fprintf(mkconf->output, "m_feature_%s=\"", lwr);
			fputs_escape_sh(feature->defaults, mkconf->output);
			fputs("\"\n", mkconf->output);
		}

		feature = feature->next;
	}
}

static void
mkconf_preprocess_getopt_long(struct mkconf *mkconf) {
	struct mkconf_feature *feature = mkconf->features;

	while (feature != NULL) {
		char lwr[strlen(feature->name) + 1];
		char opt[sizeof (lwr)];

		strntolower(lwr, feature->name, sizeof (lwr));
		und2dashes(opt, lwr, sizeof (opt));

		fprintf(mkconf->output,
			"\t--enable-%s) m_feature_%s=1 ;;\n"
			"\t--with-%s=*) m_feature_%s=$m_optarg ;;\n"
			"\t--disable-%s|--without-%s) unset m_feature_%s ;;\n",
			opt, lwr, opt, lwr, opt, opt, lwr);

		feature = feature->next;
	}
}

static void
mkconf_preprocess_package_vars(struct mkconf *mkconf) {
	/* No need to escape the strings, they have been checked earlier and should be safe */
	fprintf(mkconf->output,
		"m_package_name='%s'\n"
		"m_package_version='%s'\n",
		mkconf->package.name, mkconf->package.version);
}

static void
mkconf_preprocess_features_summary(struct mkconf *mkconf) {
	struct mkconf_feature *feature = mkconf->features;

	if (feature != NULL) {
		fputs("printf 'Selected features:\\n'\n", mkconf->output);
		do {
			char lwr[strlen(feature->name) + 1];

			strntolower(lwr, feature->name, sizeof (lwr));

			fprintf(mkconf->output, "printf '\\tCONFIG_%s is \"%%s\"\\n' \"${m_feature_%s}\"\n",
				feature->name, lwr);
			feature = feature->next;
		} while (feature != NULL);
	} else {
		fputs("printf 'No features defined.\\n'\n", mkconf->output);
	}
}

static void
mkconf_preprocess_makefile_heredoc_features(struct mkconf *mkconf) {
	struct mkconf_feature *feature = mkconf->features;

	while (feature != NULL) {
		char lwr[strlen(feature->name) + 1];

		strntolower(lwr, feature->name, sizeof (lwr));

		fprintf(mkconf->output, "CONFIG_%s=${m_feature_%s}\n", feature->name, lwr);
		feature = feature->next;
	}

	if (mkconf->features != NULL) {
		fputc('\n', mkconf->output);
	}
}

static void
mkconf_preprocess_makefile_heredoc(struct mkconf *mkconf) {
	static const char accept[] = "$\\@";
	char *line = NULL;
	size_t n = 0;
	ssize_t len;

	rewind(mkconf->makefile_template);

	fputs("cat > GNUmakefile <<EOF\n", mkconf->output);

	mkconf_preprocess_makefile_heredoc_features(mkconf);

	while (len = getline(&line, &n, mkconf->makefile_template), len > 0) {
		const char *prev = line, *curr;

		while (curr = strpbrk(prev, accept), curr != NULL) {
			fwrite(prev, 1, curr - prev, mkconf->output);
			if (*curr == '@') {
				const char * const var = curr + 1;
				const char *end = var;

				while (isalnum(*end) || *end == '_') {
					end++;
				}

				if (*end == '@') {
					fprintf(mkconf->output, "${%.*s}", (int)(end - var), var);
					prev = end + 1;
				} else {
					fputc('@', mkconf->output);
					prev = var;
				}
			} else {
				fputc('\\', mkconf->output);
				fputc(*curr, mkconf->output);
				prev = curr + 1;
			}
		}
		fputs(prev, mkconf->output);
	}

	fputs("EOF\n", mkconf->output);

	free(line);
}

static void
mkconf_preprocess(struct mkconf *mkconf) {
	static const struct {
		const char * const name;
		void (* const func)(struct mkconf *);
	} tags[] = {
		{ "usage", mkconf_preprocess_usage },
		{ "defaults", mkconf_preprocess_defaults },
		{ "getopt_long", mkconf_preprocess_getopt_long },
		{ "package_vars", mkconf_preprocess_package_vars },
		{ "features_summary", mkconf_preprocess_features_summary },
		{ "makefile_heredoc", mkconf_preprocess_makefile_heredoc },
	};
	static const char tagprefix[] = "@{mkconf_";
	const int tagend = '}';
	char *line = NULL;
	int lineno = 0;
	size_t n = 0;
	int err = 0;
	ssize_t len;

	while (++lineno, len = getline(&line, &n, mkconf->output_template), len > 0) {

		if (strncmp(line, tagprefix, sizeof (tagprefix) - 1) != 0) {
			fputs(line, mkconf->output);
			continue;
		}

		const char * const name = line + sizeof (tagprefix) - 1;
		const char * const end = strchr(name, tagend);

		if (end == NULL) {
			mkconf_err(mkconf, "output:%d: unterminated tag", lineno);
			continue;
		}

		const size_t namelen = end - name;
		unsigned int index = 0;

		while (index < sizeof (tags) / sizeof (*tags)
			&& (strncmp(tags[index].name, name, namelen) != 0
				|| strlen(tags[index].name) != namelen)) {
			index++;
		}

		if (index == sizeof (tags) / sizeof (*tags)) {
			mkconf_err(mkconf, "output:%d: unknown tag '%.*s'", lineno, (int)namelen, name);
			continue;
		}

		tags[index].func(mkconf);
	}

	free(line);
}

static void
mkconf_close_files(struct mkconf *mkconf) {
	fclose(mkconf->output);
	fclose(mkconf->makefile_template);
	fclose(mkconf->output_template);
}

static void noreturn
mkconf_usage(const char *progname) {

	fprintf(stderr, "usage: %s [-c <bconf>] [-o <output>] [-I <output-template>] [-M <makefile-template>] [<name> [<version>]]\n", progname);

	exit(EXIT_FAILURE);
}

static struct mkconf_args
mkconf_parse_args(int argc, char **argv) {
	struct mkconf_args args = {
		.bconf = CONFIG_DEFAULT_BCONF,
		.output = CONFIG_DEFAULT_OUTPUT,
		.output_template = CONFIG_DEFAULT_OUTPUT_TEMPLATE,
		.makefile_template = CONFIG_DEFAULT_MAKEFILE_TEMPLATE,
	};
	int c;

	while (c = getopt(argc, argv, ":c:o:I:M:"), c != -1) {
		switch (c) {
		case 'c':
			args.bconf = optarg;
			break;
		case 'o':
			args.output = optarg;
			break;
		case 'I':
			args.output_template = optarg;
			break;
		case 'M':
			args.makefile_template = optarg;
			break;
		case ':':
			warnx("Option -%c requires an operand", optopt);
			mkconf_usage(*argv);
		case '?':
			warnx("Unrecognized option -%c", optopt);
			mkconf_usage(*argv);
		}
	}

	if (argc - optind > 2) {
		warnx("Too many arguments");
		mkconf_usage(*argv);
	}

	return args;
}

int
main(int argc, char **argv) {
	const struct mkconf_args args = mkconf_parse_args(argc, argv);
	struct mkconf mkconf;

	switch (argc - optind) {
	case 0:
		mkconf_init(&mkconf);
		break;
	case 1:
		mkconf_init_from_name(&mkconf, argv[optind]);
		break;
	case 2:
		mkconf_init_from_name_and_version(&mkconf, argv[optind], argv[optind + 1]);
		break;
	default:
		abort();
	}

	mkconf_parse_features(&mkconf, args.bconf);

	mkconf_open_files(&mkconf, &args);

	mkconf_preprocess(&mkconf);

	if (mkconf.errors.first != NULL) {
		mkconf_print_errors(&mkconf, args.output);
		unlink(args.output);
	} else {
		if (fchmod(fileno(mkconf.output), 0755) != 0) {
			warn("chmod %s", args.output);
		}
	}

	mkconf_close_files(&mkconf);

	return EXIT_SUCCESS;
}
