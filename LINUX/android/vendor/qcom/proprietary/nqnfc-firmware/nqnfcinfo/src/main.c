/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <cutils/properties.h>
#include <sys/types.h>
#include <linux/nfc/nfcinfo.h>

int main()
{
    int ret = 0;
    int fp = 0;
    int len = 0;
    char valueStr[PROPERTY_VALUE_MAX] = {0};
    unsigned int chip_version = 0x00;
    char chip_ver[5];
    unsigned int major_version = 0x00;
    unsigned int minor_version = 0x00;
    unsigned int rom_version = 0x00;
    char firmware_version[10];
    union nqx_uinfo nqx_info;

    printf("... NQ driver info ...\n");

    if ((ret = (fp = open("/dev/nq-nci", O_RDWR))) < 0)
    {
        printf("NQ open error retcode = %d, errno = %d\n", ret, errno);
        exit(1);
    }
    printf("NQ open : Ret = %d\n", ret);

    nqx_info.i = ioctl(fp, NFCC_GET_INFO, 0);
    printf("NQ version : info = %d\n", nqx_info.i);
    chip_version = nqx_info.info.chip_type;
    printf("NQ version : chip id = %x\n", chip_version);

    len = __system_property_get("sys.nfc.nq.chipid", valueStr);
    if (len > 0)
        printf ("%s: sys.nfc.nq.chipid: %s \n", __FUNCTION__, valueStr);

    snprintf(chip_ver, 5, "0x%02x", chip_version);
    __system_property_set("sys.nfc.nq.chipid", chip_ver);
    len = __system_property_get("sys.nfc.nq.chipid", valueStr);
    printf ("After system property set : sys.nfc.nq.chipid: %s \n", valueStr);

    rom_version = nqx_info.info.rom_version;
    printf("NQ  : rom_version = %x\n", rom_version);
    major_version = nqx_info.info.fw_major;
    printf("NQ  : major_version = %x\n",major_version);
    minor_version = nqx_info.info.fw_minor;
    printf("NQ  : minor_version = %x\n",minor_version);

    snprintf(firmware_version, 10, "%02x.%02x.%02x", rom_version,
             major_version, minor_version);
    printf("firmware_version = %s\n", firmware_version);
    __system_property_set("sys.nfc.nq.fwver", firmware_version);
    len = __system_property_get("sys.nfc.nq.fwver", valueStr);
    printf ("After system property set : sys.nfc.nq.fwver: %s \n", valueStr);
    close(fp);

    return 0;
}
