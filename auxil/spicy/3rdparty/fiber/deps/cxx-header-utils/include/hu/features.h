#ifndef HU_FEATURES_H
#define HU_FEATURES_H

/* clang-format off */
/**
 * \def hu_has_builtin(x)
 * Check if the builtin x is provided by the compiler.
 *
 * Example: __has_builtin(__builtin_mul_overflow)
 *
 * Supported by:
 * - Clang since at least 3.0
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-builtin)
 */
/* clang-format on */
#ifndef __has_builtin
#    define HU_HAVE_has_builtin_P 0
#    define hu_has_builtin(x) 0
#else
#    define HU_HAVE_has_builtin_P 1
#    define HU_HAVE_has_builtin 1
#    define hu_has_builtin __has_builtin
#endif

/* clang-format off */
/**
 * \def hu_has_feature(x)
 * Check if the standardised language feature x is available.
 *
 * Examples:
 * - hu_has_feature(cxx_variable_templates)
 * - hu_has_feature(c_alignas)
 * - hu_has_feature(cxx_rtti)
 *
 * Supported by:
 * - Clang since at least 3.0
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-feature-and-has-extension)
 */
/* clang-format on */
#ifndef __has_feature
#    define HU_HAVE_has_feature_P 0
#    define hu_has_feature(x) 0
#else
#    define HU_HAVE_has_feature_P 1
#    define HU_HAVE_has_feature 1
#    define hu_has_feature __has_feature
#endif

/* clang-format off */
/**
 * \def hu_has_extension(x)
 * Check if the language feature/extension x is available.
 * Examples:
 * - hu_has_extension(cxx_variable_templates)
 * - hu_has_extension(c_alignas)
 * - hu_has_extension(cxx_rtti)
 *
 * Supported by:
 * - Clang since at least 3.0
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-feature-and-has-extension)
 */
/* clang-format on */
#ifndef __has_extension
#    define HU_HAVE_has_extension_P 0
#    define hu_has_extension hu_has_feature
#else
#    define HU_HAVE_has_extension_P 1
#    define HU_HAVE_has_extension 1
#    define hu_has_extension __has_extension
#endif

/* clang-format off */
/**
 * \def hu_has_cpp_attribute(x)
 * Check if the C++ attribute [[x]] is supported. Standardised in C++20.
 *
 * Supported by:
 * - Clang since 3.6
 * - GCC since 5.1
 * - MSVC since 1920 / VS2019 v16.0.1
 *
 * Links:
 * - [C++20 Paper P0941](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0941r2.html#recs.hasattr)
 * - [cppreference overview](https://en.cppreference.com/w/User:D41D8CD98F/feature_testing_macros)
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-cpp-attribute)
 * - [GCC documentation](https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005fcpp_005fattribute.html#g_t_005f_005fhas_005fcpp_005fattribute)
 * - [MSVC announcement](https://docs.microsoft.com/en-us/cpp/overview/what-s-new-for-visual-cpp-in-visual-studio?view=vs-2019)
 */
/* clang-format on */
#ifndef __has_cpp_attribute
#    define HU_HAVE_has_cpp_attribute_P 0
#    define hu_has_cpp_attribute(x) 0
#else
#    define HU_HAVE_has_cpp_attribute_P 1
#    define HU_HAVE_has_cpp_attribute 1
#    define hu_has_cpp_attribute __has_cpp_attribute
#endif

/* clang-format off */
/**
 * \def hu_has_c_attribute(x)
 *
 * From the Clang release notes: Detect whether a double square-bracket
 * attribute is supported in C mode. This attribute syntax can be enabled with
 * the -fdouble-square-bracket-attributes flag.
 *
 * Supported by:
 * - Clang since 6.0 in C mode, use the -fdouble-square-bracket-attributes flag.
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-c-attribute)
 */
/* clang-format on */
#ifndef __has_c_attribute
#    define HU_HAVE_has_c_attribute_P 0
#    define hu_has_c_attribute(x) 0
#else
#    define HU_HAVE_has_c_attribute_P 1
#    define HU_HAVE_has_c_attribute 1
#    define hu_has_c_attribute __has_c_attribute
#endif

/* clang-format off */
/**
 * \def hu_has_attribute(x)
 *
 * Check if a the gnu style attribute x is supported.
 *
 * Examples:
 * - hu_has_attribute(returns_nonnull)
 * - hu_has_attribute(__noreturn__)
 *
 * Supported by:
 * - Clang since at least 3.0, fixed in 3.5 to be target aware.
 * - GCC since 5.1
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-include-next)
 * - [GCC documentation](https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005fattribute.html)
 */
/* clang-format on */

#ifndef __has_attribute
#    define HU_HAVE_has_attribute_P 0
#    define hu_has_attribute(x) 0
#else
#    define HU_HAVE_has_attribute_P 1
#    define HU_HAVE_has_attribute 1
#    define hu_has_attribute __has_attribute
#endif

