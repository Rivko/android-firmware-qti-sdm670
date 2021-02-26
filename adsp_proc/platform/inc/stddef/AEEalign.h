#ifndef AEEALIGN_H
#define AEEALIGN_H
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#if defined(__GNUC__) || defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 220000)
#define AEE_ALIGN(t, a)    t __attribute__((aligned(a)))
#elif defined(_MSC_VER)
#define AEE_ALIGN(t, a)    __declspec(align(a)) t
#elif defined(__ARMCC_VERSION)
#define AEE_ALIGN(t, a)    __align(a) t
#else
#error Unknown compiler
#endif

/*
=====================================================================
MACROS DOCUMENTATION
=====================================================================

AEE_ALIGN()

Description:
    Specify alignment of a type or variable

Definition:
      #if defined(__GNUC__) || defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 220000)
      #define AEE_ALIGN(t, a)    t __attribute__((aligned(a)))
      #elif defined(_MSC_VER)
      #define AEE_ALIGN(t, a)    __declspec(align(a)) t
      #elif defined(__ARMCC_VERSION)
      #define AEE_ALIGN(t, a)    __align(a) t
      #else
      #error Unknown compiler
      #endif

Parameters:
  t: thing to be aligned, can be a type or a variable
  a: desired alignment in bytes

Evaluation Value:
   none.

Comments: AEE_ALIGN()'s capabilities depend upon the compiler.
   Compilers supported and tested include:
    Microsoft: Vc7, Vs8 ARM:
    ADS1.2, RVDS 2.1-3.1 
    GNU: various 3.x and 4.x versions for various platforms 
         (Cygwin, Linux-x86, ARM-elf, QDSP6)

   Compiler notes:
      ARM, ADS1.2, and pre-2.2 RVDS compilers with GNU extensions 
      turned off (the default): Only the __align() form of AEE_ALIGN() is 
        supported.  Unfortunately, the __align() directive only works
        as a top-level storage-class modifier: i.e. it can't be used in 
        types, function parameters, or in fields of top-level objects.

   Example of AEE_ALIGN() on a top-level object:

      static AEE_ALIGN(char x, 8);

   Examples of AEE_ALIGN() on types:

       typedef AEE_ALIGN(struct foo1 {
          char x;
       }, 8) foo1;

       AEE_ALIGN(typedef struct foo2 {
          char x;
       } foo2, 8);

       AEE_ALIGN(typedef char char_8, 8);

       typedef struct foo3 {
          char_8 x;
       } foo3;

       typedef struct foo4 {
          char  c;
          char_8 x;
       } foo4;

       typedef struct foo5 {
          AEE_ALIGN(char x, 8);
       } foo5;

       typedef struct foo6 {
          char c;
          AEE_ALIGN(char x, 8);
       } foo6;

   Example of AEE_ALIGN() on a non-top-level object:

       static struct {
         AEE_ALIGN(char x, 8);
       } aligned_x;

=====================================================================
*/

#endif /* #ifndef AEEALIGN_H */

