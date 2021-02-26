/** @file I2Cgeneric.c

  I2C Generic App

             Copyright (c) 2014 - 2017 Qualcomm Technologies Incorporated.
                              All Rights Reserved.
                     Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/21/17   vg      Added test case for kpi
 04/10/17	ag		Updated API's to QupV3
 08/21/14   unr     Refactored

=============================================================================*/


#include "I2Capp.h"
#include "I2CKpi.h"

#define IS_INVALID_I2CADDR(addr) ((addr < 0x08)||(addr > 0x77)) ? TRUE : FALSE

/* I2C Transfer Context */
#define TRANSFER_DIRECTION_READ  1
#define TRANSFER_DIRECTION_WRITE 0

/* I2C Slave Device Types */
#define SLAVE_TYPE_DEF 0
#define SLAVE_TYPE_REG 1
#define SLAVE_TYPE_MEM 2

#define DATA_SIZE 2048

#define MAX_FREQ            3
#define MAX_DATA_SIZE       9      
#define TEST_CNT            1

static uint8 bbuf [DATA_SIZE] = {0, 0, 0, 0};
static uint8 rbuf [DATA_SIZE] = {0, 0, 0, 0};
static uint8 wbuf [DATA_SIZE] = {1, 2, 3, 4};

static int bus_feq [MAX_FREQ]={100,400,1000};
static int bus_data_size[MAX_DATA_SIZE]={1,8,16,32,64,256,512,1024,2048};
static uint8 freq_idx,data_idx,test_cnt=0;
static uint32 length;

static BusDebugInfo BusDebug;

/* Command Line Types */
typedef enum
{
  I2C_CORE_NUMBER = 0,
  SLAVE_ADDRESS,
  BUS_SPEED,
  TRANSFER_DIRECTION,
  OFFSET,
  LOOP_COUNT,
  BUS_DETECT,
  CMD_INVALID,
} command_option;

/* Command Parameters */
typedef struct
{
  UINT32 i2c_core_number;
  UINT32 slave_address;
  UINT32 bus_speed;
  UINT16 transfer_length;
  UINT16 offset;
  UINT16 slave_type;
  UINT16 loop_count;
  UINT32 transfer_direction;
  uint32 bus_scan;
} slave_context;

static slave_context sctxt;

slave_context *get_slave_context (UINTN argc, CHAR8 **argv);
VOID i2c_test_hex_dump (IN UINT8 *buffer, IN UINT32 length);
VOID i2c_test_hex_stream_to_buf (UINT8 *buffer, const CHAR8 *stream, UINT16 *length);
i2c_status i2c_bus_scan (uint32 i2c_core_number);

