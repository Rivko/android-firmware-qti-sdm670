/*
  @file app_stor.c
  @brief This file is  storarge test code to test all qsee_stor APIs.

*/
/*===========================================================================
  Copyright (c) 2011 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  ===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR FILE
  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/sampleapp/src/app_stor.c#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $

  # when       who     what, where, why
  # --------   ---     ---------------------------------------------------------
  # 12/07/12  phucd   Initial Version
  # 06/04/15  phucd   Adding function to convert QStor ErrCode to string
  # 07/15/15  phucd   Increasing size of RPMB test partition to 64*512 = 32MB, and 33 sectors data transfer at one shot
  #				   (FR28278:RPMB enhancement support for UFS and eMMC)
  #	                Modified RWProfileTest function to profile read/write of 1, 16, 32, 15 and 31 sectors
  ======================================================================================================================*/

#include "com_dtypes.h"
#include "qsee_log.h"
#include "qsee_heap.h"
#include "qsee_stor.h"
#include "qsee_timer.h"


#define TZ_RPMB_TEST_NUM_SECTOR_MAX	2048  //number of sectors used to add a new partition in RPMB (1MB)
qsee_stor_device_id_type rpmb_device_id = QSEE_STOR_EMMC_RPMB;
#define TZ_RPMB_PARITION_ID_0  0xA002
#define TZ_RPMB_PARITION_ID_1  0xF003


#define TZ_GPT_TEST_NUM_SECTOR_MAX	4  //number of sectors used to add a new test partition in GPT physical partition
#define TZ_GPT_PARITION_ID_0  1
qsee_stor_device_id_type gpt_device_id = QSEE_STOR_EMMC_GPP1;
#define TZ_MAX_SECTOR_PER_TRANSACTION 33
#define TZ_BUFF_SIZE_TEST	(TZ_MAX_SECTOR_PER_TRANSACTION*512)  // size of buffer hold for 18 sectors
#define TZ_BUFF_BACKUP_SIZE (TZ_RPMB_TEST_NUM_SECTOR_MAX*512)  //size of entire partition

extern const char* get_ta_app_name();
qsee_stor_device_id_type gDeviceID = QSEE_STOR_EMMC_RPMB;
qsee_stor_device_handle_t gDevHandle = NULL;
qsee_stor_client_handle_t gClientHandle = NULL;
qsee_stor_client_info_t   gClientInfo;
uint8 gBackupBuff[TZ_BUFF_BACKUP_SIZE]={0x00};
uint8 gBuff[TZ_BUFF_SIZE_TEST]={0x00};
uint8 gBuff2[TZ_BUFF_SIZE_TEST]={0x00};

typedef struct ErrCodes{
  int value;
  char* name;
} ErrCodesType;

ErrCodesType error_codes[] = {
  { QSEE_STOR_SUCCESS, 						"QSEE_STOR_SUCCESS" },    	//0
  { QSEE_STOR_ERROR, 						"QSEE_STOR_ERROR" },    	//-1
  { QSEE_STOR_INVALID_PARAM_ERROR, 			"QSEE_STOR_INVALID_PARAM_ERROR" }, 	//-2
  { QSEE_STOR_NOT_FOUND_ERROR, 				"QSEE_STOR_NOT_FOUND_ERROR" },     	//-3
  { QSEE_STOR_PARTI_NOT_FOUND_ERROR, 			"QSEE_STOR_PARTI_NOT_FOUND_ERROR" },
  { QSEE_STOR_VERSION_MISMATCH, 				"QSEE_STOR_VERSION_MISMATCH" },
  { QSEE_STOR_OUT_OF_RESOURCES, 				"QSEE_STOR_OUT_OF_RESOURCES" },
  { QSEE_STOR_NOT_SUPPORTED, 					"QSEE_STOR_NOT_SUPPORTED" },
  { QSEE_STOR_RPMB_UNKNOWN_ERROR, 			"QSEE_STOR_RPMB_UNKNOWN_ERROR" },
  { QSEE_STOR_RPMB_MAC_ERROR, 				"QSEE_STOR_RPMB_MAC_ERROR" },
  { QSEE_STOR_RPMB_WRITE_COUNTER_ERROR, 		"QSEE_STOR_RPMB_WRITE_COUNTER_ERROR" },
  { QSEE_STOR_RPMB_ADDR_ERROR, 				"QSEE_STOR_RPMB_ADDR_ERROR" },
  { QSEE_STOR_RPMB_WRITE_ERROR,			 	"QSEE_STOR_RPMB_WRITE_ERROR" },
  { QSEE_STOR_RPMB_READ_ERROR, 				"QSEE_STOR_RPMB_READ_ERROR" },
  { QSEE_STOR_RPMB_NOT_PROVISIONED_ERROR,		"QSEE_STOR_RPMB_NOT_PROVISIONED_ERROR" },
  { QSEE_STOR_RPMB_MAC_GENERATION_ERROR, 		"QSEE_STOR_RPMB_MAC_GENERATION_ERROR" },
  { QSEE_STOR_RPMB_RNG_GENERATION_ERROR, 		"QSEE_STOR_RPMB_RNG_GENERATION_ERROR" },
  { QSEE_STOR_RPMB_NONCE_ERROR, 				"QSEE_STOR_RPMB_NONCE_ERROR" },
  { QSEE_STOR_RPMB_PROVISIONED_ERROR, 		"QSEE_STOR_RPMB_PROVISIONED_ERROR" },
  { QSEE_STOR_PARTITION_FOUND, 				"QSEE_STOR_PARTITION_FOUND" },
  { QSEE_STOR_APP_ID_ERROR, 					"QSEE_STOR_APP_ID_ERROR" },
  { QSEE_STOR_RPMB_AUTOPROV_DISABLED,			"QSEE_STOR_RPMB_AUTOPROV_DISABLED" }, 	//-21
  { QSEE_STOR_ACCESS_VIOLATION_ERROR,			"QSEE_STOR_ACCESS_VIOLATION_ERROR" },   //-22
  { 0, 0 }
};

