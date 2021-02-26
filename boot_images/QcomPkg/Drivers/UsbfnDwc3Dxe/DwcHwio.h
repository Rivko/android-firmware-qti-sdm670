/**
 * @file DwcHwio.h
 *
 * @ingroup CommonLayer
 *
 * @brief Common layer hardware access abstraction.
 *
 * Provides macros for abstracting access to the Snopsys USB core's hardware registers.
 *
 * @copyright Copyright (C) 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
#ifndef _DWC_HWIO_H_
#define _DWC_HWIO_H_

#include "DwcPlatform.h"
#include "HALdwcHWIO.h"

/*******************************************************************************
 * Register access macros. These may need to be tweaked depending on the format
 * of the included hwio header.
 ******************************************************************************/

#define DWC_ADDR(base,hwiosym)                        HWIO_##hwiosym##_ADDR(base)
#define DWC_ADDRI(base,hwiosym,index)                 HWIO_##hwiosym##_ADDR(base,index)

#define DWC_OFFS(hwiosym)                             HWIO_##hwiosym##_OFFS
#define DWC_OFFSI(hwiosym,index)                      HWIO_##hwiosym##_OFFS(0,index)

#define DWC_BMSK(hwiosym,field)                       HWIO_##hwiosym##_##field##_BMSK
#define DWC_SHFT(hwiosym,field)                       HWIO_##hwiosym##_##field##_SHFT


#define DWC_HWIO_IN(base,hwiosym)                     DWC_PLAT_READ_REG(DWC_ADDR(base,hwiosym))
#define DWC_HWIO_INI(base,hwiosym,index)              DWC_PLAT_READ_REG(DWC_ADDRI(base,hwiosym,index))

#define DWC_HWIO_OUT(base,hwiosym,val)                DWC_PLAT_WRITE_REG(DWC_ADDR(base,hwiosym), val)
#define DWC_HWIO_OUTI(base,hwiosym,index,val)         DWC_PLAT_WRITE_REG(DWC_ADDRI(base,hwiosym,index),val)

#define DWC_HWIO_INF(base,hwiosym,field)              ((DWC_HWIO_IN(base,hwiosym) & DWC_BMSK(hwiosym,field)) >> DWC_SHFT(hwiosym,field))
#define DWC_HWIO_INFI(base,hwiosym,index,field)       ((DWC_HWIO_INI(base,hwiosym,index) & DWC_BMSK(hwiosym,field)) >> DWC_SHFT(hwiosym,field))

#define DWC_HWIO_OUTF(base,hwiosym,field,val)         DWC_HWIO_OUT(base,hwiosym,((DWC_HWIO_IN(base,hwiosym) & (~(DWC_BMSK(hwiosym,field)))) | ((val<<DWC_SHFT(hwiosym,field)) & (DWC_BMSK(hwiosym,field)))))
#define DWC_HWIO_OUTFI(base,hwiosym,index,field,val)  DWC_HWIO_OUTI(base,hwiosym,index,((DWC_HWIO_INI(base,hwiosym,index) & (~(DWC_BMSK(hwiosym,field)))) | ((val<<DWC_SHFT(hwiosym,field)) & (DWC_BMSK(hwiosym,field)))))



#endif /* _DWC_DWC_H_ */