static void i2c_test_cmd_help (void)
{
  /* EFI_D_ERROR will print always */

  DEBUG((EFI_D_ERROR,"\ni2ctest v1.0\n"));

  DEBUG((EFI_D_ERROR,"Copyright (c) 2014 Qualcomm Technologies, Inc.\n"));
  DEBUG((EFI_D_ERROR,"             All Rights Reserved.\n"));
  DEBUG((EFI_D_ERROR,"     Qualcomm Confidential and Proprietary\n"));

  DEBUG((EFI_D_ERROR,"\nUsage:\n"));
  DEBUG((EFI_D_ERROR,"  start i2ctest.efi \"[OPTIONS]\"\n"));

  DEBUG((EFI_D_ERROR,"\nDescription:\n"));
  DEBUG((EFI_D_ERROR,"  Read or Write from an I2C slave on a particular I2C core. If\n"));
  DEBUG((EFI_D_ERROR,"  no options are specified, then run a test on I2C eeprom. For \n"));
  DEBUG((EFI_D_ERROR,"  a generic test, the maximum data that can be written or read\n"));
  DEBUG((EFI_D_ERROR,"  is limited to 64 bytes.\n"));

  DEBUG((EFI_D_ERROR,"\nOPTIONS:\n"));
  DEBUG((EFI_D_ERROR,"  -h, --help                 Print this help text\n"));
  DEBUG((EFI_D_ERROR,"  -c, --core <number>        Index to I2C core ranging 1-12\n"));
  DEBUG((EFI_D_ERROR,"  -s, --slaveaddress <addr>  7-bit I2C slave address in hex\n"));
  DEBUG((EFI_D_ERROR,"  -b, --busspeed <speed>     I2C Bus Speed in KHz\n"));
  DEBUG((EFI_D_ERROR,"  -r, --read <length>        Length of bytes to be read\n"));
  DEBUG((EFI_D_ERROR,"  -w, --write <hex_stream>   Hex stream: no spaces and no 0x\n"));
  DEBUG((EFI_D_ERROR,"  -o, --offset <offset>      Register or Memory offset specified in\n"));
  DEBUG((EFI_D_ERROR,"                             hex. For register offset, use 0xXX. For\n"));
  DEBUG((EFI_D_ERROR,"                             memory offset, use 0xXXXX.\n"));
  DEBUG((EFI_D_ERROR,"  -l,  --loop <iterations>   Number of iterations to run the test for\n"));
  DEBUG((EFI_D_ERROR,"  -krw,--kpi <KPI>           To run the test for KPI Numbers of read & write \n"));
  DEBUG((EFI_D_ERROR,"  -kr, --kpi <KPI>           To run the test for KPI Numbers of read          \n"));
  DEBUG((EFI_D_ERROR,"  -kw, --kpi <KPI>           To run the test for KPI Numbers of write         \n"));
    
  DEBUG((EFI_D_ERROR,"\nExamples:\n"));
  DEBUG((EFI_D_ERROR,"  // Perform an EEPROM test if EEPROM is available\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest\n\n"));
  DEBUG((EFI_D_ERROR,"  // Write 5 bytes to slave 0x36 on I2C core 6 at 100 KHz\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest \"--core 6 -s 0x36 -b 100 -w 0000C0FFEE\"\n\n"));
  DEBUG((EFI_D_ERROR,"  // Read 10 bytes from slave 0x39 on I2C core 9 at 400 KHz, thrice.\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest \"-c 9 -s 0x39 --busspeed 400 -r 10 --loop 3\"\n\n"));
  DEBUG((EFI_D_ERROR,"  // Read 10 bytes at offset 0x10 from a register type slave.\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest \"-c 9 -s 0x39 -b 400 -r 10 -o 0x10\"\n\n"));
  DEBUG((EFI_D_ERROR,"  // Read 10 bytes at offset 0x3010 from a memory type slave.\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest \"-c 9 -s 0x39 -b 400 -r 10 -o 0x3010\"\n\n"));
  DEBUG((EFI_D_ERROR,"  // Write bytes at offset 0x30 to a register type slave.\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest \"-c 9 -s 0x34 -b 100 -w 01020304 -o 0x30\"\n\n"));
  DEBUG((EFI_D_ERROR,"  // Perform an EEPROM RW test with KPI numbers\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest -krw\n\n"));
  DEBUG((EFI_D_ERROR,"  // Perform an EEPROM Read test with KPI numbers\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest -kr\n\n"));
  DEBUG((EFI_D_ERROR,"  // Perform an EEPROM Write test with KPI numbers\n"));
  DEBUG((EFI_D_ERROR,"  start I2CTest -kw\n\n"));
 
}

