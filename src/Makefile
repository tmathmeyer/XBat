# See LICENSE file for copyright and license details.

include config.mk

all: xbat

xbat: xbat.o lwbi.o

.c.o: config.mk
	@echo CC -c $<
	@${CC} -c $< ${CFLAGS}

xbat xbat_path:
	@echo CC -o $@
	@${CC} -o $@ $+ ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f xbat *.o

install: xbat
	cp xbat /usr/bin/xbat

uninstall:
	rm -f /usr/bin/xbat

update: clean xbat
