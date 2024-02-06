#include <string.h>
#include <stdio.h>
#include <err.h>
#include "seg.h"

int
main(int argc, char** argv)
{
	char segname[1024];
	struct slist* list;
	struct seg* seg;
	struct seg* dif;
	struct spec this;
	struct spec that;

	if (argc != 2)
		errx(1, "usage: %s language", *argv);

	snprintf(segname, 1024, "./%s.seg", argv[1]);
	if ((list = loadsegs(segname)) == NULL)
		errx(1, "cannot read %s", segname);

	/* Every segment matches its own features,
	 * and is the only one with such name and such features. */
	for (seg = list->head; seg; seg = seg->next) {
		this.mask = ~0;
		this.feat = seg->feat;
		if (!segmatch(seg, &this))
			errx(1, "%s does not match ot festures", seg->name);
		for (dif = seg->next; dif; dif = dif->next) {
			that.mask = ~0;
			that.feat = dif->feat;
			if (strcmp(seg->name, dif->name) == 0)
				errx(1, "%s appears twice", seg->name);
			if (segmatch(seg, &that))
				errx(1, "%s matches %s", seg->name, dif->name);
		}
	}

	return 0;
}
