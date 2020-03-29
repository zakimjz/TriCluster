CC = g++ 
CFLAGS = -O3 -Wno-deprecated  
HDRS = ./timeutil.h 

OBJS = timeutil.o triCluster.o

TARGET = triCluster 

default: $(TARGET)

clean:
	rm -rf *~ *.o $(TARGET)

triCluster: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -o triCluster $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -o $*.o -c $<
