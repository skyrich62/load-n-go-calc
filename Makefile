CXX = /usr/local/gcc-10.1.0/bin/g++
CXXFLAGS = -g -I ../PEGTL/include -std=c++17
LXXFLAGS = -g

all: calc

calc: main.cc
	$(CXX) $(CXXFLAGS) main.cc -o calc

clean:
	rm -rf main
