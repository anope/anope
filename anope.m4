dnl Macro: anope_CHECK_TYPE_SIZES
dnl
dnl Check the size of several types and define a valid int16_t and int32_t.
dnl
AC_DEFUN(anope_CHECK_TYPE_SIZES,
[dnl Check type sizes
dnl AC_CHECK_SIZEOF(short)
dnl AC_CHECK_SIZEOF(int)
dnl AC_CHECK_SIZEOF(long)
dnl if test "$ac_cv_sizeof_int" = 2 ; then
dnl   AC_CHECK_TYPE(int16_t, int)
dnl   AC_CHECK_TYPE(u_int16_t, unsigned int)
dnl elif test "$ac_cv_sizeof_short" = 2 ; then
dnl   AC_CHECK_TYPE(int16_t, short)
dnl   AC_CHECK_TYPE(u_int16_t, unsigned short)
dnl else
dnl   AC_MSG_ERROR([Cannot find a type with size of 16 bits])
dnl fi
dnl if test "$ac_cv_sizeof_int" = 4 ; then
dnl   AC_CHECK_TYPE(int32_t, int)
dnl   AC_CHECK_TYPE(u_int32_t, unsigned int)
dnl elif test "$ac_cv_sizeof_short" = 4 ; then
dnl   AC_CHECK_TYPE(int32_t, short)
dnl   AC_CHECK_TYPE(u_int32_t, unsigned short)
dnl elif test "$ac_cv_sizeof_long" = 4 ; then
dnl   AC_CHECK_TYPE(int32_t, long)
dnl   AC_CHECK_TYPE(u_int32_t, unsigned long)
dnl else
dnl  AC_MSG_ERROR([Cannot find a type with size of 32 bits])
dnl fi
AC_CHECK_TYPE(uint8_t, AC_DEFINE(HAVE_UINT8_T, 1, "Has uint8_t type"))
AC_CHECK_TYPE(u_int8_t, AC_DEFINE(HAVE_U_INT8_T, 1, "Has u_int8_t type"))
AC_CHECK_TYPE(int16_t, AC_DEFINE(HAVE_INT16_T, 1, "Has int16_t type"))
AC_CHECK_TYPE(uint16_t, AC_DEFINE(HAVE_UINT16_T, 1, "Has uint16_t type"))
AC_CHECK_TYPE(u_int16_t, AC_DEFINE(HAVE_U_INT16_T, 1, "Has u_int16_t type"))
AC_CHECK_TYPE(int32_t, AC_DEFINE(HAVE_INT32_T, 1, "Has int32_t type"))
AC_CHECK_TYPE(uint32_t, AC_DEFINE(HAVE_UINT32_T, 1, "Has uint32_t type"))
AC_CHECK_TYPE(u_int32_t, AC_DEFINE(HAVE_U_INT32_T, 1, "Has u_int32_t type"))
])
