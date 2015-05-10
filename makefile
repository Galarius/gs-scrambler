OBJS = test.o core.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

core_test: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o core_test

test.o: test.cpp core.h core_memory.h
	$(CC) $(CFLAGS) test.cpp

core.o: core.h core.cpp core.tpp core_types.h core_memory.h
	$(CC) $(CFLAGS) core.cpp

clean:
	\rm *.o *~ core_test