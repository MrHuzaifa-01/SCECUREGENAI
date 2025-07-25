#ifndef HU_OS_H
#define HU_OS_H

/* Families */
#define HU_OS_POSIX_P 0
#define HU_OS_BSD_P 0   /* implies POSIX */
#define HU_OS_APPLE_P 0 /* implies POSIX, but not BSD */

#define HU_OS_LINUX_P 0     /* implies POSIX */
#define HU_OS_FREEBSD_P 0   /* implies BSD */
#define HU_OS_OPENBSD_P 0   /* implies BSD */
#define HU_OS_DRAGONFLY_P 0 /* implies BSD */
#define HU_OS_NETBSD_P 0    /* implies BSD */
#define HU_OS_OSX_P 0       /* implies APPLE */
#define HU_OS_IOS_P 0       /* implies APPLE */
#define HU_OS_WINDOWS_P 0
#define HU_OS_ANDROID_P 0 /* implies LINUX */
#define HU_OS_SOLARIS_P 0 /* implies POSIX */
#define HU_OS_WEB_P 0

#ifdef HU_OS_FREESTANDING
#    define HU_OS_FREESTANDING_P 1
#else
#    define HU_OS_FREESTANDING_P 0
#endif

#if defined(__linux__) || defined(__linux) || defined(__gnu_linux__)
#    undef HU_OS_LINUX_P
#    define HU_OS_LINUX_P 1
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#    undef HU_OS_WINDOWS_P
#    define HU_OS_WINDOWS_P 1
#    define HU_OS_WINDOWS 1
#endif

#ifdef __APPLE__
/* safe to include in any language, only uses CPP constructs */
#    include <TargetConditionals.h>
#    undef HU_OS_APPLE_P
#    define HU_OS_APPLE_P 1
#    define HU_OS_APPLE 1
#    if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#        undef HU_OS_IOS_P
#        define HU_OS_IOS_P 1
#        define HU_OS_IOS 1
#    else
#        undef HU_OS_OSX_P
#        define HU_OS_OSX_P 1
#        define HU_OS_OSX 1
#    endif
#endif

#ifdef __ANDROID__
#    undef HU_OS_ANDROID_P
#    define HU_OS_ANDROID_P 1
#    define HU_OS_ANDROID 1
#endif

#ifdef __FreeBSD__
#    undef HU_OS_FREEBSD_P
#    define HU_OS_FREEBSD_P 1
#    define HU_OS_FREEBSD 1
#endif

#ifdef __OpenBSD__
#    undef HU_OS_OPENBSD_P
#    define HU_OS_OPENBSD_P 1
#    define HU_OS_OPENBSD 1
#endif

#ifdef __DragonFly__
#    undef HU_OS_DRAGONFLY_P
#    define HU_OS_DRAGONFLY_P 1
#    define HU_OS_DRAGONFLY 1
#endif

#ifdef __NetBSD__
#    undef HU_OS_NETBSD_P
#    define HU_OS_NETBSD_P 1
#    define HU_OS_NETBSD 1
#endif

#if defined(__EMSCRIPTEN__) || defined(__wasm) || defined(__wasm__) ||         \
  defined(__wasm32) || defined(__wasm32__)
#    undef HU_OS_WEB_P
#    define HU_OS_WEB_P 1
#    define HU_OS_WEB 1
#endif

#if defined(__sun) || defined(__sun__)
#    undef HU_OS_SOLARIS_P
#    define HU_OS_SOLARIS_P 1
#    define HU_OS_SOLARIS 1
#endif

#if (defined(_POSIX_VERSION) || defined(__unix__) || defined(__unix) ||        \
     HU_OS_APPLE_P)
#    undef HU_OS_POSIX_P
#    define HU_OS_POSIX_P 1
#    define HU_OS_POSIX 1
#endif

#if HU_OS_FREEBSD_P || HU_OS_OPENBSD_P || HU_OS_DRAGONFLY_P || HU_OS_NETBSD_P
#    undef HU_OS_BSD_P
#    define HU_OS_BSD_P 1
#    define HU_OS_BSD 1
#endif

#if (HU_OS_LINUX_P + HU_OS_FREEBSD_P + HU_OS_OPENBSD_P + HU_OS_DRAGONFLY_P +   \
     HU_OS_NETBSD_P + HU_OS_OSX_P + HU_OS_IOS_P + HU_OS_WINDOWS_P +            \
     HU_OS_WEB_P + HU_OS_SOLARIS_P + HU_OS_FREESTANDING_P) != 1
#    error "BUG: HU_OS_*_P not properly defined"
#endif

#endif
