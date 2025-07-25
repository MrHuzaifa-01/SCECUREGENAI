#ifndef HU_ANNOTATIONS_H
#define HU_ANNOTATIONS_H

#ifndef HU_COMPILER_H
#    include <hu/compiler.h>
#endif
#ifndef HU_FEATURES_H
#    include <hu/features.h>
#endif
#ifndef HU_LANG_H
#    include <hu/lang.h>
#endif
#ifndef HU_MACROS_H
#    include <hu/macros.h>
#endif

#if HU_COMP_MSVC_P
#    include <Sal.h>
#endif

#define HU_BOOL_CONTEXT(p) (!!(p))

#if HU_CXX_P
#    define HU_BOOL_TRUE true
#    define HU_BOOL_FALSE false
#else
#    define HU_BOOL_TRUE 1
#    define HU_BOOL_FALSE 0
#endif

#if HU_C_99_P || HU_CXX_11_P || HU_COMP_GNUC_P
#    define HU_HAVE_PRAGMA_P 1
#    define HU_HAVE_PRAGMA 1
#    define HU_PRAGMA _Pragma
#else
#    define HU_HAVE_PRAGMA_P 0
#    define HU_PRAGMA(x)
#endif

#if HU_HAVE_PRAGMA_P && HU_COMP_GNUC_P
#    define HU_HAVE_PRAGMA_GCC_P 1
#    define HU_HAVE_PRAGMA_GCC 1
#    define HU_PRAGMA_GCC(x) HU_PRAGMA(HU_TOSTR(GCC x))
#else
#    define HU_HAVE_PRAGMA_GCC_P 0
#    define HU_PRAGMA_GCC(x)
#endif

#if HU_HAVE_PRAGMA_P && HU_COMP_CLANG_P
#    define HU_HAVE_PRAGMA_CLANG_P 1
#    define HU_HAVE_PRAGMA_CLANG 1
#    define HU_PRAGMA_CLANG(x) HU_PRAGMA(HU_TOSTR(clang x))
#else
#    define HU_HAVE_PRAGMA_CLANG_P 0
#    define HU_PRAGMA_CLANG(x)
#endif

#if HU_HAVE_PRAGMA_P && HU_COMP_INTEL_P
#    define HU_HAVE_PRAGMA_INTEL_P 1
#    define HU_HAVE_PRAGMA_INTEL 1
#    define HU_PRAGMA_INTEL(x) HU_PRAGMA(HU_TOSTR(x))
#else
#    define HU_HAVE_PRAGMA_INTEL_P 0
#    define HU_PRAGMA_INTEL(x)
#endif

#if HU_COMP_MSVC_P
#    define HU_HAVE_PRAGMA_MSVC_P 1
#    define HU_HAVE_PRAGMA_MSVC 1
#    define HU_PRAGMA_MSVC(...) __pragma(__VA_ARGS__)
#else
#    define HU_HAVE_PRAGMA_MSVC_P 0
#    define HU_PRAGMA_MSVC(...)
#endif

#if HU_COMP_GNUC_P
#    define HU_HAVE_GNU_ATTR_P 1
#    define HU_HAVE_GNU_ATTR 1
#    if HU_CXX_11_P || defined(__cpp_attributes)
#        define HU_GNU_ATTR(attr) [[gnu::attr]]
#        define HU_GNU_LEGACY_ATTR(attr) __attribute__((attr))
#    else
#        define HU_GNU_ATTR(attr) __attribute__((attr))
#    endif
#    define HU_GNU_LEGACY_ATTR(attr) __attribute__((attr))
#else
#    define HU_HAVE_GNU_ATTR_P 0
#    define HU_GNU_ATTR(...)
#    define HU_GNU_LEGACY_ATTR(...)
#endif

