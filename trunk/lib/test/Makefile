.SUFFIXES : .cpp .o

INC_DIR = -I /usr/include -I /opt/local/include -I ..
LIB_DIR = -L /usr/lib -L /opt/local/lib -L ..
LIB = -lComm -lgtest

CPP=g++
CPPFLAGS = -c -g $(INC_DIR)

OBJS = test.o
SRCS = $(OBJS:.o=.cpp)

all: test

test: $(OBJS)
	$(CPP) -o test $(OBJS) $(LIB_DIR) $(LIB)
	./test

clean:
	rm -f *.o test

tag:
	ctags *
