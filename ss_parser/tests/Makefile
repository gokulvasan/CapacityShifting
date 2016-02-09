CC=gcc

INCLUDE=-I../include -I../conf
TESTED_OBJECTS=tested/ss_parser.o tested/ss_inject.o tested/ss_tsaheylu.o
OBJECTS=./bad_header.o test_fields.o test_inject.o test_tsaheylu.o

CFLAGS=-Wall -Wextra -Wno-unused-parameter -O3 $(INCLUDE) -DSYS_HOST
#ifdef CTEST_COLOR_OK
CFLAGS+=-DCOLOR_OK
#endif


all: tests
	./tests

tests: tests.c common.h ctest.h $(TESTED_OBJECTS) $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(INCLUDE)

tested:
	rm -rf tested
	mkdir tested

tested/%.o: ../src/%.c common.h tested
	$(CC) $(CFLAGS) $< -c -o $@

%.o: %.c %.h common.h
	$(CC) $(CFLAGS) $< -c -o $@


.PHONY: clean
clean:
	rm -rf tests *.o tested

