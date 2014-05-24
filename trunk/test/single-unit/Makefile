all:
	make -C ../../server/
	c++  -c -g -ggdb -I /usr/include -I /opt/local/include -I ../../lib -I . -I ../../server -c -o test.o test.cpp
	g++ -o test -g -ggdb test.o ../../server/CMakeFiles/bmdb.dir/Table.cpp.o -L /usr/lib -L /opt/local/lib -L /usr/local/lib -L ../../lib -lBmdb -levent -lpthread
	./test

clean:
	rm -f *.o test

val:
	valgrind -v --track-origins=yes --tool=memcheck --leak-check=full --show-reachable=yes ./test