#if HU_COMP_CLANG_P
#    define HU_HAVE_CLANG_ATTR_P 1
#    define HU_HAVE_CLANG_ATTR 1
#    if HU_CXX_11_P || defined(__cpp_attributes)
#        define HU_CLANG_ATTR(attr) [[clang::attr]]
#    else
#        define HU_CLANG_ATTR(attr) __attribute__((attr))
#    endif
#    define HU_CLANG_LEGACY_ATTR(attr) __attribute__((attr))
#else
#    define HU_HAVE_CLANG_ATTR_P 0
#    define HU_CLANG_ATTR(...)
#    define HU_CLANG_LEGACY_ATTR(...)
#endif

#ifdef __COUNTER__
#    define HU_HAVE_CPP_COUNTER_P 1
#    define HU_HAVE_CPP_COUNTER 1
#    define HU_CAT_COUNTER(x) HU_CAT(x, __COUNTER__)
#else
#    define HU_HAVE_CPP_COUNTER_P 0
#    define HU_CAT_COUNTER(x) HU_CAT(x, __LINE__)
#endif

#if HU_COMP_MSVC_P
#    define HU_NOOP __noop
#else
#    define HU_NOOP(...) ((void) 0)
#endif

/* C++ Attributes */

#if HU_CXX_11_P
#    define HU_HAVE_NORETURN_P 1
#    define HU_NORETURN [[noreturn]]
#else
#    if HU_COMP_GNUC_P || hu_has_attribute(noreturn)
#        define HU_HAVE_NORETURN_P 1
#        define HU_NORETURN HU_GNU_LEGACY_ATTR(noreturn)
#    elif HU_COMP_MSVC_P || hu_has_declspec_attribute(noreturn)
#        define HU_HAVE_NORETURN_P 1
#        define HU_NORETURN __declspec(noreturn)
#    elif HU_C_11_P
#        define HU_HAVE_NORETURN_P 1
#        define HU_NORETURN _Noreturn
#    else
#        define HU_HAVE_NORETURN_P 0
#        define HU_NORETURN
#    endif
#endif

#if HU_CXX_14_P || hu_has_cpp_attribute(deprecated)
#    define HU_HAVE_DEPRECATED_P 1
#    define HU_DEPRECATED_NOMSG [[deprecated]]
#    define HU_DEPRECATED(msg) [[deprecated(msg)]]
#else
#    if HU_COMP_GNUC_P || hu_has_attribute(deprecated)
#        define HU_HAVE_DEPRECATED_P 1
#        define HU_DEPRECATED_NOMSG HU_GNU_LEGACY_ATTR(deprecated)
#        define HU_DEPRECATED(msg) HU_GNU_LEGACY_ATTR(deprecated(msg))
#    elif HU_COMP_MSVC_P || hu_has_declspec_attribute(deprecated)
#        define HU_HAVE_DEPRECATED_P 1
#        define HU_DEPRECATED_NOMSG __declspec(deprecated)
#        define HU_DEPRECATED(msg) __declspec(deprecated(msg))
#    else
#        define HU_HAVE_DEPRECATED_P 0
#        define HU_DEPRECATED
#    endif
#endif

#if HU_COMP_GNUC_P
#    define HU_HAVE_FORCE_INLINE_P 1
#    define HU_HAVE_FORCE_INLINE 1
#    define HU_FORCE_INLINE HU_GNU_LEGACY_ATTR(always_inline)
#elif HU_COMP_MSVC_P
#    define HU_HAVE_FORCE_INLINE_P 1
#    define HU_HAVE_FORCE_INLINE 1
#    define HU_FORCE_INLINE __forceinline
#else
#    define HU_FORCE_INLINE
#    define HU_HAVE_FORCE_INLINE_P 0
#endif

#if HU_COMP_GNUC_P
#    define HU_HAVE_NOINLINE_P 1
#    define HU_HAVE_NOINLINE 1
#    define HU_NOINLINE HU_GNU_LEGACY_ATTR(noinline)
#elif HU_COMP_MSVC_P || hu_has_declspec_attribute(noinline)
#    define HU_HAVE_NOINLINE_P 1
#    define HU_HAVE_NOINLINE 1
#    define HU_NOINLINE __declspec(noinline)
#else
#    define HU_HAVE_NOINLINE_P 0
#    define HU_NOINLINE
#endif

