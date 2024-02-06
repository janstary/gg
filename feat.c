#include <string.h>
#include "feat.h"

char* features[] = {
	/* major classes */
	"syllabic"	,
	"consonantal"	,
	"approximant"	,
	"sonorant"	,
	/* laryngeal */
	"voice"		,
	"sg"	,
	"cg"	,
	/* manner */
	"continuant"	,
	"nasal"		,
	"strident"	,
	"lateral"	,
	"delayed"	,
	"vibrant"	,
	/* place */
	/* 	labial */
	"round"	,	
	/*	coronal */
	"coronal"	,	
	"anterior"	,	
	"distributed"	,	
	/*	dorsal */
	/*     +vowels */
	"high"		,
	"low"		,
	"back"		,
	"tense"		,
	/*	radical */
	"atr"	,
	"rtr"	,
	/*	glottal */
	"glottal"	,
	/* length */
	"long"
};

unsigned numfeat = sizeof(features) / sizeof(char*);

int
getfeat(char* name)
{
	unsigned i, f;
	unsigned len;
	if (name == NULL)
		return -1;
	len = strlen(name);
	for (f = -1, i = 0; i < numfeat; i++) {
		if (strncmp(features[i], name, len) == 0) {
			f = i;
			break;
		}
	}
	if (i == numfeat)
		return -1;
	if (strlen(features[i]) == len)
		return f;
	/* If this was shorter than the full name,
	 * make sure the short prefix is unique. */
	for (i++; i < numfeat; i++) {
		if (strncmp(features[i], name, len) == 0) {
			return -1;
		}
	}
	return f;
}
