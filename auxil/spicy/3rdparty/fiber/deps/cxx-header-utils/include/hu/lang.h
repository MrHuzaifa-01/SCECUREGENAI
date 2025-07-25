#ifndef HU_LANG_H
#define HU_LANG_H

#define HU_C_P 0
#define HU_C_89_P 0
#define HU_C_99_P 0
#define HU_C_11_P 0
#define HU_C_17_P 0
#define HU_C_18_P HU_C_17_P

#define HU_C_PREREQ(x) 0

#define HU_CXX_P 0
#define HU_CXX_98_P 0
#define HU_CXX_11_P 0
#define HU_CXX_14_P 0
#define HU_CXX_17_P 0
#define HU_CXX_20_P 0

#define HU_CXX_EXCEPTIONS_P 0
#define HU_CXX_RTTI_P 0
#define HU_CXX_PREREQ(x) 0

#define hu_constexpr
#define hu_constexpr14
#define hu_constexpr17
#define hu_constexpr20
#define hu_explicit
#define hu_override
#define hu_final
#define hu_noexcept
#define hu_noexcept1(x)

#if defined(__ASSEMBLER__) /* GNUC */ ||                                       \
  defined(__ASSEMBLY__) /* Linux kernel */
#    define HU_ASM_P 1
#    define HU_ASM 1
#else
#    define HU_ASM_P 0
#endif

#if defined(__STDC__) || defined(__STDC_VERSION__)

#    ifndef __cplusplus
#        define HU_BEGIN_EXTERN_C
#        define HU_END_EXTERN_C
#        define HU_EXTERN_C

#        if defined(__clang__) ||                                              \
          (defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ >= 403))
#            define hu_c_implicit_cast_(T, x, cnt)                             \
                __extension__({                                                \
                    T hu_c_implicit_cast_val##cnt = (x);                       \
                    hu_c_implicit_cast_val##cnt;                               \
                })
#            define hu_c_implicit_cast(T, x)                                   \
                hu_c_implicit_cast_(T, x, __COUNTER__)
#        else
#            define hu_c_implicit_cast(T, x) (x)
#        endif

#        define hu_cxx_static_cast(T, x) hu_c_implicit_cast(T, x)
#        define hu_cxx_const_cast(T, x) hu_c_implicit_cast(T, x)
#        define hu_cxx_reinterpret_cast(T, x) hu_c_implicit_cast(T, x)
#        define hu_cxx_cast(T, x) hu_c_implicit_cast(T, x)
#        define hu_static_cast(T, x) ((T)(x))
#        define hu_const_cast(T, x) ((T)(x))
#        define hu_reinterpret_cast(T, x) ((T)(x))
#    endif

#    undef HU_C_P
#    define HU_C_P 1
#    define HU_C 1
#    undef HU_C_89_P
#    define HU_C_89_P 1
#    define HU_C_89 1

#    ifdef __STDC_VERSION__
#        define HU_STDC_VERSION __STDC_VERSION__
#    else
#        define HU_STDC_VERSION 198901L
#    endif

#    undef HU_C_PREREQ
#    define HU_C_PREREQ(x)                                                     \
        (HU_STDC_VERSION >= ((x) + ((x) < 80L) * 100L + 1900L) * 100L)

#    if HU_C_PREREQ(99)
#        undef HU_C_99_P
#        define HU_C_99_P 1
#        define HU_C_99 1
#    endif

#    if HU_C_PREREQ(11)
#        undef HU_C_11_P
#        define HU_C_11_P 1
#        define HU_C_11 1
#    endif

#    if HU_C_PREREQ(17)
#        undef HU_C_17_P
#        define HU_C_17_P 1
#        define HU_C_17 1
#        define HU_C_18 1
#    endif

#endif /* defined(__STDC__) || defined(__STDC_VERSION__)  */

#ifdef __cplusplus

#    define HU_BEGIN_EXTERN_C extern "C" {
#    define HU_END_EXTERN_C }
#    define HU_EXTERN_C extern "C"