char* ErrorToMsg(int code)
{
  for (int i = 0; error_codes[i].name; ++i)
  {
    if (error_codes[i].value == code)
      return error_codes[i].name;
  }
  return "unknown";
}

/************************ Helper functions **********************************/
int ProfileWrite(uint32 StartSector, uint32 nSector)
{
  unsigned long total_time;
  int ret = 0;//SUCCESS;
  total_time = qsee_get_uptime();
  ret = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                StartSector,							// start_sector = 0
                                nSector,           //num_sector
                                gBuff);
  total_time = qsee_get_uptime() - total_time;
  qsee_log(QSEE_LOG_MSG_DEBUG,"----- Write %d Sector, size= %d bytes, took %u ms -----", nSector, (nSector*512),total_time);
  if (ret != QSEE_STOR_SUCCESS)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write buffer, qsee_stor_write_sectors return=%s",ErrorToMsg(ret));
  }
  return ret;
}

int ProfileRead(uint32 StartSector, uint32 nSector)
{
  unsigned long total_time;
  int ret = 0;//SUCCESS;
  total_time = qsee_get_uptime();
  ret = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
                               StartSector,							// start_sector = 0
                               nSector,           //num_sector
                               gBuff);
  total_time = qsee_get_uptime() - total_time;
  qsee_log(QSEE_LOG_MSG_DEBUG,"----- Read %d Sectors, size= %d bytes, took %u ms -----", nSector, (nSector*512), total_time);
  if (ret != QSEE_STOR_SUCCESS)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read buffer, qsee_stor_read_sectors return=%s",ErrorToMsg(ret));
  }
  return ret;
}
int BufferBackup(uint8 *pBuf, uint32 start_sector, uint32 num_sectors)
{
  int       status = QSEE_STOR_SUCCESS;
  if (NULL == pBuf)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- invalid parameter---------------");
    return QSEE_STOR_ERROR;
  }
  /* Restore the test sectors */
  status  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
                                   start_sector,							// start_sector = 0
                                   num_sectors,
                                   pBuf);
  if (QSEE_STOR_SUCCESS != status) {
    qsee_log(QSEE_LOG_MSG_ERROR, "Backup: Fail to readSectors, return=%s\n--------------",ErrorToMsg(status));
  }
  return status;
}

int BufferRestore(uint8 *pBuf, uint32 start_sector, uint32 num_sectors)
{
  int       status = QSEE_STOR_SUCCESS;
  if (NULL == pBuf)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- invalid parameter---------------");
    return QSEE_STOR_ERROR;
  }
  /* Restore the test sectors */
  status  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                    start_sector,							// start_sector = 0
                                    num_sectors,
                                    pBuf);
  if (QSEE_STOR_SUCCESS != status) {
    qsee_log(QSEE_LOG_MSG_ERROR, "Restore: Fail to writeSectors, return = %d\n--------------",ErrorToMsg(status));
  }
  return status;
}

void qsee_stor_memset(uint8* pBuff, uint32 sizeBuff,uint8 pattern)
{
  int i=0;
  for(i=0; i< sizeBuff; i++)
  {
    pBuff[i]=pattern;
  }
}

/************************ Test functions **********************************/
int qsee_stor_deviceInitTest()
{
  int ret = 0;//SUCCESS;
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceInitTest Test start...---------------------------");

  ret = qsee_stor_device_init(rpmb_device_id,0,&gDevHandle);
  if (QSEE_STOR_SUCCESS!=ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "qsee_stor_device_init return=%s. Test FAILED!",ErrorToMsg(ret));
  }
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceInitTest Test PASSED---------------------------");
  return ret;
}

int qsee_stor_deviceAddOpenPartitionTest(uint8 partition_guid)
{
  int ret = 0;//SUCCESS;
  uint32 num_sectors = 0;
  uint32 partition_id = 0;

  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceAddOpenPartitionTest start...---------------------------");

  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
    num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX;

  }
  else{
    gDeviceID = gpt_device_id;
    num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX; //GPT

  }
  const char* app_name = get_ta_app_name();
  qsee_log(QSEE_LOG_MSG_DEBUG, "--   qsee_stor_open_partition() app_name=%s",app_name);
  if (strncmp(app_name, "SampleApp64", sizeof("SampleApp64")))
  {
    partition_id = TZ_RPMB_PARITION_ID_0;
  }
  else
  {
    partition_id = TZ_RPMB_PARITION_ID_1;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "--   qsee_stor_open_partition() app_name=%s partition_id = %x",app_name, partition_id);
  ret = qsee_stor_open_partition(&gDevHandle, partition_id, &gClientHandle);
  if ((QSEE_STOR_SUCCESS != ret) || (gClientHandle == NULL))
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "--   qsee_stor_open_partition() id = %x FAILED! return=%s",
             partition_id, ErrorToMsg(ret));
    qsee_log(QSEE_LOG_MSG_DEBUG, "--   Add a new partition...... partition_id = %x", partition_id);
    ret = qsee_stor_add_partition(&gDevHandle, partition_id, num_sectors);
    if( QSEE_STOR_SUCCESS != ret )
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "--   qsee_stor_add_partition() id = %x FAILED! return=%s", 
               partition_id, ErrorToMsg(ret));
      return ret;
    }
    qsee_log(QSEE_LOG_MSG_DEBUG, " Successfully added partition id = %x", partition_id);
    ret = qsee_stor_open_partition(&gDevHandle, partition_id ,&gClientHandle);
    if ((QSEE_STOR_SUCCESS != ret) || (gClientHandle == NULL))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to open partition_id = %x, return=%s",
               partition_id, ErrorToMsg(ret));
      return ret;
    }
    qsee_log(QSEE_LOG_MSG_DEBUG, "-- Successfully open  partition  = %x", partition_id);

  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_deviceAddOpenPartitionTest PASSED---------------------------");
  return ret;
}

