#!/bin/sh
glrootpath=/glftpd
FLAGS="3"
export FLAGS

$glrootpath/bin/sitewho | grep -e "Up:" | grep -e "Dn:" | grep -e "Total:" | awk '{ printf("%i %0.1f %i %0.1f %i %0.1f\n", $3, $5, $8, $10, $13, $15) }'
