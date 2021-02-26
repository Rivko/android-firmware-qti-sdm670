/*==============================================================================
*  @file wfd_netutils.h
*  @par DESCRIPTION:
*       Header file of the wfd network utilities
*
*  Copyright (c) 2012,2014-2017 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
==============================================================================*/

#ifndef _WFD_NETUTILS_H_
#define _WFD_NETUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

enum NetType : int;

int getP2pInterfaceName(char* pInterfaceName, int size, NetType netType);

int getLocalIpAddress(char* pLocalIpAddr, int size, NetType netType);

// API to retrieve LOCAL IP from connected socket
// API returns 0 if successful and < 1 otherwise
int getLocalIpSocket (int SocketID, char* IP);

// API to retrieve Peer Mac address from ARP table
int getPeerMacAddress(char* pMacAddr, char* pPeerIpAddr, int size, NetType netType);

unsigned int getLinkSpeed(char *pMacAddr, unsigned int size, bool& queryIfName, NetType netType);

int socketClose(int nSock);

bool getIPSockPair(bool bPair, int *nSock1, int * nSock2,
                    int *nPort1, int *nPort2, bool tcp);

bool getNetworkProperties(const char *, NetType netType,
                  unsigned int *nss,
                  unsigned int *rateflags,
                  unsigned int *freq,
                  unsigned int *enable_4k);
#ifdef __cplusplus
}
#endif

#endif /* _WFD_NETUTILS_H_ */
