#ifndef COMDEF_H
#define COMDEF_H
/*===========================================================================

                   S T A N D A R D    D E C L A R A T I O N S

DESCRIPTION
  This header file contains general types and macros that are of use
  to all modules.  The values or definitions are dependent on the specified
  target.  T_WINNT specifies Windows NT based targets, otherwise the
  default is for ARM targets.

       T_WINNT  Software is hosted on an NT platforn, triggers macro and
                type definitions, unlike definition above which triggers
                actual OS calls

DEFINED CONSTANTS

       Name      Definition
       -------   --------------------------------------------------------
       ON        Asserted condition
       OFF       Deasserted condition

       NULL      Pointer to nothing

       PACK()    Used to declare a C type packed for either GCC or ARM
                 compilers.

       *** DEPRECATED - WON'T WORK FOR newer versions (3.x) of GCC ***
       PACKED    Used to indicate structures which should use packed
                 alignment
       *** DEPRECATED - WON'T WORK FOR newer versions (3.x) of GCC ***

       INLINE    Used to inline functions for compilers which support this

Copyright (c) 1990-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/comdef.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/08   tjc     Fix ALIGN macro for GCC targets
                   Add FEATURE_NO_INTLOCK and FEATURE_NO_TASKLOCK
02/05/08   ejv     Sync with version from MSMSHARED_UTILS.00.00.11.
                   Move BM macro to sys.h.
12/11/07   pj      #defined INTLOCK()/INTFREE() to no-op for WINCE_OAL.
06/05/07   trc     Added HANDPACK() macro allowing for explicit hand-packing
05/08/07   trc     Added STATIC pseudo-keyword, make "" by defining NO_STATIC
04/20/07   trc     Added PACK() macro for true ARM/GCC packing portability
3/12/07    RC      Added EXTERM, BM() and DEC_TO_0() macros.
04/02/06   rp      Featurized the define for boolean since 32bit mode boot code
                   needs all variable to be 32bit wide
03/15/06   tmz     Merge changes from 6800
08/25/04   jac     Suppress lint error 413 for FPOS macro
08/23/04    gr     Added !defined _ARM_ASM_ protection for function
                   prototypes.
03/22/04    gr     Undid the unconditional defining away of STATIC. This is not
                   needed.
11/19/03    gr     Added multiple definition protection for the ARR_SIZE
                   macro.
10/18/03    gr     Moved TASKLOCK/TASKFREE into this file from rex.h. DMSS
                   expects to pick these up just by including comdef.h.
10/17/03    gr     Made inclusion of target.h unconditional (TG is now
                   deprecated).
10/16/03    gr     Defined LOCAL away unconditionally. DMSS doesn't build with
                   LOCAL set to static.
01/07/04   ajn     Re-integrated 11/14/03 change
11/20/03   ak      ARR_SIZE conflict with BREW files fixed
11/14/03   ajn     Removed #include "rex.h" to avoid circular include issue.
                   Instead, prototypes for rex_int_lock()/rex_int_free()
                   have been explicitly included for INTLOCK/INTFREE macros.
10/01/03    gr     Deleted misleading INTLOCK comments.
06/12/02   ems     Added SWIG preprocessor work around
06/11/01    gr     Added MOD_BY_POWER_OF_TWO macro.
04/12/01    sq     Added inpdw and outpdw macros
           vas     Added solaris support
04/06/01    gr     Removed the definitions of abs and labs.
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                   Added include for stdlib.h within WINNT ifdef
                   Added support for ARM_ASM
                   Added support SWIG preprocessor.
09/09/99   tac     Merged in a bunch of panther changes.
08/11/99   jct     Corrected conditional compilation around labs to check for
                   labs instead of abs
06/30/99   jct     Added PC_EMULATOR capability for INTLOCK and INTLOCKSAV
06/15/99   jct     Cleanup to make work in Windows NT environment.  Addition
                   of int(n) types where n is 8,16,32,64 and addition of
                   unit64.  Removal of OS comments and defines as this is replaced
                   by T_WINNT.  Conditional inclusion of MAX, MIN, labs, abs,
                   NULL definitions.  Removal of volatile from packed definition
                   stdlib.h included for Win32 based targets since this replaces several
                   of the define here.  Changed out uts calls to be comet calls.
                   Added a defines for Windows NT targets to define away symbols
                   no longer supported
04/09/99    ms     Lint cleanup.
04/01/99    ms     Lint cleanup.
02/17/99    ms     Parenthesized abs and labs.
                   Defined __packed to volatile to exploit lint.
12/16/98   jct     Removed 80186 support
12/01/98    ms     Removed definition of bsp_io_base.
11/20/98    ms     Typecast to volatile pointers in inp,outp,inpw and outpw.
10/10/98   jct     Honing of OS and T_ definitions, added T_WINNT as a target
                   which triggers something different then which OS since
                   we may be running REX on an NT platform, but strictly
                   speaking, the application software is using REX, not NT
                   as the OS.  Changed ROM for WINNT/ARM targets to be nothing
                   instead of const
09/09/98   jct     Updates to merge ARM support with 186 support, added some
                   new type aliases, added some segmented architecture macros,
                   updated comments, removed unused code, updated declaration
                   of inp/outp to have cdecl
08/01/98   jct     Updates to support ARM processor
03/25/96   jah     Moved SET_VECT to 80186.h
02/06/96   fkm     Put ()s around Macros to Make Lint Happy
01/12/95   jah     Updated dependency in MACRO INTLOCK_SAV / INTFREE_SAV.
12/20/94   jah     Corrected typo in ARR_SIZE() macro comments
03/11/94   jah     Added #ifdef _lint of NULL as 0 for near/far lint problem.
06/11/93   jah     Changed FPOS() to cast the pointer to 'near' to avoid the
                   compiler complaints about lost segments.
01/28/93   twp     Added ARR_SIZE macro to return number of array elements.
07/17/92   jah     Changed int1 from 'char' to 'signed char'
06/10/92   jah     Added WORD_LO/HI, INC_SAT
05/19/92   jah     Added header comments for macros & made them lint-friendly
05/04/92   jah     Added define for ROM
03/02/92   jah     Added in/outp prototypes, INT* Macro comments w/lint
02/20/92   arh     Added LOCAL macro and ifdef/endif around file

===========================================================================*/


