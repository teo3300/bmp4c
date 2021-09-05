FLAGS = -Wall -Werror -Wpedantic -Wconversion -O2
INCLUDES = include
SOURCES = source
BINARIES = bin

######################################################################

HEADERS = $(shell ls $(INCLUDES)/*.hpp 2> /dev/null)
OBJECTS = $(shell ls $(BINARIES)/*.o 2> /dev/null)

FLAGS += -I$(INCLUDES)

test: test.cpp $(OBJECTS) $(HEADERS)
	g++ $(FLAGS) -o $@ $< $(OBJECTS)

bin/Img.o: source/Img.cpp $(HEADERS)
	g++ $(FLAGS) -c -o bin/Img.o source/Img.cpp

clean:
	-@ rm bin/* 2>/dev/null || true