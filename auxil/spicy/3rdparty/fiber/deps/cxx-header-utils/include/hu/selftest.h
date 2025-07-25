#ifndef HU_SELFTEST_H
#define HU_SELFTEST_H

#ifndef HU_HU_H
#    include <hu/hu.h>
#endif

hu_static_assert(HU_SIZEOF_SHORT == sizeof(short));
hu_static_assert(HU_SIZEOF_INT == sizeof(int));
hu_static_assert(HU_SIZEOF_LONG == sizeof(long));
hu_static_assert(HU_SIZEOF_LONG_LONG == sizeof(long long));
hu_static_assert(HU_SIZEOF_LONG_LONG == 8);
hu_static_assert(HU_SIZEOF_PTR * 8 == HU_BITS);
hu_static_assert(HU_SIZEOF_PTR == sizeof(void *));

#if HU_DATA_MODEL_IP16_P
hu_static_assert(HU_SIZEOF_SHORT == 2);
hu_static_assert(HU_SIZEOF_INT == 2);
hu_static_assert(HU_SIZEOF_LONG == 4);
hu_static_assert(HU_SIZEOF_PTR == 2);
#elif HU_DATA_MODEL_LP32_P
hu_static_assert(HU_SIZEOF_SHORT == 2);
hu_static_assert(HU_SIZEOF_INT == 2);
hu_static_assert(HU_SIZEOF_LONG == 4);
hu_static_assert(HU_SIZEOF_PTR == 4);
#elif HU_DATA_MODEL_ILP32_P
hu_static_assert(HU_SIZEOF_SHORT == 2);
hu_static_assert(HU_SIZEOF_INT == 4);
hu_static_assert(HU_SIZEOF_LONG == 4);
hu_static_assert(HU_SIZEOF_PTR == 4);
#elif HU_DATA_MODEL_LLP64_P
hu_static_assert(HU_SIZEOF_SHORT == 2);
hu_static_assert(HU_SIZEOF_INT == 4);
hu_static_assert(HU_SIZEOF_LONG == 4);
hu_static_assert(HU_SIZEOF_PTR == 8);
#elif HU_DATA_MODEL_LP64_P
hu_static_assert(HU_SIZEOF_SHORT == 2);
hu_static_assert(HU_SIZEOF_INT == 4);
hu_static_assert(HU_SIZEOF_LONG == 8);
hu_static_assert(HU_SIZEOF_PTR == 8);
#elif HU_DATA_MODEL_ILP64_P
hu_static_assert(HU_SIZEOF_SHORT == 2);
hu_static_assert(HU_SIZEOF_INT == 8);
hu_static_assert(HU_SIZEOF_LONG == 8);
hu_static_assert(HU_SIZEOF_PTR == 8);
#elif HU_DATA_MODEL_SILP64_P
hu_static_assert(HU_SIZEOF_SHORT == 8);
hu_static_assert(HU_SIZEOF_INT == 8);
hu_static_assert(HU_SIZEOF_LONG == 8);
hu_static_assert(HU_SIZEOF_PTR == 8);
#endif

#if HU_ARCH_X86_P || HU_ARCH_AVR_P || HU_ARCH_RISCV_P
hu_static_assert(HU_LITTLE_ENDIAN_P);
#endif

#if HU_OS_LINUX_P
hu_static_assert(HU_OBJFMT_ELF_P);
#elif HU_OS_APPLE_P
hu_static_assert(HU_OBJFMT_MACHO_P);
#endif

#endif