/* clang-format off */
/**
 * \def hu_has_declspec_attribute(x)
 *
 * Check if a the MSVC style __declspec attribute x is supported.
 *
 * Examples:
 * - hu_has_declspec_attribute(restrict)
 * - hu_has_attribute(__dllexport__)
 *
 * Supported by:
 * - Clang since 3.6
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-declspec-attribute)
 */
/* clang-format on */

#ifndef __has_declspec_attribute
#    define HU_HAVE_has_declspec_attribute_P 0
#    define hu_has_declspec_attribute(x) 0
#else
#    define HU_HAVE_has_declspec_attribute_P 1
#    define HU_HAVE_has_declspec_attribute 1
#    define hu_has_declspec_attribute __has_declspec_attribute
#endif

/* clang-format off */
/**
 * \def hu_is_identifier(x)
 *
 * From the Clang documentation: This function-like macro takes a single
 * identifier argument that might be either a reserved word or a regular
 * identifier. It evaluates to 1 if the argument is just a regular identifier
 * and not a reserved word, in the sense that it can then be used as the name of
 * a user-defined function or variable. Otherwise it evaluates to 0.
 *
 * Examples:
 * - hu_is_identifier(_Alignas)
 * - hu_is_identifier(__nullptr)
 *
 * Supported by:
 * - Clang since 3.5
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#is-identifier)
 */
/* clang-format on */

#ifndef __is_identifier
#    define HU_HAVE_has_is_identifier_P 0
#    define hu_is_identifier(x) 0
#else
#    define HU_HAVE_has_is_identifier_P 1
#    define HU_HAVE_has_is_identifier 1
#    define hu_is_identifier __is_identifier
#endif

/* clang-format off */
/**
 * \def hu_has_include(x)
 *
 * From the Clang documentation: This function-like macro takes a single file
 * name string argument that is the name of an include file. It evaluates to 1
 * if the file can be found using the include paths, or 0 otherwise.
 *
 * Examples:
 * - hu_has_include(<filesystem>)
 * - hu_has_include("hu/features.h")
 *
 * Supported by:
 * - Clang since at least 3.0
 * - GCC since 5.1
 * - MSVC since 1911 / VS2017 Update 3 v15.3.3
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-include)
 * - [GCC documentation](https://gcc.gnu.org/onlinedocs/cpp/_005f_005fhas_005finclude.html#g_t_005f_005fhas_005finclude)
 * - [MSVC documentation](https://docs.microsoft.com/en-us/cpp/preprocessor/hash-if-hash-elif-hash-else-and-hash-endif-directives-c-cpp?view=vs-2019#__has_include)
 */
/* clang-format on */

#if defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ == 9
/* GCC 4.9.x is buggy */
#    undef __has_include
#    undef __has_include_next
#endif

#ifndef __has_include
#    define HU_HAVE_has_include_P 0
#    define hu_has_include(x) 0
#    define hu_maybe_has_include(x) 1
#else
#    define HU_HAVE_has_include_P 1
#    define HU_HAVE_has_include 1
#    define hu_has_include __has_include
#    define hu_maybe_has_include __has_include
#endif

/* clang-format off */
/**
 * \def hu_has_include_next(x)
 *
 * From the Clang documentation: This function-like macro takes a single file
 * name string argument that is the name of an include file. It is like
 * __has_include except that it looks for the second instance of the given file
 * found in the include paths. It evaluates to 1 if the second instance of the
 * file can be found using the include paths, or 0 otherwise.
 *
 * Examples:
 * - hu_has_include_next(<filesystem>)
 * - hu_has_include_next("hu/features.h")
 *
 * Supported by:
 * - Clang since at least 3.0
 * - GCC since 5.1
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-include-next)
 * - [GCC 5 release announcement](https://gcc.gnu.org/gcc-5/changes.html)
 */
/* clang-format on */

#ifndef __has_include_next
#    define HU_HAVE_has_include_next_P 0
#    define hu_has_include_next(x) 0
#else
#    define HU_HAVE_has_include_next_P 1
#    define HU_HAVE_has_include_next 1
#    define hu_has_include_next __has_include_next
#endif

/* clang-format off */
/**
 * \def hu_has_warning(x)
 *
 * Check if the compiler understands the warning flag x. It does not check
 * wether the warning is enabled or not.
 *
 * Examples:
 * - hu_has_warning("-Wimplicit-fallthrough")
 * - hu_has_warning("-Wconversion")
 *
 * Supported by:
 * - Clang since at least 3.0
 *
 * Links:
 * - [Clang documentation](https://clang.llvm.org/docs/LanguageExtensions.html#has-warning)
 */
/* clang-format on */

#ifndef __has_warning
#    define HU_HAVE_has_warning_P 0
#    define hu_has_warning(x) 0
#else
#    define HU_HAVE_has_warning_P 1
#    define HU_HAVE_has_warning 1
#    define hu_has_warning __has_warning
#endif

#endif
