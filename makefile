#Similar makefile as my Shell Program Implementation:
#https://github.com/shahaddin/Unix-Shell-Program

#Shahaddin Gafarov 
#I have simplified the make file I have used, In my previous implemention I have made the command: if [ -f cv ]; then echo SUCCESS; fi builtin
#Basically what my makefile does is upon typing make, compiles the gcc and gives an executible cv.o(gcc -Wall -Werror -c cv.c)

#defining the compiler to be used
CC = gcc
#the flags of the compiler
CFLAGS = -Wall -Werror -pthread

#our target is to the cv
all: cv

#for getting cv as executable, we need cv.o first
cv: cv.o
#this line turns the object file into executable using -o command
	@$(CC) cv.o -o cv

#for getting cv.o, we need cv.c first
cv.o: cv.c
#this is the command to compiler
#-c is for creating object file(.0) instead of an executable
	@$(CC) $(CFLAGS) -c cv.c

#default cleanup operation
clean:
	rm -f cv.o

#I used this as a source for building my makefile
#Sources:https://makefiletutorial.com/