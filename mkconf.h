/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef MKCONF_H
#define MKCONF_H

#include <stdio.h> /* FILE */

struct mkconf_error {
	char *message;
	struct mkconf_error *next;
};

struct mkconf_feature {
	char *name, *description, *defaults;
	struct mkconf_feature *next;
};

struct mkconf {
	struct {
		const char *name, *version;
	} package;
	struct {
		struct mkconf_error *first, *last;
	} errors;
	struct mkconf_feature *features;
	FILE *output, *output_template, *makefile_template;
};

void __attribute__((format(printf, 2, 3)))
mkconf_err(struct mkconf *mkconf, const char *format, ...);

/* MKCONF_H */
#endif