#if HU_HAVE_NORETURN_P
#    define HU_HAVE_NORETURN 1
#endif

#if HU_CXX_17_P || hu_has_cpp_attribute(maybe_unused)
#    define HU_HAVE_MAYBE_UNUSED_P 1
#    define HU_HAVE_MAYBE_UNUSED 1
#    define HU_MAYBE_UNUSED [[maybe_unused]]
#elif HU_COMP_GNUC_P || hu_has_attribute(unused)
#    define HU_HAVE_MAYBE_UNUSED_P 1
#    define HU_HAVE_MAYBE_UNUSED 1
#    define HU_MAYBE_UNUSED HU_GNU_LEGACY_ATTR(unused)
#else
#    define HU_HAVE_MAYBE_UNUSED_P 1
#    define HU_MAYBE_UNUSED
#endif

#if HU_CXX_17_P || hu_has_cpp_attribute(nodiscard)
#    define HU_HAVE_NODISCARD_P 1
#    define HU_NODISCARD [[nodiscard]]
#else
#    if HU_COMP_CLANG_P && hu_has_attribute(warn_unused_result)
#        define HU_HAVE_NODISCARD_P 1
#        define HU_NODISCARD HU_GNU_LEGACY_ATTR(warn_unused_result)
#    elif HU_COMP_GCC_P || HU_COMP_INTEL_P
/* dont define HU_NODISCARD for GCC, since it cant be silenced via a (void) cast
 */
#        define HU_HAVE_NODISCARD_P 0
#        define HU_NODISCARD
#    elif HU_COMP_MSVC_P && _MSC_VER >= 1700
#        define HU_HAVE_NODISCARD_P 1
#        define HU_NODISCARD _Check_return_
#    endif
#endif

#if HU_COMP_GNUC_P
#    define HU_HAVE_WARN_UNUSED_P 1
#    define HU_HAVE_WARN_UNUSED 1
#    define HU_WARN_UNUSED HU_GNU_LEGACY_ATTR(warn_unused_result)
#elif defined(HU_NODISCARD)
#    define HU_HAVE_WARN_UNUSED_P 1
#    define HU_HAVE_WARN_UNUSED 1
#    define HU_WARN_UNUSED HU_NODISCARD
#else
#    define HU_HAVE_WARN_UNUSED_P 0
#endif

#ifndef HU_NODISCARD
#    define HU_HAVE_NODISCARD_P HU_HAVE_WARN_UNUSED_P
#    if HU_HAVE_NODISCARD_P
#        define HU_HAVE_NODISCARD 1
#    endif
#    define HU_NODISCARD HU_WARN_UNUSED
#endif

#if HU_HAVE_NODISCARD_P
#    define HU_HAVE_NODISCARD 1
#endif

#if HU_CXX_P || HU_C_99_P
#    define HU_HAVE_INLINE_P 1
#    define HU_HAVE_INLINE 1
#    define HU_INLINE inline
#elif HU_COMP_GNUC_P || HU_COMP_MSVC_P
#    define HU_HAVE_INLINE_P 1
#    define HU_HAVE_INLINE 1
#    define HU_INLINE __inline
#else
#    define HU_HAVE_INLINE_P 0
#    define HU_INLINE
#endif

#if HU_COMP_MSVC_P
#    define HU_EXTRA_INLINE
#else
#    define HU_EXTRA_INLINE inline
#endif

#if HU_HAVE_DEPRECATED_P
#    define HU_HAVE_DEPRECATED 1
#endif

#if hu_has_attribute(used) || HU_COMP_GNUC_P
#    define HU_HAVE_KEEP_SYMBOL_P 1
#    define HU_HAVE_KEEP_SYMBOL 1
#    define HU_KEEP_SYMBOL HU_GNU_LEGACY_ATTR(used)
#else
#    define HU_HAVE_KEEP_SYMBOL_P 0
#    define HU_KEEP_SYMBOL
#endif

