#ifndef QURT_ATOMIC_OPS_H
#define QURT_ATOMIC_OPS_H
/**
  @file qurt_atomic_ops.h 
  @brief  Prototypes of kernel atomic operations API

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/


/*
 * Australian Public Licence B (OZPLB)
 *
 * Version 1-0
 *
 * Copyright (c) 2007, Open Kernel Labs, Inc.
 *
 * All rights reserved. 
 *
 * Developed by: Embedded, Real-time and Operating Systems Program (ERTOS)
 *               National ICT Australia
 *               http://www.ertos.nicta.com.au
 *
 * Permission is granted by National ICT Australia, free of charge, to
 * any person obtaining a copy of this software and any associated
 * documentation files (the "Software") to deal with the Software without
 * restriction, including (without limitation) the rights to use, copy,
 * modify, adapt, merge, publish, distribute, communicate to the public,
 * sublicense, and/or sell, lend or rent out copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimers.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimers in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of National ICT Australia, nor the names of its
 *       contributors, may be used to endorse or promote products derived
 *       from this Software without specific prior written permission.
 *
 * EXCEPT AS EXPRESSLY STATED IN THIS LICENCE AND TO THE FULL EXTENT
 * PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS PROVIDED "AS-IS", AND
 * NATIONAL ICT AUSTRALIA AND ITS CONTRIBUTORS MAKE NO REPRESENTATIONS,
 * WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS
 * REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE, OR OF TITLE,
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT,
 * THE ABSENCE OF LATENT OR OTHER DEFECTS, OR THE PRESENCE OR ABSENCE OF
 * ERRORS, WHETHER OR NOT DISCOVERABLE.
 *
 * TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL
 * NATIONAL ICT AUSTRALIA OR ITS CONTRIBUTORS BE LIABLE ON ANY LEGAL
 * THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER
 * LIABILITY, INCLUDING (WITHOUT LIMITATION) LOSS OF PRODUCTION OR
 * OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS
 * OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE, PROFIT OR GOODWILL, OR
 * OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT,
 * CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES, ARISING OUT OF OR IN
 * CONNECTION WITH THIS LICENCE, THE SOFTWARE OR THE USE OF OR OTHER
 * DEALINGS WITH THE SOFTWARE, EVEN IF NATIONAL ICT AUSTRALIA OR ITS
 * CONTRIBUTORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH CLAIM, LOSS,
 * DAMAGES OR OTHER LIABILITY.
 *
 * If applicable legislation implies representations, warranties, or
 * conditions, or imposes obligations or liability on National ICT
 * Australia or one of its contributors in respect of the Software that
 * cannot be wholly or partly excluded, restricted or modified, the
 * liability of National ICT Australia or the contributor is limited, to
 * the full extent permitted by the applicable legislation, at its
 * option, to:
 * a.  in the case of goods, any one or more of the following:
 * i.  the replacement of the goods or the supply of equivalent goods;
 * ii.  the repair of the goods;
 * iii. the payment of the cost of replacing the goods or of acquiring
 *  equivalent goods;
 * iv.  the payment of the cost of having the goods repaired; or
 * b.  in the case of services:
 * i.  the supplying of the services again; or
 * ii.  the payment of the cost of having the services supplied again.
 *
 * The construction, validity and performance of this licence is governed
 * by the laws in force in New South Wales, Australia.
 */

/*
 * Author: Malcolm Purvis <malcolmp@ok-labs.com>
 *
 * This file is only included by the main atomic_ops.h, so all of that
 * file's definitions are available.
 */

///* Sanity check to ensure the smp flag is set in machines.py */
//#if defined(__ATOMIC_OPS_IN_KERNEL__) && !defined(MACHINE_SMP) && CONFIG_NUM_UNITS > 1
//#error CONFIG_NUM_UNITS > 1 but smp not defined in machines.py.
//#endif
#define QURT_INLINE  __attribute__((always_inline))
/*=============================================================================
                        FUNCTIONS
=============================================================================*/
static inline QURT_INLINE unsigned int
qurt_atomic_set(unsigned int* target, unsigned int value)
{
    unsigned long tmp;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       memw_locked(%2, p0) = %3\n"
        "       if !p0 jump 1b\n"
        : "=&r" (tmp),"+m" (*target)
        : "r" (target), "r" (value)
        : "p0");
    return value;
}

/* Bitwise operations. */
static inline QURT_INLINE void
qurt_atomic_and(unsigned int* target, unsigned int mask)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = and(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target),"r" (mask)
        : "p0");
}

