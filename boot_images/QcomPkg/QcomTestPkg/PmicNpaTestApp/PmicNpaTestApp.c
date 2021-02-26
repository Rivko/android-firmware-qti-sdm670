/*============================================================================
PmicNpaTestApp.c

A very basic example of Pmic NPA Test use. 

Copyright (c) 2013 - 2015, Qualcomm Technologies Inc. All rights reserved. 
All Rights Reserved.
QUALCOMM Proprietary

=======================================================================*/

/* =======================================================================

                                          Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
06/25/15    al      Porting changes
06/13/14    va      Updating HDMI EXT NPA node and PCIE 
05/23/14    va      Fail safe when Npa resource are already acquired by destined clients (USB and Display)
04/28/14    va      Expose protocol to call npa test init for tests run
4/03/14     va      Fix for calling this application multiple time and for Display client
1/07/13     va      new File

============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/EFIPmicVersion.h>

#include <Library/TestInterface.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
//Test protocol header 
#include <Protocol/EFIPmicNpaTest.h>

EFI_STATUS PmicNpaTestApp_createTestClients(boolean createClient);

//npa header 
#include "npa.h"
#include "pmapp_npa.h"
EFI_STATUS PmicNpaTestApp_usb(boolean issueRequest);
EFI_STATUS PmicNpaTestApp_display(boolean issueRequest);
EFI_STATUS PmicNpaTestApp_pcie( boolean issueRequest);
EFI_STATUS PmicNpatestApp_ProcessCommand (UINTN Argc, CHAR8** Argv, boolean *createClient, boolean *issueRequest);

static EFI_QCOM_PMIC_VERSION_PROTOCOL       *PmicVersionProtocol = NULL;
static EFI_PM_DEVICE_INFO_TYPE         deviceInfo;
/*----------------------------------------------------------------------------
 #Defines 
----------------------------------------------------------------------------*/

#define GOTOERRORIFNULL(Str,Status,Error_Exit) if (NULL == Str){ Status = EFI_OUT_OF_RESOURCES; goto Error_Exit; }
#define GOTOERROR(Status,Error_Exit) if (EFI_SUCCESS != Status ) { goto Error_Exit; } 

/*----------------------------------------------------------------------------
 main
----------------------------------------------------------------------------*/
  EFI_STATUS
  EFIAPI
  PmicNpaTestAppMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
  {
    int status;
    UINTN  Argc;
    CHAR8  **Argv = NULL;
    boolean  createClient = FALSE, issueRequest = FALSE;

    //Act on cmd line arguments if fails continue for charge
    status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
    
    if(Argc > 0 )//Act only if arguments are present
    {
      AsciiPrint("Setting Arguments \r\n");
      status = PmicNpatestApp_ProcessCommand(Argc, Argv, &createClient, &issueRequest);
    }else
    {
      AsciiPrint("Arguments not provided \r\n");
    }

    TEST_START("NPA");

    AsciiPrint("PmicNpaTestApp: Try Creating Test Clients \r\n ");
    status = PmicNpaTestApp_createTestClients(createClient);

    AsciiPrint("PmicNpaTestApp: Test USB NPA request \r\n ");

    status = PmicNpaTestApp_usb(issueRequest);
    AsciiPrint("PmicNpatestApp: USB Return:%d.\r\n", status);

    AsciiPrint("PmicNpaTestApp: Test Display & EXT_HDMI Request \r\n ");

    status = PmicNpaTestApp_display(issueRequest);
    AsciiPrint("PmicNpatestApp: Display Return:%d.\r\n", status);

    status = PmicNpaTestApp_pcie(issueRequest);
    AsciiPrint("PmicNpatestApp: PCIE Return:%d.\r\n", status);

    TestStatus("NPA",status);
    TEST_STOP("NPA");

    (void)PmicVersionProtocol;
    (void)deviceInfo;

    return EFI_SUCCESS;

 }