slave_context *get_slave_context (UINTN argc, CHAR8 **argv)
{
  UINT32 index = 0;

  while (index < argc)
  {
    command_option CmdOp = CMD_INVALID;

    if ((AsciiStriCmp (*(argv + index), "--core") == 0) ||
        (AsciiStriCmp (*(argv + index), "-c") == 0))
    {
      CmdOp = I2C_CORE_NUMBER;
    }
    else if ((AsciiStriCmp (*(argv + index), "--slaveaddress") == 0) ||
             (AsciiStriCmp (*(argv + index), "-s") == 0))
    {
      CmdOp = SLAVE_ADDRESS;
    }
    else if ((AsciiStriCmp (*(argv + index), "--busspeed") == 0) ||
             (AsciiStriCmp (*(argv + index), "-b") == 0))
    {
      CmdOp = BUS_SPEED;
    }
    else if ((AsciiStriCmp (*(argv + index), "--read") == 0)  ||
             (AsciiStriCmp (*(argv + index), "--write") == 0) ||
             (AsciiStriCmp (*(argv + index), "-r") == 0)      ||
             (AsciiStriCmp (*(argv + index), "-w") == 0))
    {
      CmdOp = TRANSFER_DIRECTION;
    }
    else if ((AsciiStriCmp (*(argv + index), "--offset") == 0) ||
             (AsciiStriCmp (*(argv + index), "-o") == 0))
    {
      CmdOp = OFFSET;
    }
    else if ((AsciiStriCmp (*(argv + index), "--loop") == 0) ||
             (AsciiStriCmp (*(argv + index), "-l") == 0))
    {
      CmdOp = LOOP_COUNT;
    }
	else if ((AsciiStriCmp (*(argv + index), "--detect") == 0) ||
             (AsciiStriCmp (*(argv + index), "-d") == 0))
    {
      CmdOp = BUS_DETECT;
    }

    switch (CmdOp)
    {
      case I2C_CORE_NUMBER:
        index++;
        sctxt.i2c_core_number = (UINT32) AsciiStrDecimalToUintn (*(argv + index));
        break;

      case SLAVE_ADDRESS:
        index++;
        sctxt.slave_address = (UINT32) AsciiStrHexToUintn(*(argv + index));
        if (TRUE == IS_INVALID_I2CADDR(sctxt.slave_address))
        {
          DEBUG((EFI_D_ERROR,"Invalid slave addr: %a\n", *(argv + index)));
          return NULL;
        }
        break;

      case BUS_SPEED:
        index++;
        sctxt.bus_speed = (UINT32) AsciiStrDecimalToUintn (*(argv + index));
        break;

      case TRANSFER_DIRECTION:
        if ((AsciiStriCmp (*(argv + index), "--read") == 0) ||
            (AsciiStriCmp (*(argv + index), "-r") == 0))
        {
          sctxt.transfer_direction = TRANSFER_DIRECTION_READ;
        }
        else
        {
          sctxt.transfer_direction = TRANSFER_DIRECTION_WRITE;
        }

        index++;

        if (sctxt.transfer_direction == TRANSFER_DIRECTION_READ)
        {
          sctxt.transfer_length = (UINT16) AsciiStrDecimalToUintn (*(argv + index));
          if (sctxt.transfer_length > DATA_SIZE)
          {
            sctxt.transfer_length = DATA_SIZE;
          }
        }
        else
        {
          i2c_test_hex_stream_to_buf (wbuf, *(argv + index), &sctxt.transfer_length);
        }
        break;

      case OFFSET:
        index++;
        /* The TYPE macro itself defines the length of the offset */
        if (AsciiStrLen (*(argv + index)) == (2 + (SLAVE_TYPE_MEM << 1)))
        {
          sctxt.slave_type = SLAVE_TYPE_MEM;
        }
        else if (AsciiStrLen (*(argv + index)) == (2 + (SLAVE_TYPE_REG << 1)))
        {
          sctxt.slave_type = SLAVE_TYPE_REG;
        }
        else
        {
          sctxt.slave_type = SLAVE_TYPE_DEF;
        }
        sctxt.offset = (UINT16) AsciiStrHexToUintn (*(argv + index));
        break;

      case LOOP_COUNT:
        index++;
        sctxt.loop_count = (UINT16) AsciiStrDecimalToUintn (*(argv + index));
        break;
		
      case BUS_DETECT:
        index++;
        sctxt.bus_scan = (UINT16) AsciiStrDecimalToUintn (*(argv + index));
        break;
		
      case CMD_INVALID:
      default:
        DEBUG((EFI_D_ERROR,"Invalid command line option: %a\n", *(argv + index)));
        break;
    }

    if (CmdOp == CMD_INVALID)
    {
      return NULL;
    }

    index++;
  }

  return &sctxt;
}


void TransStart(uint32 IsWrite,uint8 vSlaveId, uint32  bus_Frequency_KHz, uint32 length)
{
    
	if (IsWrite)
    {
		BusDebug.busTrans[BusDebug.busidx].cmd = WRITE;
		DEBUG((EFI_D_ERROR, "WRITE ->"));
	}
	else
	{
		BusDebug.busTrans[BusDebug.busidx].cmd = READ;
		DEBUG((EFI_D_ERROR, "READ  ->"));
	}
						  
	BusDebug.busTrans[BusDebug.busidx].bus_Frequency_KHz =  bus_Frequency_KHz;
	BusDebug.busTrans[BusDebug.busidx].dataLen = length;
	BusDebug.busTrans[BusDebug.busidx].sid     = vSlaveId;
	  
    DEBUG((EFI_D_ERROR, "Freq(khz) = %d |", (uint32) BusDebug.busTrans[BusDebug.busidx].bus_Frequency_KHz ));
    DEBUG((EFI_D_ERROR, "Data size = %d |",(uint32)length));
						  
    ArmArchTimerReadReg (CntPct, (VOID *)&starting_tick);
	BusDebug.busTrans[BusDebug.busidx].startTime = starting_tick;
}