static inline QURT_INLINE unsigned int
qurt_atomic_and_return(unsigned int* target, unsigned int mask)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = and(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic_or(unsigned int* target, unsigned int mask)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = or(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");
}

static inline QURT_INLINE unsigned int
qurt_atomic_or_return(unsigned int* target, unsigned int mask)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = or(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic_xor(unsigned int* target, unsigned int mask)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = xor(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");
}

static inline QURT_INLINE unsigned int
qurt_atomic_xor_return(unsigned int* target, unsigned int mask)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = xor(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");

    return result;
}

/* Bitfield operations. */

static inline QURT_INLINE void
qurt_atomic_set_bit(unsigned int *target, unsigned int bit)
{
    unsigned int result;
		int aword = bit / (sizeof(unsigned int) * 8); 
    unsigned int sbit = bit % (sizeof(unsigned int) * 8);
		unsigned int *wtarget= (unsigned int *)&target[aword];

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = setbit(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*wtarget)
        : "r" (wtarget), "r" (sbit)
        : "p0");
}

static inline QURT_INLINE void
qurt_atomic_clear_bit(unsigned int *target, unsigned int bit)
{
    unsigned int result;
		int aword = bit / (sizeof(unsigned int) * 8); 
    unsigned int sbit = bit % (sizeof(unsigned int) * 8);
		unsigned int *wtarget= (unsigned int *)&target[aword];

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = clrbit(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*wtarget)
        : "r" (wtarget), "r" (sbit)
        : "p0");
}

static inline QURT_INLINE void
qurt_atomic_change_bit(unsigned int *target, unsigned int bit)
{
    unsigned int result;
		int aword = bit / (sizeof(unsigned int) * 8); 
    unsigned int sbit = bit & 0x1f;
		unsigned int *wtarget= (unsigned int *)&target[aword];

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = togglebit(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*wtarget)
        : "r" (wtarget),"r" (sbit)
        : "p0");
}


/* Arithmetic operations. */
static inline QURT_INLINE void
qurt_atomic_add(unsigned int *target, unsigned int v)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");
}

static inline QURT_INLINE unsigned int
qurt_atomic_add_return(unsigned int *target, unsigned int v)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic_sub(unsigned int *target, unsigned int v)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = sub(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");
}

static inline QURT_INLINE unsigned int
qurt_atomic_sub_return(unsigned int *target, unsigned int v)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = sub(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic_inc(unsigned int *target)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = add(%0, #1)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target)
        : "p0");
}

static inline QURT_INLINE unsigned int
qurt_atomic_inc_return(unsigned int *target)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = add(%0, #1)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target)
        : "p0");

    return result;
}


static inline QURT_INLINE void
qurt_atomic_dec(unsigned int *target)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = add(%0, #-1)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target)
        : "p0");
}

static inline QURT_INLINE unsigned int
qurt_atomic_dec_return(unsigned int *target)
{
    unsigned int result;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = add(%0, #-1)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target)
        : "p0");

    return result;
}

/* General operations. */

static inline QURT_INLINE unsigned int
qurt_atomic_compare_and_set(unsigned int* target,
                       unsigned int old_val,
                       unsigned int new_val)
{
    unsigned int current_val;

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       p0 = cmp.eq(%0, %3)\n"
        "       if !p0 jump 2f\n"
        "       memw_locked(%2, p0) = %4\n"
        "       if !p0 jump 1b\n"
        "2:\n"
        : "=&r" (current_val),"+m" (*target)
        : "r" (target), "r" (old_val), "r" (new_val)
        : "p0");

    return current_val == old_val;
}

static inline QURT_INLINE void
qurt_atomic_barrier(void)
{
    __asm__ __volatile__ (
        ""
        :
        :
        :
        "memory");
}

static inline QURT_INLINE void
qurt_atomic_barrier_write(void)
{
    qurt_atomic_barrier();
}

static inline QURT_INLINE void
qurt_atomic_barrier_write_smp(void)
{
    qurt_atomic_barrier();
}

static inline QURT_INLINE void
qurt_atomic_barrier_read(void)
{
    qurt_atomic_barrier();
}

static inline QURT_INLINE void
qurt_atomic_barrier_read_smp(void)
{
    qurt_atomic_barrier();
}

static inline QURT_INLINE void
qurt_atomic_barrier_smp(void)
{
    qurt_atomic_barrier();
}



/*============================
 *       64 Bits support
 *============================ */
 
static inline QURT_INLINE unsigned long long
qurt_atomic64_set(unsigned long long* target, unsigned long long value)
{
    unsigned long long tmp;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       memd_locked(%2, p0) = %3\n"
        "       if !p0 jump 1b\n"
        : "=&r" (tmp),"+m" (*target)
        : "r" (target), "r" (value)
        : "p0");
    return value;
}

/* Bitwise operations. */
static inline QURT_INLINE void
qurt_atomic64_and(unsigned long long* target, unsigned long long mask)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = and(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target),"r" (mask)
        : "p0");
}

static inline QURT_INLINE unsigned long long
qurt_atomic64_and_return(unsigned long long* target, unsigned long long mask)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = and(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic64_or(unsigned long long* target, unsigned long long mask)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = or(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");
}

static inline QURT_INLINE unsigned long long
qurt_atomic64_or_return(unsigned long long* target, unsigned long long mask)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = or(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic64_xor(unsigned long long* target, unsigned long long mask)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = xor(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");
}

static inline QURT_INLINE unsigned long long
qurt_atomic64_xor_return(unsigned long long* target, unsigned long long mask)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = xor(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask)
        : "p0");

    return result;
}

