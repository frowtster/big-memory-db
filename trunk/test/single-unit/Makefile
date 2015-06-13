all:
	make -C ../../lib/
	make -C ../../server_db/
	c++  -c -g -ggdb -I /usr/include -I /opt/local/include -I ../../lib -I . -I ../../server_db -c -o test-singlecol.o test-singlecol.cpp
	g++ -o test-singlecol -g -ggdb test-singlecol.o ../../server_db/CMakeFiles/bmdb.dir/Table.cpp.o -L /usr/lib -L /opt/local/lib -L /usr/local/lib -L ../../lib -lBmdb -levent -lpthread
	./test-singlecol
	c++  -c -g -ggdb -I /usr/include -I /opt/local/include -I ../../lib -I . -I ../../server_db -c -o test-multicol.o test-multicol.cpp
	g++ -o test-multicol -g -ggdb test-multicol.o ../../server_db/CMakeFiles/bmdb.dir/Table.cpp.o -L /usr/lib -L /opt/local/lib -L /usr/local/lib -L ../../lib -lBmdb -levent -lpthread
	./test-multicol

clean:
	rm -f *.o test-simple test-multicol

val:
	valgrind -v --track-origins=yes --tool=memcheck --leak-check=full --show-reachable=yes ./test

tag:
	ctags -R . ../../lib ../../server_db/
