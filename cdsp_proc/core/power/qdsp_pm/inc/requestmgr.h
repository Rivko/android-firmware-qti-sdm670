/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * @file requestmgr.h
 * @brief request manager header file
 *
 * This file include type definition for request manager, it's used inside adsppm.
 *  Created on: Feb 13, 2012
 *      Author: alisonc
 */

#ifndef REQUESTMGR_H_
#define REQUESTMGR_H_

#include "adsppm.h"
#include "coreUtils.h"
#include "clientmgr.h"


/**
 * @enum RMValidFlagType - ADSPPM Request manager table valid status
 */
typedef enum
{
    RM_InValid                = 0,
    RM_Valid                  = 1,
    RM_Valid_Flag_Force32Bits = 0x7FFFFFFF //!< forcing enum to be 32 bit
} RMValidFlagType;

typedef enum
{
    NEW_REQUEST                  = 0, // Completely new or updated
    ACCEPTED_REQUEST             = 1, // Processed and accepted
    RELEASED_REQUEST             = 2, // Just released
    UNDEFINED_REQUEST            = 3, // Undefined / invalid
    RequestLifecycle_Force32Bits = 0x7FFFFFFF //!< forcing enum to be 32 bit
} AdsppmRequestLifecycleType;

/**
 * @struct RMHandleType - request manager context used to hold information for client and resource
 */
typedef struct
{
    coreUtils_Q_LinkType link;               //!< link to next element in list/queue
    RMValidFlagType      validFlag;          //!< Flag showing whether request data is valid
    RMValidFlagType      validFlag_Ext;      //!< Flag showing whether extended request data is valid
    uint32               clientId;
    AdsppmRscIdType      resourceId;
    const AdsppmClientType *pClient;         //!< Pointer to requesting client 
    void                 *pRequestData;      //!< Pointer to request data
    void                 *pRequestData_Ext;  //!< Pointer to request data extension if required
    AdsppmRequestLifecycleType lifecycleState;
    boolean              synchronousRequest; //!< True if client requested synchronous response; false if asynchronous
} RMHandleType;


/**
 * @fn RM_AddRequest - fill in the client’s request data for the specified resource and mark the request as valid.
 * @param input uint32 clientId - client ID.
 * @param input AdsppmRscIdType resourceId - resource ID that client request.
 * @param input void* pAdsppmReqData - pointer for request data, it'll cast to different data structure based on resource ID.
 * @param input uint32 extData - if non-zero extended data will be updated instead of request data.
 * @param input boolean syncRequest - TRUE if the client has requested synchronous response, FALSE if asynchronous
 * @return completion code
 */
Adsppm_Status RM_AddRequest(uint32 clientId, AdsppmRscIdType resourceId, void *pAdsppmReqData, uint32 extData, boolean syncRequest);

/**
 * @fn RM_ReleaseRequest - mark request for the client/resource pair as invalid.
 * @param clientId - ID of the client releasing the resource
 * @param resourceId - Resource ID being released
 * @param input uint32 extData - if non-zero extended data will be updated instead of request data.
 * @return completion status @see Adsppm_Status
 */
Adsppm_Status RM_ReleaseRequest(uint32 clientId, AdsppmRscIdType resourceId, uint32 extData);

/**
 * @fn send request to resource managers.
 * @param resourceId - Resource ID the request is made for.
 * @return completion status @see Adsppm_Status
 */
Adsppm_Status RM_RequestResource(AdsppmRscIdType resourceId);

/**
 * @fn RM_GetResourceInfo - get resource information, no need to register client
 * @param rsc_Id - Resource ID the get infor is made for
 * @param pInfo  - the pointer that pointer to information
 */
Adsppm_Status RM_GetResourceInfo(AdsppmRscIdType rsc_Id, void *pInfo, AdsppmInfoType infoType);

/**
 * @fn RM_Init - init RM, create resource mutex
 * @return Adsppm_Status
 */
Adsppm_Status RM_Init(void);

/**
 * Set up resource request state for a new client.  After successful completion
 * of this call, requests from the new client can be accepted.
 */
Adsppm_Status RM_SetupClient(const AdsppmClientType* client);

/**
 * Tear down resource request state for a client.  After successful completion
 * of this call, the client can be destroyed.
 */
Adsppm_Status RM_TeardownClient(uint32 clientId);

#endif /* REQUESTMGR_H_ */

