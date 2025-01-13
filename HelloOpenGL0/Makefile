OBJS = simple.o 
INCLUDES = -I../include
LIBS = -L../lib
LDFLAGS = -lglad -lglfw3 
PROGRAM = simple
CFLAGS = -g -std=c++11


ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    LDFLAGS += -lopengl32 -lgdi32
    PROGRAM :=$(addsuffix .exe,$(PROGRAM))
    COMPILER := g++
else ifeq ($(shell uname -s),Darwin)     # is MACOSX
    LDFLAGS += -framework Cocoa -framework OpenGL -framework IOKit
    COMPILER = clang++
endif

simple: clean $(OBJS)
	$(COMPILER) -o $(PROGRAM) $(OBJS) $(LIBS) $(LDFLAGS)

simple.o: simple.c
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c simple.c
	
RM = rm	-f
ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    RM := del
endif

clean: 
	$(RM) $(OBJS) $(PROGRAM)