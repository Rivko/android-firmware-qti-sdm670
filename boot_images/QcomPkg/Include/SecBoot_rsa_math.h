/** @file SecBoot_rsa_math.h
 
DESCRIPTION:
   Implementation a big number library to support modular
   arithmetic, with the primary goal of doing a fast
   modular exponentiation.

   References:

  "Handbook of Applied Cryptography (HAC)" by A. Menezes Chapter 14
   Various papers by Cetin kaya Koc

   Design Notes:

   The BigInt representation is unsigned as I have not found a
   use for signed big numbers yet.

   Divison is the trusted school-book method as given in HAC and is
   quite slow.

   Multiply/Squares/Adds use hand-tuned ARM assembly code to optimize
   critical sections with in the inner loops.

  Copyright (c) 2010-2015 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
**/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ----    -------------------------------------------
01/21/15   sk      Changed typedef for compilation

=============================================================================*/


#ifndef SECBOOT_RSA_MATH_H_
#define SECBOOT_RSA_MATH_H_

/*=============================================================================

                               INCLUDE FILES

=============================================================================*/
#include "SecBoot_i.h"
#include "SecBoot_api.h"

/*=============================================================================

                            CONSTANT DEFINITIONS

=============================================================================*/
#define BN_UNROLL_LOOPS

// Arm target specific code
#if defined USE_ARM

//    #define USE_ARMASM 1
#endif

#define BN_FAST_SQUARING 1
#define BN_CORE_REQD 7 // what the reqd inside the modexp function.
#define BN_STORAGE_SIZE (BN_CORE_REQD + 6) // core + for i/o, external usage.
                                           // (6)
#define BLONG_SIZE sizeof(BLONG)
#define BN_BYTES 4 /*bytes for word */
#define BN_BITS_PER_WORD (BN_BYTES * 8)
#define BLONGS_PER_MAX_KEY  (MAX_KEY_SIZE/ 32)
/* To deal with 64bit ints */
#define TOP_HALF_MASK 0xFFFF0000
#define BOT_HALF_MASK 0x0000FFFF
#define BLONG_MAX_ELEMENTS ((BLONGS_PER_MAX_KEY*2)+1)
/*=======================================================================
                               MACRO FUNCTION
=========================================================================*/
#define HI(x) ((uint64)x >> 32)
#define LO(x) (BLONG)(x)
#define BL_MAX(a,b) (((a)>(b))?a:b)  //roll our own MAX ;)
#define BL_MIN(a,b) (((a)<(b))?a:b)  //roll our own MIN ;)
#define BIGINT_CREATE 1
#define BIGINT_FREE 0
#define BIGINT_Free(BigIntPtr) \
                          BIGINT_ManageStorage(BIGINT_FREE,BigIntPtr)
#define BIGINT_Create(nMaxSize, BigIntPtr) \
                          BIGINT_ManageStorage(BIGINT_CREATE,BigIntPtr)
/*#define BIGINT_FREE(BigIntPtr) BIGINT_ManageStorage('f',BigIntPtr)
#define BIGINT_ExpandSize(BigIntPtr, newSize) \
          BIGINT_ManageStorage('e',BigIntPtr)*/
#ifdef USE_ARMASM
        //(C,S) == a*b + c + d
        #define MULADD2(C, S, a, b, c, d)  \
        __asm {                  \
                MOV   a1, a;          \
                MOV   a2, b;          \
                MOV   a3, d;          \
                MOV   a4, ##0;         \
                ADDS  a3, a3, c;      \
                ADC   a4, a4, ##0;     \
                UMLAL a3, a4, a1, a2; \
                MOV   C, a4;          \
                MOV   S, a3;          \
        }
        #else
        #define MULADD2(C, S, a, b, c, d)  \
        { \
                BN_ULLONG uv = c + (BN_ULLONG)a * b + d;  \
                S  = LO(uv);   \
                C  = HI(uv);   \
        }
#endif

#ifdef USE_ARMASM
        //(C,S) = a + b + c;
        #define ADD3(C, S, P,Q,R)     \
        __asm {                        \
                MOV   a1, P;            \
                MOV   a2, ##0;          \
                ADDS  a1, a1, Q;        \
                ADC   a2, a2, ##0;      \
                ADDS  a1, a1, R;        \
                ADC   a2, a2, ##0;      \
                MOV   C, a2;            \
                MOV   S, a1;            \
        }
        #else
        //(C,S) = a + b + c;
        #define ADD3(C, S, P,Q,R)     \
        {                            \
        BN_ULLONG uv = P;          \
        uv += Q; uv += R;          \
        S  = LO(uv);               \
        C  = HI(uv);               \
        }
#endif

#ifdef USE_ARMASM
        //(C,S) = a - b - c
        #define SUB3(C, S, P,Q,R)     \
        __asm {                    \
                MOV   a1, P;            \
                MOV   a2, ##0;          \
                SUBS  a1, a1, Q;        \
                MOVCC a2, ##1;          \
                SUBS  a1, a1, R;        \
                MOVCC a2, ##1;          \
                MOV   C, a2;            \
                MOV   S, a1;            \
        }
#endif

#define PTRSWAP(a,b,t)  ((void) ((a) = (t) ((UINTN)(a) ^ (UINTN)(b)), \
                                 (b) = (t) ((UINTN)(a) ^ (UINTN)(b)), \
                                 (a) = (t) ((UINTN)(a) ^ (UINTN)(b))) )

