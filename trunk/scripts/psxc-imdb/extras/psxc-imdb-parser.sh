#!/bin/bash

# Bash is very limited in formatting output. You'd do better in perl or some
# other "real" language. But since this is just an example, I'll do this in
# what most people manages to read. The script in itself is not important -
# the variables are, since I expect you to experiment a bit with this..
# If you manage to create a decent script which formats in a generic way,
# and don't mind spreading the wealth, please send it to me so I can include
# it.
#
VERSION=2.5

IFSORIG=$IFS
IFS="^"

# Initialize variables. bash is a bit limited, so we gotta do a "hack"
c=1
for a in `echo $@ | sed "s|\" \"|^|g"`; do
b[c]=$a
let c=c+1
done

IFS=$IFSORIG

# Give the variables some sensible names
IMDBDATE=${b[1]}
IMDBDOTFILE=${b[2]}
IMDBRELPATH=${b[3]}
IMDBDIRNAME=${b[4]}
IMDBURL=${b[5]}
IMDBTITLE=${b[6]}
IMDBGENRE=${b[7]}
IMDBRATING=${b[8]}
IMDBCOUNTRY=${b[9]}
IMDBLANGUAGE=${b[10]}
IMDBCERTIFICATION=${b[11]}
IMDBRUNTIME=${b[12]}
IMDBDIRECTOR=${b[13]}
IMDBBUSINESSDATA=${b[14]}
IMDBPREMIERE=${b[15]}
IMDBLIMITED=${b[16]}
IMDBVOTES=${b[17]}
IMDBSCORE=${b[18]}
IMDBNAME=${b[19]}
IMDBYEAR=${b[20]}
IMDBNUMSCREENS=${b[21]}
IMDBISLIMITED=${b[22]}
IMDBCASTLEADNAME=${b[23]}
IMDBCASTLEADCHAR=${b[24]}
IMDBTAGLINE=${b[25]}
IMDBPLOT=${b[26]}
IMDBBAR=${b[27]}
IMDBCASTING=${b[28]}
IMDBCOMMENTSHORT=${b[29]}
IMDBCOMMENTFULL=${b[30]}

#########################################################################
# From here on you should just paste/format the output to your liking.

#
# enter your code here....
#

#
#########################################################################
# You should always exit with a 0 - the parent won't give a crap anyway,
# but just to make sure...
exit 0

