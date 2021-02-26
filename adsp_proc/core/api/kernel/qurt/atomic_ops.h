#ifndef ATOMIC_OPS_H
#define ATOMIC_OPS_H
/**
  @file atomic_ops.h 

  @brief  Type definitions backwards compatible

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
 * Author: Carlos Dyonisio <medaglia@ok-labs.com>
 */

#include <qurt_atomic_ops.h>

/*
 * Kernel and Userland definitions.
 *
 * The atomic ops API is common between the kernel and userland
 * but many of the implementation details differ.  
 */
//#ifdef KENGE_PISTACHIO

//#define __ATOMIC_OPS_IN_KERNEL__

typedef unsigned int atomic_plain_word_t;

//#else /* In userland */

//#include <comdef.h> //#define inline __inline
//#include <l4/types.h>

//typedef L4_Word_t atomic_plain_word_t;

//#endif /* kernel or userland */

/*-------------------------------------------------------------------------*/
                        /* Atomic Ops API. */

/*
 * IMPORTANT!
 * If you plan to change the structure atomic_word_t, please add the new
 * elements after value. For more information read the comment in
 * arch/arm/libs/atomic_ops/v5/src/arm_atomic_ops.spp:66
 */

typedef struct {
    volatile atomic_plain_word_t value;
} atomic_word_t;

#define ATOMIC_INIT(i)  { (i) }

static inline void
atomic_init(atomic_word_t *a, atomic_plain_word_t v)
{
    a->value = v;
}

#if defined(ARCH_ARM) && defined(ARCH_VER) && (ARCH_VER < 6) && \
         (!defined(__ATOMIC_OPS_IN_KERNEL__) || defined(MACHINE_SMP))

/* 
 * If it is ARMv4/v5, the function declarations may change
 * and are defined in the arch specific header file,
 * as some of then can't be declared static because of
 * the assembler implementation.
 */

#else 

atomic_plain_word_t atomic_set(atomic_word_t* target,
                                      atomic_plain_word_t value);

/* Bitwise operations. */
void atomic_and(atomic_word_t* target,
                       atomic_plain_word_t mask);
atomic_plain_word_t atomic_and_return(atomic_word_t* target,
                                             atomic_plain_word_t mask);

void atomic_or(atomic_word_t* target,
                      atomic_plain_word_t mask);
atomic_plain_word_t atomic_or_return(atomic_word_t* target,
                                            atomic_plain_word_t mask);

void atomic_xor(atomic_word_t* target,
                       atomic_plain_word_t mask);
atomic_plain_word_t atomic_xor_return(atomic_word_t* target,
                                             atomic_plain_word_t mask);

/* Bitfield operations. */

void atomic_set_bit(atomic_word_t *target, unsigned int bit);
void atomic_clear_bit(atomic_word_t *target, unsigned int bit);
void atomic_change_bit(atomic_word_t *target, unsigned int bit);

/* Arithmetic operations. */
void atomic_add(atomic_word_t *target, atomic_plain_word_t v);
atomic_plain_word_t atomic_add_return(atomic_word_t *target,
                                             atomic_plain_word_t v);

void atomic_sub(atomic_word_t *target, atomic_plain_word_t v);
atomic_plain_word_t atomic_sub_return(atomic_word_t *target,
                                             atomic_plain_word_t v);

void atomic_inc(atomic_word_t *target);
atomic_plain_word_t atomic_inc_return(atomic_word_t *target);

void atomic_dec(atomic_word_t *target);
atomic_plain_word_t atomic_dec_return(atomic_word_t *target);

/* General operations. */

int atomic_compare_and_set(atomic_word_t *target,
                                  atomic_plain_word_t old_val,
                                  atomic_plain_word_t new_val);

/* Memory barrier operations. */

void atomic_barrier_write(void);
void atomic_barrier_write_smp(void);
void atomic_barrier_read(void);
void atomic_barrier_read_smp(void);
void atomic_barrier(void);
void atomic_barrier_smp(void);

/* Architecture independent definitions. */

static inline atomic_plain_word_t atomic_read(atomic_word_t *target)
{
    return target->value;
}

static inline void atomic_compiler_barrier(void)
{
#if defined(__RVCT__) || defined(__RVCT_GNU__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 200000))
    __memory_changed();
#elif defined(__ADS__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 110000))
    __asm("");
#else
    __asm__ __volatile__ (""::: "memory");
#endif
}


typedef unsigned long long atomic64_plain_word_t;

typedef struct {
    volatile atomic64_plain_word_t value;
} atomic64_word_t;

static inline void
atomic64_init(atomic64_word_t *a, atomic64_plain_word_t v)
{
    a->value = v;
}

/*********************
 *   SUPPORT 64 bit  *
 *********************/

atomic64_plain_word_t atomic64_set(atomic64_word_t* target,
                                      atomic64_plain_word_t value);

/* Bitwise operations. */
void atomic64_and(atomic64_word_t* target,
                       atomic64_plain_word_t mask);
atomic64_plain_word_t atomic64_and_return(atomic64_word_t* target,
                                             atomic64_plain_word_t mask);

void atomic64_or(atomic64_word_t* target,
                      atomic64_plain_word_t mask);
atomic64_plain_word_t atomic64_or_return(atomic64_word_t* target,
                                            atomic64_plain_word_t mask);

void atomic64_xor(atomic64_word_t* target,
                       atomic64_plain_word_t mask);
atomic64_plain_word_t atomic64_xor_return(atomic64_word_t* target,
                                             atomic64_plain_word_t mask);

/* Bitfield operations. */

void atomic64_set_bit(atomic64_word_t *target, unsigned int bit);
void atomic64_clear_bit(atomic64_word_t *target, unsigned int bit);
void atomic64_change_bit(atomic64_word_t *target, unsigned int bit);

/* Arithmetic operations. */
void atomic64_add(atomic64_word_t *target, atomic64_plain_word_t v);
atomic64_plain_word_t atomic64_add_return(atomic64_word_t *target,
                                             atomic64_plain_word_t v);

void atomic64_sub(atomic64_word_t *target, atomic64_plain_word_t v);
atomic64_plain_word_t atomic64_sub_return(atomic64_word_t *target,
                                             atomic64_plain_word_t v);

void atomic64_inc(atomic64_word_t *target);
atomic64_plain_word_t atomic64_inc_return(atomic64_word_t *target);

void atomic64_dec(atomic64_word_t *target);
atomic64_plain_word_t atomic64_dec_return(atomic64_word_t *target);

/* General operations. */

int atomic64_compare_and_set(atomic64_word_t *target,
                                  atomic64_plain_word_t old_val,
                                  atomic64_plain_word_t new_val);

/* Memory barrier operations. */

void atomic64_barrier_write(void);
void atomic64_barrier_write_smp(void);
void atomic64_barrier_read(void);
void atomic64_barrier_read_smp(void);
void atomic64_barrier(void);
void atomic64_barrier_smp(void);

/*---------------------------------------------------------------------------*/

/* Architecture independent definitions. */

static inline atomic64_plain_word_t atomic64_read(atomic64_word_t *target)
{
    return target->value;
}

static inline void atomic64_compiler_barrier(void)
{
#if defined(__RVCT__) || defined(__RVCT_GNU__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 200000))
    __memory_changed();
#elif defined(__ADS__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 110000))
    __asm("");
#else
    __asm__ __volatile__ (""::: "memory");
#endif
}

#endif


/* Architecture dependent definitions. */
#include <atomic_ops_plat.h>

#endif /* ATOMIC_OPS_H */