EFI_STATUS PmicNpaTestApp_usb( boolean issueRequest)
{
  EFI_STATUS Status = EFI_SUCCESS;
  // handle for the client.
  npa_client_handle reqClient = NULL;
  npa_query_type    query_result;
  npa_query_status  query_status = NPA_QUERY_NO_VALUE;

  query_status = npa_query_by_name(PMIC_NPA_GROUP_ID_USB_HS1, NPA_QUERY_CURRENT_STATE, &query_result);
  if (NPA_QUERY_SUCCESS == query_status){
    AsciiPrint("PmicNpaTestApp: NPA USB resouce node Exists \r\n ");
    //issue request only when it is asked
    if (TRUE == issueRequest){
      //Query client and issue request only when indicated by arguments
      reqClient =  npa_create_sync_client(
                     PMIC_NPA_GROUP_ID_USB_HS1,          // Connect to the usb hs1 resource
                     "usb",// Descriptive name
                     NPA_CLIENT_REQUIRED); /* Your client type --  types can be:
                                            * NPA_CLIENT_REQUIRED: Request must honor request 
                                            * NPA_CLIENT_IMPULSE: Poke the resource - node defines the impulse response
                                            * NPA_CLIENT_LIMIT_MAX: Set maximum value for the resource
                                            * NPA_CLIENT_VECTOR: Vector request
                                            * NPA_CLIENT_SUPPRESSIBLE: Suppressible Scalar request -- requests 
                                            *                          honored only in active set
                                            * NPA_CLIENT_SUPPRESSIBLE_VECTOR: Suppressible Vector request 
                                            */

      if ( NULL == reqClient)
      {
        AsciiPrint("PmicNpaTestApp: Failed to retrieve NPA USB Handle \r\n ");
        return EFI_DEVICE_ERROR;
      }
      AsciiPrint("PmicNpaTestApp: NPA USB issuing NPA Request \r\n ");
      // Once the client is valid, we can issue a request.  Since the plugin is
      // the identity plugin, our driver will always be called. 
      npa_issue_required_request(reqClient, PMIC_NPA_MODE_ID_GENERIC_ACTIVE );   //Sends Activate to the driver

      npa_issue_required_request(reqClient, PMIC_NPA_MODE_ID_GENERIC_STANDBY);//Sends the driver to LPM1/standby
      //complete the request  
      npa_complete_request(reqClient);           //Sends the driver a 0

      npa_destroy_client(reqClient);
    }
    else{
      AsciiPrint("PmicNpaTestApp: NPA USB request not issued \r\n ");
      Status = EFI_SUCCESS;
    }
  }
  else{
    AsciiPrint("PmicNpaTestApp: NPA USB Query Resource by Name Failed query_status = (%d)\r\n ", query_status);
    Status = EFI_DEVICE_ERROR;
  }
  
  return Status;
}


