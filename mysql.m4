# Configure paths for mysql-client, GPLv2
#$Id$
#ken restivo modified 2001/08/04 to remove NULLs and use 0, in case NULL undefined.
# Markus Fischer <[EMAIL PROTECTED]>,  23.9.1999
# URL : http://josefine.ben.tuwien.ac.at/~mfischer/m4/mysql-client.m4
# Last Modified : Thu Sep 23 14:24:15 CEST 1999
#
# written from scratch

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
