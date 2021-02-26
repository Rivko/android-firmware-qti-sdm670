/**@file WatchDogLibHWIO.h

  Copyright (c) 2017, Qualcomm Technologies Inc. All rights reserved.<BR>

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 
 --------   ---     -----------------------------------------------------------
 07/31/16   c_mku   Added Watchdog pet Mechanism
=============================================================================*/
#ifndef __WATCHDOGLIBHWIO_H__
#define __WATCHDOGLIBHWIO_H__


#include <HALhwio.h>
#include <msmhwiobase.h>

#define APSS_WDT_TMR1_APSS_WDT_REG_BASE                                    (APSS_HM_BASE      + 0x00180000)
#define APSS_WDT_TMR1_APSS_WDT_REG_BASE_PHYS                               (APSS_HM_BASE_PHYS + 0x00180000)
#define APSS_WDT_TMR1_APSS_WDT_REG_BASE_OFFS                               0x00180000

//WDT_RESET Register
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_ADDR                                 (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000004)
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_PHYS                                 (APSS_WDT_TMR1_APSS_WDT_REG_BASE_PHYS + 0x00000004)
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_OFFS                                 (APSS_WDT_TMR1_APSS_WDT_REG_BASE_OFFS + 0x00000004)
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_RMSK                                        0x1
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_RESET_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_RESET_BMSK                                  0x1
#define HWIO_APSS_WDT_TMR1_WDOG_RESET_RESET_SHFT                                  0x0

//WDT_CTL Register
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ADDR                                   (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000008)
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_PHYS                                   (APSS_WDT_TMR1_APSS_WDT_REG_BASE_PHYS + 0x00000008)
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_OFFS                                   (APSS_WDT_TMR1_APSS_WDT_REG_BASE_OFFS + 0x00000008)
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_RMSK                                          0x3
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_CTL_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_UNMASKED_INT_ENABLE_BMSK                      0x2
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_UNMASKED_INT_ENABLE_SHFT                      0x1
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_UNMASKED_INT_ENABLE_DISABLE_FVAL              0x0
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_UNMASKED_INT_ENABLE_ENABLE_FVAL               0x1
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_BMSK                                   0x1
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_SHFT                                   0x0
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_DISABLE_FVAL                           0x0
#define HWIO_APSS_WDT_TMR1_WDOG_CTL_ENABLE_ENABLE_FVAL                            0x1

//WDT_STATUS Register
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_ADDR                                (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x0000000c)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_PHYS                                (APSS_WDT_TMR1_APSS_WDT_REG_BASE_PHYS + 0x0000000c)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_OFFS                                (APSS_WDT_TMR1_APSS_WDT_REG_BASE_OFFS + 0x0000000c)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_RMSK                                0x801fffff
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_STATUS_ADDR, HWIO_APSS_WDT_TMR1_WDOG_STATUS_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_STATUS_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_FROZEN_BMSK                         0x80000000
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_FROZEN_SHFT                               0x1f
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_FROZEN_NO_FVAL                             0x0
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_FROZEN_YES_FVAL                            0x1
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_COUNT_BMSK                            0x1ffffe
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_COUNT_SHFT                                 0x1
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_EXPIRED_STATUS_BMSK                        0x1
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_EXPIRED_STATUS_SHFT                        0x0
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_EXPIRED_STATUS_NO_FVAL                     0x0
#define HWIO_APSS_WDT_TMR1_WDOG_STATUS_EXPIRED_STATUS_YES_FVAL                    0x1

//WDT_BITE_TIME Register
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR                             (APSS_WDT_TMR1_APSS_WDT_REG_BASE      + 0x00000014)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_PHYS                             (APSS_WDT_TMR1_APSS_WDT_REG_BASE_PHYS + 0x00000014)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_OFFS                             (APSS_WDT_TMR1_APSS_WDT_REG_BASE_OFFS + 0x00000014)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_RMSK                                0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR, HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_RMSK)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR, m)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR,v)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_ADDR,m,v,HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_IN)
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_DATA_BMSK                           0xfffff
#define HWIO_APSS_WDT_TMR1_WDOG_BITE_TIME_DATA_SHFT                               0x0

#endif /* __WATCHDOGLIBHWIO_H__ */