/*===========================================================================

                            Data Declarations

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif      
#include "com_dtypes.h"

#ifndef _MSC_VER
#include "target.h"
#include "armasm.h"
#endif

#if defined FEATURE_L4  && !defined FEATURE_L4_KERNEL && !defined BUILD_BOOT_CHAIN && !defined ARCH_QDSP6
  #ifndef _ARM_ASM_
    #include <l4/cust/msm_syscall.h>
  #endif
#endif

/* For NT apps we want to use the Win32 definitions and/or those
** supplied by the Win32 compiler for things like NULL, MAX, MIN
** abs, labs, etc.
*/
#ifdef T_WINNT
   #ifndef WIN32
      #define WIN32
   #endif
   #include <stdlib.h>
   
#endif

/* Macro to indicate that a variable is not being used to suppress lint
** warnings
*/
#define NOTUSED(i) if(i){}

#define WCE_MSG  RETAILMSG
#define WCE_MSG6 RETAILMSG
#define WCE_MSGT RETAILMSG
#define WCE_MSG_SDBG RETAILMSG

#if defined(FEATURE_QUBE)
/* QDSP6 tools release after Dec. 12 no longer define addr_t in qtypes.h */
typedef  void *            addr_t;
#endif /* !QDSP6_LEGACY_ADDR_T */

/* ---------------------------------------------------------------------
** Compiler Keyword Macros
** --------------------------------------------------------------------- */


#if (! defined T_WINNT) && (! defined __GNUC__)
  
    /* Non WinNT Targets */

    #if defined(__ARMCC_VERSION) 
      #define PACKED __packed
      #define PACKED_POST
      #define ALIGN(__value) __align(__value)
      #define POST_ALIGN(__value)
      #define INLINE __inline
      #define inline __inline
      #define PACKED_UINT16_ACCESS(PTR) *((__packed uint16 *) (PTR))
      #define PACKED_UINT16 __packed uint16
      #define PACKED_UINT32_ACCESS(PTR) *((__packed uint32 *) (PTR))
      #define PACKED_UINT32 __packed uint32
      #define PACKED_UINT64_ACCESS(PTR) *((__packed uint64 *) (PTR))
      #define PACKED_UINT64 __packed uint64
      #define PACKED_INT16_ACCESS(PTR) *((__packed int16 *) (PTR))
      #define PACKED_INT16 __packed int16
      #define PACKED_INT32_ACCESS(PTR) *((__packed int32 *) (PTR))
      #define PACKED_INT32 __packed uint32
      #define PACKED_INT64_ACCESS(PTR) *((__packed int64 *) (PTR))
      #define PACKED_INT64 __packed int64
    #else  /* __GNUC__ */
      #define PACKED 
      #ifndef FEATURE_WINCE
        #define ALIGN(__value) __attribute__((__aligned__(__value)))
        #define POST_ALIGN(__value)
        #define PACKED_POST    __attribute__((__packed__))
      #else /* FEATURE_WINCE */
        #define ALIGN(__value)
        #define POST_ALIGN(__value)
        #define PACKED_POST
        #define INLINE 
      #endif /* FEATURE_WINCE */
    
      #ifndef INLINE
        #define INLINE inline
      #endif
    #endif /* defined (__GNUC__) */
  
  #define far
  #define near
  #define _far
  #define _near
  #define _pascal
  #define _interrupt

