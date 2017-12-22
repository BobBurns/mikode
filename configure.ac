#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Mikode], [1.2.1], [reburns@protonmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([ncurses], [mvwaddch])
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h stdlib.h string.h unistd.h getopt.h dirent.h])
# check for chip or pi
AC_CHECK_FILE(/opt/vc/include/bcm_host.h, [AC_DEFINE([HAVE__BCM_HOST], [1], ["have pi specific header file"])], [AC_DEFINE([HAVE__BCM_HOST], [0], ["NO pi spcecific header file"])])

# check for dir type to get gpio pin numbers.  wont work without it.
AC_CHECK_MEMBER([struct dirent.d_type], [AC_DEFINE([HAVE__DT_LNK], [1], ["have dir types to check CHIP gpio"])], [AC_DEFINE([HAVE__DT_LNK], [0], ["does NOT have dir types to check CHIP gpio"])], [[#include <dirent.h>]])


AS_IF([test -e /sys/firmware/devicetree/base/model && grep 'NextThing C.H.I.P.' /sys/firmware/devicetree/base/model], [AC_DEFINE([HAVE__NTC_MODEL], [1], ["has specific chip name"])], [AC_DEFINE([HAVE__NTC_MODEL], [0], ["NO chip name found"])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset strncasecmp strndup strtol])

AM_CONDITIONAL([PI], [test -e /opt/vc/include/bcm_host.h])
AM_CONDITIONAL([CHIP], [test -e /sys/firmware/devicetree/base/model && grep 'NextThing C.H.I.P.' /sys/firmware/devicetree/base/model])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
