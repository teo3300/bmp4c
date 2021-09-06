FLAGS = -Wall -Werror -Wpedantic -Wconversion -O2
INCLUDES = include
SOURCES = source
BINARIES = bin

######################################################################

HEADERS = $(shell ls $(INCLUDES)/*.hpp 2> /dev/null)
OBJECTS = $(shell ls $(BINARIES)/*.o 2> /dev/null)

FLAGS += -I$(INCLUDES)

test: test.cpp bin/Img.o $(HEADERS)
	g++ $(FLAGS) -o $@ $< bin/Img.o

main: main.cpp bin/helper.o $(HEADERS)
	g++ $(FLAGS) -o $@ $< bin/helper.o

bin/Img.o: source/Img.cpp $(HEADERS)
	g++ $(FLAGS) -c -o bin/Img.o source/Img.cpp

bin/helper.o: source/helper.cpp $(HEADERS)
	g++ $(FLAGS) -c -o bin/helper.o source/helper.cpp

clean:
	-@ rm test main bin/* 2>/dev/null || true