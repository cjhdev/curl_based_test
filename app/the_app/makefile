DIR_ROOT := ../..

LDFLAGS += $(shell curl-config --libs)

CFLAGS += -std=c99 -pedantic -Wall -g 
CFLAGS += -I$(DIR_ROOT)/include
CFLAGS += -DVERSION=\"$(shell cat $(DIR_ROOT)/VERSION)\"

SRC := $(notdir $(wildcard $(DIR_ROOT)/src/*.c)) the_app.c
OBJ := $(SRC:.c=.o)

VPATH += $(DIR_ROOT)/src
VPATH += .

CC := gcc

$(info $(OBJ))

bin/the_app: $(addprefix build/,$(OBJ))
	@ echo linking $@
	@ $(CC) $^ -o $@ $(LDFLAGS)

build/%.o: %.c
	@ echo building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*

.PHONY: clean
