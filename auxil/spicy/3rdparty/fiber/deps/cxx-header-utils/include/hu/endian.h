#ifndef HU_ENDIAN_H
#define HU_ENDIAN_H

#ifndef HU_ARCH_H
#    include <hu/arch.h>
#endif

#define HU_ENDIANNESS_LITTLE 0x0001
#define HU_ENDIANNESS_BIG 0x0100

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ||  \
  (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN)
#    define HU_ENDIANNESS HU_ENDIANNESS_LITTLE
#endif

#if (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) ||     \
  (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) ||                   \
  defined(__BIG_ENDIAN__)
#    define HU_ENDIANNESS HU_ENDIANNESS_BIG
#endif

#ifndef HU_ENDIANNESS
#    if (HU_ARCH_X86_P || HU_ARCH_AVR_P || HU_ARCH_RISCV_P ||                  \
         HU_ARCH_MSP430_P || defined(_WIN32))
#        define HU_ENDIANNESS HU_ENDIANNESS_LITTLE
#    elif HU_ARCH_ARM_P
#        if defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__)
#            define HU_ENDIANNESS HU_ENDIANNESS_BIG
#        elif defined(__ARMEL__) || defined(__THUMBEL__) ||                    \
          defined(__AARCH64EL__)
#            define HU_ENDIANNESS HU_ENDIANNESS_LITTLE
#        endif
#    elif HU_ARCH_MIPS_P
#        if defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSBE__)
#            define HU_ENDIANNESS HU_ENDIANNESS_BIG
#        elif defined(_MIPSEL) || defined(__MIPSEL) || defined(___MIPSEL__)
#            define HU_ENDIANNESS HU_ENDIANNESS_LITTLE
#        endif
#    endif
#endif

#ifndef HU_ENDIANNESS
#    error "BUG: HU_ENDIANNESS not defined"
#endif

#if HU_ENDIANNESS == HU_ENDIANNESS_LITTLE
#    define HU_LITTLE_ENDIAN_P 1
#    define HU_BIG_ENDIAN_P 0
#    define HU_LITTLE_ENDIAN
#elif HU_ENDIANNESS == HU_ENDIANNESS_BIG
#    define HU_LITTLE_ENDIAN_P 0
#    define HU_BIG_ENDIAN_P 1
#    define HU_BIG_ENDIAN 1
#else
#    error "BUG: invalid value for HU_ENDIANNESS"
#endif

#endif
