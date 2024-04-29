CC = g++
CFLAGS = -O2
LIBS = `pkg-config --cflags --libs opencv`

SRC = capture_image.cpp
TARGET = capture_image

all: $(TARGET)

$(TARGET): $(SRC)
	    $(CC) $(CFLAGS) $(LIBS) $(SRC) -o $(TARGET)

clean:
	    rm -f $(TARGET)

