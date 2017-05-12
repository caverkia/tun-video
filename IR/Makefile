#CC = g++
TARGET = main
SOURCE = main.cc ir.cc Entry.cc Speed.cc GPS.cc ir.h Entry.h Speed.h GPS.h GPSVec.h GPSVec.cc

#OPT = -O3 -DNDEBUG

#CFLAGS += -Wall -c -I. -I./include -I/usr/include/ $(OPT)

#LDFLAGS += -Wall -lssl -lcrypto -g -ggdb
#LIBOBJECTS = \ 
#HEADERS = *.h \
#TEST = test

#all:$(TEST)

#clean:rm -f $(TEST) *.o

#test: main.o $(LIBOBJECTS) $(CC) main.o $(LIBOJBECTS) $(LDFLAGS) -o $@

%.o: %.cc ${HEADERS} Makefile $(CC) $(CFLAGS) $< -o $@

$(TARGET):$(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE)

clean:
	rm -r main.o
