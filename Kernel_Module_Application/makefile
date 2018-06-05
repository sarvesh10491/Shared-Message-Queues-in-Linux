# the compiler: gcc for C program
TOOLDIR = /opt/iot-devkit/1.7.3/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux
ifeq ($(TEST_TARGET), Galileo2)
	CC=$(TOOLDIR)/i586-poky-linux-gcc
else
	CC=gcc
endif

LIBS = -pthread -lm -lrt
CFLAGS  = -Wall
DEPS = queue_ops.h
TARGET = msg_queue


all: $(TARGET)

$(TARGET): $(TARGET).c $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LIBS)

clean:
	$(RM) $(TARGET)