#if hu_has_attribute(cold) || HU_COMP_GNUC_P
#    define HU_HAVE_MARK_COLD_P 1
#    define HU_HAVE_MARK_COLD 1
#    define HU_MARK_COLD HU_GNU_LEGACY_ATTR(cold)
#else
#    define HU_HAVE_MARK_COLD_P 0
#    define HU_MARK_COLD
#endif

#if hu_has_attribute(hot) || HU_COMP_GNUC_P
#    define HU_HAVE_MARK_HOT_P 1
#    define HU_HAVE_MARK_HOT 1
#    define HU_MARK_HOT HU_GNU_LEGACY_ATTR(hot)
#else
#    define HU_HAVE_MARK_HOT_P 0
#    define HU_MARK_HOT
#endif

#if hu_has_attribute(artificial) || HU_GNUC_PREREQ(4, 3, 0)
#    define HU_HAVE_ARTIFICIAL_P 1
#    define HU_HAVE_ARTIFICIAL 1
#    define HU_ARTIFICIAL HU_GNU_LEGACY_ATTR(artificial)
#elif hu_has_attribute(nodebug)
#    define HU_HAVE_ARTIFICIAL_P 1
#    define HU_HAVE_ARTIFICIAL 1
#    define HU_ARTIFICIAL HU_GNU_LEGACY_ATTR(nodebug)
#else
#    define HU_HAVE_ARTIFICIAL_P 0
#    define HU_ARTIFICIAL
#endif

#if HU_COMP_GNUC_P
#    define HU_MACROLIKE_SPEC_(LINKAGE)                                        \
        HU_ARTIFICIAL HU_FORCE_INLINE HU_GNU_LEGACY_ATTR(gnu_inline) LINKAGE
#    define HU_MACROLIKE_FN HU_MACROLIKE_SPEC_(extern __inline)
#    if HU_CXX_P
#        define HU_MACROLIKE HU_MACROLIKE_SPEC_(inline)
#    else
#        define HU_MACROLIKE HU_MACROLIKE_FN
#    endif
#else
#    define HU_MACROLIKE HU_FORCE_INLINE HU_EXTRA_INLINE
#    define HU_MACROLIKE_FN HU_MACROLIKE
#endif

#if hu_has_attribute(returns_nonnull) ||                                       \
  (HU_GNUC_PREREQ(4, 9, 0) && !HU_COMP_INTEL_P)
#    define HU_HAVE_RETURNS_NONNULL_P 1
#    define HU_HAVE_RETURNS_NONNULL 1
#    define HU_RETURNS_NONNULL HU_GNU_LEGACY_ATTR(returns_nonnull)
#elif HU_COMP_MSVC_P
#    define HU_HAVE_RETURNS_NONNULL_P 1
#    define HU_HAVE_RETURNS_NONNULL 1
#    define HU_RETURNS_NONNULL _Ret_notnull_
#else
#    define HU_HAVE_RETURNS_NONNULL_P 0
#    define HU_RETURNS_NONNULL
#endif

#if HU_COMP_GNUC_P
#    define HU_HAVE_RETURNS_NOALIAS_P 1
#    define HU_HAVE_RETURNS_NOALIAS 1
#    define HU_RETURNS_NOALIAS HU_GNU_LEGACY_ATTR(malloc)
#elif HU_COMP_MSVC_P
#    define HU_HAVE_RETURNS_NOALIAS_P 1
#    define HU_HAVE_RETURNS_NOALIAS 1
#    define HU_RETURNS_NOALIAS __declspec(restrict)
#else
#    define HU_HAVE_RETURNS_NOALIAS_P 0
#    define HU_RETURNS_NOALIAS
#endif

