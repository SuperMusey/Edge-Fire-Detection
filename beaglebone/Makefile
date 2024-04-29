CC = g++
CFLAGS = -O2
LIBS = `pkg-config --cflags --libs opencv`
KERNEL_SRC = lightcontrol.c
KERNEL_TARGET = lightcontrol.ko
SRC = capture_image.cpp
TARGET = capture_image

all: $(TARGET) $(KERNEL_TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LIBS) $(SRC) -o $(TARGET)

$(KERNEL_TARGET): $(KERNEL_SRC)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -f $(TARGET) $(KERNEL_TARGET)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


