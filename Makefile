CXXFLAGS=-O2 -Wall -W -g

all: vector.o map.o vector_v2.o vector_v3.o avl_test.o avl.o
	$(CXX) $(CXXFLAGS) -o vector vector.o
	$(CXX) $(CXXFLAGS) -o vector_v2 vector_v2.o
	$(CXX) $(CXXFLAGS) -o vector_v3 vector_v3.o
	$(CXX) $(CXXFLAGS) -o map map.o
	$(CXX) $(CXXFLAGS) -o avl_test avl_test.o avl.o

clean:
	rm -f *.o vector vector_v2 vector_v3 map

vector.o: util.hpp
vector_v2.o: util.hpp
vector_v3.o: util.hpp
map.o: util.hpp
