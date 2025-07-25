#ifndef HU_LIB_HPP
#define HU_LIB_HPP

#ifndef HU_FEATURES_H
#    include <hu/features.h>
#endif

#ifndef HU_OS_H
#    include <hu/os.h>
#endif

#define HU_GLIBC_P 0
#define HU_BIONIC_P 0
#define HU_UCLIBC_P 0
#define HU_NEWLIB_P 0
#define HU_DIETLIBC_P 0

#define HU_HAVE_CXX_VERSION_HEADER_P 0

#define HU_LIBSTDCXX_P 0
#define HU_LIBCPP_P 0

#define HU_LIBC_PREREQ(ax, ay, az, bx, by, bz)                                 \
    ((ax) *10000L + (ay) *100L + (az) <= (bx) *10000L + (by) *100L + (bz))
#define HU_LIBC_PREREQ2(ax, ay, bx, by) HU_LIBC_PREREQ(0, ax, ay, 0, bx, by)

#define HU_GLIBC_PREREQ(x, y) 0
#define HU_UCLIBC_PREREQ(x, y, z) 0
#define HU_NEWLIB_PREREQ(x, y, z) 0

/* clang-format off */
#if hu_has_include(<sys/types.h>) || HU_OS_POSIX_P || HU_OS_APPLE_P
/* clang-format on */
#    include <sys/types.h>
#elif hu_has_include(<features.h>)
#    include <features.h>
#endif

#ifdef __BIONIC__
#    undef HU_BIONIC_P
#    define HU_BIONIC_P 1
#    define HU_BIONIC 1
#endif

#if defined(__GLIBC__) || defined(__GNU_LIBRARY__)
#    undef HU_GLIBC_P
#    define HU_GLIBC_P 1
#    define HU_GLIBC 1
#    undef HU_GLIBC_PREREQ
#    ifdef __GLIBC__
#        define HU_GLIBC_PREREQ(x, y)                                          \
            HU_LIBC_PREREQ2(x, y, __GLIBC__, __GLIBC_MINOR__)
#    else
#        define HU_GLIBC_PREREQ(x, y)                                          \
            HU_LIBC_PREREQ2(x, y, __GNU_LIBRARY__, __GNU_LIBRARY_MINOR__)
#    endif
#endif

#ifdef __NEWLIB__
#    undef HU_NEWLIB_P
#    define HU_NEWLIB_P 1
#    define HU_NEWLIB 1
#    undef HU_NEWLIB_PREREQ
#    define HU_NEWLIB_PREREQ(x, y, z)                                          \
        HU_LIBC_PREREQ(                                                        \
          x, y, z, __NEWLIB__, __NEWLIB_MINOR__, __NEWLIB_PATCHLEVEL__)
#endif

#ifdef __UCLIBC__
#    undef HU_UCLIBC_P
#    define HU_UCLIBC_P 1
#    define HU_UCLIBC 1
#    undef HU_UCLIBC_PREREQ
#    define HU_UCLIBC_PREREQ(x, y, z)                                          \
        HU_LIBC_PREREQ(                                                        \
          x, y, z, __UCLIBC_MAJOR__, __UCLIBC_MINOR__, __UCLIBC_SUBLEVEL__)
#endif

#ifdef __dietlibc__
#    undef HU_DIETLIBC_P
#    define HU_DIETLIBC_P 1
#    define HU_DIETLIBC 1
#endif

#if HU_CXX_P
#    if hu_has_include(<version>) || HU_CXX_PREREQ(20)
#        include <version>
#        undef HU_HAVE_CXX_VERSION_HEADER_P
#        define HU_HAVE_CXX_VERSION_HEADER_P 1
#        define HU_HAVE_CXX_VERSION_HEADER 1
#    elif hu_maybe_has_include(<ciso646>)
#        include <ciso646>
#    endif

#    if defined(__GLIBCPP__) || defined(__GLIBCXX__)
#        undef HU_LIBSTDCXX_P
#        define HU_LIBSTDCXX_P 1
#        define HU_LIBSTDCXX 1
#    endif

#    ifdef _LIBCPP_VERSION
#        undef HU_LIBCPP_P
#        define HU_LIBCPP_P 1
#        define HU_LIBCPP 1
#    endif

#endif

#endif
