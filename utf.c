#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include "utf.h"

/* Read a next leading byte of a sequence,
 * fill in the uchar structure accordingly.
 * Return the length of the started sequence,
 * as indicated by the leading byte;
 * return 0 for a non-leading byte;
 * return EOF on error. */
int
uhead(struct uchar *u)
{
	int c;
	if ((c = getchar()) == EOF)
		return EOF;
	if ((c & 0x80) == 0x00) {
		u->len = 1; /* 0xxxxxxx */
	} else if ((c & 0xe0) == 0xc0) {
		u->len = 2; /* 110xxxxx */
	} else if ((c & 0xf0) == 0xe0) {
		u->len = 3; /* 1110xxxx */
	} else if ((c & 0xf8) == 0xf0) {
		u->len = 4; /* 11110xxx */
	} else {
		warnx("%#0x is not a leading byte\n", c);
		u->len = 0;
	}
	u->seq[0] = c;
	return u->len;
}

/* Finish reading the byte sequence started in u with a leading byte.
 * Return 0 if the whole sequence is valid. If the sequence ends prematurely,
 * i.e. a new leading byte is read before the sequence's length is exhausted,
 * push the byte back and return -1. If an invalid continuation byte is read
 * (but not a new lead) return -1. Also return -1 on read error. */
int
uread(struct uchar* u)
{
	int c, i;
	for (i = 1; i < u->len; i++) {
		if ((c = getchar()) == EOF) {
			warnx("error reading continuation byte");
			return -1;
		}
		if ((c & 0xc0) != 0x80) { /* 10xxxxxx */
			warnx("%.2x is not a continuation byte\n", c);
			if (ISHEAD(c))
				ungetc(c, stdin);
			return -1;
		}
		u->seq[i] = c & 0xff;
	}
	/*printf("%.*s\n", u->len, u->seq);*/
	return 0;
}
