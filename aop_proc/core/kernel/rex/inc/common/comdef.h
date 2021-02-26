/*============================================================================

FILE:      comdef.h

DESCRIPTION: com definitions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/common/comdef.h#1 $
$Date: 2018/02/16 $

============================================================================*/

#ifndef COMDEF_H
#define COMDEF_H

#include <assert.h>
#include "cortex-m3.h"


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* standard unsigned alises */
#ifndef _UINT64_DEFINED
  #define _UINT64_DEFINED
  typedef uint64_t uint64;
#endif /* _UINT64_DEFINED */

#ifndef _UINT32_DEFINED
  #define _UINT32_DEFINED
  typedef uint32_t uint32;
#endif /* _UINT32_DEFINED */

#ifndef _UINT16_DEFINED
  #define _UINT16_DEFINED
  typedef uint16_t uint16;
#endif /* _UINT16_DEFINED */

#ifndef _UINT8_DEFINED
  #define _UINT8_DEFINED
  typedef uint8_t  uint8;
#endif /* _UINT8_DEFINED */

#ifndef _UINTNT_DEFINED
typedef  unsigned long      uintnt;      /* Unsigned Native Integer
                                            Aarch32: 32 bits
                                            Aarch64: 64 bits        */
#define _UINTNT_DEFINED
#endif

/* standard signed aliases */
#ifndef _INT64_DEFINED
  #define _INT64_DEFINED
  typedef int64_t int64;
#endif /* _INT64_DEFINED */

#ifndef _INT32_DEFINED
  #define _INT32_DEFINED
  typedef int32_t  int32;
#endif /* _INT32_DEFINED */

#ifndef _INT16_DEFINED
  #define _INT16_DEFINED
  typedef int16_t  int16;
#endif /* _INT16_DEFINED */

#ifndef _INT8_DEFINED
  #define _INT8_DEFINED
  typedef int8_t   int8;
#endif /* _INT8_DEFINED */

/* nonstandard aliases */
#ifndef _DWORD_DEFINED
  #define _DWORD_DEFINED
  typedef uint32_t dword;
#endif /* _DWORD_DEFINED */

#ifndef _WORD_DEFINED
  #define _WORD_DEFINED
  typedef uint16_t word;
#endif /* _WORD_DEFINED */

#ifndef _BYTE_DEFINED
  #define _BYTE_DEFINED
  typedef uint8_t byte;
#endif /* _BYTE_DEFINED */

#ifndef _BOOLEAN_DEFINED
  #define _BOOLEAN_DEFINED
  typedef uint8_t boolean;
#endif /* _BOOLEAN_DEFINED */

/* constants */
#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE   1   /* Boolean true value. */
#define FALSE  0   /* Boolean false value. */

#define  ON   1    /* On value. */
#define  OFF  0    /* Off value. */

#ifndef NULL
  #define NULL  0
#endif

/* dumb math macros */
#ifndef MIN
  #define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif /* MIN */
#ifndef MAX
  #define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif /* MAX */

/* compiler attributes */
#if defined(__ARMCC_VERSION) 
  #define PACKED __packed
  #define PACKED_POST
  #define ALIGN(__value) __align(__value)
  #define POST_ALIGN(__value)
  #define INLINE __inline
  #define inline __inline
#elif defined(__clang__)
  #define PACKED __packed
#endif /* __ARMCC_VERSION */

  /************************** The PACK() macro *****************************

  This block sets up the semantics for PACK() macro based upon
  upon the target compiler.

  PACK() is necessary to ensure portability of C variable/struct/union
  packing across many platforms.  For example, ARM compilers require the
  following:
    typedef __packed struct { ... } foo_t;

  But GCC requires this to achieve the same effect:
    typedef struct __attribute((__packed__)) struct { ... } foo_t;

  And, of course, Microsoft VC++ requires an alignment #pragma prologue
  and epilogue.

  To satisfy all three, the following form is recommended:

    #ifdef _WIN32
    #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
    #endif

    typedef PACK(struct)
    {
      ...
    } such_and_such_t;

    typedef PACK(struct)
    {
      ...
    } this_and_that_t;

    typedef PACK(struct)
    {
      PACK(struct)
      {
        ...
      } hdr;
      PACK(union)
      {
        such_and_such_t sas;
        this_and_that_t tat;
      } payload;
    } cmd_t;

    #ifdef _WIN32
    #pragma pack(pop) // Revert alignment to what it was previously
    #endif

*********************** BEGIN PACK() Definition ***************************/

#if defined __GNUC__
  #define PACK(x)       x __attribute__((__packed__))
#elif defined __GNUG__
  #define PACK(x)       x __attribute__((__packed__))
#elif defined __arm
  #define PACK(x)       __packed x
#elif defined _WIN32
  #define PACK(x)       x /* Microsoft uses #pragma pack() prologue/epilogue */
#else
  #define PACK(x)       x __attribute__((__packed__))
#endif

/********************** END PACK() Definition *****************************/


/********************** The HANDPACK() macro *****************************

  The HANDPACK() macro allows for hand-packing of structures/data on
  compilers which can be trusted to obey strict manual packing rules,
  thus avoiding the need for the coercive-packing PACK() macro.

  Usage of HANDPACK() is exactly the same as PACK(), with the caveat that
  it will become vestigial on compilers for which hand-packing is known to
  be safe.

  The main rule of hand-packing is that types of _known_ size must be used.
  The following are the valid sized types:
    int8
    uint8
    int16
    uint16
    int32
    uint32

  The following types are not allowed in a hand-packed structure:
    int64
    uint64
    enum

  If, for example, one needs to have a member that takes an enum type, 
  declare the packed structure member as one of the above valid types and
  cast the incoming value to that type upon assignment.

  For further details, see PACK()

******************* BEGIN HANDPACK() Definition ***************************/
#if defined __GNUC__ || defined __arm
  #define HANDPACK(x)               x
#else /* For all other compilers not special-cased above, enforce packing */
  #define HANDPACK(x)               PACK(x)
#endif
/****************** END HANDPACK() Definition *****************************/



/* ----------------------------------------------------------------------
** Lint does not understand __packed, so we define it away here.  In the
** past we did this:
**   This helps us catch non-packed pointers accessing packed structures,
**   for example, (although lint thinks it is catching non-volatile pointers
**   accessing volatile structures).
**   This does assume that volatile is not being used with __packed anywhere
**   because that would make Lint see volatile volatile (grrr).
** but found it to be more trouble than it was worth as it would emit bogus
** errors
** ---------------------------------------------------------------------- */
#ifdef _lint
  #ifdef __packed
    #undef __packed
  #endif /* __packed */
  #define __packed
#endif

#ifndef EXTERN
#define EXTERN
#endif

/* old intlock macros... probably try to get rid of these eventually */
extern unsigned intlock_nest_level;
void lock_ints(void);
void unlock_ints(void);
#define INTLOCK_SAV(was_masked) do { (void)was_masked; INTLOCK(); } while(0);
#define INTFREE_SAV(was_masked) INTFREE()
#define INTLOCK() lock_ints();
#define INTFREE() unlock_ints();

#define __ELEMENT_OFFSET(type, element) \
    (uint32)(&((type *)0x0)->element)
    
#ifdef __cplusplus
}
#endif

#endif /* COMDEF_H */

