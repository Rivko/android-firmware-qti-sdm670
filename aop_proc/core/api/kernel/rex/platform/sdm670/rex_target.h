#ifndef REX_TARGET_H
#define REX_TARGET_H

/* Target specific definitions */

#define REX_TARGET_NUM_SYS_VECTORS  16
#define REX_INTERRUPT_OFFSET REX_TARGET_NUM_SYS_VECTORS
#define REX_TARGET_NUM_IRQS         128

#define REX_TARGET_MIN_PRI 0xFF

#endif
