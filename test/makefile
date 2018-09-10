DIR_ROOT := ..
DIR_CMOCKA := $(DIR_ROOT)/vendor/cmocka
DIR_BUILD := build
DIR_BIN := bin

CC := gcc

VPATH += $(DIR_ROOT)/src
VPATH += $(DIR_CMOCKA)/src

INCLUDES += -I$(DIR_ROOT)/include
INCLUDES += -I$(DIR_CMOCKA)/include
INCLUDES += -I.

CMOCKA_DEFINES += -DHAVE_STRINGS_H
CMOCKA_DEFINES += -DHAVE_SIGNAL_H
CMOCKA_DEFINES += -DHAVE_INTTYPES_H
CMOCKA_DEFINES += -DHAVE_MALLOC_H

CFLAGS := -O0 -Wall -g -fprofile-arcs -ftest-coverage $(INCLUDES) $(CMOCKA_DEFINES)
LDFLAGS := -fprofile-arcs -g

SRC := $(notdir $(wildcard $(DIR_ROOT)/src/*.c))
SRC_CMOCKA := $(notdir $(wildcard $(DIR_CMOCKA)/src/*.c))

OBJ := $(SRC:.c=.o)
OBJ_CMOCKA := $(SRC_CMOCKA:.c=.o)

TESTS := $(basename $(wildcard tc_*.c))

LINE := ================================================================

.PHONY: clean all coverage line

all: $(addprefix $(DIR_BIN)/, $(TESTS))

line:
	@ echo $(LINE)

build_and_run: 
	@ FAIL=0; \
	for tc in $(addprefix $(DIR_BIN)/, $(TESTS)); do \
		echo $(LINE); \
		echo ""; \
		echo "$$tc:"; \
		echo ""; \
		make clean $$tc \
		&& echo "" \
		&& if ! ./$$tc; then FAIL=1; fi; \
		echo ""; \
	done; \
	exit $$FAIL

$(DIR_BUILD)/%.o: %.c
	@ echo building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(DIR_BUILD)/*

sqeaky_clean: clean
	rm -f $(DIR_BIN)/*

$(DIR_BIN)/tc_http_test: $(addprefix $(DIR_BUILD)/, tc_http_test.o mock_curl.o $(OBJ_CMOCKA) $(OBJ))
	@ echo linking $@
	$(CC) $(LDFLAGS) $^ -o $@