/* Bitfield operations. */

static inline QURT_INLINE void
qurt_atomic64_set_bit(unsigned long long *target, unsigned int bit)
{
    unsigned int result;
		unsigned int *wtarget;
		unsigned int *pwtarget = (unsigned int *)target;
		int aword = bit / (sizeof(unsigned int) * 8); 
    unsigned int sbit = bit & 0x1F;
		wtarget = (unsigned int *)&pwtarget[aword];
		

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = setbit(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*wtarget)
        : "r" (wtarget), "r" (sbit)
        : "p0");
}

static inline QURT_INLINE void
qurt_atomic64_clear_bit(unsigned long long *target, unsigned int bit)
{
    unsigned int result;
		unsigned int *wtarget;
		unsigned int *pwtarget = (unsigned int *)target;
		int aword = bit / (sizeof(unsigned int) * 8); 
    unsigned int sbit = bit & 0x1F;
		wtarget = (unsigned int *)&pwtarget[aword];

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = clrbit(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*wtarget)
        : "r" (wtarget), "r" (sbit)
        : "p0");
}

static inline QURT_INLINE void
qurt_atomic64_change_bit(unsigned long long *target, unsigned int bit)
{
    unsigned int result;
		unsigned int *wtarget;
		unsigned int *pwtarget = (unsigned int *)target;
		int aword = bit / (sizeof(unsigned int) * 8); 
    unsigned int sbit = bit & 0x1F;
		wtarget = (unsigned int *)&pwtarget[aword];

    __asm__ __volatile__(
        "1:     %0 = memw_locked(%2)\n"
        "       %0 = togglebit(%0, %3)\n"
        "       memw_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*wtarget)
        : "r" (wtarget),"r" (sbit)
        : "p0");
}


/* Arithmetic operations. */
static inline QURT_INLINE void
qurt_atomic64_add(unsigned long long *target, unsigned long long v)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");
}

static inline QURT_INLINE unsigned long long
qurt_atomic64_add_return(unsigned long long *target, unsigned long long v)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic64_sub(unsigned long long *target, unsigned long long v)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = sub(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");
}

static inline QURT_INLINE unsigned long long
qurt_atomic64_sub_return(unsigned long long *target, unsigned long long v)
{
    unsigned long long result;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = sub(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (v)
        : "p0");

    return result;
}

static inline QURT_INLINE void
qurt_atomic64_inc(unsigned long long *target)
{
    unsigned long long result;
		unsigned long long inc =1;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target),"r" (inc)
        : "p0");
}

static inline QURT_INLINE unsigned long long
qurt_atomic64_inc_return(unsigned long long *target)
{
    unsigned long long result;
		unsigned long long inc =1;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target),"r" (inc)
        : "p0");

    return result;
}


static inline QURT_INLINE void
qurt_atomic64_dec(unsigned long long *target)
{
    unsigned long long result;
		unsigned long long minus1 = -1;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target),"r" (minus1)
        : "p0");
}

static inline QURT_INLINE unsigned long long
qurt_atomic64_dec_return(unsigned long long *target)
{
    unsigned long long result;
		unsigned long long minus1 = -1;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       %0 = add(%0, %3)\n"
        "       memd_locked(%2, p0) = %0\n"
        "       if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target),"r" (minus1)
        : "p0");

    return result;
}

/* General operations. */

static inline QURT_INLINE int
qurt_atomic64_compare_and_set(unsigned long long *target,
                       unsigned long long old_val,
                       unsigned long long new_val)
{
    unsigned long long current_val;

    __asm__ __volatile__(
        "1:     %0 = memd_locked(%2)\n"
        "       p0 = cmp.eq(%0, %3)\n"
        "       if !p0 jump 2f\n"
        "       memd_locked(%2, p0) = %4\n"
        "       if !p0 jump 1b\n"
        "2:\n"
        : "=&r" (current_val),"+m" (*target)
        : "r" (target), "r" (old_val), "r" (new_val)
        : "p0");

    return current_val ==old_val;
}

static inline QURT_INLINE void
qurt_atomic64_barrier(void)
{
    __asm__ __volatile__ (
        ""
        :
        :
        :
        "memory");
}

static inline QURT_INLINE void
qurt_atomic64_barrier_write(void)
{
    qurt_atomic64_barrier();
}

static inline QURT_INLINE void
qurt_atomic64_barrier_write_smp(void)
{
    qurt_atomic64_barrier();
}

static inline QURT_INLINE void
qurt_atomic64_barrier_read(void)
{
    qurt_atomic64_barrier();
}

static inline QURT_INLINE void
qurt_atomic64_barrier_read_smp(void)
{
    qurt_atomic64_barrier();
}

static inline QURT_INLINE void
qurt_atomic64_barrier_smp(void)
{
    qurt_atomic64_barrier();
}


#endif /* QURT_ATOMIC_OPS_H */