EFI_STATUS PmicNpaTestApp_display( boolean issueRequest )
{
  EFI_STATUS Status = EFI_SUCCESS;
  // handle for the client.
  npa_client_handle reqClientDisplay        = NULL;
  npa_client_handle reqClientDisplayExtHDMI = NULL;
  npa_query_status  query_status = NPA_QUERY_NO_VALUE;
  npa_query_type    query_result;

  query_status = npa_query_by_name(PMIC_NPA_GROUP_ID_DISP_PRIM, NPA_QUERY_CURRENT_STATE, &query_result);
  if (NPA_QUERY_SUCCESS == query_status){
    AsciiPrint("PmicNpaTestApp: NPA DISPLAY resouce node Exists \r\n ");
    //issue request only when it is asked
    if (TRUE == issueRequest){

       reqClientDisplay =  npa_create_sync_client(
                           PMIC_NPA_GROUP_ID_DISP_PRIM, // Connect to the usb hs1 resource
                           "display", //"usb",   // Descriptive name
                           NPA_CLIENT_REQUIRED); /* Your client type --  types can be:
                                                  * NPA_CLIENT_REQUIRED: Request must honor request 
                                                  * NPA_CLIENT_IMPULSE: Poke the resource - node defines the impulse response
                                                  * NPA_CLIENT_LIMIT_MAX: Set maximum value for the resource
                                                  * NPA_CLIENT_VECTOR: Vector request
                                                  * NPA_CLIENT_SUPPRESSIBLE: Suppressible Scalar request -- requests 
                                                  *                          honored only in active set
                                                  * NPA_CLIENT_SUPPRESSIBLE_VECTOR: Suppressible Vector request 
                                                  */

       if ( NULL == reqClientDisplay)
       {
         AsciiPrint("PmicNpaTestApp: Failed to retrieve NPA Display Handle \r\n ");
         return EFI_DEVICE_ERROR;
       }
      // Once the client is valid, we can issue a request.  Since the plugin is
      // the identity plugin, our driver will always be called. 
      npa_issue_required_request(reqClientDisplay, PMIC_NPA_MODE_ID_GENERIC_ACTIVE );   //Sends the driver to LPM1

      npa_issue_required_request(reqClientDisplay , PMIC_NPA_MODE_ID_GENERIC_STANDBY);
      //complete the request  
      npa_complete_request(reqClientDisplay);           //Sends the driver a 0

      npa_destroy_client(reqClientDisplay);
    }    
    else{
      AsciiPrint("PmicNpaTestApp: NPA DISPLAY request not issued \r\n ");
      Status = EFI_SUCCESS;
    }
  }
  else{
    AsciiPrint("PmicNpaTestApp: NPA DISPLAY Query Resource by Name Failed query_status = (%d)\r\n ", query_status);
    Status = EFI_DEVICE_ERROR;
  }

  query_status = npa_query_by_name(PMIC_NPA_GROUP_ID_DISP_EXT_HDMI, NPA_QUERY_CURRENT_STATE, &query_result);
  if (NPA_QUERY_SUCCESS == query_status){
    AsciiPrint("PmicNpaTestApp: NPA DISPLAY EXT HDMI Resouce node Exists \r\n ");
    //issue request only when it is asked
    if (TRUE == issueRequest){
      reqClientDisplayExtHDMI =  npa_create_sync_client(
                                 PMIC_NPA_GROUP_ID_DISP_EXT_HDMI,
                                 "displayexthdmi", //"usb",   // Descriptive name
                                 NPA_CLIENT_REQUIRED); /* Your client type --  types can be:
                                                        * NPA_CLIENT_REQUIRED: Request must honor request 
                                                        * NPA_CLIENT_IMPULSE: Poke the resource - node defines the impulse response
                                                        * NPA_CLIENT_LIMIT_MAX: Set maximum value for the resource
                                                        * NPA_CLIENT_VECTOR: Vector request
                                                        * NPA_CLIENT_SUPPRESSIBLE: Suppressible Scalar request -- requests 
                                                        *                          honored only in active set
                                                        * NPA_CLIENT_SUPPRESSIBLE_VECTOR: Suppressible Vector request 
                                                        */
      
         if ( NULL == reqClientDisplayExtHDMI)
         {
           AsciiPrint("PmicNpaTestApp: Failed to retrieve NPA Display Ext HDMI Handle \r\n ");
           return EFI_DEVICE_ERROR;
         }
        // Once the client is valid, we can issue a request.  Since the plugin is
        // the identity plugin, our driver will always be called. 
        npa_issue_required_request(reqClientDisplayExtHDMI , PMIC_NPA_MODE_ID_GENERIC_ACTIVE );

        npa_issue_required_request(reqClientDisplayExtHDMI , PMIC_NPA_MODE_ID_GENERIC_STANDBY);   //Sends the driver to LPM1
        //complete the request
        npa_complete_request(reqClientDisplayExtHDMI);           //Sends the driver a 0

        npa_destroy_client(reqClientDisplayExtHDMI);
    }
    else{
      AsciiPrint("PmicNpaTestApp: NPA DISPLAY EXT HDMI request not issued \r\n ");
      Status = EFI_SUCCESS;
    }
  }
  else{
    AsciiPrint("PmicNpaTestApp: NPA DISPLAY EXT HDMI Query Resource by Name Failed query_status = (%d)\r\n ", query_status);
    Status = EFI_DEVICE_ERROR;
  }
  return Status;
}

