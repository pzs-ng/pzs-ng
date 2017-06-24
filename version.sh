#!/bin/sh
# extract abbreviated hash from head
GIT_PREFIX="git-"
GIT_HASH="${GIT_PREFIX}dl"
if [ -f .git/HEAD ]; then
  if [ "$(cut -c1-5 .git/HEAD)" = "ref: " ]; then
    GIT_REF=".git/$(cut -c6- .git/HEAD)"
    if [ -f ${GIT_REF} ]; then
      GIT_HASH="${GIT_PREFIX}$(basename ${GIT_REF})-$(cut -c1-8 ${GIT_REF})"
    fi
  else
    GIT_HASH="$GIT_PREFIX$(cut -c1-8 .git/HEAD)"
  fi
fi
echo ${GIT_HASH}

cat << EOF > zipscript/include/ng-version.h
#ifndef _NG_VERSION_H_
#define _NG_VERSION_H_

#define NG_VERSION "${GIT_HASH}"

#endif
EOF
