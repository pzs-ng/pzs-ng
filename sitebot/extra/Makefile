## ##
# # # 
#   # akefile for dark0n3's zipscript-c
#   #
#   #

CC = gcc
RM = rm -f
MAKE = make

CCFLAGS = -Wall -lcrypt -O2

all:	clean compile

compile:	
		${CC} ${CCFLAGS} -o passchk passchk.c

clean:	
	$(RM) passchk
