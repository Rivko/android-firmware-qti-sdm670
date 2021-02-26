/*
==============================================================================

FILE:         rsc_hal_internal.h

DESCRIPTION:  This file contains description about RSC Hal. Rsc HAL is mainly responsible for 
              managing TCSs/AMCs associcated with RSC hw block on each subsystem


==============================================================================

                             Edit History


==============================================================================
   Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#ifndef _HAL_RSC_INTERNAL
#define _HAL_RSC_INTERNAL


#define DRV_NUM_TCS_OFFSET                  (0x3F)
#define DRV_NUM_TCS_BITS                    (6)
#define DRV0_NUM_CMDS_PER_TCS               (0xF8000000)

#define DRV0_CLEAR_IRQ_STATUS               (0x1)
#define DRV0_ENABLE_IRQ_STATUS              (0X1)

#define WRITE_REQUEST                       (0x1)
#define READ_REQUEST                        (0x0)

#define FIRE_FORGET_RESPONSE                (0x0)
#define COMPLETION_RESPONSE                 (0x1)

#define GET_REQUEST_TYPE(x)                (((x)< ReadRequest) ? WRITE_REQUEST:READ_REQUEST)
#define GET_RESPONSE_TYPE(x)               (((x)== WriteFirenForget) ? FIRE_FORGET_RESPONSE:COMPLETION_RESPONSE)

#define DRV_ID_0                            (0)
#define DRV_ID_1                            (1)
#define DRV_ID_2                            (2)


#define AMC_ENABLE_FINISH_IRQ              (0x1)
#define AMC_TRIGGER_BIT                    (0x1)
#define AMC_CONVERSION_BIT                 (0x1)

#define AMC_CLEAR_BIT                      0x0
#define AMC_SET_BIT                        0x1


/**
* Helper pre-processor
**/

#define IS_DRV_VALID(d)                             (((d) < RSC_DRV_MAX)? HAL_STATUS_SUCCESS:HAL_INVALID_DRV_MAP_ID)
#define IS_TCS_VALID(t,n)                           (((t) < n)? HAL_STATUS_SUCCESS:HAL_INVALID_TCS_ID)
#define IS_CMD_VALID(c,n)                           (((c) < n)? HAL_STATUS_SUCCESS:HAL_INVALID_CMD_ID)


#define RSC_HAL_SHFT(subsystem, name, shift)                                        subsystem##name##shift

#define RSC_HAL_BIT_SHFT(platform,name, shift)                                      RSC_HAL_SHFT(platform, name, shift)

#define RSC_HAL_BMSK(subsystem, name, mask)                                         subsystem##name##mask

#define RSC_HAL_BIT_MSK(platform,name,mask)                                         RSC_HAL_BMSK(platform, name, mask)


#define RSC_HAL_HWIO_INId_RM(subsystem,name,add,drvId, mask)            subsystem##name##_INMI (\
                                                                add,\
                                                                drvId,\
                                                                                                        RSC_HAL_BMSK(subsystem, name, mask))


#define RSC_HAL_HWIO_INId_READ_MSK(platform,name,add,drvId, mask)                   RSC_HAL_HWIO_INId_RM(platform,name,add,drvId, mask)

#define RSC_HAL_HWIO_CMD_INId_RM(subsystem,name,add,tcsid,cmdid, mask) subsystem##name##_INMI2 (\
                                                                                                 add,\
                                                                                                 tcsid,\
                                                                                                 cmdid,\
                                                                                                 RSC_HAL_BMSK(subsystem, name, mask))


#define RSC_HAL_HWIO_CMD_INId_READ_MASK(platform,name,add,tcsid,cmdid, mask)        RSC_HAL_HWIO_CMD_INId_RM(platform,name,add,tcsid,cmdid, mask)

#define RSC_HAL_HWIO_INId_R(subsystem,name,add,drvId)                   subsystem##name##_INI (\
                                                                                                 add,\
                                                                                                 drvId\
                                                                                                 )                                                                                                 


#define RSC_HAL_HWIO_INId_READ(platform,name,add,drvId)                             RSC_HAL_HWIO_INId_R(platform,name,add,drvId)

#define RSC_HAL_HWIO_INM_RM(subsystem,name,add, mask)                               subsystem##name##_INM (\
                                                                                                            add,\
                                                                                                            RSC_HAL_BMSK(subsystem, name, mask))    

#define RSC_HAL_HWIO_INM_READ(platform,name,add,mask)                               RSC_HAL_HWIO_INM_RM(platform,name,add,mask)

#define RSC_HAL_HWIO_IN_R(subsystem,name,add)                                       subsystem##name##_IN (add)

#define RSC_HAL_HWIO_IN_READ(platform,name,add)                                     RSC_HAL_HWIO_IN_R(platform,name,add)

#define RSC_HAL_HWIO_OUT_W(subsystem,name,add, val)                                 subsystem##name##_OUT (add, val)

#define RSC_HAL_HWIO_OUT_WRITE(platform,name,add, val)                              RSC_HAL_HWIO_OUT_W (platform,name,add, val)

#define RSC_HAL_HWIO_OUT_WM(subsystem,name,add, mask, val)                          subsystem##name##_OUTM (add, mask, val)

#define RSC_HAL_HWIO_OUT_WRITE_MSK(platform,name,add,mask, val)                     RSC_HAL_HWIO_OUT_WM (platform,name,add,mask, val)

#define RSC_HAL_HWIO_TCS_OUT_WM(subsystem,name,add, tcsid, mask, val)               subsystem##name##_OUTMI (add, tcsid, mask, val)

#define RSC_HAL_HWIO_TCS_OUT_WRITE_MSK(platform,name,add,tcsid, mask, val)          RSC_HAL_HWIO_TCS_OUT_WM (platform,name,add,tcsid, mask, val)

#define RSC_HAL_HWIO_TCS_OUT_W(subsystem,name,add,tcsid, val)                       subsystem##name##_OUTI (add,tcsid, val)

#define RSC_HAL_HWIO_TCS_OUT_WRITE(platform,name,add,tcsid,val)                     RSC_HAL_HWIO_TCS_OUT_W (platform,name,add,tcsid,val)

#define RSC_HAL_HWIO_TCS_CMD_OUT_WM(subsystem, name,add, tcsid,cmdid, mask, val)   subsystem##name##_OUTMI2 (add, tcsid, cmdid, mask, val)

#define RSC_HAL_HWIO_TCS_CMD_OUT_WRITE(platform, name,add,tcsid,cmdid,mask,val)   RSC_HAL_HWIO_TCS_CMD_OUT_WM(platform,name,add, tcsid,cmdid, mask, val)



#endif