int qsee_stor_devRWSingleContTest(uint8 partition_guid)
{
  int ret = 0;//SUCCESS;
  uint32 nTotalSectors = 0,nSectorsTest = TZ_MAX_SECTOR_PER_TRANSACTION, StartSector=0;
//	uint32 partitionSize = 0;
  uint32 i = 0, j=0;

  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleContTest Test start...---------------------------");
  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
  }
  else{
    gDeviceID = gpt_device_id;
  }

  ret = qsee_stor_client_get_info(&gClientHandle,
                                  &gClientInfo);
  if (QSEE_STOR_SUCCESS!=ret)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "-- qsee_stor_client_get_info , return=%s",ErrorToMsg(ret));
    return ret;
  }
  nTotalSectors = gClientInfo.total_sectors;
  //partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

  qsee_log(QSEE_LOG_MSG_DEBUG, "-- Total Sectors =%d",nTotalSectors);
  //qsee_log(QSEE_LOG_MSG_DEBUG, "-- PartitionSize =%d",partitionSize);

  // Backup buffer before testing read/write operations
  qsee_log(QSEE_LOG_MSG_DEBUG, "----Backup data before start read/write testing ---------");
  if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Backup buffer");
    return QSEE_STOR_ERROR;
  }
  qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);

  for(j=0; (j+nSectorsTest) < nTotalSectors; j=(j+nSectorsTest))
  {
    /*Logic to write data to card*/
    //StartSector = nSectorsTest*j;
    StartSector = j;
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Write %d SectorTest, start at=%d sector ---",nSectorsTest, StartSector);
    ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                   StartSector,							// start_sector = 0
                                   nSectorsTest,           //num_sector
                                   gBuff);
    if (QSEE_STOR_SUCCESS != ret)
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Write at=%d sector ---", StartSector);
      qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_write_sectors returns=%s",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Write ==> Good -- ");

  }

  if( QSEE_STOR_SUCCESS == ret )
  {
    /*Logic to read data from card*/
    for(j=0; (j+nSectorsTest) < nTotalSectors; j=(j+nSectorsTest))
    {
      qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
      /*Logic to read data from card*/
      //StartSector = nSectorsTest*j;
      StartSector = j;
      //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Read %d SectorTest , at sector %d---",nSectorsTest, StartSector);

      ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
                                    StartSector,							// start_sector = 0
                                    nSectorsTest,
                                    gBuff);
      if (ret != QSEE_STOR_SUCCESS )
      {
        qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Read @ %d Sector ---", StartSector);
        qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_read_sectors return=%s",ErrorToMsg(ret));
        //Restore Data
        BufferRestore(gBackupBuff,0,nTotalSectors);
        return ret;
      }
      //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Read  ==> Good ---");
      if( (QSEE_STOR_SUCCESS == ret) )
      {
        /* Verify buffer */
        for(i=0; i<TZ_BUFF_SIZE_TEST; i++)
        {
          if( gBuff[i] != 0x5A )
          {
            qsee_log(QSEE_LOG_MSG_ERROR, "-- Content mismatched at , gbuff[%d]=0x%x , Test failed ---",i,gBuff[i]);
            ret = QSEE_STOR_ERROR;
            //Restore Data
            BufferRestore(gBackupBuff,0,nTotalSectors);
            qsee_log(QSEE_LOG_MSG_ERROR, "--------------------- qsee_stor_devRWSingleContTest FAILED ---------------------------");
            return ret;
          }
        }
        //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Data are verified ---");
      }

    }
  }

  //Restore Data
  qsee_log(QSEE_LOG_MSG_DEBUG, "--Read/write & data verification completed. Now Restoring data...--");
  BufferRestore(gBackupBuff,0,nTotalSectors);
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleContTest PASSED ---------------------------");
  return ret;
}

