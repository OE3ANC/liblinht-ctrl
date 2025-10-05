# liblinht-ctrl Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC -Iinclude
LDFLAGS = -lgpiod

# Library files
LIB_NAME = liblinht-ctrl
STATIC_LIB = $(LIB_NAME).a
SHARED_LIB = $(LIB_NAME).so

# Source files
SOURCES = src/gpio.c src/pwm.c src/backlight.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = include/liblinht-ctrl.h

# Default target
all: $(STATIC_LIB) $(SHARED_LIB)

# Static library
$(STATIC_LIB): $(OBJECTS)
	ar rcs $@ $^

# Shared library
$(SHARED_LIB): $(OBJECTS)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# Object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(STATIC_LIB) $(SHARED_LIB) example

# Install headers and libraries (optional)
install: $(STATIC_LIB) $(SHARED_LIB)
	sudo cp $(HEADERS) /usr/include/
	sudo cp $(STATIC_LIB) $(SHARED_LIB) /usr/lib/
	sudo ldconfig

# Uninstall
uninstall:
	sudo rm -f /usr/include/liblinht-ctrl.h
	sudo rm -f /usr/lib/$(STATIC_LIB) /usr/lib/$(SHARED_LIB)
	sudo ldconfig

# Example program
example: example.c $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $< -L. -llinht-ctrl $(LDFLAGS)

.PHONY: all clean install uninstall example