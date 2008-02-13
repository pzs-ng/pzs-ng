dnl aclocal.m4: macros autoconf uses when building configure from configure.ac
dnl
dnl Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004 Eggheads Development Team
dnl
dnl This program is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License
dnl as published by the Free Software Foundation; either version 2
dnl of the License, or (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
dnl
dnl $Id: aclocal.m4,v 1.95 2004/08/21 00:16:55 wcc Exp $
dnl

dnl SSL check taken from the eggdrop SSL patch

dnl  PZSNG_SSL()
dnl
AC_DEFUN(PZSNG_SSL, [dnl
#ssl checks - hoe
AC_MSG_CHECKING(whether to include SSL support)
AC_ARG_WITH(ssl,
[  --with-ssl[=PATH]         Include SSL support (DIR is OpenSSL's install dir).],
[
  case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      AC_CHECK_LIB(ssl,SSL_accept,[AC_DEFINE(PZSNG_SSL_EXT)
      SSL_LIBS="-lssl -lcrypto"],SSL_LIBS="",-lcrypto)
      
      ;;
    *)
dnl A whole whack of possible places where this might be
      test -f $withval/openssl/ssl.h && SSL_INCLUDE="$withval"
      test -f $withval/include/openssl/ssl.h && SSL_INCLUDE="$withval/include"

      test -f $withval/lib/libssl.a && SSL_LIB="$withval/lib"
      test -f $withval/lib/ssl/libssl.a && SSL_LIB="$withval/lib/ssl"
      test -f $withval/lib/openssl/libssl.a && SSL_LIB="$withval/lib/openssl/ssl"
      test -f $withval/libssl.a && SSL_LIB="$withval"
      test -f $withval/ssl/libssl.a && SSL_LIB="$withval/ssl"
      test -f $withval/openssl/libssl.a && SSL_LIB="$withval/openssl"

      test -f $withval/lib/libcrypto.a && CRYPTO_LIB="$withval/lib"
      test -f $withval/lib/ssl/libcrypto.a && CRYPTO_LIB="$withval/lib/ssl"
      test -f $withval/lib/openssl/libcrypto.a && CRYPTO_LIB="$withval/lib/openssl/ssl"
      test -f $withval/libcrypto.a && CRYPTO_LIB="$withval"
      test -f $withval/ssl/libcrypto.a && CRYPTO_LIB="$withval/ssl"
      test -f $withval/openssl/libcrypto.a && CRYPTO_LIB="$withval/openssl"

      if test -n "$SSL_INCLUDE" && test -n "$SSL_LIB" && test -n "$CRYPTO_LIB"; then
        SSL_LIBS="-lssl -lcrypto -L$SSL_LIB -L$CRYPTO_LIB";
        AC_DEFINE(PZSNG_SSL_EXT)
        AC_MSG_RESULT(yes)
      else
        AC_MSG_RESULT(no)
	SSL_LIBS="";
        dnl AC_MSG_ERROR([You requested SSL support, but OpenSSL was not found. Please supply a pathname to OpenSSL])
      fi ;;
  esac
],[
    AC_MSG_RESULT(will try to find)
    AC_CHECK_LIB(ssl,SSL_accept,[AC_DEFINE(PZSNG_SSL_EXT)
     SSL_LIBS="-lssl -lcrypto"],SSL_LIBS="",-lcrypto)
    AC_CHECK_HEADERS(openssl/ssl.h,,SSL_INCLUDE="",)
    dnl AC_CHECK_HEADERS("openssl/ssl.h",,[AC_MSG_ERROR([OpenSSL was not found. Please supply a pathname to OpenSSL])],)
     dnl SSL_LIBS="-lssl -lcrypto"],[AC_MSG_ERROR([OpenSSL was not found. Please supply a pathname to OpenSSL])],-lcrypto)

    AC_SUBST(SSL_LIBS)dnl
    AC_SUBST(SSL_INCLUDE)dnl
])

#end of ssl checks - hoe (.16 by poci)
])dnl
