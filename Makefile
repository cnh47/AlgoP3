.PHONY: all
all : Seam


cipher : a.o
	g++ Seam.o -o Seam

cipher.o : a.cpp
	g++ -std=c++17 -c Seam.cpp

.PHONY: clean
clean :
	@rm -f *.o Seam
