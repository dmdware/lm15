CC=LD_LIBRARY_PATH=/usr/local/lib g++ -I/usr/include/ffmpeg
CFLAGS=-c -Wall -std=c++0x
LDFLAGS=-L/usr/lib64 -lSDL2 -lavcodec -lavutil -lavformat -lavfilter -lswscale -lpng -ljpeg
SOURCES=localmap.cpp\
platform.cpp\
utils.cpp\
procvid.cpp\
compframes.cpp\
estview.cpp\
texture.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=csmm

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o csmm
	rm -rf $(OBJECTS) 