#if HU_COMP_MSVC_P
#    define HU_HAVE_INOUT_NONNULL_P 1
#    define HU_HAVE_INOUT_NONNULL 1
#    define HU_IN_NONNULL _In_
#    define HU_OUT_NONNULL _Out_
#    define HU_INOUT_NONNULL _Inout_
#elif HU_CLANG_PREREQ(3, 5, 0)
#    define HU_HAVE_INOUT_NONNULL_P 1
#    define HU_HAVE_INOUT_NONNULL 1
#    define HU_IN_NONNULL HU_GNU_LEGACY_ATTR(nonnull)
#    define HU_OUT_NONNULL HU_GNU_LEGACY_ATTR(nonnull)
#    define HU_INOUT_NONNULL HU_GNU_LEGACY_ATTR(nonnull)
#else
#    define HU_HAVE_INOUT_NONNULL_P 0
#    define HU_IN_NONNULL
#    define HU_OUT_NONNULL
#    define HU_INOUT_NONNULL
#endif

#if HU_COMP_GNUC_P || hu_has_attribute(nonnull)
#    define HU_HAVE_NONNULL_PARAMS_P 1
#    define HU_HAVE_NONNULL_PARAMS 1
#    define HU_NONNULL_PARAMS(...) HU_GNU_LEGACY_ATTR(nonnull(__VA_ARGS__))
#else
#    define HU_HAVE_NONNULL_PARAMS_P 0
#    define HU_NONNULL_PARAMS(...)
#endif

#if HU_CXX_PREREQ(11)
#    define HU_ALIGNOF(T) alignof(T)
#    define HU_ALIGNAS(T) alignas(T)
#    define HU_ALIGN(n) alignas(n)
#elif HU_C_PREREQ(11)
#    define HU_ALIGNOF(T) _Alignof(T)
#    define HU_ALIGNAS(T) _Alignas(T)
#    define HU_ALIGN(n) _Alignas(n)
#elif HU_COMP_GNUC_P
#    define HU_ALIGNOF(T) __alignof__(T)
#    define HU_ALIGNAS(T) HU_GNU_LEGACY_ATTR(__aligned__(__alignof__(T)))
#    define HU_ALIGN(n) HU_GNU_LEGACY_ATTR(__aligned__(n))
#elif HU_COMP_MSVC_P
#    define HU_ALIGNOF(T) __alignof(T)
#    define HU_ALIGNAS(T) __declspec(align(__alignof(T)))
#    define HU_ALIGN(n) __declspec(align(n))
#endif

#ifdef HU_ALIGNOF
#    define HU_HAVE_ALIGNOF_P 1
#    define HU_HAVE_ALIGNOF 1
#    define HU_HAVE_ALIGNAS_P 1
#    define HU_HAVE_ALIGNAS 1
#    define HU_HAVE_ALIGN_P 1
#    define HU_HAVE_ALIGN 1
#else
#    define HU_HAVE_ALIGNOF_P 0
#    define HU_HAVE_ALIGNAS_P 0
#    define HU_HAVE_ALIGN_P 0
#    define HU_ALIGNAS(T)
#    define HU_ALIGN(n)
#endif

#if HU_COMP_GNUC_P
#    define HU_HAVE_RESTRICT_P 1
#    define HU_RESTRICT __restrict__
#elif HU_COMP_MSVC_P
#    define HU_HAVE_RESTRICT_P 1
#    define HU_RESTRICT __restrict
#elif HU_C_PREREQ(99)
#    define HU_HAVE_RESTRICT_P 1
#    define HU_RESTRICT restrict
#else
#    define HU_HAVE_RESTRICT_P 0
#    define HU_RESTRICT
#endif

#if HU_HAVE_RESTRICT_P
#    define HU_HAVE_RESTRICT 1
#endif

#if HU_COMP_GNUC_P || hu_has_attribute(packed)
#    define HU_HAVE_PACKED_P 1
#    define HU_BEGIN_PACKED
#    define HU_END_PACKED
#    define HU_PACKED HU_GNU_LEGACY_ATTR(packed)
#elif HU_COMP_MSVC_P
#    define HU_HAVE_PACKED_P 1
#    define HU_BEGIN_PACKED __pragma(pack(push, 1))
#    define HU_END_PACKED __pragma(pack(pop))
#    define HU_PACKED
#else
#    define HU_HAVE_PACKED_P 0
#    define HU_BEGIN_PACKED
#    define HU_END_PACKED
#    define HU_PACKED
#endif

