#ifndef __HALNOCHWIO_H__
#define __HALNOCHWIO_H__
/*
===========================================================================

FILE:         HALnocHwio.h

DESCRIPTION:  NOC HWIO definitions.

===========================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ICBArbLib/HALnocHwio.h#1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
2013/10/14   sds     Add support for variable block size.
2011/12/16   sds     Created.

===========================================================================
        Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                         QUALCOMM Proprietary
===========================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/
/* Standard IN/OUT/MASK/SHIFT macros */
#define NOC_IN(base,regsym)                               __nochwio_in(base,regsym)
#define NOC_INI(base,offset,size,regsym,index)            __nochwio_ini(base,offset,size,regsym,index)
#define NOC_INI2(base,regsym,in1,in2)                     __nochwio_ini2(base,regsym,in1,in2)
#define NOC_INM(base,regsym,mask)                         __nochwio_inm(base,regsym,mask)
#define NOC_INMI(base,offset,size,regsym,index,mask)      __nochwio_inmi(base,offset,size,regsym,index,mask)
#define NOC_INMI2(base,regsym,in1,in2,mask)               __nochwio_inmi2(base,regsym,in1,in2,mask)
#define NOC_OUT(base,regsym,val)                          __nochwio_out(base,regsym,val)
#define NOC_OUTI(base,offset,size,regsym,index,val)       __nochwio_outi(base,offset,size,regsym,index,val)
#define NOC_OUTI2(base,regsym,in1,in2,val)                __nochwio_outi2(base,regsym,in1,in2,val)
#define NOC_OUTM(base,regsym,mask,val)                    __nochwio_outm(base,regsym,mask,val)
#define NOC_OUTMI(base,offset,size,regsym,index,mask,val) __nochwio_outmi(base,offset,size,regsym,index,mask,val)
#define NOC_OUTMI2(base,regsym,in1,in2,mask,val)          __nochwio_outmi2(base,regsym,in1,in2,mask,val)
#define NOC_ADDR(base,regsym)                             __nochwio_addr(base,regsym)
#define NOC_ADDRI(base,offset,size,regsym,index)          __nochwio_addri(base,offset,size,regsym,index)
#define NOC_RMSK(regsym)                                  __nochwio_rmsk(regsym)
#define NOC_RSHFT(regsym)                                 __nochwio_rshft(regsym)
#define NOC_SHFT(regsym, fldsym)                          __nochwio_shft(regsym, fldsym)
#define NOC_FMSK(regsym, fldsym)                          __nochwio_fmsk(regsym, fldsym)

/* Field manipulation macros */
#define NOC_INF(base,regsym,field)                         (NOC_INM(base,regsym,NOC_FMSK(regsym,field)) >> NOC_SHFT(regsym,field))
#define NOC_INFI(base,offset,size,regsym,index,field)      (NOC_INMI(base,offset,size,regsym,index,NOC_FMSK(regsym,field)) >> NOC_SHFT(regsym,field))
#define NOC_INFI2(base,regsym,in1,in2,field)               (NOC_INMI2(base,regsym,in1,in2,NOC_FMSK(regsym,field)) >> NOC_SHFT(regsym,field))
#define NOC_OUTF(base,regsym,field,val)                    NOC_OUTM(base,regsym,NOC_FMSK(regsym, field), val << NOC_SHFT(regsym, field))
#define NOC_OUTFI(base,offset,size,regsym,index,field,val) NOC_OUTMI(base,offset,size,regsym,index,NOC_FMSK(regsym, field), val << NOC_SHFT(regsym, field))
#define NOC_OUTFI2(base,regsym,in1,in2,field,val)          NOC_OUTMI2(base,regsym,in1,in2,NOC_FMSK(regsym, field), val << NOC_SHFT(regsym, field))

/* Cached field manipulation macro */
#define NOC_INFC(regval,regsym,field)           ((regval & NOC_FMSK(regsym,field)) >> NOC_SHFT(regsym,field))

