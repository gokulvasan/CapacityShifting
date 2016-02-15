SRC=./src
INC=./include/
CONF=./conf/
CC=gcc
CFLAGS= -iquote$(INC) -iquote$(CONF) -Wall -DDEBUG 
# -DSYS_HOST
LDFLAGS=
#SOURCES= fsm_parser.c main.c
#OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ss_parser
TESTS=tests
PLAT=litmus
#OBJ=$(join $(addsuffix ../obj/, $(dir $(SOURCE))), $(notdir $(SOURCE:.cpp=.o))) 
vpath %.c ./src/:./src/arch
#OBJECTS= $(pathsubst %.c,%.o,)
OBJECTS = $(patsubst (notdir %.c),%.o,$(wildcard src/*.c src/platform/$(PLAT)/*.c))

all:  $(EXECUTABLE) 
# $(TESTS)

#$(TESTS): $(OBJECTS)
#	$(MAKE) -C ./tests/

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(CFLAGS) $(OBJECTS) -o bin/$@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf ./*.o
	rm -rf *o bin/$(EXECUTABLE)
	$(MAKE) clean -C ./tests/
