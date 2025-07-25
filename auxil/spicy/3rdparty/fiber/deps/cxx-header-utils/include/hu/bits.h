#ifndef HU_BITS_H
#define HU_BITS_H

#ifndef HU_ARCH_H
#    include <hu/arch.h>
#endif

#define HU_DATA_MODEL_IP16_P 0
#define HU_DATA_MODEL_LP32_P 0
#define HU_DATA_MODEL_ILP32_P 0
#define HU_DATA_MODEL_LLP64_P 0
#define HU_DATA_MODEL_LP64_P 0
#define HU_DATA_MODEL_ILP64_P 0
#define HU_DATA_MODEL_SILP64_P 0

#if defined(_IP16) || defined(__IP16__)
#    undef HU_DATA_MODEL_IP16_P
#    define HU_DATA_MODEL_IP16_P 1
#    define HU_SIZEOF_SHORT 2
#    define HU_SIZEOF_INT 2
#    define HU_SIZEOF_LONG 4
#    define HU_SIZEOF_PTR 2
#endif

#if defined(_LP32) || defined(__LP32__)
#    undef HU_DATA_MODEL_LP32_P
#    define HU_DATA_MODEL_LP32_P 1
#    define HU_SIZEOF_SHORT 2
#    define HU_SIZEOF_INT 2
#    define HU_SIZEOF_LONG 4
#    define HU_SIZEOF_PTR 4
#endif

#if defined(_ILP32) || defined(__ILP32__) ||                                   \
  (defined(_WIN32) && !defined(_WIN64))
#    undef HU_DATA_MODEL_ILP32_P
#    define HU_DATA_MODEL_ILP32_P 1
#    define HU_SIZEOF_SHORT 2
#    define HU_SIZEOF_INT 4
#    define HU_SIZEOF_LONG 4
#    define HU_SIZEOF_PTR 4
#endif

#if defined(_LLP64) || defined(__LLP64__) ||                                   \
  (defined(_WIN32) && defined(_WIN64))
#    undef HU_DATA_MODEL_LLP64_P
#    define HU_DATA_MODEL_LLP64_P 1
#    define HU_SIZEOF_SHORT 2
#    define HU_SIZEOF_INT 4
#    define HU_SIZEOF_LONG 4
#    define HU_SIZEOF_PTR 8
#endif

#if defined(_LP64) || defined(__LP64__)
#    undef HU_DATA_MODEL_LP64_P
#    define HU_DATA_MODEL_LP64_P 1
#    define HU_SIZEOF_SHORT 2
#    define HU_SIZEOF_INT 4
#    define HU_SIZEOF_LONG 8
#    define HU_SIZEOF_PTR 8
#endif

#if defined(_ILP64) || defined(__ILP64__)
#    undef HU_DATA_MODEL_ILP64_P
#    define HU_DATA_MODEL_ILP64_P 1
#    define HU_SIZEOF_SHORT 2
#    define HU_SIZEOF_INT 8
#    define HU_SIZEOF_LONG 8
#    define HU_SIZEOF_PTR 8
#endif

#if defined(_SILP64) || defined(__SILP64__)
#    undef HU_DATA_MODEL_SILP64_P
#    define HU_DATA_MODEL_SILP64_P 1
#    define HU_SIZEOF_SHORT 8
#    define HU_SIZEOF_INT 8
#    define HU_SIZEOF_LONG 8
#    define HU_SIZEOF_PTR 8
#endif

#if !defined(HU_SIZEOF_PTR) && defined(__SIZEOF_SHORT__) &&                    \
  defined(__SIZEOF_INT__) && defined(__SIZEOF_LONG__) &&                       \
  defined(__SIZEOF_POINTER__)
#    define HU_SIZEOF_SHORT __SIZEOF_SHORT__
#    define HU_SIZEOF_INT __SIZEOF_INT__
#    define HU_SIZEOF_LONG __SIZEOF_LONG__
#    define HU_SIZEOF_PTR __SIZEOF_POINTER__

#    if __SIZEOF_POINTER__ == 2
#        undef HU_DATA_MODEL_IP16_P
#        define HU_DATA_MODEL_IP16_P 1
#    elif __SIZEOF_POINTER__ == 4
#        if __SIZEOF_INT__ == 2
#            undef HU_DATA_MODEL_LP32_P
#            define HU_DATA_MODEL_LP32_P 1
#        else
#            undef HU_DATA_MODEL_ILP32_P
#            define HU_DATA_MODEL_ILP32_P 1
#        endif
#    else
#        if __SIZEOF_LONG__ == 4
#            undef HU_DATA_MODEL_LLP64_P
#            define HU_DATA_MODEL_LLP64_P 1
#        elif __SIZEOF_INT__ == 4
#            undef HU_DATA_MODEL_LP64_P
#            define HU_DATA_MODEL_LP64_P 1
#        elif __SIZEOF_SHORT__ == 2
#            undef HU_DATA_MODEL_ILP64_P
#            define HU_DATA_MODEL_ILP64_P 1
#        else
#            undef HU_DATA_MODEL_SILP64_P
#            define HU_DATA_MODEL_SILP64_P 1
#        endif
#    endif
#endif

#define HU_SIZEOF_POINTER HU_SIZEOF_PTR
#define HU_SIZEOF_LONG_LONG 8

#ifndef HU_INTERNAL_BITS
#    define HU_INTERNAL_BITS (HU_SIZEOF_PTR * 8)
#endif

#define HU_BITS HU_INTERNAL_BITS

#if HU_BITS == 16
#    define HU_BITS_16 1
#    define HU_BITS_32_P 0
#    define HU_BITS_64_P 0
#    define HU_BITS_16_P 1
#elif HU_BITS == 32
#    define HU_BITS_16_P 0
#    define HU_BITS_32_P 1
#    define HU_BITS_64_P 0
#    define HU_BITS_32 1
#elif HU_BITS == 64
#    define HU_BITS_16_P 0
#    define HU_BITS_32_P 0
#    define HU_BITS_64_P 1
#    define HU_BITS_64 1
#else
#    error "BUG: HU_BITS has invalid value"
#endif

#if (HU_DATA_MODEL_IP16_P + HU_DATA_MODEL_LP32_P + HU_DATA_MODEL_ILP32_P +     \
     HU_DATA_MODEL_LLP64_P + HU_DATA_MODEL_LP64_P + HU_DATA_MODEL_ILP64_P +    \
     HU_DATA_MODEL_SILP64_P) != 1
#    error "BUG: HU_DATA_MODEL_*_P not properly detected"
#endif

#endif
