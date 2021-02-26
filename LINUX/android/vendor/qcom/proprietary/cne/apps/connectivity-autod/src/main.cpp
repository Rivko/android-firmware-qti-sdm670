/**
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <cutils/log.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>
#include "dsi_netctrl.h"
#include "qmi.h"
#include "InetAddr.h"

#define LOG_TAG "data-autod"

#define DSI_IP_FAMILY_4   "IP"
#define DSI_IP_FAMILY_6   "IPV6"
#define DSI_IP_FAMILY_4_6 "IPV4V6"

#define DSI_PROFILE_3GPP2_OFFSET (1000)

#define SASTORAGE_DATA(addr)    (addr).__data

#define MAX_RING_SIZE 3

#define EVENT_NONE 0
#define EVENT_IDLE 1
#define EVENT_CONNECTED 2
#define EVENT_UNKNOWN 3

#define NDC_ACTION_NETWORK_UP 0
#define NDC_ACTION_NETWORK_DOWN 1
#define NDC_ACTION_CALL_UP 2
#define NDC_ACTION_CALL_DOWN 3

#define SUBNET_MAX_LENGTH (INET_ADDRSTRLEN + 4)

int write_idx = 0;
int read_idx = 0;

int events[MAX_RING_SIZE];

static const char* ndcPath = "/system/bin/ndc";
static const char* netId = "25";

pthread_mutex_t autod_ring_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t autod_ring_cond = PTHREAD_COND_INITIALIZER;

char device_name[DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 1];
int hasCall = -1;
int index = 0;
int isDeviceObtained = DSI_ERROR;
int isIpInfoObtained = DSI_ERROR;
pid_t child_pid;
pid_t grand_child_pid;
int status;
char mSubnetIPV4AddrFormatted[INET_ADDRSTRLEN + 4] = {0};

typedef struct {
  dsi_hndl_t handle;
  const char* tech;
  const char* family;
  int profile;
} dsi_test_call_info_t;

char mInterfaceIPV4Addr[INET_ADDRSTRLEN];
char mGatewayIPV4Addr[INET_ADDRSTRLEN];
char mDnspIPV4Addr[INET_ADDRSTRLEN];
char mDnssIPV4Addr[INET_ADDRSTRLEN];
char mSubnetMaskIPV4Addr[INET_ADDRSTRLEN + 4];

int profileID = 0; //Default value

dsi_test_call_info_t dsi_net_hndl[1];
void daemonNetCtrlCB (dsi_hndl_t hndl, void *user_data, dsi_net_evt_t evt, dsi_evt_payload_t *payload_ptr);

/*------------------------------------------------------------------------------
 * FUNCTION      putEvent
 *
 * DESCRIPTION   Adds the incoming event onto the event queue and issues the
 *               condition signal
 *
 * RETURN VALUE  - bool
 *
 * SIDE EFFECTS  -
 *----------------------------------------------------------------------------*/
bool putEvent(int event) {
  pthread_mutex_lock(&autod_ring_mutex);
  ALOGI(" putEvent(%d) write=%d read=%d", event, write_idx, read_idx);
  bool ret_val = ((write_idx + 1) % MAX_RING_SIZE) != read_idx;
  if (ret_val) {
    events[write_idx] = event;
    write_idx = (write_idx + 1) % MAX_RING_SIZE;
    pthread_cond_signal(&autod_ring_cond);
  }
  pthread_mutex_unlock(&autod_ring_mutex);
  return ret_val;
}

/*------------------------------------------------------------------------------
 * FUNCTION      getEvent
 *
 * DESCRIPTION   Waits on a Condition Signal
 *
 * RETURN VALUE  - (int) Event that it receives
 *
 * SIDE EFFECTS  -
 *----------------------------------------------------------------------------*/