int qsee_stor_devRWSingleAltTest(uint8 partition_guid)
{
  int ret = 0;//SUCCESS;
  uint32 nTotalSectors = 0,nSectorsTest = TZ_MAX_SECTOR_PER_TRANSACTION, StartSector=0;
  //uint32 partitionSize = 0;
  uint32 i = 0, j=0;
  uint8 ReadBuff[TZ_BUFF_SIZE_TEST] = {0};


  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleAltTest Test start...---------------------------");
  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
  }
  else{
    gDeviceID = gpt_device_id;
  }
  ret = qsee_stor_client_get_info(&gClientHandle,
                                  &gClientInfo);
  if (QSEE_STOR_SUCCESS!=ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to get_info, return=%s--",ErrorToMsg(ret));
    return ret;
  }
  nTotalSectors = gClientInfo.total_sectors;
  //partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

  // Backup buffer before testing read/write operations
  if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Backup buffer");
    return QSEE_STOR_ERROR;
  }
  qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);
  for(j=0; (j+nSectorsTest) < nTotalSectors; j=(j+nSectorsTest))
  {
    /*Logic to write data to card*/
    //StartSector = nSectorsTest*j;
    StartSector = j;
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Write %d SectorTest at a time start at=%d sector---",nSectorsTest, StartSector);
    ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                   StartSector,							// start_sector = 0
                                   nSectorsTest,           //num_sector
                                   gBuff);
    if (ret != QSEE_STOR_SUCCESS )
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Write at=%d sector ---", StartSector);
      qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_write_sectors returns=%s--",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Write ==> Good -- ");
    /*Logic to read data from card*/
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Read %d SectorTest at a time start at sector %d ---",nSectorsTest, StartSector);
    qsee_stor_memset(ReadBuff,TZ_BUFF_SIZE_TEST,0x00);
    ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
                                  StartSector,							// start_sector = 0
                                  nSectorsTest,
                                  ReadBuff);
    if (ret != QSEE_STOR_SUCCESS )
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read at=%d sector ---", StartSector);
      qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_read_sectors returns=%s --",ErrorToMsg(ret));

      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Read  ==> Good ---");

    /* Verify buffer */
    for(i=0; i<TZ_BUFF_SIZE_TEST; i++)
    {
      if( ReadBuff[i] != 0x5A )
      {
        qsee_log(QSEE_LOG_MSG_ERROR, "-- Content mismatched at , Readbuff[%d]= 0x%x, Test failed ---",i,ReadBuff[i]);
        // Restore partition
        BufferRestore(gBackupBuff,0,nTotalSectors);
        qsee_log(QSEE_LOG_MSG_ERROR, "--------------------- qsee_stor_devRWSingleAltTest FAILED---------------------------");
        return ret;

      }
    }
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Data are verified ---");
  }
  // Restore partition
  //qsee_log(QSEE_LOG_MSG_DEBUG, "--Read/write & data verification completed. Now Restoring data...--");
  BufferRestore(gBackupBuff,0,nTotalSectors);
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWSingleAltTest PASSED---------------------------");
  return ret;
}

int qsee_stor_devRWDoubleContTest(uint8 partition_guid)
{
  int ret = 0;//SUCCESS;
  uint32 nTotalSectors = 0,nSectorsTest = TZ_MAX_SECTOR_PER_TRANSACTION, StartSector=0;
  //uint32 partitionSize = 0;
  uint32 i = 0, j=0, count = 0;
  uint8 pattern = 0x00;

  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWDoubleContTest start...---------------------------");
  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
  }
  else{
    gDeviceID = gpt_device_id;
  }
  ret = qsee_stor_client_get_info(&gClientHandle,
                                  &gClientInfo);
  if (QSEE_STOR_SUCCESS!=ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to get_info, return=%s",ErrorToMsg(ret));
    return ret;
  }
  nTotalSectors = gClientInfo.total_sectors;
  //partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

  // Backup buffer before testing read/write operations
  if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Backup buffer");
    return QSEE_STOR_ERROR;
  }
  qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);  // pattern 0x5A
  qsee_stor_memset(gBuff2,TZ_BUFF_SIZE_TEST,0x7F); // pattern 0xFF

  for(j=0; ((j+nSectorsTest )<nTotalSectors); j=(j+nSectorsTest))
  {
    /*Logic to write data to card*/
    //StartSector = nSectorsTest*j;
    StartSector = j;
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Write 2 SectorTest at a time start at =%d sector ---",StartSector);
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Count = %d",count);
    if( (count%2) )
    {
      //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Write %d sectors, @ %dth with pattern 0x5A---",nSectorsTest, StartSector);
      ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                     StartSector,							// start_sector = 0
                                     nSectorsTest,           //num_sector
                                     gBuff);				// buffer with pattern 0x5A
    }else{
      //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Write %d sectors, @ %dth with pattern 0x7F---",nSectorsTest, StartSector);
      ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                     StartSector,							// start_sector = 0
                                     nSectorsTest,           //num_sector
                                     gBuff2);				// buffer filled with pattern 0xFF

    }
    if (ret != QSEE_STOR_SUCCESS )
    {

      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write @ %dth sector ---", StartSector);
      qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_write_sectors returns=%s",ErrorToMsg(ret));

      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    count++;
    //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Write ==> Good -- ");
  }
  qsee_log(QSEE_LOG_MSG_DEBUG, "---- All Writes ==> Good -- ");
  if (QSEE_STOR_SUCCESS == ret)
  {
    count = 0;
    /*Logic to read data from card*/
    for(j=0; ((j+nSectorsTest) < nTotalSectors); j=(j+nSectorsTest))
    {
      qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
      /*Logic to read data from card*/
      //StartSector = nSectorsTest*j;
      StartSector = j;
      //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Read %d Sectors, @ %dth sector ---",nSectorsTest, StartSector);

      ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
                                    StartSector,							// start_sector = 0
                                    nSectorsTest,
                                    gBuff);
      if (ret != QSEE_STOR_SUCCESS )
      {
        qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read @ %dth sector ---", StartSector);
        qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_read_sectors returns=%s",ErrorToMsg(ret));
        BufferRestore(gBackupBuff,0,nTotalSectors);
        return ret;
      }
      //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Read  ==> Good ---");
      /* Verify buffer */
      for(i=0; i<TZ_BUFF_SIZE_TEST; i++)
      {
        if( count%2 )
          pattern = 0x5A;
        else
          pattern = 0x7F;

        if( gBuff[i] != pattern )
        {
          qsee_log(QSEE_LOG_MSG_ERROR, "-- Content mismatched at , pbuff[%d]=0x%x , Test failed ---",i,gBuff[i]);
          ret = QSEE_STOR_ERROR;
          BufferRestore(gBackupBuff,0,nTotalSectors);
          qsee_log(QSEE_LOG_MSG_ERROR, "-------------------- qsee_stor_devRWDoubleContTest FAILED ---------------------------");
          return ret;
        }
      }
      count++;
      //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Data are verified ---");
    }
    // Restore partition
  }
  qsee_log(QSEE_LOG_MSG_DEBUG, "---- Read/Write and Data Integrity are completed---");

  BufferRestore(gBackupBuff,0,nTotalSectors);
  qsee_log(QSEE_LOG_MSG_DEBUG, "-------------------- qsee_stor_devRWDoubleContTest PASSED ---------------------------");
  return ret;
}

