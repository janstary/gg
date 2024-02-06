#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <err.h>

#include "utf.h"
#include "seg.h"
#include "rule.h"

#define GGDIR ".gg"

void
addwhite(struct slist* list)
/* FIXME https://en.wikipedia.org/wiki/Whitespace_character#Unicode */
{
	if (list == NULL)
		return;
	addseg(mkseg(" ", 0), list);
	addseg(mkseg("\t", 0), list);
	addseg(mkseg("\n", 0), list);
}

int
process(struct seg** buf, struct slist* segs, struct rlist* ruls)
{
	uint32_t feat;
	struct seg* seg;
	struct rule* rule = NULL;
	static char* ws = NULL;
	if (segs == NULL)
		return -1;
	if (ruls == NULL)
		return -1;
	if (buf == NULL)
		return -1;
	if ((seg = buf[2]) && seg->feat == 0) {
		/* These are the pseudo-segments representing whitespace.
		 * We nee to recognize a word boundary, a morpheme boundary,
		 * a word-initial segment, a syllable nucleaus, etc (probably
		 * by more pseudo-features added to the canonocal segment
		 * upon reading it as input). For now, just print out
		 * the whitespace and pretend it wasn't there. FIXME */
		/* But only print it _after_ processing the context */
		ws = strdup(seg->name);
		buf[2] = NULL;
		return 0;
	}
	if ((seg = buf[1]) == NULL) {
		buf[1] = buf[2];
		buf[2] = NULL;
		return 0;
	}
	/* Does any of the rules apply? */
	for (feat = 0, rule = ruls->head; rule; rule = rule->next) {
		if (segmatch(seg, rule->old)
		&& (!rule->lc || (buf[0] && segmatch(buf[0], rule->lc)))
		&& (!rule->rc || (buf[2] && segmatch(buf[2], rule->rc)))) {
			feat = apply(rule->new, seg->feat);
			/* FIXME: quit after first hit? */
			break;
		}
	}
	if (feat) {
		if ((seg = segbyfeat(feat, segs)) == NULL) {
			warnx("Changed to an unknown segment of %#010x", feat);
			return -1;
		}
	}
	buf[0] = buf[1];
	buf[1] = buf[2];
	buf[2] = NULL;
	prseg(seg);
	if (ws) {
		printf("%s", ws);
		free(ws);
		ws = 0;
	}
	return 0;
}

int
main(int argc, char** argv)
{
	int c;
	struct uchar u;
	struct seg* seg;
	struct stat s, r;
	char segname[1024];
	char rulname[1024];
	char dirname[1024];
	struct seg* buf[3] = { NULL, NULL, NULL };
	struct slist* segs = NULL;
	struct rlist* ruls = NULL;

	while ((c = getopt(argc, argv, "ds")) != -1) switch (c) {
		default:
			break;
	}
	argc -= optind;
	argv += optind;

	if (argc != 1)
		errx(1, "usage: gg [-d] language");

	snprintf(segname, 1024, "%s.seg", *argv);
	snprintf(rulname, 1024, "%s.rul", *argv);
	if (stat(segname, &s) == -1 || stat(rulname, &r) == -1) {
		snprintf(dirname, 1024, "%s/%s", getenv("HOME"), GGDIR);
		snprintf(segname, 1024, "%s/%s.seg", dirname, *argv);
		snprintf(rulname, 1024, "%s/%s.rul", dirname, *argv);
	}
	if (stat(segname, &s) == -1 || stat(rulname, &r) == -1) {
		errx(1, "Cannot find segments and rules for %s", *argv);
	}
	if ((segs = loadsegs(segname)) == NULL)
		errx(1, "Error loading segments from %s", segname);
	if ((ruls = loadrules(rulname)) == NULL)
		errx(1, "Error loading rules from %s", rulname);

	/* Add the pseudo-segments
	 * representing whitespace */
	addwhite(segs);

	while (uhead(&u) != EOF) {
		if (u.len)
			uread(&u);
		if ((seg = segbyname(&u, segs)) == NULL)
			warnx("unknown segment '%.*s'", u.len, u.seq);
		buf[2] = seg;
		process(buf, segs, ruls);
	}
	/* flush the buffer */
	process(buf, segs, ruls);

	return ferror(stdin) ? 1 : 0;
}
