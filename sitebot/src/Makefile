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

all:	clean gl2
old:	clean gl1

gl2:	
		${CC} ${CCFLAGS} -o passchk passchk.c
		${CC} ${CFFLAGS} -o ng-bandwidth ng-bw.c

gl1:	
		${CC} ${CCFLAGS} -o passchk passchk-old.c
		${CC} ${CFFLAGS} -o ng-bandwidth ng-bw.c

clean:	
	$(RM) ng-bandwidth passchk
