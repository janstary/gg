#ifndef _RULE_H_
#define _RULE_H_

#include "seg.h"

struct rule {
	struct spec* lc;
	struct spec* rc;
	struct spec* old;
	struct spec* new;
	struct rule* next;
};

struct rlist {
	struct rule* head;
	struct rule* tail;
};

struct rlist* loadrules(char*);

#endif
