#ifndef __TEETEST64_UTILS_H__
#define __TEETEST64_UTILS_H__

#define CURRENT_EL_EL0             0
#define CURRENT_EL_EL1             1
#define CURRENT_EL_EL2             2
#define CURRENT_EL_EL3             3

uint32_t get_current_el(void);
uintnt cur_cpu_num(void);

#define EXCLUSIVE_LOCK_ALIGNMENT   128

typedef struct
{
  volatile uint64_t lock __attribute__((aligned(EXCLUSIVE_LOCK_ALIGNMENT)));
} spinlock_t;

static inline int       spinlock_obtain(spinlock_t* lock)   { return 0; }
static inline int       spinlock_release(spinlock_t* lock)  { return 0; }
static inline boolean   spinlock_is_mine(spinlock_t* lock)  { return 1; }

#endif //__TEETEST64_UTILS_H__
