SRC = eggtimer.c
OBJ = ${SRC: .c=.o}
LIBS = -lX11
CC = cc
CFLAGS = -std=c99
LDFLAGS = -s ${LIBS}

all: options eggtimer

options:
	@echo eggtimer build options:
	@echo "CFLAGS	= ${CFLAGS}"
	@echo "LDFLAGS	= ${LDFLAGS}"
	@echo "CC	= ${CC}"

.c.o:
	@echo CC -c $<
	@${CC} -c $< ${CFLAGS}

eggtimer: eggtimer.o
		@echo CC -o $@
		@${CC} -o $@ eggtimer.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f eggtimer ${OBJ}

