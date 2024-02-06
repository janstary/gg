BINS	= gg
MANS	= gg.1
OBJS	= utf.o feat.o seg.o rule.o
TEST	= test-feat test-seg test-rule

CFLAGS	= -Wall -pedantic

PREFIX	= ${HOME}
LANDIR	= ${HOME}/.gg
BINDIR	= ${PREFIX}/bin/
MANDIR	= ${PREFIX}/man/man1/

all: ${BINS}

gg: ${OBJS} gg.c
	${CC} ${CFLAGS} -o gg ${OBJS} gg.c

test-feat: test-feat.c ${OBJS}
	${CC} ${CFLAGS} -o test-feat test-feat.c ${OBJS}

test-seg: test-seg.c ${OBJS}
	${CC} ${CFLAGS} -o test-seg test-seg.c ${OBJS}

test-rule: test-rule.c ${OBJS}
	${CC} ${CFLAGS} -o test-rule test-seg.c ${OBJS}

test: $(TEST) ${BINS}
	./test-feat
	./test-seg czech
	#./test-rule
	#./gg czech < czech.in | diff -u - czech.out
	./gg czech < czech.in

feat.o:	feat.c feat.h
rule.o:	rule.c rule.h
seg.o:	seg.c seg.h
utf.o:	utf.c utf.h

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<

lint: ${MANS}
	mandoc -Tlint ${MANS}

install: ${BINS} ${MANS}
	install -d -m 755 ${BINDIR} && install -m 755 ${BINS} ${BINDIR}
	install -d -m 755 ${MANDIR} && install -m 644 ${MANS} ${MANDIR}
	install -d -m 755 ${SEGDIR} && install -m 644 -b *.{seg,rul} ${LANDIR}

uninstall:
	( cd ${BINDIR} && rm -f ${BINS} )
	( cd ${MANDIR} && rm -f ${MANS} )

clean:
	rm -f ${BINS} ${TEST} ${OBJS} *~

