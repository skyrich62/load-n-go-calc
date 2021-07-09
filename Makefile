CXX = /usr/local/gcc-9.2.0/bin/g++
CXXFLAGS = -g -I ../PEGTL/include -std=c++17
LXXFLAGS = -g

INCS = \
    visitor.h \
    evaluator.h \
    symbol_scope.h \
    grammar.h \
    node.h \
    overloaded.h

OBJS = main.o evaluator.o symbol_scope.o visitor.o

PROGS = calc

all: calc

calc: $(OBJS) $(INCS)
	$(CXX) $(LXXFLAGS) $(OBJS) -o calc

clean:
	rm -rf *.o $(PROGS)
