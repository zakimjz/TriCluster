CC = g++ 
CFLAGS = -O3 -Wno-deprecated  
HDRS = ./timeutil.h 

OBJS = timeutil.o adc.o

LIBS =
TARGET = adc 

default: $(TARGET)

clean:
	rm -rf *~ *.o $(TARGET) agrf flip

adc: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -o adc $(OBJS) $(LIBS)

agrf:
	$(CC) $(CFLAGS) -o agrf agrf.cpp $(LIBS)

flip:
	$(CC) $(CFLAGS) -o flip flip.cpp $(LIBS)


.SUFFIXES: .o .cpp .cc

.cpp.o:
	$(CC) $(CFLAGS) -o $*.o -c $<

.cc.o:
	$(CC) $(CFLAGS) -o $*.o -c $<