int getEvent() {
  pthread_mutex_lock(&autod_ring_mutex);
  ALOGI(" getEvent() write=%d read=%d", write_idx, read_idx);
  while (write_idx == read_idx) {
    pthread_cond_wait(&autod_ring_cond, &autod_ring_mutex);
  }
  int ret_val = events[read_idx];
  read_idx = (read_idx + 1) % MAX_RING_SIZE;
  pthread_mutex_unlock(&autod_ring_mutex);
  return ret_val;
}

/*------------------------------------------------------------------------------
 * FUNCTION      getAddrAsInt
 *
 * RETURN VALUE  - (int) Address
 *----------------------------------------------------------------------------*/
uint32_t getAddrAsInt(uint data[], int size) {
    if (data == NULL || size < 4) {
      ALOGE ("ERROR invalid inputs");
      return 0;
    }
    return (data[0]<<24) | (data[1]<<16) | (data[2]<<8) | data[3];
}

/*------------------------------------------------------------------------------
 * FUNCTION      createBitMask
 * DESCRIPTION   pass prefix length/mask size as argument ; Returns integer value
 *               of mask
 *
 * RETURN VALUE  - (int) Bitmask
 *----------------------------------------------------------------------------*/
uint32_t createBitMask(unsigned int masksize) {
    if (masksize > 32) {
      ALOGE ("ERROR mask size too large");
      return 0;
    }
    return (~0x0) << (32 - masksize);
}

/*------------------------------------------------------------------------------
 * FUNCTION      getAddrAsStr
 * DESCRIPTION   Pass args:
 *               retAddr pointer - to store the address as String
 *               size - to pass as arg to snprintf and validate buffer overflow
 *               addr - integer address the needs to be converted
 *
 * RETURN VALUE  - bool : false - if buffer overflows
 *----------------------------------------------------------------------------*/
bool getAddrAsStr(char *retAddr, int size, uint32_t addr) {
    if (retAddr == NULL || size < INET_ADDRSTRLEN) {
      ALOGE ("ERROR Invalid inputs");
      return false;
    }
    int ret = snprintf(retAddr, size, "%d.%d.%d.%d", (addr>>24)&0xff, (addr>>16)&0xff, (addr>>8)&0xff, addr&0xff);
    ALOGI("Subnet Address %s", retAddr);
    return ret < size;
}

/*------------------------------------------------------------------------------
 * FUNCTION      getSubnetMaskAddr
 * DESCRIPTION   Pass args:
 *               ipaddr_ptr - IP Address
 *               subnetMask - subnet mask size or prefix length
 *               Calculates subnet mask address and store it in global value
 *
 * RETURN VALUE  - bool
 *----------------------------------------------------------------------------*/
bool getSubnetMaskAddr(char *ipaddr_ptr, unsigned int subnetMask) {
  char mSubnetIPV4Addr[INET_ADDRSTRLEN] = {0};
  bool ret_val = false;
  uint ip_addr[4];
  unsigned long int subnet_addr = 0;

  ALOGI ("Get addr as int");
  if(ipaddr_ptr == NULL | subnetMask > 32) {
    ALOGE ("ERROR Invalid inputs");
  }

  for (int i = 0; i < 4 ; i++) {
    ip_addr[i] = ipaddr_ptr[i];
  }

  uint32_t addr = getAddrAsInt(ip_addr, 4);
  uint32_t mask = createBitMask(subnetMask);
  uint32_t maskedAddr = addr & mask;

  if (!getAddrAsStr(mSubnetIPV4Addr, INET_ADDRSTRLEN, maskedAddr))
    ALOGE("ERROR in getting subnet address");
  else {
    ALOGI("Subnet Address [%s]", mSubnetIPV4Addr);
    snprintf(mSubnetIPV4AddrFormatted, INET_ADDRSTRLEN + 4, "%s/%d", mSubnetIPV4Addr, subnetMask);
    ALOGI("Formatted Subnet mask Address [%s]", mSubnetIPV4AddrFormatted);
    ret_val = true;
  }
//Add buffer overflow check for snprintf
  return ret_val;
}