#else /* T_WINNT || TARGET_OS_SOLARIS || __GNUC__ */

  /* WINNT or SOLARIS based targets*/
  #if defined (__GNUC__) || defined (__GNUG__)
    /* The macros PACKED and __packed are deprecated.  The following lines will
     * create compiler warnings.  Change PACKED to use PACK() instead.
     */
    #define PACKED 
    #define ALIGN(__value) __attribute__((aligned(__value)))
    #define POST_ALIGN(__value)
    #define PACKED_POST __attribute__((__packed__)) 

     // workarounds to lack of GCC support for pointer-to-unaligned-intrinsic
        typedef struct __attribute__((packed))
        { uint16 x; }
        unaligned_uint16;
        typedef struct __attribute__((packed))
        { uint32 x; }
        unaligned_uint32;
        typedef struct __attribute__((packed))
        { uint64 x; }
        unaligned_uint64;
        typedef struct __attribute__((packed))
        { int16 x; }
        unaligned_int16;
        typedef struct __attribute__((packed))
        { int32 x; }
        unaligned_int32;
        typedef struct __attribute__((packed))
        { int64 x; }
        unaligned_int64;
        #define PACKED_UINT16 unaligned_uint16
        #define PACKED_UINT16_ACCESS(PTR) ((unaligned_uint16 *) (PTR))->x
        #define PACKED_UINT32 unaligned_uint32
        #define PACKED_UINT32_ACCESS(PTR) ((unaligned_uint32 *) (PTR))->x
        #define PACKED_UINT64 unaligned_uint64
        #define PACKED_UINT64_ACCESS(PTR) ((unaligned_uint64 *) (PTR))->x
        #define PACKED_INT16 unaligned_int16
        #define PACKED_INT16_ACCESS(PTR) ((unaligned_int16 *) (PTR))->x
        #define PACKED_INT32 unaligned_int32
        #define PACKED_INT32_ACCESS(PTR) ((unaligned_int32 *) (PTR))->x
        #define PACKED_INT64 unaligned_int64
        #define PACKED_INT64_ACCESS(PTR) ((unaligned_int64 *) (PTR))->x
       
  #else
    #define PACKED
    #define __packed
    #define ALIGN(__value)
    #define POST_ALIGN(__value)
    #define PACKED_POST
    #define inline __inline
    /* Ignore gcc-style attributes on msvc builds */
    #define __attribute__(x)

    #define PACKED_UINT16 uint16
    #define PACKED_UINT16_ACCESS(PTR) (*((uint16*)PTR))
    #define PACKED_UINT32 uint32
    #define PACKED_UINT32_ACCESS(PTR) (*((uint32*)PTR))
    #define PACKED_UINT64 uint64
    #define PACKED_UINT64_ACCESS(PTR) (*((uint64*)PTR))
    #define PACKED_INT16 int16
    #define PACKED_INT16_ACCESS(PTR) (*((int16*)PTR))
    #define PACKED_INT32 int32
    #define PACKED_INT32_ACCESS(PTR) (*((int32*)PTR))
    #define PACKED_INT64 int64
    #define PACKED_INT64_ACCESS(PTR) (*((int64*)PTR))
  
  #endif

  #ifndef INLINE

    /* INLINE is defined to __inline because WINNT targets work fine with it
     * and defining it to nothing does not work because the inline function
     * definition is then multiply defined.  Solaris may need a different value.
     */
    #ifdef __GNUC__
      #define INLINE static inline
    #else
      #define INLINE __inline
    #endif /*  __GNUC__ */
  #endif /*  INLINE  */
  #define _pascal
  #define _far
  #define far
  #define near
  #define _near

  #define _interrupt
  #define _fmemcpy memcpy

#endif /* T_WINNT */

#ifndef CDECL
  #ifdef _MSC_VER
    #define CDECL __cdecl
  #else
    #define CDECL
  #endif /* _MSC_VER */
#endif /* CDECL */
#define cdecl
#define _cdecl