EFI_STATUS PmicNpaTestApp_pcie( boolean issueRequest)
{
  EFI_STATUS Status = EFI_SUCCESS;
  // handle for the client.
  npa_client_handle reqClient = NULL;
  npa_query_type    query_result;
  npa_query_status  query_status = NPA_QUERY_NO_VALUE;

  query_status = npa_query_by_name(PMIC_NPA_GROUP_ID_PCIE, NPA_QUERY_CURRENT_STATE, &query_result);
  if (NPA_QUERY_SUCCESS == query_status){
    AsciiPrint("PmicNpaTestApp: NPA PCIE resouce node Exists \r\n ");
    //issue request only when it is asked
    if (TRUE == issueRequest){
      //Query client and issue request only when indicated by arguments
      reqClient =  npa_create_sync_client(
                     PMIC_NPA_GROUP_ID_PCIE,          // Connect to the PCIE  resource
                     "pcie",// Descriptive name
                     NPA_CLIENT_REQUIRED); /* Your client type --  types can be:
                                            * NPA_CLIENT_REQUIRED: Request must honor request 
                                            * NPA_CLIENT_IMPULSE: Poke the resource - node defines the impulse response
                                            * NPA_CLIENT_LIMIT_MAX: Set maximum value for the resource
                                            * NPA_CLIENT_VECTOR: Vector request
                                            * NPA_CLIENT_SUPPRESSIBLE: Suppressible Scalar request -- requests 
                                            *                          honored only in active set
                                            * NPA_CLIENT_SUPPRESSIBLE_VECTOR: Suppressible Vector request 
                                            */

      if ( NULL == reqClient)
      {
        AsciiPrint("PmicNpaTestApp: Failed to retrieve NPA PCIE Handle \r\n ");
        return EFI_DEVICE_ERROR;
      }
      AsciiPrint("PmicNpaTestApp: NPA PCIE issuing NPA Request \r\n ");
      // Once the client is valid, we can issue a request.  Since the plugin is
      // the identity plugin, our driver will always be called. 
      npa_issue_required_request(reqClient, PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL );   //Sends Activate to the driver

      npa_issue_required_request(reqClient, PMIC_NPA_MODE_ID_PCIE_OFF);//Sends the driver to LPM1/standby
      //complete the request  
      npa_complete_request(reqClient);           //Sends the driver a 0

      npa_destroy_client(reqClient);
    }
    else{
      AsciiPrint("PmicNpaTestApp: NPA PCIE request not issued \r\n ");
      Status = EFI_SUCCESS;
    }
  }
  else{
    AsciiPrint("PmicNpaTestApp: NPA PCIE Query Resource by Name Failed query_status = (%d)\r\n ", query_status);
    Status = EFI_DEVICE_ERROR;
  }
  
  return Status;
}


/**
  Duplicates ASCII string 
  
  @param  Source  Null Terminated string of characters 
  @param  Length  source String length  
  @return  Duplicated String  if successful otherwise return NULL
  
  note: Caller responsibility to Free returned String
**/
CHAR8 * AsciiStrNDup(
  CONST CHAR8               *Source,
  UINTN                     Length
)
{
  CHAR8 *Dest = NULL;
  if(NULL == Source || 0 == Length)
    return NULL;

  Dest = AllocatePool (Length + 1);
  if (NULL == Dest)
  {
    return NULL;
  }

  AsciiStrnCpy(Dest, Source, Length + 1);

  return Dest;
}

