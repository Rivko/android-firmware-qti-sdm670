/**
-----------------------------------------------------------------------------
Copyright (c) 2011-2013 Qualcomm Technologies, Inc.
All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------------
 *
 * @file btnvtool.cpp
 * This file contains the tool to Read and Write into BT-NV Persist File
 *
 */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Author: agaja $


  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-03-03  akg  Initial Version.
  2011-09-28  rrr  Moved the implementation to CPP, for having BD address being
                   programmed twice if previous BD address was random generated
                   at boot.
  2012-02-22  rrr  Persistent file name renamed as hidden.
===========================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include "bt_nv.h"
#include "comdef.h"
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <cutils/properties.h>

#if defined(BT_QSOC_GET_ITEMS_FROM_NV)
#include "bt_qmi_dms_client.h"
#endif
#include <utils/Log.h>
#define BASE_16 16

#define BT_QSOC_REF_CLK_19_2MHZ       19200000
#define BT_QSOC_REF_CLK_32_0MHZ       32000000
#define BT_QSOC_CLK_SHARING_ENABLED   1
#define BT_QSOC_CLK_SHARING_DISABLED  0

#define QMI_INIT_MAX_TRY              (5)
#define QMI_INIT_RETRY_INTERVAL_SEC   (2)

#define BLUETOOTH_MAC_ADDR_BOOT_PROPERTY "ro.boot.btmacaddr"

