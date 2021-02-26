/** @a5x_memcpy.S
  Optimized memcpy

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/5/2014  keng    Initial revision
=============================================================================*/

;//**************************************************************************
;// void memcpy_a5x(addr_t dest addr_t src, int len)
;//**************************************************************************        

GCC_ASM_EXPORT(memcpy_a5x)

.func                
.align	8

memcpy_a5x:
#define PRFMLDLO (3)
#define PRFMLDHI (128)
#define PRFMSIZE (64)


// 1) PLD nothing when size is <= (PRFMLDLO*2)
// 2) PLD just lo when size is <= (PRFMLDHI*2)
// 3) PLD first loop using both lo and hi up to (PRFMLDHI-PRFMLDLO)
// 4) PLD 
// Strategy: pld just lo when size is <= PRFMMIN

	cmp	x0, x1
	b.eq	memcpy_exit
	cbz	x2, memcpy_exit
	prfm	PLDL1KEEP, [x1, #0]
	mov	x10, x0		// Save for return...necessary?
	asr	x4, x2, #6
	cbz	x4, 8f
	// we have at least 63 bytes to achieve 64-byte alignment
	neg	x3, x1		// calculate count to get SOURCE aligned
	ands	x3, x3, #0x3F	
	b.eq	a64_memcpy_8916_64B_aligned		// already aligned
	// alignment fixup, small to large (favorable alignment)
	tbz	x3, #0, 1f
	ldrb	w5, [x1], #1
	strb	w5, [x0], #1
1:	tbz	x3, #1, 2f
	ldrh	w6, [x1], #2
	strh	w6, [x0], #2
2:	tbz	x3, #2, 3f
	ldr	w8, [x1], #4
	str	w8, [x0], #4
3:	tbz	x3, #3, 4f
	ldr	x9, [x1], #8
	str	x9, [x0], #8
4:	tbz	x3, #4, 5f
	ldp	x5, x6, [x1], #16
	stp	x5, x6, [x0], #16
5:	tbz	x3, #5, 6f
	ldp	x5, x6, [x1], #16
	ldp	x7, x8, [x1], #16
	stp	x5, x6, [x0], #16
	stp	x7, x8, [x0], #16
6:	sub	x2, x2, x3		// fixup count after alignment
a64_memcpy_8916_64B_aligned:             
	asr	x4, x2, #6		// use 64-byte loops
	cbz	x4, 8f			// if there are less than 64 bytes left, go to the tail code

	cmp	x4, #(8*1024)		// 512k
	b.ge	.Llargecopy

	cmp	x4, #1
	b.eq	99f

	sub	x4, x4, #1		// logic path below transfers one extra 64-byte set
	
	sub	x1, x1, #32
	sub	x0, x0, #32		// offset dest index to compensate for the initial store index
	ldp	q0, q1, [x1, #32]	// perform 1st 32-byte fetch outside main loop		
	ldp	q2, q3, [x1, #64]!	// offset dest index by amount fetched outside the loop
7:	stp	q0, q1, [x0, #32]
	subs	x4, x4, #1
	ldp	q0, q1, [x1, #32]
	stp	q2, q3, [x0, #64]!
	ldp	q2, q3, [x1, #64]!
	b.ne	7b
	stp	q0, q1, [x0, #32]
	stp	q2, q3, [x0, #64]
	add	x1, x1, #32
	add	x0, x0, #96

	ands	x2, x2, 0x3F
	b.eq	14f
	b	8f

.align 6
.Llargecopy:
	sub	x4, x4, #128		// remove 8K worth of loops from the prfm loop

77:	subs	x4, x4, #1
	prfm	PLDL1KEEP, [x1, #(8*1024)]
//        ld1	{v0.16B, v1.16B, v2.16B, v3.16B}, [x1], #64
//        st1     {v0.16B, v1.16B, v2.16B, v3.16B}, [x0], #64
        ldp         q0, q1, [x1], #32
        stp         q0, q1, [x0], #32
        ldp         q2, q3, [x1], #32     
        stp         q2, q3, [x0], #32        
	b.ne	77b
	mov	x4, #128		// do 8K worth of loops without pld
99:	subs	x4, x4, #1
        ldp         q0, q1, [x1], #32
        stp         q0, q1, [x0], #32
        ldp         q2, q3, [x1], #32     
        stp         q2, q3, [x0], #32        
//        ld1	{v0.16B, v1.16B, v2.16B, v3.16B}, [x1], #64
//        st1	{v0.16B, v1.16B, v2.16B, v3.16B}, [x0], #64
	b.ne	99b

	ands	x2, x2, 0x3F
	beq	14f

8:	tbz	x2, #5, 9f
	ldp	x5, x6, [x1], #16
	ldp	x7, x8, [x1], #16
	stp	x5, x6, [x0], #16
	stp	x7, x8, [x0], #16
9:	tbz	x2, #4, 10f
	ldp	x5, x6, [x1], #16
	stp	x5, x6, [x0], #16
10:	tbz	x2, #3, 11f
	ldr	x5, [x1], #8
	str	x5, [x0], #8
11:	tbz	x2, #2, 12f
	ldr	w5, [x1], #4
	str	w5, [x0], #4
12:	tbz	x2, #1, 13f
	ldrh	w5, [x1], #2
	strh	w5, [x0], #2
13:	tbz	x2, #0, 14f
	ldrb	w5, [x1], #1
	strb	w5, [x0], #1
14:	mov	x0, x10

memcpy_exit:        
	ret

.endfunc                
