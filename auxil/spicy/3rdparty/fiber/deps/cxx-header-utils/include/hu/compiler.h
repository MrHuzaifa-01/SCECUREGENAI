#ifndef HU_COMPILER_H
#define HU_COMPILER_H

#define HU_COMP_CLANG_P 0
#define HU_COMP_COMPCERT_P 0
#define HU_COMP_GCC_P 0
#define HU_COMP_IBMXL_P 0 /* On newer versions implies HU_COMP_CLANG_P */
#define HU_COMP_INTEL_P 0
#define HU_COMP_MSVC_P 0

#define HU_COMP_GNUC_P 0

#define HU_VERSION(x, y, z) ((x) *10000L + (y) *100L + (z))
#define HU_VERSION_PREREQ(x, y, z, v) HU_VERSION(x, y, z) <= v

#define HU_CLANG_VERSION 0
#define HU_COMPCERT_VERSION 0
#define HU_GCC_VERSION 0
#define HU_GNUC_VERSION 0
#define HU_IBMXL_VERSION 0
#define HU_INTEL_VERSION 0
#define HU_MSVC_VERSION 0

#ifdef __GNUC__
#    undef HU_COMP_GNUC_P
#    define HU_COMP_GNUC_P 1
#    define HU_COMP_GNUC 1
#    undef HU_GNUC_VERSION
#    define HU_GNUC_VERSION                                                    \
        HU_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#endif

#ifdef __clang__
#    undef HU_COMP_CLANG_P
#    define HU_COMP_CLANG_P 1
#    define HU_COMP_CLANG 1
#    undef HU_CLANG_VERSION
#    define HU_CLANG_VERSION                                                   \
        HU_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__INTEL_COMPILER)
#    undef HU_COMP_INTEL_P
#    define HU_COMP_INTEL_P 1
#    define HU_COMP_INTEL 1
#    undef HU_INTEL_VERSION
#    define HU_INTEL_VERSION __INTEL_COMPILER
#elif defined(__GNUC__)
#    undef HU_COMP_GCC_P
#    define HU_COMP_GCC_P 1
#    define HU_COMP_GCC 1
#    undef HU_GCC_VERSION
#    define HU_GCC_VERSION HU_GNUC_VERSION
#endif

#ifdef __COMPCERT__
#    undef HU_COMP_COMPCERT_P
#    define HU_COMP_COMPCERT_P 1
#    define HU_COMP_COMPCERT 1
#    undef HU_COMPCERT_VERSION
#    define HU_COMPCERT_VERSION __COMPCERT_VERSION__
#endif

#ifdef __ibmxl__
#    undef HU_COMP_IBMXL_P
#    define HU_COMP_IBMXL_P 1
#    define HU_COMP_IBMXL 1
#    undef HU_IBMXL_VERSION
#    define HU_IBMXL_VERSION                                                   \
        HU_VERSION(__ibmxl_version__, __ibmxl_release__, __ibmxl_modification__)
#endif

#if defined(_MSC_VER) && !HU_COMP_INTEL_P
#    undef HU_COMP_MSVC_P
#    define HU_COMP_MSVC_P 1
#    define HU_COMP_MSVC 1
#    undef HU_MSVC_VERSION
#    define HU_MSVC_VERSION _MSC_VER
#endif

/* define alias */
#define HU_COMP_GNULIKE_P HU_COMP_GNUC_P

#if HU_COMP_GNUC_P
#    define HU_COMP_GNULIKE 1
#endif

#define HU_CLANG_PREREQ(x, y, z) HU_VERSION_PREREQ(x, y, z, HU_CLANG_VERSION)
#define HU_COMPCERT_PREREQ(x, y) ((x) *100L + (y) <= HU_COMPCERT_VERSION)
#define HU_GCC_PREREQ(x, y, z) HU_VERSION_PREREQ(x, y, z, HU_GCC_VERSION)
#define HU_GNUC_PREREQ(x, y, z) HU_VERSION_PREREQ(x, y, z, HU_GNUC_VERSION)
#define HU_INTEL_PREREQ(x, y) ((x) *100L + (y) <= HU_INTEL_VERSION)
#define HU_IBXML_PREREQ(x, y, z) HU_VERSION_PREREQ(x, y, z, HU_IBMXL_VERSION)
#define HU_MSVC_PREREQ(x, y) ((x) *100L + (y) <= HU_MSVC_VERSION)

#if HU_COMP_MSVC_P && !HU_COMP_INTEL_P
/* FIXME: clang-cl is not 100% "bug-compatible" with the MS preprocessor, we
 * detect it as MS compatible preprocessor for the time beeing */
#    ifdef _MSVC_TRADITIONAL
#        define HU_PP_CONFORMANT_P (!_MSVC_TRADITIONAL)
#    else
#        define HU_PP_CONFORMANT_P 0
#    endif
#else
#    define HU_PP_CONFORMANT_P 1
#endif

#if ((HU_COMP_CLANG_P || HU_COMP_IBMXL_P) + HU_COMP_COMPCERT_P +               \
     HU_COMP_GCC_P + HU_COMP_INTEL_P + HU_COMP_MSVC_P) != 1
#    error "BUG: HU_COMP_*_P not properly detected"
#endif

#endif
