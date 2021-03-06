CC = c99
CFLAGS = -g -Wall -O3 -pthread --std=c99
CPPFLAGS = -Imongoose -Ijsmn -D_GNU_SOURCE

all: awaitor

awaitor: jsmn/libjsmn.a mongoose/mongoose.o

clean:
	cd jsmn && $(MAKE) clean
	rm -rf awaitor mongoose/mongoose.o .jsmn.hg TAGS

TAGS:
	ctags -eR \
	--exclude=.git \
	--exclude=example* .

# -----------
#  DEPS
# -----------

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

mongoose/%.o: CPPFLAGS += -DMONGOOSE_NO_FILESYSTEM -DMONGOOSE_NO_CGI -DMONGOOSE_NO_THREADS
mongoose/%.o : mongoose/%.c

mongoose/%.c :
	git submodule init
	git submodule update

