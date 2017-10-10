all: dtree.exe

clean:
    rm dtree.o dtree.exe

dtree.exe: dtree.o
    g++ -g -o dtree dtree.o

dtree.o:
    g++ -c -g dtree.cpp