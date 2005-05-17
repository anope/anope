dnl Macro: unet_CHECK_TYPE_SIZES
dnl
dnl Check the size of several types and define a valid int16_t and int32_t.
dnl
AC_DEFUN(unreal_CHECK_TYPE_SIZES,
[dnl Check type sizes
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
if test "$ac_cv_sizeof_int" = 2 ; then
  AC_CHECK_TYPE(int16_t, int)
  AC_CHECK_TYPE(u_int16_t, unsigned int)
elif test "$ac_cv_sizeof_short" = 2 ; then
  AC_CHECK_TYPE(int16_t, short)
  AC_CHECK_TYPE(u_int16_t, unsigned short)
else
  AC_MSG_ERROR([Cannot find a type with size of 16 bits])
fi
if test "$ac_cv_sizeof_int" = 4 ; then
  AC_CHECK_TYPE(int32_t, int)
  AC_CHECK_TYPE(u_int32_t, unsigned int)
elif test "$ac_cv_sizeof_short" = 4 ; then
  AC_CHECK_TYPE(int32_t, short)
  AC_CHECK_TYPE(u_int32_t, unsigned short)
elif test "$ac_cv_sizeof_long" = 4 ; then
  AC_CHECK_TYPE(int32_t, long)
  AC_CHECK_TYPE(u_int32_t, unsigned long)
else
  AC_MSG_ERROR([Cannot find a type with size of 32 bits])
fi
])

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all, and "pthread-config"
# which is a program returning the flags for the Pth emulation library.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt pthread-config"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
# pthread: Linux, etcetera
# --thread-safe: KAI C++
# pthread-config: use pthread-config program (for GNU Pth library)

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthread or
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthread -pthreads pthread -mt $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

                pthread-config)
                AC_CHECK_PROG(acx_pthread_config, pthread-config, yes, no)
                if test x"$acx_pthread_config" = xno; then continue; fi
                PTHREAD_CFLAGS="`pthread-config --cflags`"
                PTHREAD_LIBS="`pthread-config --ldflags` `pthread-config --libs`"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: threads are created detached by default
        # and the JOINABLE attribute has a nonstandard name (UNDETACHED).
        AC_MSG_CHECKING([for joinable pthread attribute])
        AC_TRY_LINK([#include <pthread.h>],
                    [int attr=PTHREAD_CREATE_JOINABLE;],
                    ok=PTHREAD_CREATE_JOINABLE, ok=unknown)
        if test x"$ok" = xunknown; then
                AC_TRY_LINK([#include <pthread.h>],
                            [int attr=PTHREAD_CREATE_UNDETACHED;],
                            ok=PTHREAD_CREATE_UNDETACHED, ok=unknown)
        fi
        if test x"$ok" != xPTHREAD_CREATE_JOINABLE; then
                AC_DEFINE(PTHREAD_CREATE_JOINABLE, $ok,
                          [Define to the necessary symbol if this constant
                           uses a non-standard name on your system.])
        fi
        AC_MSG_RESULT(${ok})
        if test x"$ok" = xunknown; then
                AC_MSG_WARN([we do not know how to create joinable pthreads])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
                *-aix* | *-freebsd* | *-darwin*) flag="-D_THREAD_SAFE";;
                *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
                PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with cc_r
        AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

dnl # Configure paths for mysql-client, GPLv2
dnl #$Id$
dnl #ken restivo modified 2001/08/04 to remove NULLs and use 0, in case NULL undefined.
dnl # Markus Fischer <[EMAIL PROTECTED]>,  23.9.1999
dnl # URL : http://josefine.ben.tuwien.ac.at/~mfischer/m4/mysql-client.m4
dnl # Last Modified : Thu Sep 23 14:24:15 CEST 1999
dnl #
dnl # written from scratch

dnl Test for libmysqlclient and 
dnl define MYSQLCLIENT_CFLAGS, MYSQLCLIENT_LDFLAGS and MYSQLCLIENT_LIBS
dnl usage:
dnl AM_PATH_MYSQLCLIENT(
dnl     [MINIMUM-VERSION, 
dnl     [ACTION-IF-FOUND [, 
dnl     ACTION-IF-NOT-FOUND ]]])
dnl

AC_DEFUN(AM_PATH_MYSQLCLIENT,
[
AC_ARG_WITH(mysqlclient-prefix, 
                [  --with-mysqlclient-prefix=PFX Prefix where mysqlclient is 
installed],
            mysqlclient_prefix="$withval",
            mysqlclient_prefix="")

AC_ARG_WITH(mysqlclient-include, [  --with-mysqlclient-include=DIR Directory pointing 
             to mysqlclient include files],
            mysqlclient_include="$withval",
            mysqlclient_include="")

AC_ARG_WITH(mysqlclient-lib,
[  --with-mysqlclient-lib=LIB  Directory pointing to mysqlclient library
                          (Note: -include and -lib do override
                           paths found with -prefix)
],
            mysqlclient_lib="$withval",
            mysqlclient_lib="")

    AC_MSG_CHECKING([for mysqlclient ifelse([$1], , ,[>= v$1])])
    MYSQLCLIENT_LDFLAGS=""
    MYSQLCLIENT_CFLAGS=""
    MYSQLCLIENT_LIBS="-lmysqlclient"
    mysqlclient_fail=""

    dnl test --with-mysqlclient-prefix
        for tryprefix in /usr /usr/local /usr/mysql /usr/local/mysql /usr/pkg $msqlclient_prefix; do
                #testloop
                for hloc in lib/mysql lib ; do
                        if test -e "$tryprefix/$hloc/libmysqlclient.so"; then
                MYSQLCLIENT_LDFLAGS="-L$tryprefix/$hloc"
                        fi
                done

                for iloc in include/mysql include; do
                        if test -e "$tryprefix/$iloc/mysql.h"; then
                MYSQLCLIENT_CFLAGS="-I$tryprefix/$iloc"
            fi
        done
                # testloop
        done

    dnl test --with-mysqlclient-include
    if test "x$mysqlclient_include" != "x" ; then
                echo "checking for mysql includes... "
        if test -d "$mysqlclient_include/mysql" ; then
            MYSQLCLIENT_CFLAGS="-I$mysqlclient_include"
                        echo " found $MYSQLCLIENT_CFLAGS"
        elif test -d "$mysqlclient_include/include/mysql" ; then
            MYSQLCLIENT_CFLAGS="-I$mysqlclient_include/include"
                        echo " found $MYSQLCLIENT_CFLAGS"
        elif test -d "$mysqlclient_include" ; then
            MYSQLCLIENT_CFLAGS="-I$mysqlclient_include"
                        echo "found $MYSQLCLIENT_CFLAGS"
                else
                        echo "not found!  no include dir found in $mysqlclient_include"
        fi
    fi

    dnl test --with-mysqlclient-lib
    if test "x$mysqlclient_lib" != "x" ; then
                echo "checking for mysql libx... "
        if test -d "$mysqlclient_lib/lib/mysql" ; then
            MYSQLCLIENT_LDFLAGS="-L$mysqlclient_lib/lib/mysql"
                        echo "found $MYSQLCLIENT_LDFLAGS"
        elif test -d "$mysqlclient_lib/lin" ; then
            MYSQLCLIENT_LDFLAGS="-L$mysqlclient_lib/lib"
                        echo "found $MYSQLCLIENT_LDFLAGS"
        else
            MYSQLCLIENT_LDFLAGS="-L$mysqlclient_lib"
                        echo "defaultd to $MYSQLCLIENT_LDFLAGS"
        fi
    fi

    ac_save_CFLAGS="$CFLAGS"
    ac_save_LDFLAGS="$LDFLAGS"
    ac_save_LIBS="$LIBS"
    CFLAGS="-v $CFLAGS $MYSQLCLIENT_CFLAGS"
    LDFLAGS="$LDFLAGS $MYSQLCLIENT_LDFLAGS"
    LIBS="$LIBS $MYSQLCLIENT_LIBS"
    dnl if no minimum version is given, just try to compile
    dnl else try to compile AND run
        AC_TRY_COMPILE([
            #include <mysql.h>
            #include <mysql_version.h>
        ],[
            mysql_connect( 0, 0, 0, 0);
        ], [AC_MSG_RESULT(yes $MYSQLCLIENT_CFLAGS $MYSQLCLIENT_LDFLAGS)
           CFLAGS="$ac_save_CFLAGS"
           LDFLAGS="$ac_save_LDFLAGS"
           LIBS="$ac_save_LIBS"
           ifelse([$2], ,:,[$2])
        ],[
                        echo "no"
                        echo "can't compile a simple app with mysql_connnect in it. 
bad."
          mysqlclient_fail="yes"
        ])

    if test "x$mysqlclient_fail" != "x" ; then
            dnl AC_MSG_RESULT(no)
            echo
            echo "***"
            echo "*** mysqlclient test source had problems, check your config.log ."
            echo "*** Also try one of the following switches :"
            echo "***   --with-mysqlclient-prefix=PFX"
            echo "***   --with-mysqlclient-include=DIR"
            echo "***   --with-mysqlclient-lib=DIR"
            echo "***"
            CFLAGS="$ac_save_CFLAGS"
            LDFLAGS="$ac_save_LDFLAGS"
            LIBS="$ac_save_LIBS"
            ifelse([$3], ,:,[$3])
    fi

    CFLAGS="$ac_save_CFLAGS"
    LDFLAGS="$ac_save_LDFLAGS"
    LIBS="$ac_save_LIBS"
    AC_SUBST(MYSQLCLIENT_LDFLAGS)
    AC_SUBST(MYSQLCLIENT_CFLAGS)
    AC_SUBST(MYSQLCLIENT_LIBS)
])
