#ifndef _UTF_H_
#define _UTF_H_

#define ISHEAD(c) (			\
	((c & 0x80) == 0x00)	||	\
	((c & 0xe0) == 0xc0)	||	\
	((c & 0xf0) == 0xe0)	||	\
	((c & 0xf8) == 0xf0)		)

struct uchar {
	int	len;
	char	seq[4];
};

int uhead(struct uchar*);
int uread(struct uchar*);

#endif
