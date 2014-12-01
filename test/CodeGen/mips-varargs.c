// RUN: %clang -target mips-unknown-linux -O3 -S -o - -emit-llvm %s | FileCheck %s -check-prefix=ALL -check-prefix=O32
// RUN: %clang -target mips64-unknown-linux -O3 -S -o - -emit-llvm -mabi=n32 %s | FileCheck %s -check-prefix=ALL -check-prefix=N32
// RUN: %clang -target mips64-unknown-linux -O3 -S -o - -emit-llvm %s | FileCheck %s -check-prefix=ALL -check-prefix=N64

#include <stdarg.h>

typedef int v4i32 __attribute__ ((__vector_size__ (16)));

int test_v4i32(char *fmt, ...) {
  va_list va;

  va_start(va, fmt);
  v4i32 v = va_arg(va, v4i32);
  va_end(va);

  return v[0];
}

// ALL: define i32 @test_v4i32(i8*{{.*}} %fmt, ...)
// ALL: entry:
//
// O32:   %va = alloca i8*, align [[PTRALIGN:4]]
// N32:   %va = alloca i8*, align [[PTRALIGN:4]]
// N64:   %va = alloca i8*, align [[PTRALIGN:8]]
//
// ALL:   %va1 = bitcast i8** %va to i8*
// ALL:   call void @llvm.va_start(i8* %va1)
// ALL:   %ap.cur = load i8** %va, align [[PTRALIGN]]
//
// O32:   %0 = ptrtoint i8* %ap.cur to [[PTRTYPE:i32]]
// N32:   %0 = ptrtoint i8* %ap.cur to [[PTRTYPE:i32]]
// N64:   %0 = ptrtoint i8* %ap.cur to [[PTRTYPE:i64]]
//
// Vectors are 16-byte aligned, however the O32 ABI has a maximum alignment of
// 8-bytes since the base of the stack is 8-byte aligned.
// O32:   %1 = add i32 %0, 7
// O32:   %2 = and i32 %1, -8
//
// N32:   %1 = add i32 %0, 15
// N32:   %2 = and i32 %1, -16
//
// N64:   %1 = add i64 %0, 15
// N64:   %2 = and i64 %1, -16
//
// ALL:   %3 = inttoptr [[PTRTYPE]] %2 to <4 x i32>*
// ALL:   %4 = inttoptr [[PTRTYPE]] %2 to i8*
// ALL:   %ap.next = getelementptr i8* %4, [[PTRTYPE]] 16
// ALL:   store i8* %ap.next, i8** %va, align [[PTRALIGN]]
// ALL:   %5 = load <4 x i32>* %3, align 16
// ALL:   call void @llvm.va_end(i8* %va1)
// ALL:   %vecext = extractelement <4 x i32> %5, i32 0
// ALL:   ret i32 %vecext
// ALL: }