/*************** The __NON_DEMAND_PAGED_FUNCTION__ macro ******************

  The __NON_DEMAND_PAGED_FUNCTION(_END)__ macro allows for specific 
  function in a file to be placed in a non-demand paged memory section 
  in the memory map, even if the file itself was set to be demand paged.

  This is enabled by a feature - FEATURE_NON_DEMAND_PAGED_FUNCTION. If the
  feature is off, the macro is defined to NULL.

************** BEGIN __NON_DEMAND_PAGED_FUNCTION__ Definition *************/
#if defined(FEATURE_NON_DEMAND_PAGED_FUNCTION) && defined(__ARMCC_VERSION)
  #define __NON_DEMAND_PAGED_FUNCTION__     _Pragma("arm section code=\"NON_DL\"")
  #define __NON_DEMAND_PAGED_FUNCTION_END__ _Pragma("arm section code")
#else
  #define __NON_DEMAND_PAGED_FUNCTION__
  #define __NON_DEMAND_PAGED_FUNCTION_END__
#endif // #if defined(FEATURE_NON_DEMAND_PAGED_FUNCTION) && defined(__ARMCC_VERSION)
/************* END __NON_DEMAND_PAGED_FUNCTION__ Definition ***************/


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

/* ----------------------------------------------------------------------
**                          STANDARD MACROS
** ---------------------------------------------------------------------- */

/* Macro to explicitly designate that a function is externalized.
** Only to be used in the scope of the .c file (in the header file the
** reserved C word "extern" is to be used).
**
** To be more explicit about local vs. externalized functions, each function
** must have "static" or "EXTERN" in front of its source file definition.
*/
#ifndef EXTERN
#define EXTERN
#endif


/*===========================================================================

MACRO MEM_B
MACRO MEM_W

DESCRIPTION
  Take an address and dereference it as a byte/word, allowing access to an
  arbitrary memory byte/word.

PARAMETERS
  x     address to be dereferenced

DEPENDENCIES
  None

RETURN VALUE
  MEM_B byte at the given address
  MEM_W word at the given address

SIDE EFFECTS
  None

===========================================================================*/

#define  MEM_B( x )  ( *( (byte *) (x) ) )
#define  MEM_W( x )  ( *( (word *) (x) ) )



/*===========================================================================

MACRO MAX
MACRO MIN

DESCRIPTION
  Evaluate the maximum/minimum of 2 specified arguments.

PARAMETERS
  x     parameter to compare to 'y'
  y     parameter to compare to 'x'

DEPENDENCIES
  'x' and 'y' are referenced multiple times, and should remain the same
  value each time they are evaluated.

RETURN VALUE
  MAX   greater of 'x' and 'y'
  MIN   lesser of 'x' and 'y'

SIDE EFFECTS
  None

===========================================================================*/
#ifndef MAX
   #define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif

#ifndef MIN
   #define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif



/*===========================================================================

MACRO FPOS

DESCRIPTION
  This macro computes the offset, in bytes, of a specified field
  of a specified structure or union type.

PARAMETERS
  type          type of the structure or union
  field         field in the structure or union to get the offset of

DEPENDENCIES
  None

RETURN VALUE
  The byte offset of the 'field' in the structure or union of type 'type'.

SIDE EFFECTS
  The lint error "Warning 545: Suspicious use of &" is suppressed within
  this macro.  This is due to the desire to have lint not complain when
  'field' is an array.

===========================================================================*/

#define FPOS( type, field ) \
    /*lint -e545 */ ( (dword) &(( type *) 0)-> field ) /*lint +e545 */



/*===========================================================================

MACRO FSIZ

DESCRIPTION
  This macro computes the size, in bytes, of a specified field
  of a specified structure or union type.

PARAMETERS
  type          type of the structure or union
  field         field in the structure or union to get the size of

DEPENDENCIES
  None

RETURN VALUE
  size in bytes of the 'field' in a structure or union of type 'type'

SIDE EFFECTS
  None

===========================================================================*/

#define FSIZ( type, field ) sizeof( ((type *) 0)->field )



/*===========================================================================

MACRO FLIPW

DESCRIPTION
  Takes a 2 byte array, with the most significant byte first, followed
  by the least significant byte, and converts the quantity into a word
  with Intel byte order (LSB first)

PARAMETERS
  ray   array of 2 bytes to be converted to a word

DEPENDENCIES
  None

RETURN VALUE
  word in Intel byte order comprised of the 2 bytes of ray.

SIDE EFFECTS
  None

===========================================================================*/

#define  FLIPW( ray ) ( (((word) (ray)[0]) * 256) + (ray)[1] )