int qsee_stor_devRWDoubleAltTest(uint8 partition_guid)
{
  int ret = 0;//SUCCESS;
  uint32 nTotalSectors = 0,nSectorsTest = TZ_MAX_SECTOR_PER_TRANSACTION, StartSector= 0;
  //uint32 partitionSize = 0;
  uint32 i = 0, j=0, count = 0;
  uint8 pattern = 0x00;
  uint8 ReadBuff[TZ_BUFF_SIZE_TEST]={0x00};

  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWDoubleAltTest start...---------------------------");
  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
  }
  else{
    gDeviceID = gpt_device_id;
  }
  ret = qsee_stor_client_get_info(&gClientHandle,
                                  &gClientInfo);
  if (QSEE_STOR_SUCCESS!=ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to get_info, return=%s",ErrorToMsg(ret));
    return ret;
  }
  nTotalSectors = gClientInfo.total_sectors;
  //partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;


  // Backup buffer before testing read/write operations
  if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Backup buffer");
    return QSEE_STOR_ERROR;
  }
  qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);
  qsee_stor_memset(gBuff2,TZ_BUFF_SIZE_TEST,0x7A);
  qsee_log(QSEE_LOG_MSG_DEBUG, "-- Filling RPMB test partition by writting %d till reaching size of test parition=%d sectors ---",nSectorsTest, nTotalSectors);
  for(j=0; ( (j+nSectorsTest) < nTotalSectors); j=(j+nSectorsTest) )
  {
    /*Logic to write data to card*/
    //StartSector = nSectorsTest*j;
    StartSector = j;
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Write %d sectors at start @ %d th sector ---",nSectorsTest, StartSector);
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Count = %d",count);
    if( count % 2 )
    {
      //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Write % sectors, @ %dth sector with pattern 0x5A---",nSectorsTest, StartSector);
      pattern = 0x5A;
      ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                     StartSector,							// start_sector = 0
                                     nSectorsTest,           //num_sector
                                     gBuff);
    }
    else
    {
      //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Write %d sectors, @ %dth sector with pattern 0x7A---",nSectorsTest, StartSector);
      pattern = 0x7A;
      //qsee_log(QSEE_LOG_MSG_DEBUG, "pReadbuff[0]=0x%x ---",gBuff2[i]);
      ret  = qsee_stor_write_sectors(&gClientHandle,             //ClientHandle
                                     StartSector,							// start_sector = 0
                                     nSectorsTest,           // 2
                                     gBuff2);
    }
    if (ret != QSEE_STOR_SUCCESS )
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write @ %d sector ---", StartSector);
      qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_write_sectors returns=%s",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Write ==> Good -- ");

    /*Logic to read data from card*/
    //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Read %d Test, @ %dth sector ---",nSectorsTest, StartSector);
    qsee_stor_memset(ReadBuff,TZ_BUFF_SIZE_TEST,0x00);
    ret  = qsee_stor_read_sectors(&gClientHandle,             //ClientHandle
                                  StartSector,							// start_sector = 0
                                  nSectorsTest,
                                  ReadBuff);
    if (ret != QSEE_STOR_SUCCESS )
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read @ %dth sector ---", StartSector);
      qsee_log(QSEE_LOG_MSG_ERROR, "-- qsee_stor_read_sectors returns=%s",ErrorToMsg(ret));

      BufferRestore(gBackupBuff,0,nTotalSectors);
      return  ret;
    }
    //qsee_log(QSEE_LOG_MSG_DEBUG, "---- Read  ==> Good ---");
    /* Verify buffer */
    for(i=0; i < TZ_BUFF_SIZE_TEST; i++)
    {
      //qsee_log(QSEE_LOG_MSG_DEBUG, "pattern = 0x%x, pReadbuff[%d]=0x%x ---",pattern,i, ReadBuff[i]);
      if( ReadBuff[i] != pattern )
      {
        qsee_log(QSEE_LOG_MSG_ERROR, "-- Content mismatched at , pReadbuff[%d]=0x%x , Test failed ---",i,ReadBuff[i]);
        ret = QSEE_STOR_ERROR;
        BufferRestore(gBackupBuff,0,nTotalSectors);
        qsee_log(QSEE_LOG_MSG_ERROR, "----------------------- qsee_stor_devRWDoubleAltTest FAILED---------------------------");
        return ret;

      }
    }
    //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Data are verified ---");
    count++;
  } // end for loop
  // Restore partition
  qsee_log(QSEE_LOG_MSG_DEBUG, "-- Write/Read and Data are verified ---");
  BufferRestore(gBackupBuff,0,nTotalSectors);
  qsee_log(QSEE_LOG_MSG_DEBUG, "----------------------- qsee_stor_devRWDoubleAltTest PASSED---------------------------");
  return ret;
}

