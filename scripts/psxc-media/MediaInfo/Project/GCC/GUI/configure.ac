AC_INIT(../../../Source/Common/Core.cpp)
AC_PROG_CXX()
AC_LANG_CPLUSPLUS()

dnl *************************************************************************
dnl Preferences
dnl *************************************************************************

AC_ARG_ENABLE([unicode],  [AS_HELP_STRING([--enable-unicode], [Unicode or not @<:@default=check@:>@])],         [], [enable_unicode=check])
AC_ARG_ENABLE([shared],   [AS_HELP_STRING([--enable-shared],  [Use shared library instead of static link])],    [], [enable_shared=yes])

dnl *************************************************************************
dnl builtin libs
dnl *************************************************************************

AC_CHECK_FILE(../../../../Shared/Source/System/WxWidgets/configure, enable_WxWidgets="builtin", enable_WxWidgets="system")

dnl *************************************************************************
dnl wxWidgets
dnl *************************************************************************

dnl -------------------------------------------------------------------------
dnl Config
if [[ "$enable_WxWidgets" = "system" ]] ; then
  AC_PATH_PROG(WX_CONFIG, wx-config, no, $PATH:/usr/local/bin:/usr/bin )
else
  AC_CHECK_FILE(../../../../Shared/Source/System/WxWidgets/wx-config, WX_CONFIG=../../../../Shared/Source/System/WxWidgets/wx-config, WX_CONFIG=no)
fi
if [[ "$WX_CONFIG" = "no" ]] ; then
  AC_MSG_ERROR("Could not find wx-config: install wxWidgets (2.6 or 2.8) first.")
fi

dnl -------------------------------------------------------------------------
dnl Version
enable_WxWidgets_Version=`${WX_CONFIG} --version=2.8 --release`
if [[ "${enable_WxWidgets_Version}" = 2.8 ]]
then
  echo "WxWidgets 2.8 found."
else
  enable_WxWidgets_Version=`${WX_CONFIG} --version=2.6 --release`
  if [[ "${enable_WxWidgets_Version}" = 2.6 ]]
  then
    echo "WxWidgets 2.6 found."
  else
    ${WX_CONFIG} $Config_Unicode --list
    AC_MSG_ERROR([Unable to locate a suitable configuration of wxWidgets 2.6 or 2.8.
      install wxWidgets (2.6 or 2.8) first.
      ])
  fi
fi

dnl -------------------------------------------------------------------------
dnl Unicode
if [[ "${enable_unicode}" = check ]]
then
  enable_WxWidgets_Unicode=`${WX_CONFIG} --version=$enable_WxWidgets_Version --unicode=yes --release`
  if [[ "${enable_WxWidgets_Unicode}" = "${enable_WxWidgets_Version}" ]]
  then
    enable_unicode=yes
	CXXFLAGS="$CXXFLAGS -DUNICODE -D_UNICODE"
  else
    enable_unicode=no
  fi
else
  if [[ "${enable_unicode}" = no ]]
  then
    enable_WxWidgets_NoUnicode=`${WX_CONFIG} --version=$enable_WxWidgets_Version --unicode=no --release`
    if [[ "${enable_WxWidgets_NoUnicode}" = "${enable_WxWidgets_Version}" ]]
    then
      enable_unicode=no
    else
      AC_MSG_ERROR([Unable to locate a suitable configuration of wxWidgets 2.6 or 2.8.
        install wxWidgets (2.6 or 2.8) with no unicode first.
        ])
    fi
  else
    enable_WxWidgets_Unicode=`${WX_CONFIG} --version=$enable_WxWidgets_Version --unicode=yes --release`
    if [[ "${enable_WxWidgets_Unicode}" = "${enable_WxWidgets_Version}" ]]
    then
      enable_unicode=yes
    else
      AC_MSG_ERROR([Unable to locate a suitable configuration of wxWidgets 2.6 or 2.8.
        install wxWidgets (2.6 or 2.8) with unicode first.
        ])
    fi
  fi
fi
  
dnl -------------------------------------------------------------------------
dnl need GUI
Wx_BaseName="`$WX_CONFIG --unicode=$enable_unicode --basename`"
if [[ "${Wx_BaseName}" = wx_base ]]
then
  AC_MSG_ERROR([Unable to locate a suitable configuration of wxWidgets 2.6 or 2.8.
    install wxWidgets (2.6 or 2.8) with GUI first.
    ])
fi
if [[ "${Wx_BaseName}" = wx_baseu ]]
then
  AC_MSG_ERROR([Unable to locate a suitable configuration of wxWidgets 2.6 or 2.8.
    install wxWidgets (2.6 or 2.8) with GUI first.
    ])
fi

dnl -------------------------------------------------------------------------
dnl Makefile
LIBS="$LIBS `$WX_CONFIG --unicode=$enable_unicode --libs`"
CXXFLAGS="$CXXFLAGS `$WX_CONFIG --unicode=$enable_unicode --cxxflags`"

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
  LDFLAGS="$LDFLAGS -L../../../../Shared/Library/GCC_Linux_i386"
  LDFLAGS="$LDFLAGS -L../../../../MediaInfoLib/Release/GCC_Linux_i386/Library"
  enable_shared=builtin
else
  CXXFLAGS="$CXXFLAGS -DMEDIAINFO_DLL"
  LIBS="$LIBS -ldl"
  AC_CHECK_FILE(../../../Include/MediaInfoDLL/MediaInfoDLL.h, [INCS="$INCS -I../../../Include"], [INCS="$INCS -I../../../../MediaInfoLib/Source"])
  enable_shared=system
fi

dnl *************************************************************************
dnl MakeFile
dnl *************************************************************************

AC_SUBST(INCS, $INCS)
AC_OUTPUT(Makefile)

echo "Configuration finnished"
echo "  Using Shared MediaInfo Library : $enable_shared"
echo "  Using WxWidgets :                $enable_WxWidgets"
echo "  Using Unicode :                  $enable_unicode"
