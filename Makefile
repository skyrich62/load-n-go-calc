CXX = /usr/local/gcc-9.2.0/bin/g++
CXXFLAGS = -g -I ../PEGTL/include -std=c++17
LXXFLAGS = -g

INCS = \
    dotter.h \
    visitor.h \
    traversal.h \
    semantic_analysis.h \
    evaluator.h \
    symbol_scope.h \
    grammar.h \
    node.h \
    overloaded.h

OBJS = \
    main.o \
    evaluator.o \
    dotter.o \
    symbol_scope.o \
    traversal.o \
    visitor.o \
    semantic_analysis.o

PROGS = calc

all: calc

calc: $(OBJS) $(INCS)
	$(CXX) $(LXXFLAGS) $(OBJS) -o calc

clean:
	rm -rf *.o $(PROGS)