void TransEnd()
{
	
	ArmArchTimerReadReg (CntPct, (VOID *)&ending_tick);
	/* Convert ticks to micro seconds */
    ArmArchTimerReadReg (CntFrq, (VOID *) &TimetickFreq);
	total_ticks = ending_tick - starting_tick;
	uSeconds = MultU64x32(total_ticks , 1000000);
    uSeconds = DivU64x32(uSeconds, TimetickFreq);
	
	BusDebug.busTrans[BusDebug.busidx].EndTime = ending_tick;
	BusDebug.busTrans[BusDebug.busidx].duration = uSeconds;
	DEBUG((EFI_D_ERROR, "Duration(usec) = %d |", uSeconds));
	tput = length *8*1000;
	tput = tput/uSeconds;
	DEBUG((EFI_D_ERROR, "Speed = %ld kpbs\n ", tput));
    BusDebug.busidx++;
}

i2c_status i2c_bus_scan (uint32 i2c_core_number)
{
  i2c_status istatus = I2C_SUCCESS;
  VOID      *i2c_handle      = NULL;
  UINT32    bytes_written    = 0;
  UINT32    offset_len       = 0;
  i2c_slave_config cfg;

  /* Initialize the I2C Core for eeprom */
  istatus = i2c_open((i2c_instance) (i2c_core_number - 1), &i2c_handle);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize I2C %d\n", istatus));
    goto error;
  }
  
  cfg.bus_frequency_khz = 100;
  cfg.slave_address = 8;
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;
  cfg.core_configuration2 = 0;
  
  offset_len = 0;
  sctxt.offset = 1;
  sctxt.transfer_length = 1;
  wbuf[0] = 1;
  
  for (; cfg.slave_address < 120; cfg.slave_address++)
  { 
      istatus = i2c_write (i2c_handle, &cfg, sctxt.offset, offset_len, wbuf, sctxt.transfer_length, &bytes_written, 2500);
      if (I2C_SUCCESS == istatus)
      {
        DEBUG((EFI_D_ERROR, "Slave device found @ 0x%x Address\n", (uint32) cfg.slave_address));
      }
  }
  
error:
  if (i2c_handle != NULL)
  {
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      DEBUG((EFI_D_ERROR, "Failed to DeInitialize I2C %d\n", istatus));
    }
	
  }
  return I2C_SUCCESS;
  
}
i2c_status i2c_test_slave (UINT32 bus_scan)
{
  i2c_status istatus = I2C_SUCCESS;

  VOID      *i2c_handle      = NULL;
  UINT32    bytes_read       = 0;
  UINT32    bytes_written    = 0;
  UINT32    i                = 0;
  UINT32    offset_len       = 0;

  i2c_slave_config cfg;
  
  if(bus_scan)
  {
	istatus = i2c_bus_scan (sctxt.i2c_core_number);
	return istatus;
  }

  /* Initialize the I2C Core for eeprom */
  istatus = i2c_open((i2c_instance) (sctxt.i2c_core_number - 1), &i2c_handle);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize I2C %d\n", istatus));
    goto error;
  }

  cfg.bus_frequency_khz = sctxt.bus_speed;
  cfg.slave_address = sctxt.slave_address;
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;
  cfg.core_configuration2 = 0;

  if (sctxt.slave_type == SLAVE_TYPE_MEM)
  {
    offset_len = 2;
  }
  else if (sctxt.slave_type == SLAVE_TYPE_REG)
  {
    offset_len = 1;
  }
  else
  {
    offset_len = 0;
  }

  /* Initialize Buffers */
  for (i = 0; i < DATA_SIZE; i++)
  {
    rbuf[i] = 0;
  }

  if (sctxt.loop_count == 0)
  {
    sctxt.loop_count = 1;
  }

  for (i = 0; i < sctxt.loop_count; i++)
  {
    if (sctxt.transfer_direction == TRANSFER_DIRECTION_READ)
    {
      istatus = i2c_read (i2c_handle, &cfg, sctxt.offset, offset_len, rbuf, sctxt.transfer_length, &bytes_read, 2500);
      if (I2C_SUCCESS != istatus)
      {
        DEBUG((EFI_D_ERROR, "Generic Read Failed %d\n", (uint32) istatus));
        goto error;
      }
      else
      {
        i2c_test_hex_dump (rbuf, sctxt.transfer_length);
      }
    }
    else
    {
      istatus = i2c_write (i2c_handle, &cfg, sctxt.offset, offset_len, wbuf, sctxt.transfer_length, &bytes_written, 2500);
      if (I2C_SUCCESS != istatus)
      {
        DEBUG((EFI_D_ERROR, "EEPROM Write Failed %d\n", (uint32) istatus));
        goto error;
      }
      else
      {
        i2c_test_hex_dump (wbuf, sctxt.transfer_length);
      }
    }
  }

