#ifndef _SEG_H_
#define _SEG_H_

#include <stdint.h>
#include "utf.h"

struct seg {
	char *		name;
	uint32_t	feat;
	struct seg*	next;
};

struct spec {
	uint32_t feat;	/* a bitmask of +features, ie their indices */
	uint32_t mask;	/* a bitmask of which features are specified */
};

struct slist {
	struct seg* head;
	struct seg* tail;
};

void prseg(struct seg*);
void prspec(struct spec*);
struct seg* segread(char*);
struct seg* mkseg(char*, uint32_t);
struct seg* segbyname(struct uchar*, struct slist*);
struct seg* segbyfeat(uint32_t, struct slist*);
int segmatch(struct seg*, struct spec*);
uint32_t apply(struct spec*, uint32_t);

struct slist* loadsegs(char*);
void addseg(struct seg*, struct slist*);

#endif
