// Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
// Qualcomm Technologies Proprietary and Confidential.

/**
 * @file
 *
 * This file contains definitions that have no dependencies except on the C
 * compiler.  This file should include only toolchain-provided header files.
 * It defines macros or functions that are of some C standard or otherwise
 * supported, but which are not implemented consistently in all compilers.
 * Also, it contains macros that arguably should be standardized, or whose
 * usage is recommended.
 *
 */

#ifndef __CDEFS_H
#define __CDEFS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

/**
 * Equivalent of C11's static_assert(), except that it does not work in file
 * scope.
 */
#define c_static_assert(cond) \
  do { switch(0){case 0:case ((cond) ? 1 : 0):;} } while (0)

/**
 * Cast away a "const" qualifier without triggering the `-Wconst-qual`
 * warning.
 */
#define c_const_cast(type, val)   ((type) (uintptr_t) val)

/**
 * Return the alignment of a type.  Result is of type size_t.
 * This is equivalent to C11's _Alignof and alignof.
 */
#define c_alignof(type) \
  offsetof( struct { char a; type b; }, b)

/**
 * Return a pointer to a structure of the specified type, given a pointer to
 * and name of one of its members.
 */
#define c_containerof(ptr, type, member) \
  ((type*) (((uintptr_t)(void*)ptr) - offsetof(type, member)))

/**
 * Return the *length* of an array, which is the number of *elements* it
 * contains.  This is not to be confused with the array's *size*, as
 * returned by sizeof, which is the number of *bytes* it occupies in memory.
 *
 * The macro aims to mitigate situations in which a pointer is accidentally
 * supplied instead of an array by evaluating to zero. "a == &a" is always
 * true for arrays, and not generally for pointers.
 */
#define C_LENGTHOF(array)                     \
  ((void*) &(array) == (void*) (array)        \
   ? sizeof (array) / sizeof *(array)         \
   : 0)

/**
 * Iterate through all indices in an array.
 */
#define C_FOR_ARRAY(var, array) \
  for (var = 0; var < C_LENGTHOF(array); ++var)

/**
 * Zero a value (set all its bytes to zero).
 */
#define C_ZERO(value) \
  memset(&(value), 0, sizeof(value))

/**
 * Zero a range of array elements.
 */
#define C_ZERO_RANGE(ptr, len) \
  memset((ptr), 0, (len) * sizeof *(ptr))


#endif // __CDEFS_H