error:

  if (i2c_handle != NULL)
  {
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      DEBUG((EFI_D_ERROR, "Failed to DeInitialize I2C %d\n", istatus));
    }
  }

  return istatus;
}

i2c_status i2c_test_kpi_rw (void)
{
  i2c_status istatus = I2C_SUCCESS;

  VOID      *i2c_handle      = NULL;
  UINT32    bytes_read       = 0;
  UINT32    bytes_written    = 0;
  UINT16    offset           = 0;

  i2c_slave_config cfg;
 
  
  /* Initialize the I2C Core for eeprom */
  istatus = i2c_open((i2c_instance) (PcdGet32(EEPROMI2CCore) - 1), &i2c_handle);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize I2C %d\n", istatus));
    goto error;
  }

  cfg.slave_address = PcdGet32(EEPROMSlaveAddress);
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;


  		 for(test_cnt =0; test_cnt < TEST_CNT;test_cnt++)
			{	
				DEBUG((EFI_D_ERROR, "*********************************************** \n"));
				for(freq_idx=0; freq_idx < MAX_FREQ;freq_idx++)
				{	  
				
					  cfg.bus_frequency_khz = bus_feq[freq_idx];
					  
					  
					  for(data_idx=0; data_idx < MAX_DATA_SIZE ; data_idx++)
					  {  
                          length = bus_data_size[data_idx];
						
						  
						  TransStart(0,cfg.slave_address ,cfg.bus_frequency_khz,length);
						  
						  
						  istatus = i2c_read (i2c_handle, &cfg, offset, 2, bbuf, length, &bytes_read, 2500);
						  if (I2C_SUCCESS != istatus)
						  {
							DEBUG((EFI_D_ERROR, "EEPROM Backup Failed %d\n", (uint32) istatus));
							goto error;
						  }
					      
						 gBS->Stall (PcdGet32(EEPROMWriteCycleTime));
						   
						 TransStart(1,cfg.slave_address ,cfg.bus_frequency_khz,length);
				
						  istatus = i2c_write (i2c_handle, &cfg, offset, 2, wbuf, DATA_SIZE, &bytes_written, 2500);
						  if (I2C_SUCCESS != istatus)
						  {
							DEBUG((EFI_D_ERROR, "EEPROM Write Failed %d\n", (uint32) istatus));
							goto error;
						  }
						  
						  TransEnd();
					      
						  gBS->Stall (PcdGet32(EEPROMWriteCycleTime));
					}   
					
					DEBUG((EFI_D_ERROR, "***********************************************\n"));
				}
			}		

error:

  if (i2c_handle != NULL)
  {
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      DEBUG((EFI_D_ERROR, "Failed to DeInitialize I2C %d\n", istatus));
    }
  }

  return istatus;
}

i2c_status i2c_test_kpi_read (void)
{
  i2c_status istatus = I2C_SUCCESS;

  VOID      *i2c_handle      = NULL;
  UINT32    bytes_read       = 0;
  UINT16    offset           = 0;

  i2c_slave_config cfg;
 
  
  /* Initialize the I2C Core for eeprom */
  istatus = i2c_open((i2c_instance) (PcdGet32(EEPROMI2CCore) - 1), &i2c_handle);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize I2C %d\n", istatus));
    goto error;
  }

  cfg.slave_address = PcdGet32(EEPROMSlaveAddress);
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;


  		 for(test_cnt =0; test_cnt < TEST_CNT;test_cnt++)
			{	
				DEBUG((EFI_D_ERROR, "*********************************************** \n"));
				for(freq_idx=0; freq_idx < MAX_FREQ;freq_idx++)
				{	  
				
					  cfg.bus_frequency_khz = bus_feq[freq_idx];
					  
					  
					  for(data_idx=0; data_idx < MAX_DATA_SIZE ; data_idx++)
					  {  
                          length = bus_data_size[data_idx];
						
						  
						  TransStart(0,cfg.slave_address ,cfg.bus_frequency_khz,length);
						  
						  
						  istatus = i2c_read (i2c_handle, &cfg, offset, 2, bbuf, length, &bytes_read, 2500);
						  if (I2C_SUCCESS != istatus)
						  {
							DEBUG((EFI_D_ERROR, "EEPROM Backup Failed %d\n", (uint32) istatus));
							goto error;
						  }
						  
						  TransEnd();
					}   
					
					DEBUG((EFI_D_ERROR, "***********************************************\n"));
					
				}
			}		

