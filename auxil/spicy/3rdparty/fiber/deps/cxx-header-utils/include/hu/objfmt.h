#ifndef HU_OBJFMT_H
#define HU_OBJFMT_H

#define HU_OBJFMT_ASMJS_P 0 /* emscripten */
#define HU_OBJFMT_COFF_P 0
#define HU_OBJFMT_ELF_P 0
#define HU_OBJFMT_MACHO_P 0
#define HU_OBJFMT_WASM_P 0 /* emscripten */

#ifdef __asmjs__
#    undef HU_OBJFMT_ASMJS_P
#    define HU_OBJFMT_ASMJS_P 1
#    define HU_OBJFMT_ASMJS 1
#endif

#ifdef _WIN32
#    undef HU_OBJFMT_COFF_P
#    define HU_OBJFMT_COFF_P 1
#    define HU_OBJFMT_COFF 1
#endif

#ifdef __ELF__
#    undef HU_OBJFMT_ELF_P
#    define HU_OBJFMT_ELF_P 1
#    define HU_OBJFMT_ELF 1
#endif

#ifdef __MACH__
#    undef HU_OBJFMT_MACHO_P
#    define HU_OBJFMT_MACHO_P 1
#    define HU_OBJFMT_MACHO 1
#endif

#ifdef __wasm__
#    undef HU_OBJFMT_WASM_P
#    define HU_OBJFMT_WASM_P 1
#    define HU_OBJFMT_WASM 1
#endif

#ifdef __GNUC__
#    define HU_GNU_DSO_ATTR(x) __attribute__((x))
#else
#    define HU_GNU_DSO_ATTR(x)
#endif

#if HU_OBJFMT_COFF_P || defined(_MSC_VER)
/* assume this compiler understands declspec, compiler errors are better than
 * strange linker errors */
#    define HU_DSO_EXPORT __declspec(dllexport)
#    define HU_DSO_IMPORT __declspec(dllimport)
#    define HU_DSO_EXPORT_PROTECTED HU_DSO_EXPORT
#    define HU_DSO_IMPORT_PROTECTED HU_DSO_IMPORT
#    define HU_DSO_HIDDEN
#    define HU_DSO_INTERNAL
#elif defined(__GNUC__) && HU_OBJFMT_ELF_P
#    define HU_DSO_EXPORT HU_GNU_DSO_ATTR(visibility("default"))
#    define HU_DSO_IMPORT HU_GNU_DSO_ATTR(visibility("default"))
#    define HU_DSO_EXPORT_PROTECTED HU_GNU_DSO_ATTR(visibility("protected"))
#    define HU_DSO_IMPORT_PROTECTED HU_GNU_DSO_ATTR(visibility("protected"))
#    define HU_DSO_HIDDEN HU_GNU_DSO_ATTR(visibility("hidden"))
#    define HU_DSO_INTERNAL HU_GNU_DSO_ATTR(visibility("internal"))
#elif defined(__GNUC__)
#    define HU_DSO_EXPORT HU_GNU_DSO_ATTR(visibility("default"))
#    define HU_DSO_IMPORT HU_GNU_DSO_ATTR(visibility("default"))
#    define HU_DSO_EXPORT_PROTECTED HU_GNU_DSO_ATTR(visibility("default"))
#    define HU_DSO_IMPORT_PROTECTED HU_GNU_DSO_ATTR(visibility("default"))
#    define HU_DSO_HIDDEN HU_GNU_DSO_ATTR(visibility("hidden"))
#    define HU_DSO_INTERNAL HU_GNU_DSO_ATTR(visibility("hidden"))
#else
#    define HU_DSO_EXPORT
#    define HU_DSO_IMPORT
#    define HU_DSO_EXPORT_PROTECTED
#    define HU_DSO_IMPORT_PROTECTED
#    define HU_DSO_HIDDEN
#    define HU_DSO_INTERNAL
#endif

#if (HU_OBJFMT_ASMJS_P + HU_OBJFMT_ELF_P + HU_OBJFMT_MACHO_P +                 \
     HU_OBJFMT_COFF_P + HU_OBJFMT_WASM_P) != 1
#    error "BUG: HU_OBJFMT_*_P not properly detected"
#endif

#endif
