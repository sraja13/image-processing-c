# Midterm Project Makefile

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g
TARGET=project
EXECS=project img_cmp

all: $(EXECS)

# Links together files needed to create the project executable
project: project.o ppm_io.o image_manip.o
	$(CC) $(CFLAGS) -o $@ project.o ppm_io.o image_manip.o -lm

# Links together files needed to create the image comparison executable
img_cmp: img_cmp.o ppm_io.o
	$(CC) $(CFLAGS) -o $@ img_cmp.o ppm_io.o

# Links together files needed to create the checkerboard executable
checkerboard: checkerboard.o ppm_io.o
	$(CC) $(CFLAGS) -o $@ checkerboard.o ppm_io.o

# Compile the project source code
project.o: project.c ppm_io.h image_manip.h
	$(CC) $(CFLAGS) -c project.c

# Compile checkerboard source code
checkerboard.o: checkerboard.c ppm_io.h
	$(CC) $(CFLAGS) -c checkerboard.c

# Compile img_cmp source code
img_cmp.o: img_cmp.c ppm_io.h
	$(CC) $(CFLAGS) -c img_cmp.c

# Compile the image manipulation source code
image_manip.o: image_manip.c image_manip.h ppm_io.h
	$(CC) $(CFLAGS) -c image_manip.c

# Compile the ppm i/o source code
ppm_io.o: ppm_io.c ppm_io.h
	$(CC) $(CFLAGS) -c ppm_io.c

# Removes all object files and the executables,
# so we can start fresh
clean:
	rm -f *.o $(EXECS)