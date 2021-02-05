# Makefile for the socket programming example
#

hiracs_objects = HiRACS.o Focuser.o signalHandler.o

all : HiRACS

# Focuser: $(focuser_objects)
# 	g++ -o Focuser $(focuser_objects)

HiRACS: $(hiracs_objects)
	g++ -o HiRACS $(hiracs_objects)

Focuser: Focuser.cpp Focuser.h
signalHandler: signalHandler.cpp signalHandler.h
HiRACS: HiRACS.cpp HiRACS.h Focuser.h signalHandler.h CameraException.h

clean:
	rm -f *.o Focuser HiRACS
