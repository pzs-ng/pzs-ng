## ##
# # # 
#   # akefile for dark0n3's zipscript-c
#   #
#   #

CC = gcc
RM = rm -f
MAKE = make

CCFLAGS = -Wall -lcrypto -O2
CFFLAGS = -Wall -static -O2

all:	clean compile

compile:	
		${CC} ${CCFLAGS} -o passchk passchk.c
		${CC} ${CFFLAGS} -o ng-bandwidth ng-bw.c

clean:	
	$(RM) ng-bandwidth passchk
