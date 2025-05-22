#all:
#	gcc -o main src/main.c src/glad.c -lGL -lglfw -Iinclude -lm

CC = gcc
CFLAGS = -Wall -lm -Iinclude

ifeq ($(OS), Windows_NT)
  $(info Windows detected)
  LIBS = -Llib/win32 -lglfw3 -lopengl32 -lgdi32 -Lwin32 -lglu32 -lglut32win
else
  $(info Linux detected)
  LIBS = -Llib/linux -lglfw3 -lGL -lGLU -lglut
endif

all:
	$(CC) -o main src/main.c src/glad/glad.c $(CFLAGS) $(LIBS)