#define __nochwio_in(base,regsym)                               NOC_##regsym##_IN(base)
#define __nochwio_ini(base,offset,size,regsym,index)            NOC_##regsym##_INI(base,offset,size,index)
#define __nochwio_ini2(base,regsym,in1,in2)                     NOC_##regsym##_INI2(base,in1,in2)
#define __nochwio_inm(base,regsym,mask)                         NOC_##regsym##_INM(base,mask)
#define __nochwio_inmi(base,offset,size,regsym,index,mask)      NOC_##regsym##_INMI(base,offset,size,index,mask)
#define __nochwio_inmi2(base,regsym,in1,in2,mask)               NOC_##regsym##_INMI2(base,in1,in2,mask)
#define __nochwio_out(base,regsym,val)                          NOC_##regsym##_OUT(base,val)
#define __nochwio_outi(base,offset,size,regsym,index,val)       NOC_##regsym##_OUTI(base,offset,size,index,val)
#define __nochwio_outi2(base,regsym,in1,in2,val)                NOC_##regsym##_OUTI2(base,in1,in2,val)
#define __nochwio_outm(base,regsym,mask,val)                    NOC_##regsym##_OUTM(base,mask,val)
#define __nochwio_outmi(base,offset,size,regsym,index,mask,val) NOC_##regsym##_OUTMI(base,offset,size,index,mask,val)
#define __nochwio_outmi2(base,regsym,in1,in2,mask,val)          NOC_##regsym##_OUTMI2(base,in1,in2,mask,val)
#define __nochwio_addr(base,regsym)                             NOC_##regsym##_ADDR(base)
#define __nochwio_addri(base,offset,size,regsym,index)          NOC_##regsym##_ADDR(base,offset,size,index)
#define __nochwio_rmsk(regsym)                                  NOC_##regsym##_RMSK
#define __nochwio_fmsk(regsym, fldsym)                          NOC_##regsym##_##fldsym##_BMSK
#define __nochwio_rshft(regsym)                                 NOC_##regsym##_SHFT
#define __nochwio_shft(regsym, fldsym)                          NOC_##regsym##_##fldsym##_SHFT

#define HWIO_INTLOCK()
#define INTLOCK()

#define HWIO_INTFREE()
#define INTFREE()

#define __inp(port)         (*((volatile unsigned char *) (port)))
#define __inpw(port)        (*((volatile unsigned short *) (port)))
#define __inpdw(port)       (*((volatile unsigned int *) (port)))

#define __outp(port, val)   (*((volatile unsigned char *) (port)) = ((unsigned char) (val)))
#define __outpw(port, val)  (*((volatile unsigned short *) (port)) = ((unsigned short) (val)))
#define __outpdw(port, val) (*((volatile unsigned int *) (port)) = ((unsigned int) (val)))


#define in_byte(addr)               (__inp(addr))
#define in_byte_masked(addr, mask)  (__inp(addr) & (mask)) 
#define out_byte(addr, val)         __outp(addr,val)
#define out_byte_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK();    \
  (void) out_byte( io, shadow); \
  shadow = (shadow & (unsigned short)(~(mask))) | ((unsigned short)((val) & (mask))); \
  HWIO_INTFREE()
#define out_byte_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_byte( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_word(addr)              (__inpw(addr))
#define in_word_masked(addr, mask) (__inpw(addr) & (mask))
#define out_word(addr, val)        __outpw(addr,val)
#define out_word_masked(io, mask, val, shadow)  \
  HWIO_INTLOCK( ); \
  shadow = (shadow & (unsigned short)(~(mask))) |  ((unsigned short)((val) & (mask))); \
  (void) out_word( io, shadow); \
  HWIO_INTFREE( )
#define out_word_masked_ns(io, mask, val, current_reg_content)  \
  (void) out_word( io, ((current_reg_content & (unsigned short)(~(mask))) | \
                       ((unsigned short)((val) & (mask)))) )

#define in_dword(addr)              (__inpdw(addr))
#define in_dword_masked(addr, mask) (__inpdw(addr) & (mask))
#define out_dword(addr, val)        __outpdw(addr,val)
#define out_dword_masked(io, mask, val, shadow)  \
   HWIO_INTLOCK( ); \
   shadow = (shadow & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask))); \
   (void) out_dword( io, shadow); \
   HWIO_INTFREE( )
#define out_dword_masked_ns(io, mask, val, current_reg_content) \
  (void) out_dword( io, ((current_reg_content & (unsigned int)(~(mask))) | ((unsigned int)((val) & (mask)))) )

#endif /* __HALNOCHWIO_H__ */

