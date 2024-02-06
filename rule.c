#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <err.h>

#include "feat.h"
#include "seg.h"
#include "rule.h"

struct spec*
readspec(char* line)
{
	int feat;
	int bit = 0;
	char *p = line;
	struct spec* spec;
	if (line == NULL)
		return NULL;
	/*printf("reading spec: '%s'\n", line);*/
	spec = calloc(1, sizeof(struct spec));
	while ((p = strsep(&line, " \t"))) switch (*p) {
		case '\0':
			/* skip whitespace */
			break;
		case '+':
		case '-':
			bit = *p++ == '+' ? 1 : 0;
			if ((feat = getfeat(p)) == -1) {
				warnx("feature '%s' unknown", p);
				free(spec);
				return NULL;
			}
			spec->mask |= (1 << feat);
			spec->feat |= bit ? (1 << feat) : 0;
			/*printf("got %d %s\n", bit, p);*/
			/*prspec(spec);*/
			break;
		default:
			free(spec);
			warnx("'%c' unexpected in spec", *p);
			return NULL;
			break;
	}
	return spec;
}

struct rule*
parserule(char* line)
{
	char *p = NULL, *lc;
	struct rule* rule = NULL;
	if (line == NULL)
		return NULL;
	rule = calloc(1, sizeof(struct rule));
	p = strsep(&line, "[");
	p = strsep(&line, "]");
	rule->old = readspec(p);
	if ((p = strsep(&line, ">")) == NULL) {
	/* FIXME do we want that in the syntax? */
		free(rule);
		warnx("No '>' found in rule");
		return NULL;
	}
	p = strsep(&line, "[");
	p = strsep(&line, "]");
	rule->new = readspec(p);
	if ((p = strsep(&line, "/")) == NULL) {
	/* FIXME do we want that in the syntax? */
		free(rule);
		warnx("No '/' found as separator");
		return NULL;
	}
	if ((lc = strsep(&line, "_")) == NULL) {
		free(rule);
		warnx("No '_' found for context");
		return NULL;
	}
	if ((p = strsep(&lc, "["))) {
		p = strsep(&lc, "]");
		rule->lc = readspec(p);
	}
	if ((p = strsep(&line, "["))) {
		p = strsep(&line, "]");
		rule->rc = readspec(p);
	}
	/* FIXME check trailing garbage? */
	return rule;
}

void
addrule(struct rule* rule, struct rlist* list)
{
	if (rule == NULL)
		return;
	if (list == NULL)
		return;
	if (list->head == NULL) {
		list->head = rule;
		list->tail = rule;
	} else {
		list->tail->next = rule;
		list->tail = rule;
	}
}

struct rlist*
loadrules(char* file)
{
	FILE* fp = NULL;
	struct rlist* list = NULL;
	struct rule* rule = NULL;
	char *line = NULL;
	size_t cap = 0;
	ssize_t len;

	if ((fp = fopen(file, "r")) == NULL) {
		warnx("Error opening %s", file);
		return NULL;
	}
	if ((list = calloc(1, sizeof(struct rlist))) == NULL)
		err(1, NULL);
	while ((len = getline(&line, &cap, fp)) > 0) {
		line[--len] = '\0';
		if (*line == '\0')
			continue;
		if (*line == '#')
			continue;
		if (isspace(*line))
			continue;
		if ((rule = parserule(line)) == NULL) {
			warnx("Error parsing '%s'", line);
			free(list);
			return NULL;
		}
		addrule(rule, list);
	}
	free(line);
	return list;
}
