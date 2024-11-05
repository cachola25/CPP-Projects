# note: stackTest10 uses a stack-block-size of 10

CXX = g++
CXXFLAGS = -g -std=c++11
TARGETS = stackTest stackTest10

all: $(TARGETS)

stackTest: stack.o main.o
	$(CXX) $(CXXFLAGS) -o stackTest stack.o main.o

stackTest10: stack10.o main.o
	$(CXX) $(CXXFLAGS) -o stackTest10 stack10.o main.o

stack.o: stack.cpp stack.h
	$(CXX) $(CXXFLAGS) -c stack.cpp

stack10.o: stack.cpp stack.h
	$(CXX) $(CXXFLAGS) -DSTACK_BLOCK_SIZE=10 -c stack.cpp -o stack10.o

main.o: main.cpp stack.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f *.o $(TARGETS)