int qsee_stor_AdversarialTest(uint8 partition_guid)
{
  int ret = QSEE_STOR_SUCCESS;
  uint32 num_sectors = 0;
  uint32 partition_id;

  qsee_stor_device_info_t device_info;

  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
    num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX;

  }
  else{
    gDeviceID = gpt_device_id;
    num_sectors = TZ_RPMB_TEST_NUM_SECTOR_MAX; //GPT

  }
  ret = qsee_stor_open_partition(NULL,TZ_RPMB_PARITION_ID_0,&gClientHandle);
  if (QSEE_STOR_SUCCESS == ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, " Test1: Expecting api returns FAILED, ret=%s, TEST FAILED!!!",ErrorToMsg(ret));
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, " Test1: qsee_stor_open_partition w/NULL Handle, ret=%s. Test PASSED!",ErrorToMsg(ret));
  }

  //AddPartitionTwice test
  ret = qsee_stor_add_partition(&gDevHandle,TZ_RPMB_PARITION_ID_0,num_sectors);
  if( QSEE_STOR_SUCCESS == ret )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "Test2: Add same partition twice, expecting api returns FAIL, ret=%s, TEST FAILED",ErrorToMsg(ret));
    //return ret;
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "Test2:  Add same partition twice failed as expected, ret=%s. Test PASSED!",ErrorToMsg(ret));
  }

  //AddPartitionTwice test
  ret = qsee_stor_add_partition(NULL,TZ_RPMB_PARITION_ID_0,num_sectors);
  if( QSEE_STOR_SUCCESS == ret )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "Test3: Testing qsee_stor_add_partition with NULL DevHandle FAILED");
    //return ret;
  }
  else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "Test3: add_partition w/NULL Handle ret=%s as expected. Test PASSED!",ErrorToMsg(ret));
  }

  //AddNewPartition with numsectors exceeds the RPMB physical partition
  partition_id = 0xFF00;
  ret = qsee_stor_device_get_info(&gDevHandle,
                                  &device_info);
  if( QSEE_STOR_SUCCESS != ret )
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "Test4: qsee_stor_device_get_info ret=%s",ErrorToMsg(ret));
  }
  num_sectors = device_info.available_sectors;
  qsee_log(QSEE_LOG_MSG_DEBUG, " DeviceInfo.available_sectors = %ld",num_sectors);
  qsee_log(QSEE_LOG_MSG_DEBUG, " DeviceInfo.total_sectors = %ld", device_info.total_sectors);

  ret = qsee_stor_add_partition(&gDevHandle,partition_id,(num_sectors + 1));
  if( QSEE_STOR_SUCCESS == ret )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "Test4: qsee_stor_add_partition w/ num_sector exceeds available sectors in Physical Parition\n");
    qsee_log(QSEE_LOG_MSG_ERROR, "Test4: Expecting api returns error instead of SUCCESS, TEST FAILED!",ErrorToMsg(ret));
    //return ret;
  }
  else{
    qsee_log(QSEE_LOG_MSG_DEBUG, "Test4: api returns %s as expected",ErrorToMsg(ret));
    qsee_log(QSEE_LOG_MSG_DEBUG, "Test4: Adding Test partition w/total sectors exceeds physical RPMB size. TEST PASSED");
  }

  return QSEE_STOR_SUCCESS;
}

