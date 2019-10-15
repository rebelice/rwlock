TOP := .
CC := gcc
CFLAGS := -lpthread -g
SRC := test/main.c src/rwlock.c
TAR := main
READER := 5
WRITER := 1
LOG := log.txt

all: $(TAR)

$(TAR): $(SRC)
	$(CC) -I$(TOP) $(CFLAGS) $(SRC) -o $@

checker: test/check.c
	$(CC) -I$(TOP) $^ -o $@

check: checker
	./checker

run: $(TAR)
	./$(TAR) $(READER) $(WRITER) > $(LOG)

clean:
	rm $(TAR) checker $(LOG)

help:
	@cat README