/*===========================================================================

MACRO FLOPW

DESCRIPTION
  Reverses bytes of word value and writes them to a 2 byte array.
  Effectively the reverse of the FLIPW macro.

PARAMETERS
  ray   array to receive the 2 bytes from 'val'
  val   word to break into 2 bytes and put into 'ray'

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define  FLOPW( ray, val ) \
  (ray)[0] = ((val) / 256); \
  (ray)[1] = ((val) & 0xFF)



/*===========================================================================

MACRO B_PTR
MACRO W_PTR

DESCRIPTION
  Casts the address of a specified variable as a pointer to byte/word,
  allowing byte/word-wise access, e.g.
  W_PTR ( xyz )[ 2 ] = 0x1234;  -or-    B_PTR ( xyz )[ 2 ] = 0xFF;

PARAMETERS
  var   the datum to get a word pointer to

DEPENDENCIES
  None

RETURN VALUE
  Word pointer to var

SIDE EFFECTS
  None

===========================================================================*/

#define  B_PTR( var )  ( (byte *) (void *) &(var) )
#define  W_PTR( var )  ( (word *) (void *) &(var) )
#define  D_PTR( var )  ( (dword *) (void *) &(var) )


/*===========================================================================

MACRO WORD_LO
MACRO WORD_HI

DESCRIPTION
  Take a word and extract the least-significant or most-significant byte.

PARAMETERS
  xxx   word to extract the Low/High from

DEPENDENCIES
  None

RETURN VALUE
  WORD_LO       Least significant byte of xxx.
  WORD_HI       Most significant byte of xxx.

SIDE EFFECTS
  None

===========================================================================*/

#define  WORD_LO(xxx)  ((byte) ((word)(xxx) & 255))
#define  WORD_HI(xxx) \
  /*lint -e572 -e778 */  ((byte) ((word)(xxx) >> 8))  /*lint +e572 +e778 */
 /* Constant expression evaluates to 0 in operation '>>'*/


/*===========================================================================

MACRO RND8

DESCRIPTION
  RND8 rounds a number up to the nearest multiple of 8.

PARAMETERS
  x     Number to be rounded up

DEPENDENCIES
  None

RETURN VALUE
  x rounded up to the nearest multiple of 8.

SIDE EFFECTS
  None

===========================================================================*/

#define RND8( x )       ((((x) + 7) / 8 ) * 8 )


/*===========================================================================

MACRO INTLOCK / INTFREE

DESCRIPTION
  INTLOCK Saves current interrupt state on stack then informs the kernel
  not to process interrupts in this thread. One interrupt may still occur, 
  but it will not be processed until either INTFREE is called or a context 
  switch to another thread which has interrupts enabled is made.

  INTFREE Restores previous interrupt state from stack.  Used in conjunction
  with INTLOCK.

  INTLOCK/INTFREE may be nested, in which case inner INTLOCK/INTFREE
  pairs will have no effect.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  INTLOCK stops interrupt processing.
  INTFREE restores the interrupt mask saved previously.

===========================================================================*/
#if defined T_WINNT

   #define INTLOCK()
   #define INTFREE()

#elif defined FEATURE_WINCE_OAL
  /* WM kernel manages interrupts itself using native windows system calls */
  #define INTLOCK()
  #define INTFREE()

#elif !defined _ARM_ASM_ && (!defined FEATURE_L4 && !defined FEATURE_BLAST || defined FEATURE_L4_KERNEL || defined ARCH_QDSP6)

  #ifdef __cplusplus
    extern "C" {
  #endif
  extern dword rex_int_lock(void);
  extern dword rex_int_free(void);
#ifdef FEATURE_WINCE
      extern int rex_ints_are_locked( void );
#endif

  #ifdef __cplusplus
    }
  #endif

#if (!defined FEATURE_NO_REX)

  #define  INTLOCK( )  { dword sav = rex_int_lock();
  #define  INTFREE( )  if(!sav) (void)rex_int_free();}
                       
#endif

#endif


/*===========================================================================

MACRO MSM_INTLOCK / MSM_INTFREE

DESCRIPTION
  MSM_INTLOCK Saves current interrupt state on stack then locks interrupts. 
  No interrupts will be taken until either MSM_INTFREE is called or a context 
  switch to another thread which has interrupts enabled is made. This
  macro is slower than INTLOCK and should only be used when interrupt 
  locking is needed to ensure timing. To simply prevent context switching, 
  use the INTLOCK/INTFREE macros above.

  MSM_INTFREE Restores previous interrupt state from stack. Used in conjunction
  with MSM_INTLOCK.

  MSM_INTLOCK/MSM_INTFREE may be nested, in which case inner 
  MSM_INTLOCK/MSM_INTFREE pairs will have no effect. It may also be nested
  inside or outside of INTLOCK/INTFREE.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  MSM_INTLOCK prevents any interrupts from occurring until MSM_INTFREE is
    called or a context switch to a thread with interrupts enabled is made.
  MSM_INTFREE restores the interrupt mask saved previously.

===========================================================================*/
#ifndef _ARM_ASM
#if defined FEATURE_L4
  #ifdef __cplusplus
    extern "C" {
  #endif
      extern dword rex_msm_int_lock( void );
      extern void rex_msm_int_free( dword set );
  #ifdef __cplusplus
    }
  #endif

  #define MSM_INTLOCK()  { dword sav = rex_msm_int_lock();
  #define MSM_INTFREE()  rex_msm_int_free( sav ); }

