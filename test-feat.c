#include <err.h>
#include "feat.h"

extern char* features[];
extern unsigned numfeat;

int
main(void)
{
	unsigned i;

	/* Every feature is found by its name */
	for (i = 0; i < numfeat; i++) {
		if (getfeat(features[i]) != i)
			return 1;
	}

	/* An empty name must not be found */
	if (getfeat("") != -1)
		return 1;

	/* A nonexistent name must not be found */
	if (getfeat("ventriloqual") != -1)
		return 1;

	/* A non-unique prefix must fail (cons, cont) */
	if (getfeat("con") != -1)
		return 1;

	return 0;
}
