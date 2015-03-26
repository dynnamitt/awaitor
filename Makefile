CC = c99
CFLAGS = -g -Wall -O3
LDLIBS = -lfcgi

# main binary
awaitor: jsmn/libjsmn.a

jsmn/%.a: jsmn/Makefile
	cd jsmn && $(MAKE)

jsmn/Makefile: JSMN_REPO = http://bitbucket.org/zserge/jsmn
jsmn/Makefile: JSMN_COMMIT = 6979f2e
jsmn/Makefile:
	hg clone $(JSMN_REPO) .$@.hg || \
	( cd .$@.hg; hg update; )
	( cd .$@.hg; \
	hg up $(JSMN_COMMIT); \
	hg archive ../$@; )

clean:
	rm awaitor
	cd jsmn && $(MAKE) clean
	rm -rf .jsmn.hg




