CXX = /usr/local/gcc-10.1.0/bin/g++
CXXFLAGS = -g -I ../PEGTL/include -std=c++17
LXXFLAGS = -g

INCS = \
    evaluator.h \
    grammar.h \
    node.h \
    overloaded.h

OBJS = main.o evaluator.o

PROGS = calc

all: calc

calc: $(OBJS) $(INCS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o calc

clean:
	rm -rf *.o $(PROGS)
