/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2016 Qualcomm Atheros Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Proprietary and Confidential.
 */

/*===========================================================================

                  FTM QTIP Source File: Edit History


when       who       what, where, why
--------   ---       ----------------------------------------------------------
03/22/16   calebj     Created a source file to implement routines for QTIP
		      Server

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <ctype.h>
#include <strings.h>

#ifdef ANDROID
#include <cutils/properties.h>
#endif

#include "testcmd6174.h"
#include "libtcmd.h"

#include "myftm_wlan.h"
#include "myftm_qtip.h"


int replyReceived = 0;
char rcvbuffer[MAXSTREAMLENGTH];
char rcvbuffer2[MAXSTREAMLENGTH];
int rcvbufferlen = 0;
int verbose = 0;
char ifname[16];
int mutex_HWwait = 0;

static void getIfName(char *ifname)
{
#ifdef ANDROID
	char ifprop[92];
#endif
	char *src;
	char defIfname[16] = {'\0'};
	char linebuf[1024];
	FILE *f = fopen("/proc/net/wireless", "r");
	if (f) {
		while (fgets(linebuf, sizeof(linebuf)-1, f)) {
			if (strchr(linebuf, ':')) {
				char *dest = defIfname;
				char *p = linebuf;
				while (*p && isspace(*p)) ++p;
				while (*p && *p != ':')
					*dest++ = *p++;
				*dest = '\0';
				break;
			}
		}
		fclose(f);
	} else {
		defIfname[0] = '\0';
	}

	src = defIfname;
#ifdef ANDROID
	if (property_get("wifi.interface", ifprop, defIfname)) {
		src = ifprop;
	}
#endif
	memcpy(ifname, src, IFNAMSIZ);
}


void cmdReplyFunc(void *buf, int len)
{
	rcvbufferlen = len;
	memcpy(&(rcvbuffer2[4]), buf, len);
	rcvbuffer2[0] = len & 0xFF;
	rcvbuffer2[1] = (len >> 8) & 0xFF;
	rcvbuffer2[2] = (len >> 16) & 0xFF;
	rcvbuffer2[3] = (len >> 24) & 0xFF;
	fprintf(stderr, "TLV length got %d bytes from target\n",rcvbufferlen);
	replyReceived = 1;
}



void prtStream(char *stream, int streamLen)
{
	int n;
	for (n=0; n<streamLen; n++)
	{
		if (n == 0)	fprintf(stderr, "    *-------------*\n");
		if (n == 32) fprintf(stderr, "    *-------------*\n");
		if (n%4 == 0) fprintf(stderr, "%3d |", n);
		fprintf(stderr, " %02x",(unsigned char)stream[n]);
		if (n%4 == 3)
			fprintf(stderr, " | %u\n",
				*((unsigned int *)&stream[n-3]));
		if (n == streamLen - 1)
			fprintf(stderr, "    *-------------*\n");
	}
}


void clientHandler(void* arg)
{
	unsigned int timeout = 0;
	int flag_socketactive = 1;
	int clientSocket = (int)*((int*)arg);
	fprintf(stderr, "%s: Socket: %d \n", __func__, clientSocket);

	tcmd_tx_init(ifname, cmdReplyFunc);
	while(flag_socketactive)
	{
		int len_read = 0;

		while (len_read == 0)
		{
			len_read = read(clientSocket, rcvbuffer,
					sizeof(rcvbuffer));
			if(len_read < 0)
			{
				fprintf(stderr, "Error: socket read %d\n",
					errno);
				flag_socketactive = 0;
				break;
			}
			if (len_read == 0)
			{
				usleep(1000);	// 1ms
				timeout++;
			}
			if (timeout > 1000) // 1sec
			{
				fprintf(stderr, "socket hw timeout... \n");
				flag_socketactive = 0;
				break;
			}
		}

		if (len_read >= MAXSTREAMLENGTH - 4)
		{
			fprintf(stderr,
				"Error: stream too long (%d)... "
				"closing socket\n", MAXSTREAMLENGTH);
			flag_socketactive = 0;
		}

		if (len_read > 0)
		{
			fprintf(stderr, "%d, %d, %d, %d\n", rcvbuffer[0],
				rcvbuffer[1], rcvbuffer[2], rcvbuffer[3]);
			if (verbose)
			{
				fprintf(stderr, "         RECV\n");
				prtStream(rcvbuffer, len_read);
			}
			else
				fprintf(stderr, "read, %d bytes\n",
					len_read); // print message length

			// MUTEX WAIT
			while (mutex_HWwait) {}
			mutex_HWwait = 1;

			//=========================================
			// relay message to/from DUT
			//-----------------------------------------
			char tempbuf[MAXSTREAMLENGTH];
			memcpy(&(tempbuf[0]), &rcvbuffer[4], len_read -4);
			tcmd_tx(tempbuf, len_read - 4, 1);
			while (!replyReceived)
			{
				// wait for reply from DUT
			}
			replyReceived = 0;
			if (verbose)
			{
				fprintf(stderr, "         SEND\n");
				prtStream(rcvbuffer2, rcvbufferlen + 4);
			}
			else
				fprintf(stderr, "sent, %d bytes\n",
					rcvbufferlen + 4);

			write(clientSocket, (rcvbuffer2), rcvbufferlen + 4);

			// MUTEX RELEASE
			mutex_HWwait = 0;
		}
		else
		{
			flag_socketactive = 0;
		}
	}

	fprintf(stderr, "closing socket\n");
	//shutdown(clientSocket,SD_SEND);
	close(clientSocket);

}


void qtip()
{
	int listenSocket, clientSocket, tempSocket;
	struct sockaddr_in serverInf;
	int port = 9877;
	int i;
	int flag_serverrunning = 1;
	//verbose = 1;


	//=========================================
	// DUT setup
	//-----------------------------------------
	memset(ifname, '\0', 16);
	strlcpy(ifname, "wifi0", sizeof(ifname));
	getIfName(ifname);

	//=========================================
	// server setup
	//-----------------------------------------
	bzero((char *) &serverInf, sizeof(serverInf));
	serverInf.sin_family=AF_INET;
	serverInf.sin_addr.s_addr=INADDR_ANY;
	serverInf.sin_port=htons(port);

	listenSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(listenSocket < 0)
	{
		fprintf(stderr, "listenSocket creation failed.\n");
		return;
	}
	if(bind(listenSocket, (struct sockaddr *)(&serverInf),
				sizeof(serverInf)) < 0)
	{
		fprintf(stderr, "Unable to bind socket!\n");
		close(listenSocket);
		return;
	}


	//=========================================
	// main server loop
	//-----------------------------------------
	while (flag_serverrunning)
	{

		listen(listenSocket, 1);
		clientSocket = 0;
		while(clientSocket == 0)
		{
			fprintf(stderr,
				"Waiting for incoming connections...\n");
			clientSocket=accept(listenSocket,NULL,NULL);
		}
		pthread_t clientHandler_thread;
		tempSocket = clientSocket;
		fprintf(stderr, "%s: Client connected! Socket: %d \n",
				__func__, tempSocket);
		if (pthread_create(&clientHandler_thread, NULL,
					(void *) clientHandler, &tempSocket))
		{
			printf("Error launching client handler thread.\n");
		}
	}

	close(listenSocket);
	return;
}
