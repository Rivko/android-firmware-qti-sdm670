/*-----------------------------------------------------------------------------
   Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary.
-----------------------------------------------------------------------------*/

#ifndef SYSMON_QOS_CLOCK_H
#define SYSMON_QOS_CLOCK_H

typedef enum
{
    SYSMON_QOS_CLIENT_ID_QDSPPM = 0,
    /**< Client ID to be used by QDSPPM when requesting for QoS enable */    
    
    SYSMON_QOS_CLIENT_ID_SYSMON = 1,
    /**< Client ID to be used by SysMon when requesting for QoS enable */
} sysmonQosClientId_t;

/******************************************************************************
 * Function: sysmon_qos_clock_enable
 * Description:
 *      Enable/Disable QoS clock which is needed for danger and external 
 *      throttle.
 * Params: 
 *      enable: 1 - for enabling QOS clock, 0 - for disabling QoS clock
 * Return:
 *      0 - for success
 *      -1 - for failure
******************************************************************************/
int sysmon_qos_clock_enable(unsigned char enable);

/******************************************************************************
 * Function: sysmon_qos_clock_enable_clientid
 * Description:
 *      Enable/Disable QoS clock which is needed for danger and external 
 *      throttle.
 * Params: 
 *      clientId: appropriate enum from sysmonQosClientId_t
 *      enable: 1 - for enabling QOS clock, 0 - for disabling QoS clock
 * Return:
 *      0 - for success
 *      -1 - for failure
******************************************************************************/
int sysmon_qos_clock_enable_clientid(sysmonQosClientId_t clientId, 
                                     unsigned char enable);

#endif //SYSMON_QOS_CLOCK_H