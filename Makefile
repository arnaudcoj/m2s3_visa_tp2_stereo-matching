CC=g++
LDFLAGS=-lm -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_imgproc
LIBS=
INCLUDES= -I/usr/include
TARGET= $(wildcard *.cpp)
OBJ= $(TARGET:.cpp=.o)
CFLAGS= -g -std=c++11

all: stereo-matching

execute: stereo-matching
	./stereo-matching left.png right.png left.xml right.xml -d

stereo-matching: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

clean:
	rm *.o stereo-matching
