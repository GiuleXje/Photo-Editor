#314CA Pal Roberto Giulio

# compiler
CC = gcc

# compiler flags
CFLAGS = -Wall -Wextra -g

# object files
OBJ = image_editor.o func.o

build: func image_editor
	$(CC) $(OBJ) -o image_editor -lm -g

func: func.c func.h
	$(CC) $(CFLAGS) -c $@.c

image_editor: image_editor.c
	$(CC) $(CFLAGS) -c $@.c

run: build
	./image_editor

pack:
	zip -FSr 3XYCA_PalRobertoGiulio_Tema3.zip README Makefile *.c *.h

clean:
	rm -f *.o image_editor

.PHONY: pack clean