#    define hu_cxx_static_cast(T, x) static_cast<T>(x)
#    define hu_cxx_const_cast(T, x) const_cast<T>(x)
#    define hu_cxx_reinterpret_cast(T, x) reinterpret_cast<T>(x)
#    define hu_cxx_cast(T, x) ((T)(x))
#    define hu_static_cast hu_cxx_static_cast
#    define hu_const_cast hu_cxx_const_cast
#    define hu_reinterpret_cast hu_cxx_reinterpret_cast

#    undef HU_CXX_P
#    define HU_CXX_P 1
#    define HU_CXX 1

#    undef HU_CXX_98_P
#    define HU_CXX_98_P 1

#    if defined(__cpp_exceptions) || defined(__EXCEPTIONS) ||                  \
      defined(_CPPUNWIND)
#        undef HU_CXX_EXCEPTIONS_P
#        define HU_CXX_EXCEPTIONS_P 1
#        define HU_CXX_EXCEPTIONS 1
#    endif

#    if defined(__cpp_rtti) || defined(__GXX_RTTI) || defined(_CPPRTTI) ||     \
      defined(__INTEL_RTTI__)
#        undef HU_CXX_RTTI_P
#        define HU_CXX_RTTI_P 1
#        define HU_CXX_RTTI 1
#    endif

#    undef HU_CXX_PREREQ
#    define HU_CXX_PREREQ(x)                                                   \
        (__cplusplus >= ((x) + ((x) < 80L) * 100L + 1900L) * 100L)

#    if defined(__cpp_rtti) || defined(__cpp_exceptions) ||                    \
      defined(__cpp_attributes) || defined(__cpp_constexpr) ||                 \
      defined(__cpp_static_assert) || defined(__cpp_lambdas)
#        define HU_CXX_FEATURE_TESTS_P 1
#        define HU_CXX_FEATURE_TESTS 1
#    else
#        define HU_CXX_FEATURE_TESTS_P 0
#    endif

#    ifdef __cpp_constexpr
#        define HU_CPP_CONSTEXPR __cpp_constexpr
#    else
#        define HU_CPP_CONSTEXPR 0L
#    endif

#    if HU_CXX_PREREQ(11)
#        undef HU_CXX_11_P
#        define HU_CXX_11_P 1
#        define HU_CXX_11 1
#    endif

#    if HU_CXX_PREREQ(14)
#        undef HU_CXX_14_P
#        define HU_CXX_14_P 1
#        define HU_CXX_14 1
#    endif

#    if HU_CXX_PREREQ(17)
#        undef HU_CXX_17_P
#        define HU_CXX_17_P 1
#        define HU_CXX_17 1
#    endif

#    if HU_CXX_PREREQ(20)
#        undef HU_CXX_20_P
#        define HU_CXX_20_P 1
#        define HU_CXX_20 1
#    endif

#    if HU_CXX_PREREQ(11)
#        undef hu_constexpr
#        define hu_constexpr constexpr
#        undef hu_explicit
#        undef hu_override
#        undef hu_final
#        undef hu_noexcept
#        undef hu_noexcept1
#        define hu_explicit explicit
#        define hu_override override
#        define hu_final final
#        define hu_noexcept noexcept
#        define hu_noexcept1 noexcept
#    endif

#    if HU_CXX_PREREQ(14) || HU_CPP_CONSTEXPR >= 201304L
#        undef hu_constexpr14
#        define hu_constexpr14 constexpr
#    endif

#    if HU_CXX_PREREQ(17) || HU_CPP_CONSTEXPR >= 201603L
#        undef hu_constexpr17
#        define hu_constexpr17 constexpr
#    endif

#    if HU_CXX_PREREQ(20)
#        undef hu_constexpr20
#        define hu_constexpr20 constexpr
#    endif

#endif /* __cplusplus */

#define HU_CLIKE_P (HU_C_P || HU_CXX_P)

#endif