/**
  PmicNpatestApp_parseInputArguments()
  
  @brief
..parse command line arguments into array of arguments
**/
EFI_STATUS PmicNpatestApp_parseInputArguments(CHAR8 *CmdStr, CHAR8 *arg_Array[])
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR8 *OrgStr = NULL, *temp = NULL;
  CHAR8 *firstArg = NULL, *SecArgument = NULL;
  UINT8 count = 0, i = 0;

  if (!CmdStr)
    return EFI_INVALID_PARAMETER;

  OrgStr = AsciiStrNDup(CmdStr, AsciiStrLen(CmdStr));
  GOTOERRORIFNULL(OrgStr,Status,Error_Exit);

  temp = OrgStr;

  temp = AsciiStrStr(temp, ",");
  if(NULL != temp)
  {
     //copy first argument
     count = temp - OrgStr;
     firstArg = (CHAR8 *)AllocatePool(count + 1);
     GOTOERRORIFNULL(firstArg,Status,Error_Exit);
    while(i != count)
    {
      *(firstArg + i) = *(OrgStr + i);
      i++;
    }
    *(firstArg + i ) = '\0';
    arg_Array[0] = firstArg;
    //copy first argument
    temp++;
    SecArgument = AsciiStrNDup(temp, AsciiStrLen(temp));
    GOTOERRORIFNULL(SecArgument,Status,Error_Exit);
    arg_Array[1] = SecArgument;
  }else
  {
    AsciiPrint("Bad Parameter");
  }

Error_Exit:
  FreePool(OrgStr);
  OrgStr = NULL;

  return Status;

}

#define MAX_ARGUMENTS 2
/**
  PmicNpatestApp_ProcessCommand()
  
  @brief
..Act on command line arguments
**/
EFI_STATUS PmicNpatestApp_ProcessCommand (UINTN Argc, CHAR8** Argv, boolean *createClient, boolean *issueRequest)
{
  EFI_STATUS  Status   = EFI_SUCCESS;
  CHAR8       *CmdStr  = NULL;
  CHAR8       *arg_Array[MAX_ARGUMENTS];

  if(NULL == *Argv || Argc == 0){
    return EFI_INVALID_PARAMETER;
  }

  CmdStr = Argv[0];

  AsciiPrint ("Processing Cmd: %a", CmdStr);
  AsciiPrint ("\r\n");

  Status = PmicNpatestApp_parseInputArguments(CmdStr, arg_Array);
  GOTOERROR(Status,Error_Exit);

  if((0 == AsciiStrCmp(arg_Array[0], "cc") )|| (0 == AsciiStrCmp(arg_Array[0], "CC")) || (1 == AsciiToInt(arg_Array[0])))
   *createClient = TRUE;
  else
   *createClient = FALSE;
  if ((0 == AsciiStrCmp(arg_Array[1], "ir") )|| (0 == AsciiStrCmp(arg_Array[1], "IR")) || (1 == AsciiToInt(arg_Array[1])))
    *issueRequest = TRUE;
  else
    *issueRequest = FALSE;

  FreePool(arg_Array[0]);
  arg_Array[0] = NULL;

Error_Exit:
  return Status;
}

EFI_STATUS PmicNpaTestApp_createTestClients(boolean createClient)
{
  EFI_QCOM_PMIC_NPATEST_PROTOCOL  *pNpaTestProtocol = NULL;
  EFI_STATUS                       Status           = EFI_SUCCESS;
  PMIC_NPATEST_RESOURCEINFO        pamResourceInfo;

  if ( TRUE ==  createClient ){
    /* Get Test Protocol handle Info */
    Status = gBS->LocateProtocol(&gQcomPmicNpaTestProtocolGuid,
                                NULL,
                                (VOID **)&pNpaTestProtocol);
    if (EFI_SUCCESS != Status || NULL == pNpaTestProtocol){
      AsciiPrint("Could not get NPA Test Protocol \r\n");
      return Status;
    }
    Status = pNpaTestProtocol->PmicNpaTestInit();
    AsciiPrint("Creating Test Clients Now \r\n ");
    Status = pNpaTestProtocol->CreateTestClients();
    AsciiPrint("Retrieving  Test Clients Info \r\n ");
    Status = pNpaTestProtocol->GetResourceInfo(&pamResourceInfo);
    AsciiPrint("Num of Test Node (%d), Resource Test Name (%a) \r\n ", pamResourceInfo.num_of_test_nodes, pamResourceInfo.rsrc_test_name);
  }else{
    AsciiPrint("Not Creating test Clients \r\n");
  }

  return Status;

}


