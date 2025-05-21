#all:
#	gcc -o main src/main.c src/glad.c -lGL -lglfw -Iinclude -lm

CC = gcc
CFLAGS = -Wall -lm -Iinclude

ifeq ($(OS), Windows_NT)
  $(info Windows detected)
  LIBS = -Llib/win32 -lglfw3 -lopengl32 -lgdi32 -Lwin32 -lm -lGLU -lglut
else
  $(info Linux detected)
  LIBS = -Llib/linux -lglfw3 -lGL -lGLU -lglut -lm
endif

all:
	$(CC) -o main src/main.c src/glad.c $(CFLAGS) $(LIBS)
