CC = c99
CFLAGS = -g -Wall -O3 
CPPFLAGS = -Imongoose -Ijsmn
LDLIBS = -lpthread

# main binary
awaitor: jsmn/libjsmn.a mongoose/mongoose.o

clean:
	cd jsmn && $(MAKE) clean
	rm -rf awaitor mongoose/mongoose.o .jsmn.hg

# -----------
#  DEPS
# -----------

jsmn/%.a: jsmn
	cd jsmn && $(MAKE)

mongoose/%.o: CPPFLAGS += -DMONGOOSE_NO_THREADS -DMONGOOSE_NO_FILESYSTEM -DMONGOOSE_NO_CGI

jsmn: JSMN_REPO = http://bitbucket.org/zserge/jsmn
jsmn: JSMN_COMMIT = 6979f2e
jsmn:
	hg clone $(JSMN_REPO) .$@.hg || \
	( cd .$@.hg; hg update; )
	( cd .$@.hg; \
	hg up $(JSMN_COMMIT); \
	hg archive ../$@; )

mongoose/%.c :
	git submodule update