#else

  #define MSM_INTLOCK() INTLOCK()
  #define MSM_INTFREE() INTFREE()

#endif
#endif

/*===========================================================================

MACRO FIQLOCK / FIQFREE

DESCRIPTION
  FIQLOCK Saves current FIQ state on stack then disables FIQ interrupts.
  Used in conjunction with FIQFREE.

  FIQFREE Restores previous FIQ interrupt state from stack.  Used in 
  conjunction with FIQLOCK.

PARAMETERS
  None

DEPENDENCIES
  This is currently only available in L4 based builds.

RETURN VALUE
  None

SIDE EFFECTS
  FIQLOCK turns off FIQ interrupts.
  FIQFREE restores the FIQ interrupt mask saved previously.

===========================================================================*/
#if defined FEATURE_L4 && !defined _ARM_ASM_ && !defined FEATURE_L4_KERNEL && !defined ARCH_QDSP6

  #ifdef __cplusplus
    extern "C" {
  #endif
      extern dword rex_fiq_op( dword lock );
  #ifdef __cplusplus
    }
  #endif

  #define  FIQLOCK()  { dword sav = rex_fiq_op(1);
  #define  FIQFREE()  if(!(sav & PSR_Fiq_Mask)) (void)rex_fiq_op(0);}
    
#elif defined FEATURE_BMPOS_QNK && !defined _ARM_ASM_
  
    #ifdef __cplusplus
      extern "C" {
    #endif
    extern dword rex_fiq_lock(void);
    extern void rex_fiq_free(void);
    #ifdef __cplusplus
      }
    #endif

    #define FIQLOCK()  {dword sav_var = rex_fiq_lock())
    #define FIQFREE()  if(!sav_var) rex_fiq_free();}
#else

  #define FIQLOCK()
  #define FIQFREE()

#endif

/*===========================================================================

MACRO INTLOCK_SAV / INTFREE_SAV

DESCRIPTION
  INTLOCK_SAV Saves current interrupt state in specified variable sav_var
  then disables interrupts.  Used in conjunction with INTFREE_SAV.

  INTFREE_SAV Restores previous interrupt state from specified variable
  sav_var.  Used in conjunction with INTLOCK_SAV.

PARAMETERS
  sav_var       Current flags register, including interrupt status

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  INTLOCK_SAV turn off interrupts
  INTFREE_SAV restore the interrupt mask saved previously

===========================================================================*/


#ifdef _lint    /* get lint to 'know' the parameter is accessed */

  #define  INTLOCK_SAV(sav_var)   (sav_var = 1)
  #define  INTFREE_SAV(sav_var)   (sav_var = sav_var + 1)

#else

   #if defined T_WINNT

     #define  INTLOCK_SAV( sav_var )   (sav_var = 1)
     #define  INTFREE_SAV( sav_var )   (sav_var = sav_var + 1)

   #elif !defined _ARM_ASM_ && (!defined FEATURE_L4 || defined FEATURE_L4_KERNEL || defined ARCH_QDSP6)

     #define  INTLOCK_SAV( sav_var )  sav_var = rex_int_lock()
     #define  INTFREE_SAV( sav_var )  if(!sav_var) rex_int_free()

   #endif

#endif /* END if _lint */

/*===========================================================================

MACRO FIQLOCK_SAV / FIQFREE_SAV

DESCRIPTION
  FIQLOCK Saves current FIQ state in specified variable sav_var 
  then disables FIQ interrupts.  Used in conjunction with FIQFREE_SAV.

  FIQFREE_SAV Restores previous FIQ interrupt state from specified variable
  sav_var.  Used in conjunction with FIQLOCK_SAV.

PARAMETERS
  sav_var       Current flags register, including interrupt status

DEPENDENCIES
  This is currently only available in L4 based builds.

RETURN VALUE
  None

SIDE EFFECTS
  FIQLOCK_SAV turns off FIQ interrupts.
  FIQFREE_SAV restores the FIQ interrupt mask saved previously.

===========================================================================*/
#ifdef _lint    /* get lint to 'know' the parameter is accessed */

  #define  FIQLOCK_SAV(sav_var)   (sav_var = 1)
  #define  FIQFREE_SAV(sav_var)   (sav_var = sav_var + 1)