int qsee_stor_devRWAdversarialTest(uint8 partition_guid)
{
  int ret = QSEE_STOR_SUCCESS;
  uint32 nTotalSectors = 0,nSectorsTest = TZ_MAX_SECTOR_PER_TRANSACTION, StartSector=0;
//	uint32 partitionSize = 0;
//	uint32 i = 0, j=0;
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWAdversarialTest start...---------------------------");
  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
  }
  else{
    gDeviceID = gpt_device_id;
  }
  ret = qsee_stor_client_get_info(&gClientHandle,
                                  &gClientInfo);
  if (QSEE_STOR_SUCCESS!=ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to get_info, return=%s",ErrorToMsg(ret));
    return ret;
  }
  nTotalSectors = gClientInfo.total_sectors;
  //partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

  // Backup buffer before testing read/write operations
  if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Backup buffer");
    return QSEE_STOR_ERROR;
  }
  qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);

  qsee_log(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: Invalid handle ----------------");
  ret  = qsee_stor_write_sectors(NULL,             //ClientHandle
                                 StartSector,							// start_sector = 0
                                 nSectorsTest,           //num_sector
                                 gBuff);
  if (QSEE_STOR_SUCCESS == ret )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, " RWAdv Test1: qsee_stor_write_sectors with Invalid handle returns SUCCESS - Test FAILED ---------------");
  }
  else
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: qsee_stor_write_sectors with Invalid handle ERROR as expected. Test PASSED -------");
  }

  //
  ret  = qsee_stor_read_sectors(NULL,             //ClientHandle
                                StartSector,							// start_sector = 0
                                nSectorsTest,           //num_sector
                                gBuff);
  if (QSEE_STOR_SUCCESS == ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, " RWAdv Test1: qsee_stor_read_sectors with Invalid handle returns SUCCESS - Test FAILED ---------------");
  }
  else
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, " RWAdv Test1: qsee_stor_read_sectors with Invalid handle return ERROR as expected - Test PASSED------");
  }

  // qsee_stor_ReadSectorEndOfPartition & qsee_storWriteSectorEndOfPartition Test Cases

  StartSector = TZ_RPMB_TEST_NUM_SECTOR_MAX-1;
  qsee_log(QSEE_LOG_MSG_DEBUG, " RWAdv Test2: Trying to RW at %d sector address with %d sectors ----------------",StartSector, nSectorsTest);
  ret  = qsee_stor_write_sectors(&gClientHandle,
                                 StartSector,
                                 nSectorsTest,
                                 gBuff);
  qsee_log(QSEE_LOG_MSG_DEBUG, " RWAdv Test2: qsee_stor_write_sectors return=%s", ErrorToMsg(ret));

  if (QSEE_STOR_SUCCESS == ret)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "RWAdv Test2: Write pass EndOFParition - TEST FAILED expecting api returns failed instead of SUCCESS");
  }
  else{
    qsee_log(QSEE_LOG_MSG_DEBUG, "RWAdv Test2: Write pass EndOFParition return=%s - TEST PASSED",ErrorToMsg(ret));
  }

  ret  = qsee_stor_read_sectors(&gClientHandle,
                                StartSector,
                                nSectorsTest,
                                gBuff);
  qsee_log(QSEE_LOG_MSG_DEBUG, " RWAdv Test2: return = %s", ErrorToMsg(ret));
  if (ret == QSEE_STOR_SUCCESS )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "RWAdv Test2: Read pass EndOFParition - TEST FAILED expecting api returns failed instead of SUCCESS");
  }else {
    qsee_log(QSEE_LOG_MSG_DEBUG, "RWAdv Test2: Read pass EndOFParition ret=%s - TEST PASSED",ErrorToMsg(ret));
  }

  // qsee_stor_ReadSectorInvalidAddr & qsee_stor_WriteSectorInvalidAddr Test Cases
  qsee_log(QSEE_LOG_MSG_DEBUG, " RWAdv Test3: RW @ %d sectors with %d sector ----------------" ,StartSector,nSectorsTest  );
  StartSector = TZ_RPMB_TEST_NUM_SECTOR_MAX + 1;
  ret  = qsee_stor_write_sectors(&gClientHandle,
                                 StartSector,
                                 nSectorsTest,
                                 gBuff);

  if (ret == QSEE_STOR_SUCCESS )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "RWAdv Test3: write Invalid Address - TEST FAILED, expecting api returns ERROR instead of SUCCESS");
  }
  else{
    qsee_log(QSEE_LOG_MSG_DEBUG, "RWAdv Test3:  Write Invalid Address ret=%s - TEST PASSED",ErrorToMsg(ret));
  }
  ret  = qsee_stor_read_sectors(&gClientHandle,
                                StartSector,
                                nSectorsTest,
                                gBuff);

  if (ret == QSEE_STOR_SUCCESS )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "RWAdv Test3: Read Invalid Addr - TEST FAILED, expecting api returns ERRROR instead of QSEE_STOR_SUCCESS");
  }
  else{
    qsee_log(QSEE_LOG_MSG_DEBUG, "RWAdv Test3: Read Invalid Address ret=%s - TEST PASSED",ErrorToMsg(ret));
  }

  //Restore Data
  BufferRestore(gBackupBuff,0,nTotalSectors);
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_devRWAdversarialTest END ---------------------------");
  return QSEE_STOR_SUCCESS;
}

