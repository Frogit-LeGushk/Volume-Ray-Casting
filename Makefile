CC=g++


target: build run

build: main.cpp shader.cpp glad.c
	$(CC) -c main.cpp -o main.o
	$(CC) -c shader.cpp -o shader.o
	$(CC) -c glad.c -o glad.o
	$(CC) main.o shader.o glad.o -lglfw -lGL -ldl -o exe

run: exe
	./exe

clear:
	-rm *.o 2>/dev/null
	-rm exe 2>/dev/null
	-rm *.so 2>/dev/null





