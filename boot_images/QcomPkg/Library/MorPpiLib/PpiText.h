/*                                                                    
                                                                   
                      PpiText.h  

  PPI confrimation screen text definitions. 
 
  Copyright (c) 2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved


*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 05/10/13   sp       Updated display message
 03/25/13   shl      Modified message
 02/01/13   shl      Reorg message
 01/18/12   shl      Initial version

=============================================================================*/

// defintion of strings printed on PPI screen
CHAR8 *ClearLinePtr[] =
{  
  "                                   \r\n",
  "                                   \r\n",
  "A configuration change was requested to clear this computer's \r\n", 
  "TPM (Trusted Platform Module) \r\n",
  "                                   \r\n",
  "WARNING: Clearing erases information stored on the TPM. You \r\n",
  "will lose all created keys and access to data encrypted by \r\n",
  "these keys. \r\n",
  "                                   \r\n",
  "Press F12 or Volume Up to clear the TPM \r\n",
  "Press ESC or Volume Down to reject this change request and continue \r\n"
};

CHAR8 *NoPpiClearLinePtr[] = 
{
  "                                   \r\n",
  "                                   \r\n",
  "A configuration change was requested to allow the \r\n", 
  "Operating System to clear the computer's TPM \r\n",
  "(Trusted Platform Module) without asking for user \r\n",
  "confirmation in the future. \r\n",
  "                                   \r\n",
  "NOTE: This action does not clear the TPM, but by \r\n",
  "approving this configuration change, future actions \r\n",
  "to clear the TPM will not require user confirmation. \r\n",
  "                                   \r\n",
  "WARNING: Clearing erases information stored on the TPM. \r\n",
  "                                   \r\n",
  "You will lose all created keys and access to data encrypted \r\n"
  "by these keys. \r\n",
  "                                   \r\n",
  "Press F12 or Volume Up to approve future Operating System requests to clear the TPM \r\n",
  "Press ESC or Volume Down to reject this change request and continue \r\n"
}; 
