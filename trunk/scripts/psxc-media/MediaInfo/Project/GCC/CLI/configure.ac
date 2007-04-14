AC_INIT(../../../Source/Common/Core.cpp)
AC_PROG_CXX()
AC_LANG_CPLUSPLUS()

dnl *************************************************************************
dnl Preferences
dnl *************************************************************************

AC_ARG_ENABLE([unicode],  [AS_HELP_STRING([--enable-unicode], [Unicode mode])],                                 [], [enable_unicode=no])
AC_ARG_ENABLE([shared],   [AS_HELP_STRING([--enable-shared],  [Use shared library instead of static link])],    [], [enable_shared=yes])

dnl *************************************************************************
dnl Configuration
dnl *************************************************************************

if [[ "${enable_unicode}" = no ]]
then
  CXXPFLAGS="$CXXFLAGS"
else
  CXXFLAGS="$CXXFLAGS -DUNICODE -D_UNICODE"
  enable_unicode=yes
fi

if [[ "${enable_shared}" = no ]]
then
  LIBS="$LIBS -L../../../../Shared/Library/GCC_Linux_i386"
  LIBS="$LIBS -L../../../../MediaInfoLib/Release/GCC_Linux_i386/Library"
  LDFLAGS="$LDFLAGS --static --static-libgcc"
else
  CXXFLAGS="$CXXFLAGS -DMEDIAINFO_DLL"
  LIBS="$LIBS -ldl"
  AC_CHECK_FILE(../../../Include/MediaInfoDLL/MediaInfoDLL.h, [INCS="$INCS -I../../../Include"], [INCS="$INCS -I../../../../MediaInfoLib/Source"])
  enable_shared=yes
fi

dnl *************************************************************************
dnl MakeFile
dnl *************************************************************************

AC_SUBST(INCS, $INCS)
AC_OUTPUT(Makefile)

echo "Configuration finnished"
echo "  Using Shared Library : $enable_shared"
echo "  Using Unicode :        $enable_unicode"