int qsee_stor_RWProfileTest(uint8 partition_guid)
{
  int ret = 0;//SUCCESS;
  uint32 nTotalSectors = 0,nSectorsTest = TZ_MAX_SECTOR_PER_TRANSACTION, StartSector=0;
  uint32 i = 0, j=0;
  unsigned long long total_time;
  //int    tput;
  int size = TZ_RPMB_TEST_NUM_SECTOR_MAX*512;
  int iterations = 5;

  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- qsee_stor_RWProfile Test start...---------------------------");
  if( partition_guid==0 )
  {
    gDeviceID = rpmb_device_id;
  }
  else{
    gDeviceID = gpt_device_id;
  }

  ret = qsee_stor_client_get_info(&gClientHandle,
                                  &gClientInfo);
  if (QSEE_STOR_SUCCESS!=ret)
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "-- qsee_stor_client_get_info return %s",ErrorToMsg(ret));
    return ret;
  }
  nTotalSectors = gClientInfo.total_sectors;
  //partitionSize= gClientInfo.bytes_per_sector * nTotalSectors;

  //qsee_log(QSEE_LOG_MSG_DEBUG, "-- Total Sectors =%d",nTotalSectors);

  // Backup buffer before testing read/write operations
  if( QSEE_STOR_SUCCESS != BufferBackup(gBackupBuff,0,nTotalSectors) )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to Backup buffer");
    return QSEE_STOR_ERROR;
  }
  qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x5A);

  //qsee_log(QSEE_LOG_MSG_DEBUG,"---Performance qsee_stor_write_sectors Test -with Data Size: %u", size);

  StartSector = 0;
  nSectorsTest = 1;
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- Start Profiling for %d Sector ---", nSectorsTest);
  for (i=0; i < iterations; i++)
  {
    /*Logic to write data to card*/
    if(QSEE_STOR_SUCCESS != ProfileWrite(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write buffer,  qsee_stor_write_sectors return=%s",ErrorToMsg(ret));
      BufferBackup(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
    if (QSEE_STOR_SUCCESS != ProfileRead(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read buffer, qsee_stor_read_sectors return=%s",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
  }
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- End Profiling for %d Sector ---", nSectorsTest);
  nSectorsTest = 16;
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- Start Profiling for %d Sector ---", nSectorsTest);
  for (i=0; i < iterations; i++)
  {
    /*Logic to write data to card*/
    if(QSEE_STOR_SUCCESS != ProfileWrite(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write buffer,  qsee_stor_write_sectors return=%s",ErrorToMsg(ret));
      BufferBackup(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
    if (QSEE_STOR_SUCCESS != ProfileRead(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read buffer, qsee_stor_read_sectors return=%s",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
  }
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- End Profiling for %d Sector ---", nSectorsTest);
  nSectorsTest = 32;
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- Start Profiling for %d Sector ---", nSectorsTest);
  for (i=0; i < iterations; i++)
  {
    /*Logic to write data to card*/
    if(QSEE_STOR_SUCCESS != ProfileWrite(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write buffer, qsee_stor_write_sectors return return=%s",ErrorToMsg(ret));
      BufferBackup(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
    if (QSEE_STOR_SUCCESS != ProfileRead(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read buffer, qsee_stor_read_sectors return=%s",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
  }
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- End Profiling for %d Sector ---", nSectorsTest);
  nSectorsTest = 15;
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- Start Profiling for %d Sector ---", nSectorsTest);
  for (i=0; i < iterations; i++)
  {
    /*Logic to write data to card*/
    if(QSEE_STOR_SUCCESS != ProfileWrite(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write buffer,  qsee_stor_write_sectors return=%s",ErrorToMsg(ret));
      BufferBackup(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
    if (QSEE_STOR_SUCCESS != ProfileRead(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read buffer, qsee_stor_read_sectors return=%s",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
  }
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- End Profiling for %d Sector ---", nSectorsTest);
  nSectorsTest = 31;
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- Start Profiling for %d Sector ---", nSectorsTest);
  for (i=0; i < iterations; i++)
  {
    /*Logic to write data to card*/
    if(QSEE_STOR_SUCCESS != ProfileWrite(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to write buffer,  qsee_stor_write_sectors return=%s",ErrorToMsg(ret));
      BufferBackup(gBackupBuff,0,nTotalSectors);
      return ret;
    }
    qsee_stor_memset(gBuff,TZ_BUFF_SIZE_TEST,0x00);
    if (QSEE_STOR_SUCCESS != ProfileRead(StartSector, nSectorsTest))
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "-- Fail to read buffer, qsee_stor_read_sectors return=%s",ErrorToMsg(ret));
      BufferRestore(gBackupBuff,0,nTotalSectors);
      return ret;
    }
  }
  qsee_log(QSEE_LOG_MSG_DEBUG,"--- End Profiling for %d Sector ---", nSectorsTest);
  //Restore Data
  BufferRestore(gBackupBuff,0,nTotalSectors);
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------------------- END of qsee_stor_RWProfile Test ---------------------------");
  return ret;
}

int run_stor_test()
{

  int ret = 0;
  int retVal = 0;
  uint8 partition_guid=0;

  qsee_log(QSEE_LOG_MSG_DEBUG, "---------START API FUNCTIONAL QSEE Storage  TESTS ---------------------------------");

  // SdDeviceInit Test
  retVal = qsee_stor_deviceInitTest();
  if( retVal != QSEE_STOR_SUCCESS )
  {
    //qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_deviceInitTest() FAILED! returned = %ld", retVal);
    return 1;
  }

  // SdDeviceAddRPMBPartition Test
  retVal = qsee_stor_deviceAddOpenPartitionTest(partition_guid);
  if( retVal != 0 )
  {
    //qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_deviceAddOpenPartitionTest FAILED! returned = %ld", retVal);
    return 1;
  }

  //Seq Read/Write with 1 pattern Test Cases
  retVal = qsee_stor_devRWSingleContTest(partition_guid);
  if( retVal != 0 )
  {
    //qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWSingleContTest FAILED! returned = %ld", retVal);
    return 1;
  }

  //RandomSeq Read/Write with 1 pattern Test Cases
  retVal = qsee_stor_devRWSingleAltTest(partition_guid);
  if( retVal != 0 )
  {
    //qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWSingleAltTest FAILED! returned = %ld", retVal);
    return 1;
  }

  //Seq Read/Write with 2 patterns Test Cases
  retVal = qsee_stor_devRWDoubleContTest(partition_guid);
  if( retVal != 0 )
  {
    //qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWDoubleContTest FAILED! returned = %ld", retVal);
    return 1;
  }
#if 0
  //RandomSeq Read/Write with 2 pattern Test Cases
  retVal = qsee_stor_devRWDoubleAltTest(partition_guid);
  if( retVal != 0 )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWDoubleAltTest FAILED! returned = %ld", retVal);
    return 1;
  }
#endif
  qsee_log(QSEE_LOG_MSG_DEBUG, "------------------------ END API FUNCTIONAL QSEE Storage UNIT TESTS --------------------");

  qsee_log(QSEE_LOG_MSG_DEBUG, "-- -------START API ADVERSARIAL QSEE Storage  TESTS ---------------------------------");

  retVal = qsee_stor_AdversarialTest(partition_guid);
  if ( retVal != 0)
  {
    return 1;
  }

  //RW Adversarial test cases
  retVal = qsee_stor_devRWAdversarialTest(partition_guid);
  if( retVal != 0 )
  {
    //qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_devRWDoubleAltTest FAILED! returned = %ld", retVal);
    return 1;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "-- -------START PROFILING READ/WRITE API TESTS ---------------------------------");
  retVal = qsee_stor_RWProfileTest(partition_guid);
  if( retVal != 0 )
  {
    //qsee_log(QSEE_LOG_MSG_ERROR, "   qsee_stor_RWSProfileTest FAILED! returned = %ld", retVal);
    return 1;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "---------PROFILING READ/WRITE TESTS END-------------------------------");
  qsee_log(QSEE_LOG_MSG_DEBUG, "--------- COMPLETED QSEE_STOR VALIDATION-------------------------------");
  return ret;
}
