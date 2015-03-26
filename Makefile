include CONFIG.mk

deps: jsmn 

jsmn: 
	hg clone $(JSMN_REPO) .$@.hg || \
	( cd .$@.hg; hg update; )
	( cd .$@.hg; \
	hg up $(JSMN_COMMIT); \
	hg archive ../$@; )

clean:
	rm -rf .jsmn.hg




