LIBSDL =  -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL  -lSDL_image -lSDL_mixer -lSDL_ttf -lpthread
PLATFORM=x86

PROG    = tanky
CC=$(CROSS_COMPILE)g++
INCLS	=  -I/usr/include

#source codes
SRCS = tanky.cpp playground.cpp tank.cpp sound.cpp dot.cpp figure.cpp target.cpp
#substitute .c by .o to obtain object filenames
OBJS = $(SRCS:.cpp=.o) 
 
#$< evaluates to the target's dependencies,
#$@ evaluates to the target
 
$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS)  $(LIBSDL) 
 
$(OBJS):
	$(CC) -c  $*.cpp $(INCLS) -D$(PLATFORM)
      
run:
	./$(PROG)                                                                                
clean:
	rm $(OBJS)
