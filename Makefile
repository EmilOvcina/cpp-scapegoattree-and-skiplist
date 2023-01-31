CXX=g++
SANFLAGS=-fsanitize=address -fsanitize=leak -fsanitize=undefined
CXXFLAGS := -Wall -g -O2 $(SANFLAGS)

SRCDIR=/src/
BUILDDIR=/build/

exercise1: main
	$(CXX) $(SANFLAGS) .$(BUILDDIR)main.o -o a.out

%:
	$(CXX) $(CXXFLAGS) -c -o .$(BUILDDIR)$@.o .$(SRCDIR)$@.cpp

nosan: 
	$(CXX) -Wall -g -O2 .$(SRCDIR)main.cpp -o a.out

.PHONY: clean
clean:
	rm *.out
	rm .$(BUILDDIR)*.o