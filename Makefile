CC = c99
CFLAGS = -g -Wall -O3
CPPFLAGS = -Imongoose -Ijsmn
LDLIBS = -lpthread

# main binary
awaitor: jsmn/libjsmn.a mongoose.o

jsmn/%.a: jsmn
	cd jsmn && $(MAKE)

mongoose.o: mongoose/mongoose.c
	cd $(dir $<); \
	$(CC)

mongoose/%.c :
	git submodule update

jsmn: JSMN_REPO = http://bitbucket.org/zserge/jsmn
jsmn: JSMN_COMMIT = 6979f2e
jsmn:
	hg clone $(JSMN_REPO) .$@.hg || \
	( cd .$@.hg; hg update; )
	( cd .$@.hg; \
	hg up $(JSMN_COMMIT); \
	hg archive ../$@; )

clean:
	cd jsmn && $(MAKE) clean
	rm -rf awaitor mongoose.o .jsmn.hg
