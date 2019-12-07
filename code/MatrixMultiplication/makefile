OBJS	= matrixMultiplication.o
SOURCE	= matrixMultiplication.cpp
HEADER	= 
OUT	= main
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lomp 
-lomp all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

matrixMultiplication.o: matrixMultiplication.cpp
	$(CC) $(FLAGS) matrixMultiplication.cpp -std=c11

clean:
	rm -f $(OBJS) $(OUT)
