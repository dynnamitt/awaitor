CC = c99
CFLAGS = -g -Wall -O3
LDLIBS = -lfcgi

# main binary
awaitor: jsmn/libjsmn.a

jsmn/%.a: jsmn
	cd jsmn && $(MAKE)

jsmn: JSMN_REPO = http://bitbucket.org/zserge/jsmn
jsmn: JSMN_COMMIT = 6979f2e
jsmn:
	hg clone $(JSMN_REPO) .$@.hg || \
	( cd .$@.hg; hg update; )
	( cd .$@.hg; \
	hg up $(JSMN_COMMIT); \
	hg archive ../$@; )

clean:
	rm awaitor
	cd jsmn && $(MAKE) clean
	rm -rf .jsmn.hg