#else

  #if defined FEATURE_L4 && !defined _ARM_ASM_ && !defined FEATURE_L4_KERNEL && !defined ARCH_QDSP6
  
    #ifdef __cplusplus
      extern "C" {
    #endif
        extern dword rex_fiq_op( dword lock );
    #ifdef __cplusplus
      }
    #endif
  
    #define  FIQLOCK_SAV(sav_var)  (sav_var = rex_fiq_op(1))
    #define  FIQFREE_SAV(sav_var)  if(!(sav_var & PSR_Fiq_Mask)) \
                                      (void)rex_fiq_op(0)

  #elif defined FEATURE_WINCE

    #define  FIQLOCK_SAV(sav_var)  (sav_var = FIQLockSave())
    #define  FIQFREE_SAV(sav_var)  if(!(sav_var & 0x40)) (void)FIQLockRestore()

  #elif defined ARCH_QDSP6
    #define  FIQLOCK_SAV(sav_var)   (sav_var = 1)
    #define  FIQFREE_SAV(sav_var)   (sav_var = sav_var + 1)
  
  #elif !defined _ARM_ASM_
  
    #ifdef __cplusplus
      extern "C" {
    #endif
    extern dword rex_fiq_lock(void);
    extern void rex_fiq_free(void);
    #ifdef __cplusplus
      }
    #endif

    #define FIQLOCK_SAV(sav_var)  (sav_var = rex_fiq_lock())
    #define FIQFREE_SAV(sav_var)  if(!sav_var) rex_fiq_free()
  #else
    #define FIQLOCK_SAV(sav_var)
    #define FIQFREE_SAV(sav_var)
  #endif
  
#endif


/*===========================================================================
MACRO TASKLOCK/TASKFREE/TASKLOCK_SAV/TASKFREE_SAV

DESCRIPTION
  TASKLOCK prevents context switching from the point at which it is invoked
  until a matching TASKFREE is invoked.  While task locked, the current task
  is guaranteed to not be replaced by a different task, even if the other
  task has a higher priority.  However, TASKLOCK does not block interrupts
  from occurring.  An ISR will pre-empt the task, but even if the ISR causes
  a higher priority task to become ready, control will still resume at the
  interrupted task.

  Note that a rex-wait() while a TASKLOCK is in effect will not work as
  normal, as the calling task will not be suspended.

  Also, TASKLOCK/TASKFREE have no effect if called from within an ISR.

  TASKFREE re-enables context switching.

  TASKLOCK_SAV and TASKFREE_SAV are similar to TASKLOCK and TASKFREE
  respectively, except that the TASKFREE_SAV that matches a TASKLOCK_SAV
  does not have to be at the same static scope within a function. This
  makes it easier, for instance, to enable task scheduling and return from
  the middle of a function.

===========================================================================*/
#if !defined _ARM_ASM_
#ifdef __cplusplus
   extern "C" void rex_task_lock( void);
   extern "C" void rex_task_free( void);
#else
   extern void rex_task_lock( void);
   extern void rex_task_free( void);
#endif
#endif /* !_ARM_ASM_ */

#ifdef T_WINNT
#ifdef __cplusplus
   extern "C" {
#endif
   unsigned char rexnt_is_rex_thread( void );
#ifdef __cplusplus
   }
#endif
   /* tasks must not be locked by non-REX threads */
#define TASKLOCK( ) {\
   unsigned char REXNT_IS_REX_THREAD=rexnt_is_rex_thread();\
   if ( REXNT_IS_REX_THREAD ) rex_task_lock( ); {

#define TASKFREE( ) \
   } if ( REXNT_IS_REX_THREAD ) rex_task_free( ); }

#else

#define TASKLOCK( ) { rex_task_lock( );

#define TASKFREE( ) rex_task_free( ); }

#define TASKLOCK_SAV( ) rex_task_lock( )

#define TASKFREE_SAV( ) rex_task_free( )

#endif /* T_WINNT */

/*===========================================================================

MACRO UPCASE

DESCRIPTION
  Convert a character to uppercase, the character does not have to
  be printable or a letter.

PARAMETERS
  c             Character to be converted

DEPENDENCIES
  'c' is referenced multiple times, and should remain the same value
  each time it is evaluated.

RETURN VALUE
  Uppercase equivalent of the character parameter

SIDE EFFECTS
  None

===========================================================================*/

#define  UPCASE( c ) ( ((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c) )


/*===========================================================================

MACRO DECCHK
MACRO HEXCHK

DESCRIPTION
  These character attribute macros are similar to the standard 'C' macros
  (isdec and ishex), but do not rely on the character attributes table used
  by Microsoft 'C'.

PARAMETERS
  c             Character to be examined

DEPENDENCIES
  None

RETURN VALUE
  DECCHK        True if the character is a decimal digit, else False
  HEXCHK        True if the chacters is a hexidecimal digit, else False

SIDE EFFECTS
  None

===========================================================================*/