/*------------------------------------------------------------------------------
 * FUNCTION      forkAndExecCommand
 *
 * DESCRIPTION   Takes in arguments to execute ndc command by forking the child
 *----------------------------------------------------------------------------*/
 bool forkAndExecCommand(const char* ndcPath, char** args) {
  child_pid = fork();
  if( child_pid >= 0) {
    if( child_pid == 0 ) {
      return execv(ndcPath, args);
    }
    else {
      wait(&status);
    }
  } else {
    ALOGE(" Failed to fork a Child ");
  }
  return false;
 }

/*------------------------------------------------------------------------------
 * FUNCTION      ndcAddNetwork
 *
 * DESCRIPTION   Creates a network by executing ndc command
 *               Cmd Format : ndc network create <netId>
 *----------------------------------------------------------------------------*/
int ndcAddNetwork() {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "create",
    (char *) netId,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/* Functions to bring the Call Up */

/*------------------------------------------------------------------------------
 * FUNCTION      ndcAddInterface
 *
 * DESCRIPTION   Creates an interface by executing ndc command
 *               Cmd Format : ndc network interface add <netId> <interface_name>
 *----------------------------------------------------------------------------*/
int ndcAddInterface() {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "interface",
    (char *) "add",
    (char *) netId,
    (char *) device_name,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/*------------------------------------------------------------------------------
 * FUNCTION      ndcAddGatewayRoute
 *
 * DESCRIPTION   Add route by specifying the gateway addr by executing ndc command
 *               Cmd Format : ndc network route add <netId> <interface_name>
 *               <default_addr> <gateway_addr>
 *----------------------------------------------------------------------------*/
int ndcAddGatewayRoute(char * gateway_ip_address) {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "route",
    (char *) "add",
    (char *) netId,
    (char *) device_name,
    (char *) "0.0.0.0/0",
    (char *) gateway_ip_address,
    NULL};
  return forkAndExecCommand(ndcPath, args);
  }

/*------------------------------------------------------------------------------
 * FUNCTION      ndcAddSubnetmaskRoute
 *
 * DESCRIPTION   Add route by specifying the subnetmask addr by executing ndc command
 *               Cmd Format : ndc network route add <netId> <interface_name>
 *               <subnetmask_addr>
 *----------------------------------------------------------------------------*/
int ndcAddSubnetmaskRoute(char * subnetmask_ip_address) {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "route",
    (char *) "add",
    (char *) netId,
    (char *) device_name,
    (char *) subnetmask_ip_address,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/*------------------------------------------------------------------------------
 * FUNCTION      ndcAddDns
 *
 * DESCRIPTION   Add DNSes by specifying the Primary DNS addr & Secondary DNS addr
 *               by executing ndc command
 *               Cmd Format : ndc resolver setnetdns <netId> <dns1> <dns2>
 *----------------------------------------------------------------------------*/
int ndcAddDns(char * dnsp_addr, char * dnss_addr) {
  char *args[] = {
    (char *) ndcPath,
    (char *) "resolver",
    (char *) "setnetdns",
    (char *) netId,
    (char *) dnsp_addr,
    (char *) dnss_addr,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/*------------------------------------------------------------------------------
 * FUNCTION      ndcSetDefault
 *
 * DESCRIPTION   Set Default Network by specifying the netId by executing
 *               ndc command
 *               Cmd Format: ndc network default set <netId>
 *----------------------------------------------------------------------------*/
int ndcSetDefault() {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "default",
    (char *) "set",
    (char *) netId,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/* Functions to bring the Call Down */
/*------------------------------------------------------------------------------
 * FUNCTION      ndcRemoveInterface
 *
 * DESCRIPTION   Removes the interface by executing ndc command
 *               Cmd Format : ndc network interface remove <netId> <interface_name>
 *----------------------------------------------------------------------------*/
int ndcRemoveInterface() {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "interface",
    (char *) "remove",
    (char *) netId,
    (char *) device_name,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/*------------------------------------------------------------------------------
 * FUNCTION      ndcRemoveGatewayRoute
 *
 * DESCRIPTION   Remove route by specifying the gateway addr by executing ndc command
 *               Cmd Format : ndc network route remove <netId> <interface_name>
 *               <default_addr> <gateway_addr>
 *----------------------------------------------------------------------------*/
int ndcRemoveGatewayRoute(char * gateway_ip_address) {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "route",
    (char *) "remove",
    (char *) netId,
    (char *) device_name,
    (char *) "0.0.0.0/0",
    (char *) gateway_ip_address,
    NULL};
  return forkAndExecCommand(ndcPath, args);
  }

/*------------------------------------------------------------------------------
 * FUNCTION      ndcRemoveSubnetmaskRoute
 *
 * DESCRIPTION   Remove route by specifying the subnetmask addr by executing ndc command
 *               Cmd Format : ndc network route remove <netId> <interface_name>
 *               <subnetmask_addr>
 *----------------------------------------------------------------------------*/
int ndcRemoveSubnetmaskRoute(char * subnetmask_ip_address) {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "route",
    (char *) "remove",
    (char *) netId,
    (char *) device_name,
    (char *) subnetmask_ip_address,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/*------------------------------------------------------------------------------
 * FUNCTION      ndcClearDns
 *
 * DESCRIPTION   Clear DNSes by specifying the nettId by executing ndc command
 *               Cmd Format : ndc resolver clearnetdns <netId>
 *----------------------------------------------------------------------------*/
int ndcClearDns() {
  char *args[] = {
    (char *) ndcPath,
    (char *) "resolver",
    (char *) "clearnetdns",
    (char *) netId,
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/*------------------------------------------------------------------------------
 * FUNCTION      ndcClearDefault
 *
 * DESCRIPTION   Clear Default Network by executing ndc command
 *               Cmd Format: ndc network default clear
 *----------------------------------------------------------------------------*/
int ndcClearDefault() {
  char *args[] = {
    (char *) ndcPath,
    (char *) "network",
    (char *) "default",
    (char *) "clear",
    NULL};
  return forkAndExecCommand(ndcPath, args);
}

/*------------------------------------------------------------------------------
 * FUNCTION      bringNetworkUp
 *
 * DESCRIPTION
 *----------------------------------------------------------------------------*/
bool bringNetworkUp() {
  ALOGI(" Attempting to bring Network UP ");
  ndcAddNetwork();
  return true;
}

/*------------------------------------------------------------------------------
 * FUNCTION      bringCallUp
 *
 * DESCRIPTION
 *----------------------------------------------------------------------------*/
bool bringCallUp() {
  ALOGI(" Attempting to bring Call UP ");
  
  ALOGI(" Attempting to Add Interface ");
  ndcAddInterface();

  ALOGI(" Attempting to Add Gateway Route ");
  ndcAddGatewayRoute(mGatewayIPV4Addr);

  ALOGI(" Attempting to Add SubnetMask Route ");
  ndcAddSubnetmaskRoute(mSubnetMaskIPV4Addr);

  ALOGI(" Attempting to Add DNS ");
  ndcAddDns(mDnspIPV4Addr, mDnssIPV4Addr);

  ALOGI(" Attempting to Set Default Network ");
  ndcSetDefault();

  return true;
}

/*------------------------------------------------------------------------------
 * FUNCTION      bringCallDown
 *
 * DESCRIPTION
 *----------------------------------------------------------------------------*/
bool bringCallDown() {
  ALOGI(" Attempting to bring Call DOWN ");

  ALOGI(" Attempting to Clear Default Network ");
  ndcClearDefault();

  ALOGI(" Attempting to Clear DNS ");
  ndcClearDns();

  ALOGI(" Attempting to Remove Gateway Route ");
  ndcRemoveGatewayRoute(mGatewayIPV4Addr);

  ALOGI(" Attempting to Remove SubnetMask Route ");
  ndcRemoveSubnetmaskRoute(mSubnetMaskIPV4Addr);

  ALOGI(" Attempting to Remove Interface ");
  ndcRemoveInterface();

  return true;
}

/*------------------------------------------------------------------------------
 * FUNCTION      bringNetworkDown
 *
 * DESCRIPTION   Feature not supported - Network remains up always 
 *----------------------------------------------------------------------------*/
bool bringNetworkDown() {
return true;
}
/*------------------------------------------------------------------------------
 * FUNCTION      startDsiCall
 *
 * DESCRIPTION   starts the DSI call
 *               follows the steps as follows:
 *                   1) Get Service Handle : If DSI_SUCCESS, Proceed to step 2
 *                      Else : Return DSI_ERROR
 *                   2) Set data call param
 *                   3) Start Data Call
 *
 *
 * RETURN VALUE  - DSI_SUCCESS / DSI_ERROR
 *
 * SIDE EFFECTS  - Successful dsi_start_data_call triggers Callback Function
 *                 daemonNetCtrlCB
 *----------------------------------------------------------------------------*/
int startDsiCall() {
  dsi_hndl_t dHandle = NULL;
  ALOGI("Trying to obtain service handle");
  dHandle = dsi_get_data_srvc_hndl(daemonNetCtrlCB, NULL);
  if(dHandle != NULL) {
    /* Obtain data service handle */
    dsi_net_hndl[index].handle = dHandle;
    dsi_net_hndl[index].profile = profileID; //Profile ID matching WDS_PROFILE_TYPE_3GPP with APN : wonev4v6.com
  } else {
    ALOGI("No Handle Obtained");
    ALOGI("Call Flow Failure ... Exiting");
    return DSI_ERROR;
  }

  ALOGI("Service Handle Obtained");

  /* set data call param */
  dsi_call_param_value_t param_info;

  //Set ip_family to IPv4
  dsi_net_hndl[index].family = DSI_IP_FAMILY_4;
  param_info.buf_val = NULL;
  param_info.num_val = DSI_IP_VERSION_4;
  ALOGI("Setting family to %s", DSI_IP_FAMILY_4);
  dsi_set_data_call_param(dsi_net_hndl[index].handle, DSI_CALL_INFO_IP_VERSION, &param_info);

  //Set the profile
  if (dsi_net_hndl[index].profile > 0) {
    if( dsi_net_hndl[index].profile > DSI_PROFILE_3GPP2_OFFSET ) {
        param_info.num_val = (dsi_net_hndl[index].profile - DSI_PROFILE_3GPP2_OFFSET);
        ALOGI("Setting 3GPP2 PROFILE to %d", param_info.num_val);
        dsi_set_data_call_param(dsi_net_hndl[index].handle, DSI_CALL_INFO_CDMA_PROFILE_IDX, &param_info);
    } else {
        param_info.num_val = dsi_net_hndl[index].profile;
        ALOGI("Setting 3GPP PROFILE to %d", param_info.num_val);
        dsi_set_data_call_param(dsi_net_hndl[index].handle, DSI_CALL_INFO_UMTS_PROFILE_IDX, &param_info);
    }
  }
  ALOGI("Attempting to bring call up on technology %s with IP family %s", dsi_net_hndl[index].tech, dsi_net_hndl[index].family);
  int isDsiCallStarted = dsi_start_data_call( dsi_net_hndl[index].handle );

  if ( dsi_net_hndl[index].handle ) {
    ALOGI("%d : Call %s %s on profile %d ", index,dsi_net_hndl[index].tech,dsi_net_hndl[index].family,dsi_net_hndl[index].profile);
  }
  return DSI_SUCCESS;
}

/*------------------------------------------------------------------------------
 * FUNCTION      getDeviceInfo
 *
 * DESCRIPTION   gets the device name
 *
 *
 * RETURN VALUE  - DSI_SUCCESS if the device name is obtained
 *                 else DSI_ERROR
 *
 * SIDE EFFECTS  -
 *----------------------------------------------------------------------------*/
int getDeviceInfo() {
  int isDeviceNameObtained = dsi_get_device_name(dsi_net_hndl[index].handle, device_name, DSI_CALL_INFO_DEVICE_NAME_MAX_LEN + 1);
  if(isDeviceNameObtained != DSI_SUCCESS) {
    ALOGI(" get_call_info DSI: Device Name was not obtained");
    return DSI_ERROR;
  } else {
    ALOGI(" get_call_info DSI: Device Name Obtained");
    ALOGI(" get_call_info DSI: Device Name is: %s", device_name);
  }

  return DSI_SUCCESS;
}



/*------------------------------------------------------------------------------
 * FUNCTION      getIpInfo
 *
 * DESCRIPTION   gets the IP Address Count and IP Address Information
 *               logs the following:
 *                   1) Interface IPV4 Address
 *                   2) Gateway IPV4 Address
 *                   3) DNS Primary IPV4 Address
 *                   4) DNS Secondary IPV4 Address
 *
 *
 * RETURN VALUE  - DSI_SUCCESS if ip information is obtained else DSI_ERROR
 *
 * SIDE EFFECTS  -
 *----------------------------------------------------------------------------*/
int getIpInfo() {
  dsi_addr_info_t *addrInfo = NULL;
  int addressCount = 0;
  addressCount = dsi_get_ip_addr_count(dsi_net_hndl[index].handle);
  ALOGI(" Obtained Address count is %d", addressCount);
  if ( addressCount == 0) {
    ALOGI(" ERROR : Couldn't get valid count of addr info ");
  } else {
    addrInfo = (dsi_addr_info_t *) calloc (addressCount, sizeof(dsi_addr_info_t));

    if(NULL == addrInfo) {
      ALOGI(" ERROR : Couldn't allocate addr info ");
    } else {
      dsi_hndl_t hndl = dsi_net_hndl[index].handle;
      int gotIP = dsi_get_ip_addr(hndl, addrInfo, addressCount);

      if( gotIP != DSI_SUCCESS) {
        ALOGI(" ERROR : Couldn't get the addr info : err %d", gotIP);
      } else {
        ALOGI(" IP Address Information : ENTRY");

        unsigned int subnetMaskInt = addrInfo->iface_mask;
        ALOGI(" IP Adddress Mask %d", subnetMaskInt);

        /* Process Interface IPV4 Address */
        memset(mInterfaceIPV4Addr, 0, INET_ADDRSTRLEN);
        if(NULL == inet_ntop(AF_INET, addrInfo->iface_addr_s.addr.__data, mInterfaceIPV4Addr, INET_ADDRSTRLEN)) {
          ALOGI(" IP Address Info : ERROR in getting Interface IPV4 Address");
          return isIpInfoObtained;
        } else {
          ALOGI(" Interface IPV4 address is [%s]", mInterfaceIPV4Addr);
        }

        //Process Gateway IPV4 Address
        memset(mGatewayIPV4Addr, 0, INET_ADDRSTRLEN);
        if(NULL == inet_ntop(AF_INET, addrInfo->gtwy_addr_s.addr.__data, mGatewayIPV4Addr, INET_ADDRSTRLEN)) {
          ALOGI(" IP Address Info : ERROR in getting Gateway IPV4 Address");
          return isIpInfoObtained;
        } else {
          ALOGI(" Gateway IPV4 address is [%s]", mGatewayIPV4Addr);
        }

        // Process SubnetMask IPV4 Address
        if(!getSubnetMaskAddr(addrInfo->iface_addr_s.addr.__data, subnetMaskInt)) {
          ALOGE("IP Address Info : ERROR in getting SubnetMask IPV4 Address");
          return isIpInfoObtained;
        } else {
          memset(mSubnetMaskIPV4Addr, 0, INET_ADDRSTRLEN + 4);
          strcpy(mSubnetMaskIPV4Addr, mSubnetIPV4AddrFormatted);
          ALOGI("SubnetMask IPV4 address is [%s]", mSubnetMaskIPV4Addr);
        }

        //Process DNS Primary IPV4 Address
        memset(mDnspIPV4Addr, 0, INET_ADDRSTRLEN);
        if(NULL == inet_ntop(AF_INET, addrInfo->dnsp_addr_s.addr.__data, mDnspIPV4Addr, INET_ADDRSTRLEN)) {
          ALOGI(" IP Address Info : ERROR in getting DNS Primary IPV4 Address");
          return isIpInfoObtained;
        } else {
          ALOGI(" DNS Primary IPV4 address is [%s]", mDnspIPV4Addr);
        }

        //Process DNS Secondary IPV4 Address
        memset(mDnssIPV4Addr, 0, INET_ADDRSTRLEN);
        if(NULL == inet_ntop(AF_INET, addrInfo->dnss_addr_s.addr.__data, mDnssIPV4Addr, INET_ADDRSTRLEN)) {
          ALOGI(" IP Address Info : ERROR in getting DNS Secondary IPV4 Address");
          return isIpInfoObtained;
        } else {
          ALOGI(" DNS Secondary IPV4 address is [%s]", mDnssIPV4Addr);
        }

        ALOGI(" IP Address Information : EXIT");
      }
    }
  }

  return DSI_SUCCESS;
}

/*------------------------------------------------------------------------------
 * FUNCTION      executeNdcCommands
 *
 * DESCRIPTION   Used to Execute NDC commands to do any one of the following:
 *               1. Bring Network Up
 *               2. Bring Call Up
 *               3. Bring Call Down
 *               

 *----------------------------------------------------------------------------*/
bool executeNdcCommands(int action) {
  bool ret_val = false;
  switch(action) {
    case NDC_ACTION_NETWORK_UP:
        ret_val = bringNetworkUp();
        break;

    case NDC_ACTION_CALL_UP:
        ret_val = bringCallUp();
        break;

    case NDC_ACTION_CALL_DOWN:
        ret_val = bringCallDown();
        break;

    case NDC_ACTION_NETWORK_DOWN:
         ret_val = bringNetworkDown();
         break;

    default:
         ALOGE(" Invalid Action Input");
         break;
  }
  return ret_val;
}

/*------------------------------------------------------------------------------
 * FUNCTION      daemonNetCtrlCB
 *
 * DESCRIPTION   Callback Function registered while calling
 *               dsi_get_data_srvc_hndl()
 *               Triggered after dsi data call is attempted to start
 *               Checks the evt value:
 *               it puts the Event into mutex queue if its DSI_EVT_NET_IS_CONN
 *               Indicating the call is started
 *
 * RETURN VALUE  -
 *
 * SIDE EFFECTS  -
 *----------------------------------------------------------------------------*/
void daemonNetCtrlCB (dsi_hndl_t hndl, void *user_data, dsi_net_evt_t evt, dsi_evt_payload_t *payload_ptr) {
  #pragma unused(hndl)
  #pragma unused(payload_ptr)

  dsi_test_call_info_t *call_info = NULL;
  call_info = (dsi_test_call_info_t *) user_data;
  ALOGI(" Callback function: daemonNetCtrlCB was called");

  if (evt > DSI_EVT_INVALID && evt < DSI_EVT_MAX) {
    switch( evt ) {
      case DSI_EVT_NET_NO_NET:
          ALOGI(" Call Status is Idle");
          break;

      case DSI_EVT_NET_IS_CONN:
          ALOGI(" Call Status is Connected");
          if (!putEvent(EVENT_CONNECTED)) {
            ALOGE(" Failed to put notify event in ring");
          }
          break;

      default:
          ALOGI(" Call Status is Unknown");
          break;
    }
  }
}

int configureProfileID (char* carrier) {
  if (strcasecmp("vzw", carrier) == 0) {
    profileID = 3;
  } else if ((strcasecmp("att", carrier) == 0) || (strcasecmp("tmo", carrier) == 0) || (strcasecmp("w1", carrier) == 0) ) {
    profileID = 1;
  } else {
    ALOGI( " Invalid Argument Passed : Valid Args are : att, vzw, tmo , w1");
    return false;
  }
  return true;
}

/*------------------------------------------------------------------------------
 * FUNCTION      dsi_init_ex_cb_func
 *
 * DESCRIPTION   Callback Function for dsi_init_ex call
 *               Once triggered, calls the function : startDsiCall
 *
 * RETURN VALUE  -
 *
 * SIDE EFFECTS  -
 *----------------------------------------------------------------------------*/
void dsi_init_ex_cb_func (void *user_data) {
  #pragma unused(user_data)
  ALOGI(" After calling dsi_init_ex");
  ALOGI(" Callback function: dsi_init_ex_cb_func was called");
  hasCall = startDsiCall();
}

/*------------------------------------------------------------------------------
 * FUNCTION      handleSigaction
 *
 * DESCRIPTION   handle SIGUSR1 signal by bringing the call down
 *
 *----------------------------------------------------------------------------*/
void handleSigaction(int signo) {
  ALOGI(" Process %d : Received SIGUSR1 : %d ", getpid(), signo);
  ALOGI(" Attempting to Bring the Call Down ");
  if(executeNdcCommands( NDC_ACTION_CALL_DOWN )) {
    ALOGI(" CALL DOWN ");
  } else {
    ALOGE(" Error in executing NDC commands ");
  }
}


/*------------------------------------------------------------------------------
 * FUNCTION      main
 *
 * DESCRIPTION   entry point for Connectivity-autod (Deamon)
 *
 * DEPENDENCIES  Cne, CneMsg
 *
 * RETURN VALUE  -
 *
 * SIDE EFFECTS  -
 *----------------------------------------------------------------------------*/
int main(int argc, char** argv) {
  struct sigaction action;
  action.sa_handler = &handleSigaction;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGUSR1, &action, 0);
  if (sigaction(SIGUSR1, &action, NULL) == -1) ALOGE("Can't handle signal");

  /* Handle Profile configuration based on carrier used */
  if( argc != 2) {
    ALOGE(" Invalid Number of arguments passed ");
    return false;
  } else {
    if( configureProfileID(argv[1]) ) {
      ALOGI(" Profile Configured Successfully ");
    } else {
      ALOGE("ERROR : Profile not Configured");
      return false;
    }
  }
  //ALOGI(" Network Creation : Success ");

  ALOGI(" Before calling dsi_init_ex");
  int isDsiInited =  dsi_init_ex(DSI_MODE_GENERAL, dsi_init_ex_cb_func, NULL);

  /* Wait till Call gets Connected */
  int ret_event = EVENT_NONE;
  while( true ) {
    if((ret_event = getEvent()) == EVENT_CONNECTED) break;
  }

  ALOGI(" Attempting to get Device Name");
  isDeviceObtained = getDeviceInfo();
  if( isDeviceObtained == DSI_SUCCESS) {
    if(executeNdcCommands( NDC_ACTION_NETWORK_UP )) {
      ALOGI (" Network UP : SUCCESS");
    } else {
      ALOGE (" Error in Bringing up the Network ");
    }
    ALOGI(" Attempting to get IP address information");
    isIpInfoObtained = getIpInfo();
    if ( isIpInfoObtained == DSI_SUCCESS) {
      ALOGI(" Obtained IP Address Information Successfully");
      if(executeNdcCommands( NDC_ACTION_CALL_UP )) {
        ALOGI("CALL UP");
      } else {
        ALOGE (" Error in bringing up the call ");
      }
    }
    else {
      ALOGE(" Unable to obtain Ip Address Information");
    }
  } else {
    ALOGE(" Unable to obtain Device Information");
  }

  for(;;) {
    sleep(1000);
  }

}
