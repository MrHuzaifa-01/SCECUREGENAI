#!/bin/sh

p=include/hu

cat \
    "$p"/arch.h \
    "$p"/compiler.h \
    "$p"/features.h \
    "$p"/macros.h \
    "$p"/objfmt.h \
    "$p"/os.h \
    "$p"/bits.h \
    "$p"/endian.h \
    "$p"/lang.h \
    "$p"/annotations.h \
    "$p"/lib.h \
    > hu_amalgamated.h