#define  DECCHK( c ) ((c) >= '0' && (c) <= '9')

#define  HEXCHK( c ) ( ((c) >= '0' && (c) <= '9') ||\
                       ((c) >= 'A' && (c) <= 'F') ||\
                       ((c) >= 'a' && (c) <= 'f') )


/*===========================================================================

MACRO INC_SAT

DESCRIPTION
  Increment a value, but saturate it at its maximum positive value, do not
  let it wrap back to 0 (unsigned) or negative (signed).

PARAMETERS
  val           value to be incremented with saturation

DEPENDENCIES
  None

RETURN VALUE
  val

SIDE EFFECTS
  val is updated to the new value

===========================================================================*/

#define  INC_SAT( val )  (val = ((val)+1 > (val)) ? (val)+1 : (val))


/*===========================================================================

MACRO ARR_SIZE

DESCRIPTION
  Return the number of elements in an array.

PARAMETERS
  a             array name

DEPENDENCIES
  None

RETURN VALUE
  Number of elements in array a

SIDE EFFECTS
  None.

===========================================================================*/
#define  ARR_SIZE( a )  ( sizeof( (a) ) / sizeof( (a[0]) ) )
/*===========================================================================
                     MACRO MOD_BY_POWER_OF_TWO

     Will calculate x % y, where x is a non-negative integer and
     y is a power of 2 from 2^0..2^32.

     Will work for 2^0, 2^1, 2^2, 2^3, 2^4, ... 2^32
      ie.            1,   2,   4,   8,  16, ... 4294967296
===========================================================================*/

#define MOD_BY_POWER_OF_TWO( val, mod_by ) \
           ( (dword)(val) & (dword)((mod_by)-1) )

/* -------------------------------------------------------------------------
**                       Debug Declarations
** ------------------------------------------------------------------------- */

/* Define 'SHOW_STAT' in order to view static's as globals
** (e.g. cl /DSHOW_STAT foo.c) If 'SHOW_STAT' is not defined,
** it gets defined as 'static'
*/
#ifdef LOCAL
   #undef LOCAL
#endif

/* Permanently turn on SHOW_STAT since this has always been the case anyway
*/

#define LOCAL

/* Provide a new macro that will hopefully be used correctly to conditionally
** turn static on/off
*/
#ifndef T_WINNT
  #define STATIC static
#endif // !T_WINNT
/** @endcond */


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION inp, outp, inpw, outpw, inpdw, outpdw

DESCRIPTION
  IN/OUT port macros for byte and word ports, typically inlined by compilers
  which support these routines

PARAMETERS
  inp(   xx_addr )
  inpw(  xx_addr )
  inpdw( xx_addr )
  outp(   xx_addr, xx_byte_val  )
  outpw(  xx_addr, xx_word_val  )
  outpdw( xx_addr, xx_dword_val )
      xx_addr      - Address of port to read or write (may be memory mapped)
      xx_byte_val  - 8 bit value to write
      xx_word_val  - 16 bit value to write
      xx_dword_val - 32 bit value to write

DEPENDENCIES
  None

RETURN VALUE
  inp/inpw/inpdw: the byte, word or dword read from the given address
  outp/outpw/outpdw: the byte, word or dword written to the given address

SIDE EFFECTS
  None.

===========================================================================*/
#ifndef T_WINNT
  /* ARM based targets use memory mapped i/o, so the inp/outp calls are
  ** macroized to access memory directly
  */

  #define inp(port)         (*((volatile byte *) (port)))
  #define inpw(port)        (*((volatile word *) (port)))
#endif // !T_WINNT
  #define inpdw(port)       (*((volatile dword *)(port)))

#ifndef T_WINNT
  #define outp(port, val)   (*((volatile byte *) (port)) = ((byte) (val)))
  #define outpw(port, val)  (*((volatile word *) (port)) = ((word) (val)))
#endif // !T_WINNT
  #define outpdw(port, val) (*((volatile dword *) (port)) = ((dword) (val)))




/*===========================================================================

FUNCTION enable, disable

DESCRIPTION
  Interrupt enable and disable routines.  Enable should cause the CPU to
  allow interrupts and disable should cause the CPU to disallow
  interrupts

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

#ifndef T_WINNT

  #if !defined _ARM_ASM_
    /* ARM has no such definition, so we provide one here to enable/disable
    ** interrupts
    */
    #define _disable() (void)rex_int_lock()
    #define _enable()  (void)rex_int_free()

  #endif
#endif



#ifdef __cplusplus
}
#endif

#endif  /* COMDEF_H */
