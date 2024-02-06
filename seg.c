#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <err.h>

#include "feat.h"
#include "seg.h"
#include "utf.h"

void
prseg(struct seg* seg)
{
	if (seg) {
		printf("%s", seg->name);
	}
}

struct seg*
mkseg(char* name, uint32_t feat)
{
	struct seg* seg;
	if ((seg = calloc(1, sizeof(struct seg))) == NULL)
		err(1, NULL);
	seg->name = strdup(name);
	seg->feat = feat;
	return seg;
}

void
addseg(struct seg* seg, struct slist* list)
{
	if (seg == NULL)
		return;
	if (list == NULL)
		return;
	if (list->head == NULL) {
		list->head = seg;
		list->tail = seg;
	} else {
		list->tail->next = seg;
		list->tail = seg;
	}
}

struct seg*
segbyname(struct uchar* u, struct slist* list)
{
        struct seg* seg;
	if (list == NULL)
                return NULL;
	for (seg = list->head; seg; seg = seg->next) {
		if (strncmp(seg->name, u->seq, u->len) == 0)
			return seg;
	}
        return NULL;
}

struct seg*
segbyfeat(uint32_t feat, struct slist* list)
{
        struct seg* seg;
	if (list == NULL)
                return NULL;
	for (seg = list->head; seg; seg = seg->next) {
		if (seg->feat == feat)
			return seg;
	}
        return NULL;
}

struct seg*
parseseg(char* line)
{
	int f = 0;
	char *p = NULL;
	struct seg* seg = NULL;
	if (line == NULL)
		return NULL;
	p = strsep(&line, " \t\n");
	if (p == NULL || *p == '\0') {
		warnx("no segment name given");
		return NULL;
	}
	seg = calloc(1, sizeof(struct seg));
	seg->name = strdup(p);
	while((p = strsep(&line, " \t\n"))) {
		if (*p == '\0' || isspace(*p))
			continue;
		if ((f = getfeat(p)) == -1) {
			warnx("feature '%s' unknown", p);
			return NULL;
		}
		seg->feat |= (1 << f);

	}
	return seg;
}

struct slist*
loadsegs(char* file)
{
	FILE* fp = NULL;
	struct slist* list = NULL;
	struct seg* seg = NULL;
	char *line = NULL;
	size_t cap = 0;
	ssize_t len;

	if ((fp = fopen(file, "r")) == NULL) {
		warnx("Cannot open %s", file);
		return NULL;
	}
	if ((list = calloc(1, sizeof(struct slist))) == NULL)
		err(1, NULL);
	while ((len = getline(&line, &cap, fp)) > 0) {
		line[--len] = '\0';
		if (*line == '\0')
			continue;
		if (*line == '#')
			continue;
		if ((seg = parseseg(line)) == NULL) {
			warnx("Cannot parse %s", line);
			free(list);
			return NULL;
		}
		addseg(seg, list);
	}
	free(line);
	return list;
}

int
segmatch(struct seg* seg, struct spec* spec) {
	uint32_t f, mask;
	if (seg == NULL)
		return 0;
	if (spec == NULL)
		return 0;
	for (f = 0, mask = 1; f < 32; f++, mask <<= 1) {
		if ((spec->mask & mask)
		&& ((spec->feat & mask) != (seg->feat & mask)))
			return 0;
	}
	return 1;
}

uint32_t
apply(struct spec* spec, uint32_t feat)
{
	int f;
	uint32_t out = feat;
	for (f = 0; f < 32; f++) {
		if (spec->mask & (1 << f)) {
			if (spec->feat & (1 << f))
				out |= (1 << f);
			else
				out &= ~(1 << f);
		}
	}
	return out;
}