error:

  if (i2c_handle != NULL)
  {
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      DEBUG((EFI_D_ERROR, "Failed to DeInitialize I2C %d\n", istatus));
    }
  }

  return istatus;
}

i2c_status i2c_test_kpi_write (void)
{
  i2c_status istatus = I2C_SUCCESS;

  VOID      *i2c_handle      = NULL;
  UINT32    bytes_written    = 0;
  UINT16    offset           = 0;

  i2c_slave_config cfg;
 
  
  /* Initialize the I2C Core for eeprom */
  istatus = i2c_open((i2c_instance) (PcdGet32(EEPROMI2CCore) - 1), &i2c_handle);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize I2C %d\n", istatus));
    goto error;
  }
  
  cfg.slave_address = PcdGet32(EEPROMSlaveAddress);
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;


  		 for(test_cnt =0; test_cnt < TEST_CNT;test_cnt++)
			{	
				DEBUG((EFI_D_ERROR, "*********************************************** \n"));
				for(freq_idx=0; freq_idx < MAX_FREQ;freq_idx++)
				{	  
				
					  cfg.bus_frequency_khz = bus_feq[freq_idx];
					  
					  
					  for(data_idx=0; data_idx < MAX_DATA_SIZE ; data_idx++)
					  {  
                          length = bus_data_size[data_idx];
						
						 TransStart(1,cfg.slave_address ,cfg.bus_frequency_khz,length);
				
						  istatus = i2c_write (i2c_handle, &cfg, offset, 2, wbuf, DATA_SIZE, &bytes_written, 2500);
						  if (I2C_SUCCESS != istatus)
						  {
							DEBUG((EFI_D_ERROR, "EEPROM Write Failed %d\n", (uint32) istatus));
							goto error;
						  }
						  
						  TransEnd(); 
					}   
					DEBUG((EFI_D_ERROR, "***********************************************\n"));
				}
			}		

error:

  if (i2c_handle != NULL)
  {
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      DEBUG((EFI_D_ERROR, "Failed to DeInitialize I2C %d\n", istatus));
    }
  }

  return istatus;
}


i2c_status i2c_test_generic(UINTN argc, CHAR8 **argv)
{
  i2c_status istatus = I2C_SUCCESS;

  if ((AsciiStriCmp (*argv, "--help") == 0) ||
      (AsciiStriCmp (*argv, "-h")     == 0))
  {
    i2c_test_cmd_help ();
  }
 else if ((AsciiStriCmp (*argv, "-krw") == 0))
  {
    i2c_test_kpi_rw ();
  }
  else if ((AsciiStriCmp (*argv, "-kr") == 0))
  {
    i2c_test_kpi_read ();
  }
  else if ((AsciiStriCmp (*argv, "-kw") == 0))
  {
    i2c_test_kpi_write ();
  }
  else if (NULL != get_slave_context (argc, argv))
  { 
	
    istatus = i2c_test_slave (sctxt.bus_scan);
  }
  else
  {
    DEBUG((EFI_D_ERROR, "Invalid Command Line options. Available Options are:\n"));
    i2c_test_cmd_help ();
  }

  return istatus;
}

VOID i2c_test_hex_dump (IN UINT8 *buffer, IN UINT32 length)
{
  UINT32 i = 0;

  DEBUG((EFI_D_ERROR, "\n"));
  for (i = 0; i < length; i++)
  {
    if (!(i & 7))
    {
      DEBUG((EFI_D_ERROR, "\n"));
    }
    DEBUG((EFI_D_ERROR, "%02x ", buffer[i]));
  }
  DEBUG((EFI_D_ERROR, "\n\n"));
}

VOID i2c_test_hex_stream_to_buf (UINT8 *buffer, const CHAR8 *stream, UINT16 *length)
{
  UINT16 i = 0;
  CHAR8 hex_chars[5];

  *length = AsciiStrLen (stream) >> 1;
  if (*length > DATA_SIZE)
  {
    *length = DATA_SIZE;
  }
  hex_chars[0] = '0';
  hex_chars[1] = 'x';
  hex_chars[4] = '\0';

  for (i = 0; i < *length; i++)
  {
    hex_chars[2] = *(stream + (i << 1));
    hex_chars[3] = *(stream + (i << 1) + 1);
    buffer[i] = (UINT8) AsciiStrHexToUintn(hex_chars);
  }
}