bool validate_tok(char* bdaddr_tok) {
    int i = 0;
    bool ret;
    int len = strlen(bdaddr_tok);
    if (len > 2) {
        ret = FALSE;
        ALOGE("Invalid token length");
    } else {
        ret = TRUE;
        for (i = 0; i < len; i++) {
            if ((bdaddr_tok[i] >= '0' && bdaddr_tok[i] <= '9') ||
                (bdaddr_tok[i] >= 'A' && bdaddr_tok[i] <= 'F') ||
                (bdaddr_tok[i] >= 'a' && bdaddr_tok[i] <= 'f')) {
                ret = TRUE;
                ALOGV("%s: tok %s @ %d is good", __func__, bdaddr_tok, i);
            } else {
                ret = FALSE;
                ALOGE("invalid character in tok: %s at ind: %d", bdaddr_tok, i);
                break;
            }
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
  int c;
  int option_index = 0;
  nv_persist_item_type my_nv_item;
  uint32 ref_clk_speed ;
  uint32 clk_sharing ;
  char *BD_addr_string = NULL;
  char *saveptr        = NULL;
  uint8 BD_addr[NV_BD_ADDR_SIZE];
  boolean bd_set = FALSE;
  int is_random_addr = FALSE;
  bool is_exist_addr_random = FALSE;
  /* variables for configuring on-boot
   * random BD address */
  int seed;
  int i, ret = -1;
  struct timespec sTime;
  struct stat sts;
  char filename[NAME_MAX] = {0};

  while (TRUE)
  {
    static struct option long_options[] =
    {
     {"board-address",   required_argument, NULL, 'b'},
     {"reference-clock", required_argument, NULL, 'r'},
     {"enable-clock-sharing",  required_argument, NULL, 'c'},
     {"print-all-nv",  no_argument, NULL, 'p'},
     {"on-boot-config", no_argument, NULL, 'O'},
     {"help",  no_argument, NULL, 'h'},
     {0, 0, 0, 0}
    };

    c = getopt_long (argc, argv, ":b:c:r:Op",
    long_options, &option_index);
    if (c == -1)
      break;

    switch (c)
    {
       case 'b':
         if (optarg==NULL || (17 > strlen (optarg))
           ||  (!isxdigit(optarg[ 0]))
           ||  (!isxdigit(optarg[ 1]))
           ||  ( isxdigit(optarg[ 2]))
           ||  (!isxdigit(optarg[ 3]))
           ||  (!isxdigit(optarg[ 4]))
           ||  ( isxdigit(optarg[ 5]))
           ||  (!isxdigit(optarg[ 6]))
           ||  (!isxdigit(optarg[ 7]))
           ||  ( isxdigit(optarg[ 8]))
           ||  (!isxdigit(optarg[ 9]))
           ||  (!isxdigit(optarg[10]))
           ||  ( isxdigit(optarg[11]))
           ||  (!isxdigit(optarg[12]))
           ||  (!isxdigit(optarg[13]))
           ||  ( isxdigit(optarg[14]))
           ||  (!isxdigit(optarg[15]))
           ||  (!isxdigit(optarg[16]))
           )
         {
           fprintf (stderr, "%s: option 'b' (--board-address) requires 6 hexidecimal pairs: xx.xx.xx.xx.xx.xx (%s)\n",
                        argv[0], (optarg == NULL)? "NULL": optarg);
           return 1;
         }
         BD_addr_string = optarg;
         BD_addr[5] = strtol (&BD_addr_string[ 0], 0, 16);
         BD_addr[4] = strtol (&BD_addr_string[ 3], 0, 16);
         BD_addr[3] = strtol (&BD_addr_string[ 6], 0, 16);
         BD_addr[2] = strtol (&BD_addr_string[ 9], 0, 16);
         BD_addr[1] = strtol (&BD_addr_string[12], 0, 16);
         BD_addr[0] = strtol (&BD_addr_string[15], 0, 16);
         memcpy(&my_nv_item.bd_addr, BD_addr, NV_BD_ADDR_SIZE);
         bt_nv_cmd(NV_WRITE_F,  NV_BD_ADDR_I, &my_nv_item);
         break;

       case 'O':
         snprintf(filename, NAME_MAX, "%s/%s", PERSISTENCE_PATH, BT_NV_FILE_NAME);
         ret = stat(filename, &sts);
         is_exist_addr_random = bt_nv_is_random_addr();
         if ((ret == -1 && errno == ENOENT) || (ret == 0 && sts.st_size == 0) ||
            (TRUE == is_exist_addr_random) )
         {
            ALOGE("The file %s doesn't exist or empty or addr is random %d...\n",
                                filename, is_exist_addr_random);

#if defined(BT_QSOC_GET_ITEMS_FROM_NV)
            if(TRUE == is_modem_bt_nv_supported(MODEM_NV_BD_ADDR))
            {
                int retry = 0;
                while (retry < QMI_INIT_MAX_TRY)
                {
                    if (retry > 0)
                    {
                        sleep(QMI_INIT_RETRY_INTERVAL_SEC);
                    }

                    if (bt_qmi_dms_init() == TRUE)
                    {
                        if (qmi_dms_get_bt_address(BD_addr) == TRUE)
                        {
                            ALOGE("Read MODEM_NV_BD_ADDR 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
                            BD_addr[0], BD_addr[1], BD_addr[2], BD_addr[3], BD_addr[4], BD_addr[5] );
                            bd_set = TRUE;
                        }
                        cleanup_qmi_dms();
                        break;
                    }

                    ++retry;
                    ALOGE("QMI initialization failed in %d try...\n", retry);
                }
            }
#endif

            if(!bd_set)
            {
                int i = 0;
                static char bd_addr[PROPERTY_VALUE_MAX];
                char* tok;
                char* saveptr = NULL;

                if (property_get(BLUETOOTH_MAC_ADDR_BOOT_PROPERTY, bd_addr, NULL))
                {
                    ALOGE("BD address read from Boot property: %s\n", bd_addr);
                    tok =  strtok_r(bd_addr, ":", &saveptr);
                    while (tok != NULL)
                    {
                        ALOGV("bd add [%d]: %d ", i, strtol(tok, NULL, BASE_16));
                        if (i >= 6) {
                            ALOGE("bd property of invalid length");
                            bd_set = FALSE;
                            break;
                        }
                        if (!validate_tok(tok)) {
                            ALOGE("Invalid token in BD address");
                            bd_set = FALSE;
                            break;
                        }
                        BD_addr[5-i] = strtol(tok, NULL, BASE_16);
                        tok = strtok_r(NULL, ":", &saveptr);
                        bd_set = TRUE;
                        i++;
                    }
                    if (bd_set && (i < 6)) {
                        ALOGE("bd property length is less than 6 bytes");
                        bd_set = FALSE;
                    }
                }
            }

            if(!bd_set && (FALSE == is_exist_addr_random))
            {
                /* If BD address is not programmed by the user then the random
                 * BD address generated should be persistent across target
                 * reboots.
                 */
                if (-1 == clock_gettime (CLOCK_REALTIME, &sTime))
                {
                    fprintf (stderr, "clock_gettime failed\n");
                    return 1;
                }

                seed = sTime.tv_nsec;
                srand ((unsigned int) seed);

                BD_addr[0] = (rand() & 0x0FF00000) >> 20;
                BD_addr[1] = (rand() & 0x0FF00000) >> 20;
                BD_addr[2] = (rand() & 0x0FF00000) >> 20;
                BD_addr[3] = 0x00;
                BD_addr[4] = 0x1D;
                BD_addr[5] = 0x80; //last byte bit 0 is for unicast and bit 1 to identify
                                   //the local generated address
                is_random_addr = TRUE;
                bd_set = TRUE;
                ALOGE("Writing Random BD_ADDR 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
                BD_addr[0], BD_addr[1], BD_addr[2], BD_addr[3], BD_addr[4], BD_addr[5] );
            }
            if(bd_set)
            {
                memcpy(&my_nv_item.bd_addr, BD_addr, NV_BD_ADDR_SIZE);
                /* is_random_addr = 1 implies that BD address programmed is random generated
                * is_random_addr = 0 (default) implies it to be user programmed */
                bt_nv_cmd(NV_WRITE_F,  NV_BD_ADDR_I, &my_nv_item, is_random_addr);
            }
         }
         else
         {
            fprintf (stderr, "The file %s exist...\n", filename);
         }
         break;

       case 'r':
         if(optarg==NULL)
         {
             fprintf (stderr, "%s: option 'r' (--reference-clock) requires value %d or %d.\n",
                        argv[0], BT_QSOC_REF_CLK_19_2MHZ, BT_QSOC_REF_CLK_32_0MHZ);
             return 1;
         }
         switch (ref_clk_speed = atoi (optarg))
         {
           case BT_QSOC_REF_CLK_32_0MHZ:
             /* Command to write Refclk */
             my_nv_item.bt_soc_refclock_type = NV_PS_BT_SOC_REFCLOCK_32MHZ;
             break;
           case BT_QSOC_REF_CLK_19_2MHZ:
             my_nv_item.bt_soc_refclock_type = NV_PS_BT_SOC_REFCLOCK_19P2MHZ;
             break;
           default:
             fprintf (stderr, "%s: option 'r' (--reference-clock) requires value %d or %d.\n",
                        argv[0], BT_QSOC_REF_CLK_19_2MHZ, BT_QSOC_REF_CLK_32_0MHZ);
             return 1;
         }
         bt_nv_cmd(NV_WRITE_F,  NV_BT_SOC_REFCLOCK_TYPE_I, &my_nv_item);
         break;

       case 'c':
         if(optarg==NULL)
         {
             fprintf (stderr, "%s: option 'c' (--clock-sharing) requires value %d or %d.\n",
                      argv[0], BT_QSOC_CLK_SHARING_ENABLED, BT_QSOC_CLK_SHARING_DISABLED);
             return 1;
         }
         switch (clk_sharing = atoi (optarg))
         {
           case BT_QSOC_CLK_SHARING_ENABLED:
             /* Command to write clk sharing type */
             my_nv_item.bt_soc_clk_sharing_type = NV_PS_BT_SOC_CLOCK_SHARING_ENABLED;
             break;
           case BT_QSOC_CLK_SHARING_DISABLED:
             my_nv_item.bt_soc_clk_sharing_type = NV_PS_BT_SOC_CLOCK_SHARING_DISABLED;
             break;
           default:
             fprintf (stderr, "%s: option 'c' (--clock-sharing) requires value %d or %d.\n",
                      argv[0], BT_QSOC_CLK_SHARING_ENABLED, BT_QSOC_CLK_SHARING_DISABLED);
             return 1;
         }
         bt_nv_cmd(NV_WRITE_F, NV_BT_SOC_CLK_SHARING_TYPE_I, &my_nv_item);
         break;

       case 'p':
         bt_nv_cmd(NV_READ_F, NV_BD_ADDR_I, &my_nv_item);
         fprintf (stderr, "--board-address: %x.%x.%x.%x.%x.%x \n",
                     my_nv_item.bd_addr[0],
                     my_nv_item.bd_addr[1],
                     my_nv_item.bd_addr[2],
                     my_nv_item.bd_addr[3],
                     my_nv_item.bd_addr[4],
                     my_nv_item.bd_addr[5]);
         bt_nv_cmd(NV_READ_F, NV_BT_SOC_REFCLOCK_TYPE_I, &my_nv_item);
         fprintf (stderr, "--reference-clock: %s MHz \n",
                     my_nv_item.bt_soc_refclock_type?"19.2":"32" );
         bt_nv_cmd(NV_READ_F, NV_BT_SOC_CLK_SHARING_TYPE_I, &my_nv_item);
         fprintf (stderr, "--clock-sharing: %s \n",
                     my_nv_item.bt_soc_clk_sharing_type?"Enabled":"Disabled" );
         break;

        default:
          printf ("%s: Unrecognized option: %c\n\n", argv[0], optopt);
          /* fall through */
        case 'h':
          printf ("BT NV Read/Write Tool\n\n");
          printf ("Usage: %s [options]\n", argv[0]);
          printf ("\noptional arguments:\n");
          printf ("   flag\tlong option\t  cur/dflt\tdescription\n\n");
          printf ("    -b\t--board-address\t  NV447 or rand\tBluetooth MAC address to use\n");
          printf ("    -r\t--reference-clock %d\tBTS reference clock Hz\t  %d\n",
                       BT_QSOC_REF_CLK_32_0MHZ, BT_QSOC_REF_CLK_19_2MHZ);
          printf ("    -c\t--enable-clock-sharing\t\tenable  clock sharing in BTSoC\n");
          printf ("    -p\t--print-all-nv\t\tRead and Print all BT NV items\n");
          printf ("    -O\t--on-boot-config\t\tOn boot BD address generation\n");
          printf ("\noption flags:\n");
          printf ("    -h\t--help\t\t\t\tprint this usage message\n");
          return 1;
          break;
     }
  }
  return 0;
}
