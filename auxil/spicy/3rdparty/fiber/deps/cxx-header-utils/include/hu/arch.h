#ifndef HU_ARCH_H
#define HU_ARCH_H

#define HU_ARCH_ARM_P 0
#define HU_ARCH_AVR_P 0
#define HU_ARCH_EMSCRIPTEN_P 0 // emscripten not in wasm mode
#define HU_ARCH_MIPS_P 0
#define HU_ARCH_MSP430_P 0
#define HU_ARCH_PPC_P 0
#define HU_ARCH_RISCV_P 0
#define HU_ARCH_WASM_P 0
#define HU_ARCH_X86_P 0

#if defined(__aarch64__) || defined(_M_ARM64)
#    undef HU_ARCH_ARM_P
#    define HU_ARCH_ARM_P 1
#    ifndef HU_INTERNAL_BITS
#        define HU_INTERNAL_BITS 64
#    endif
#endif

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) ||               \
  defined(_M_ARMT)
#    undef HU_ARCH_ARM_P
#    define HU_ARCH_ARM_P 1
#    ifndef HU_INTERNAL_BITS
#        define HU_INTERNAL_BITS 32
#    endif
#endif

#if HU_ARCH_ARM_P
#    define HU_ARCH_ARM 1
#endif

#if defined(__AVR) || defined(__AVR__) || defined(__AVR_ARCH__)
#    undef HU_ARCH_AVR_P
#    define HU_ARCH_AVR_P 1
#    define HU_ARCH_AVR 1
#endif

#if defined(__wasm32) || defined(__wasm32__)
#    undef HU_ARCH_WASM_P
#    define HU_ARCH_WASM_P 1
#    define HU_ARCH_WASM 1
#    ifndef HU_INTERNAL_BITS
#        define HU_INTERNAL_BITS 32
#    endif
#elif defined(__wasm) || defined(__wasm__)
#    undef HU_ARCH_WASM_P
#    define HU_ARCH_WASM_P 1
#    define HU_ARCH_WASM 1
#elif defined(__EMSCRIPTEN__)
#    undef HU_ARCH_EMSCRIPTEN_P
#    define HU_ARCH_EMSCRIPTEN_P 1
#    define HU_ARCH_EMSCRIPTEN 1
#endif

#if defined(__mips__) || defined(__mips) || defined(__MIPS__) ||               \
  defined(__mips64)
#    undef HU_ARCH_MIPS_P
#    define HU_ARCH_MIPS_P 1
#    define HU_ARCH_MIPS 1
#    ifndef HU_INTERNAL_BITS
#        if (defined(__mips) && __mips == 64) || defined(__mips64)
#            define HU_INTERNAL_BITS 64
#        endif
#    endif
#endif

#if defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) ||    \
  defined(__ppc) || defined(__ppc__) || defined(__ppc64__) ||                  \
  defined(_M_PPC) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) ||             \
  defined(__PPC__) || defined(__PPC64__)
#    undef HU_ARCH_PPC_P
#    define HU_ARCH_PPC_P 1
#    define HU_ARCH_PPC 1
#    ifndef HU_INTERNAL_BITS
#        if defined(__powerpc64__) || defined(__ppc64__) ||                    \
          defined(__PPC64__) || defined(_ARCH_PPC64)
#            define HU_INTERNAL_BITS 64
#        elif defined(__GNUC__)
#            define HU_INTERNAL_BITS 32
#        endif
#    endif
#endif

#if defined(__MSP430__) || defined(__MSP430X__) || defined(__ICCMSP430__)
#    undef HU_ARCH_MSP430_P
#    define HU_ARCH_MSP430_P 1
#    define HU_ARCH_MSP430 1
#    ifndef HU_INTERNAL_BITS
#        define HU_INTERNAL_BITS 16
#    endif
#endif

#if defined(__riscv) || defined(__riscv_xlen)
#    undef HU_ARCH_RISCV_P
#    define HU_ARCH_RISCV_P 1
#    define HU_ARCH_RISCV 1
#    if !defined(HU_INTERNAL_BITS) && defined(__riscv_xlen)
#        define HU_INTERNAL_BITS __riscv_xlen
#    endif
#endif

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_AMD64) ||          \
  defined(_M_X64)
#    undef HU_ARCH_X86_P
#    define HU_ARCH_X86_P 1
#    define HU_INTERNAL_BITS 64
#endif

#if defined(__i386__) || defined(__i386) || defined(_M_IX86) ||                \
  defined(__X86__) || defined(_X86_) || defined(__I86__)
#    undef HU_ARCH_X86_P
#    define HU_ARCH_X86_P 1
#    ifndef HU_INTERNAL_BITS
#        define HU_INTERNAL_BITS 32
#    endif
#endif

#if HU_ARCH_X86_P
#    define HU_ARCH_X86 1
#endif

#if (HU_ARCH_ARM_P + HU_ARCH_AVR_P + HU_ARCH_EMSCRIPTEN_P + HU_ARCH_MIPS_P +   \
     HU_ARCH_MSP430_P + HU_ARCH_PPC_P + HU_ARCH_RISCV_P + HU_ARCH_WASM_P +     \
     HU_ARCH_X86_P) != 1
#    error "BUG: HU_ARCH_*_P not properly defined"
#endif

#endif