#if HU_HAVE_PACKED_P
#    define HU_HAVE_PACKED 1
#endif

#if HU_COMP_CLANG_P
/* workaround for bug in old versions of clang's __has_attribute */
#    define HU_HAVE_CONST_FN_P 1
#else
#    define HU_HAVE_CONST_FN_P HU_COMP_GNUC_P || hu_has_attribute(pure)
#endif

#if HU_HAVE_CONST_FN_P
#    define HU_HAVE_CONST_FN 1
#    define HU_CONST_FN HU_GNU_LEGACY_ATTR(const)
#else
#    define HU_CONST_FN
#endif

#if HU_COMP_CLANG_P
/* workaround for bug in old versions of clang's __has_attribute */
#    define HU_HAVE_PURE_FN_P 1
#else
#    define HU_HAVE_PURE_FN_P HU_COMP_GNUC_P || hu_has_attribute(pure)
#endif

#if HU_HAVE_PURE_FN_P
#    define HU_HAVE_PURE_FN 1
#    define HU_PURE_FN HU_GNU_LEGACY_ATTR(pure)
#else
#    define HU_PURE_FN
#endif

#if HU_COMP_GNUC_P || hu_has_builtin(__builtin_expect)
#    define hu_likely(x) (__builtin_expect(HU_BOOL_CONTEXT(x), HU_BOOL_TRUE))
#    define hu_unlikely(x) (__builtin_expect(HU_BOOL_CONTEXT(x), HU_BOOL_FALSE))
#    define HU_HAVE_likely_P 1
#    define HU_HAVE_likely 1
#    define HU_HAVE_unlikely_P 1
#    define HU_HAVE_unlikely 1
#else
#    define HU_HAVE_likely_P 0
#    define HU_HAVE_unlikely_P 0
#    define hu_likely(x) (HU_BOOL_CONTEXT(x))
#    define hu_unlikely(x) (HU_BOOL_CONTEXT(x))
#endif

#if HU_COMP_GNUC_P || hu_has_builtin(__builtin_constant_p)
#    define HU_HAVE_constant_p_P 1
#    define HU_HAVE_constant_p 1
#    define hu_constant_p(x) __builtin_constant_p(x)
#else
#    define HU_HAVE_constant_p_P 0
#    define hu_constant_p(x) HU_BOOL_FALSE
#endif

#if HU_COMP_GNUC_P || hu_has_builtin(__builtin_unreachable)
#    define HU_HAVE_assume_unreachable_P 1
#    define hu_assume_unreachable() __builtin_unreachable()
#elif HU_COMP_MSVC_P
#    define HU_HAVE_assume_unreachable_P 1
#    define hu_assume_unreachable() __assume(0)
#else
#    define HU_HAVE_assume_unreachable_P 0
#    define hu_assume_unreachable() HU_NOOP()
#endif

#if HU_HAVE_assume_unreachable_P
#    define HU_HAVE_assume_unreachable 1
#endif

/**
 * \def hu_assume(x)
 * hu_assume_p(x): Communicate to the compiler that the expression `x' will
 * always evaluate to a true value. It is not guaranteed wether the expression x
 * will be evaluated at run time, so the expression may not have any side
 * effects.
 */
#if hu_has_builtin(__builtin_assume)
#    define HU_HAVE_assume_P 1
#    define hu_assume(p) __builtin_assume(HU_BOOL_CONTEXT(p))
#elif HU_COMP_MSVC_P || HU_COMP_INTEL_P
#    define HU_HAVE_assume_P 1
#    define hu_assume(p) __assume(HU_BOOL_CONTEXT(p))
#elif HU_HAVE_assume_unreachable_P
#    define HU_HAVE_assume_P 1
#    define hu_assume(p)                                                       \
        ((void) (hu_unlikely(!(p)) ? (hu_assume_unreachable(), 0) : 0))
#else
#    define HU_HAVE_assume_P 0
#    define hu_assume(p) ((void) (HU_BOOL_FALSE && (p)))
#endif

#if HU_HAVE_assume_P
#    define HU_HAVE_assume 1
#endif

