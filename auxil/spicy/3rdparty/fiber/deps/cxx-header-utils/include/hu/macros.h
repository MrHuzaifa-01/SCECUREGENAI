#ifndef HU_MACROS_H
#define HU_MACROS_H 1

#define HU_CAT_I(x, y) x##y
#define HU_CAT(x, y) HU_CAT_I(x, y)

#define HU_TOSTR_I(x) #x
#define HU_TOSTR(x) HU_TOSTR_I(x)

#endif
