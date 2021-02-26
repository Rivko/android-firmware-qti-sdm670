/*                                                                    
                                                                   
                     TpmLocal.h   

  TPM definitions, copied from fTPM code.
 
  Copyright (c) 2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved


*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 12/10/13   sm       Updated variable types
 05/22/12   shl      Fixed the compiler upgrade issue. Make pact() change
 02/12/12   shl      Fixed a warning of without end line
 01/18/12   shl      Initial version

=============================================================================*/

#define YES                         1 
#define NO                          0  


#define TPM_CC_Clear                 (UINT32)(0x00000126)    
#define TPM_CC_ClearControl          (UINT32)(0x00000127)    

#define TPM_RS_PW                    (UINT32)(0x40000009) 
#define TPM_RH_PLATFORM              (UINT32)(0x4000000C)
#define TPM_ST_SESSIONS              (UINT16)(0x8002)
#define TPM_RC_SUCCESS               (UINT32)(0x000) 

#pragma pack (1)

typedef struct
{
  UINT16    size;
  UINT8     buffer[20];
} DIGEST;

typedef struct {
    UINT32 continueSession : 1;
    UINT32 auditExclusive  : 1;
    UINT32 auditReset      : 1;
    UINT32 reserved4       : 2;
    UINT32 decrypt         : 1;
    UINT32 encrypt         : 1;
    UINT32 audit           : 1;
} SESSION;

typedef struct {
    UINT32       sessionHandle;
    DIGEST       nonce;
    SESSION      sessionAttributes;
    DIGEST       auth;
} AUTH_SESSION_COMMAND;

typedef struct 
{
  UINT32  authHandle;
} Clear_In;

typedef struct
{
  UINT32   auth;
  UINT8    disable;
} ClearControl_In;

typedef struct 
{
  UINT16 tag;
  UINT32 paramSize;
  UINT32 commandCode;
} TPM2_COMMAND_HEADER;

typedef struct 
{
  UINT16 tag;
  UINT32 paramSize;
  UINT32 responseCode;
} TPM2_RESPONSE_HEADER;

typedef struct 
{
  TPM2_COMMAND_HEADER    header;
  AUTH_SESSION_COMMAND   authSessionCmd;
  Clear_In               inputParameters;
} TPM2ClearCmdType;

typedef struct 
{
  TPM2_RESPONSE_HEADER    header;
  AUTH_SESSION_COMMAND    authSessionCmd;
} TPM2ClearRespType;

typedef struct 
{
  TPM2_COMMAND_HEADER     header;
  AUTH_SESSION_COMMAND    authSessionCmd;
  ClearControl_In         inputParameters;
} TPM2ClearControlCmdType;

typedef struct 
{
  TPM2_RESPONSE_HEADER    header;
  AUTH_SESSION_COMMAND    authSessionCmd;
} TPM2ClearControlRespType;

#pragma pack ()