#if HU_CXX_PREREQ(11)
#    define HU_HAVE_declare_auto_P 1
#    define hu_declare_auto(var, expr) auto var = (expr)
#elif HU_C_P && HU_GNUC_PREREQ(4, 9, 0)
#    define HU_HAVE_declare_auto_P 1
#    define hu_declare_auto(var, expr) __auto_type var = (expr)
#elif HU_COMP_GNUC_P
#    define HU_HAVE_declare_auto_P 1
#    define hu_declare_auto(var, expr) __typeof__(expr) var = (expr)
#else
#    define HU_HAVE_declare_auto_P 0
#endif

#if hu_has_builtin(__builtin_assume_aligned) || HU_GNUC_PREREQ(4, 7, 0)
#    define HU_HAVE_assume_aligned_P 1
/* we only support the 2 argument version */
#    define hu_assume_aligned(arg, algn) __builtin_assume_aligned(arg, algn)
#elif HU_COMP_INTEL_P && HU_HAVE_declare_auto_P
#    define HU_HAVE_assume_aligned_P 1
#    define hu_assume_aligned_helper_(var, arg, algn)                          \
        __extension__({                                                        \
            hu_declare_auto(var, arg);                                         \
            __assume_aligned(var, algn);                                       \
            var;                                                               \
        })
#    define hu_assume_aligned(arg, algn)                                       \
        hu_assume_aligned_helper_(                                             \
          HU_CAT_COUNTER(_hu_assume_aligned_tmp), arg, algn)
#elif HU_HAVE_assume_unreachable_P && HU_HAVE_assume_P && HU_COMP_GNUC_P
#    if HU_CXX_11_P || hu_has_include(<cstdint>)
#        include <cstdint>
#        define HU_HAVE_STDINT_H_ 1
#    elif HU_C_99_P || hu_has_include(<stdint.h>)
#        include <stdint.h>
#        define HU_HAVE_STDINT_H_ 1
#    endif
#    ifdef HU_HAVE_STDINT_H_
#        define HU_HAVE_assume_aligned_P 1
#        define hu_assume_aligned_helper_(var, arg, algn)                      \
            __extension__({                                                    \
                hu_declare_auto(var, arg);                                     \
                hu_assume(hu_reinterpret_cast(uintptr_t, var) % (algn) == 0);  \
                var;                                                           \
            })
#        define hu_assume_aligned(arg, algn)                                   \
            hu_assume_aligned_helper_(                                         \
              HU_CAT_COUNTER(_hu_assume_aligned_tmp), arg, algn)
#    endif
#endif

#ifndef HU_HAVE_assume_aligned_P
#    define HU_HAVE_assume_aligned_P 0
#    define hu_assume_aligned(arg, algn) (arg)
#elif HU_HAVE_assume_aligned_P
#    define HU_HAVE_assume_aligned 1
#endif

#if HU_COMP_GNUC_P
#    define HU_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif HU_COMP_MSVC_P
#    define HU_PRETTY_FUNCTION __FUNCSIG__
#else
#    define HU_PRETTY_FUNCTION __func__
#endif

#if HU_CXX_PREREQ(11)
#    define hu_static_assert_msg(p, msg) static_assert(p, msg)
#    if HU_CXX_PREREQ(17)
#        define hu_static_assert(p) static_assert(p)
#    endif
#elif HU_C_PREREQ(11)
#    define hu_static_assert_msg(p, msg) _Static_assert(p, msg)
#    if HU_C_PREREQ(17)
#        define hu_static_assert(p) _Static_assert(p)
#    endif
#elif HU_C_P
#    define hu_static_assert_msg(cond, _msg)                                   \
        typedef char HU_CAT_COUNTER(                                           \
          hu_static_assertion_failed_)[(cond) ? 1 : -1] HU_MAYBE_UNUSED
#else
#    define hu_static_assert_msg(cond, msg)
#endif

#ifndef hu_static_assert
#    define hu_static_assert(cond) hu_static_assert_msg(cond, "")
#endif

#endif