// TYPE definitions
#if defined USE_ARM
  // Arm target specific code
  typedef unsigned long long BN_ULLONG;
#endif
#if defined USE_WINDOWS
  // Windows specific code
  typedef unsigned __int64 BN_ULLONG;
#endif

#if !defined(USE_WINDOWS) && !defined(USE_ARM)
  #error "Must define platform"
#endif

//sk: Removing below for compilation fix for 8996
//typedef unsigned long BLONG;
typedef struct {
  BLONG a[BLONG_MAX_ELEMENTS];   //storage area
                                       // { KeySize (in BLONGs) * 2 } +1
  int32 n;      //current length of a
  int32 nMax;   //Maximum allocated length of a
} BigInt;
typedef struct
{
  BigInt BigInt[BN_STORAGE_SIZE];
  uint32 inUse[BN_STORAGE_SIZE];
} BigIntStorageType;

/*Main interface
r = a^p mod m*/
errno_enum_type BIGINT_ModExp  (BigInt *r, BigInt *a, BigInt *p,
                                    BigInt *m);

//I/O
BigInt * BIGINT_CreateFromBuf(const unsigned char *buf, int32 len);
BigInt * BIGINT_ManageStorage(uint32 func, BigInt *inpBigInt); // simple
                                        // 'BIGINT_Create' inside this
errno_enum_type      BIGINT_ExpandSize (BigInt *x, int32 n);
errno_enum_type      BIGINT_OutToBuf   (BigInt *x, unsigned char *to,
                                            int32 *nSize);
errno_enum_type      BIGINT_SetBigInt  (BigInt *x, BigInt *y);
void     BIGINT_SetInt     (BigInt *x, BLONG i);
#ifdef DEBUG
 void     BIGINT_MemStats(char func, int32 blong_size);
 BigInt * BIGINT_CreateRandom(uint32 size);
 BigInt * BIGINT_GetTestBigInt(uint32 byteSize);
#endif


// BigInt worker bees ...
errno_enum_type BIGINT_PreprocessMonty (BigInt *msg, BigInt *m, BLONG *pn0,
                                            BigInt *mont_msg, BigInt *mont_x);
errno_enum_type BIGINT_ModMult (BigInt *r, BigInt *x, BigInt *y,
                                    BigInt *m, const BLONG n0, BigInt *tmp);
errno_enum_type BIGINT_MontMod    (BigInt *t, BigInt *n, const BLONG n0);
errno_enum_type BIGINT_Divide             (BigInt *q, BigInt *x,
                                               BigInt *y);
errno_enum_type BIGINT_Square             (BigInt *r, BigInt *x);
errno_enum_type BIGINT_Multiply           (BigInt *r, BigInt *x,
                                               BigInt *y);
errno_enum_type BIGINT_Add                (BigInt *x, BigInt *y);
errno_enum_type BIGINT_Subtract           (BigInt *x, BigInt *y);
errno_enum_type BIGINT_LeftShift          (BigInt *r, int32 nTimes);
errno_enum_type BIGINT_RightShift         (BigInt *r, int32 nTimes);
errno_enum_type BIGINT_LeftShiftBy2Bytes  (BigInt *x);
void            BIGINT_RightShiftBy2Bytes (BigInt *x);
void            BIGINT_RemoveLeadZeroes   (BigInt *x);
int             BIGINT_NumBits            (BigInt *x);
BLONG           BIGINT_CoreInnerLoop      (BLONG *pa, BLONG *pm, BLONG yai,
                                               int32 xn);
void            BIGINT_CoreMultiply       (BLONG *ra, BLONG *xa, BLONG *ya,
                                               uint32 xn, int32 yn);
errno_enum_type BIGINT_MulWithArray       (BigInt *r, BigInt *x, BLONG *ya,
                                               int32 yn);
BLONG           BIGINT_ComputeMontInverse (BLONG x);
boolean         BIGINT_IsBitSet           (BigInt *x, int32 bit);

// Big Number (just a buffer) workers
uint32          BNAdd      (uint32 *pd, int32 nd, uint32 *ps, int32 ns);
int             BNSub      (uint32 *pd, uint32 *pa, uint32 *pb, int32 na);
void            ISquare256 (uint32 *pd, uint32 *ps);
__inline void   xIncr      (uint32 *pd, int32 n, uint32 uIncr);
void            BNNegate   (uint32 *pd, int32 n);
__inline void   xDiff      (uint32 *pd, uint32 *pa, uint32 *pb,
                                int32 n);
void            BNSquare   (uint32 *pd, uint32 *ps, uint32 ns,
                                uint32 *pt);


//Compares...
int32        BIGINT_CoreCmp        (BLONG *xa, BLONG *ya, int32 n);
int32        BIGINT_cmp            (const BigInt *x, const BigInt *y);
boolean    BIGINT_greaterEqual   (BigInt *x, BigInt *y);
boolean    BIGINT_lessEqual      (BigInt *x, BigInt *y);
boolean    BIGINT_equalTo        (BigInt *x, BigInt *y);
boolean    BIGINT_greaterThan    (BigInt *x, BigInt *y);
boolean    BIGINT_lessThan       (BigInt *x, BigInt *y);


#endif
