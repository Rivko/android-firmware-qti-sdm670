/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#include <sselog.h>
#include <drTsController.h>
#include <i2c_stub.h>
#include <stringl.h>
#include "synaptics_ts.h"
#include "synaptics_ts_i2c.h"

/* 2.3.5. Page Description table */
#define PDT_START (0x00E9)
#define PDT_END (0x00D0)
#define PDT_ENTRY_SIZE (0x0006)
#define PAGES_TO_SERVICE (10)
#define PAGE_SELECT_LEN (2)

#define F01_QUERY_LEN 21 /* the only one we actually read */
#define END_OF_TABLE (0x00)

/* The only 2 pages we care about */
#define F01 (0x01)
#define F12 (0x12)

#define MAX_NUMBER_OF_FINGERS 10
#define MAX_INTR_REQUESTS 8 /* 4.3 Table 11 */

#define MASK_8BIT 0xFF
#define MASK_7BIT 0x7F
#define MASK_6BIT 0x3F
#define MASK_5BIT 0x1F
#define MASK_4BIT 0x0F
#define MASK_3BIT 0x07
#define MASK_2BIT 0x03

/* 10.2.29. F12_2D_Ctrl28/: object data to report */
#define REPORT_TYPE (1 << 0)
#define REPORT_X_LSB (1 << 1)
#define REPORT_X_MSB (1 << 2)
#define REPORT_Y_LSB (1 << 3)
#define REPORT_Y_MSB (1 << 4)
#define REPORT_Z (1 << 5)
#define REPORT_WX (1 << 6)
#define REPORT_WY (1 << 7)
#define REPORT_ALL (0xFF)

/*
 * Function descriptor registers, 2.3.5.2. Table 4
 */
struct functionDescriptor {
  uint8_t queryBase;
  uint8_t commandBase;
  uint8_t controlBase;
  uint8_t dataBase;
  uint8_t interruptSourceCount; /* only bytes 0-2 */
  uint8_t functionNumber;
};

/*
 * device data we read and store from firmware
 */
struct device_data {
  unsigned short numberOfInterruptRegisters; /* Collected */
};

/* 10.1.6 F12_2D_Query5/: control register presence */
typedef union {
  struct {
    uint8_t sizeOfQuery6; /* spec is not clear, it says this can be 1 OR 3 bytes. 1 works...*/
    struct {
      uint8_t ctrl0:1;
      uint8_t ctrl1:1;
      uint8_t ctrl2:1;
      uint8_t ctrl3:1;
      uint8_t ctrl4:1;
      uint8_t ctrl5:1;
      uint8_t ctrl6:1;
      uint8_t ctrl7:1;
    } byte1;
    struct {
      uint8_t ctrl8:1;
      uint8_t ctrl9:1;
      uint8_t ctrl10:1;
      uint8_t ctrl11:1;
      uint8_t ctrl12:1;
      uint8_t ctrl13:1;
      uint8_t ctrl14:1;
      uint8_t ctrl15:1;
    } byte2;
    struct {
      uint8_t ctrl16:1;
      uint8_t ctrl17:1;
      uint8_t ctrl18:1;
      uint8_t ctrl19:1;
      uint8_t ctrl20:1;
      uint8_t ctrl21:1;
      uint8_t ctrl22:1;
      uint8_t ctrl23:1;
    } byte3;
    struct {
      uint8_t ctrl24:1;
      uint8_t ctrl25:1;
      uint8_t ctrl26:1;
      uint8_t ctrl27:1;
      uint8_t ctrl28:1;
      uint8_t ctrl29:1;
      uint8_t ctrl30:1;
      uint8_t ctrl31:1;
    } byte4;
  } controlRegisterPresense;
  uint8_t data[5];
} F12_2D_Query5;

/* 10.1.9. F12_2D_Query8/: data register presence */
typedef union {
  struct {
    uint8_t sizeOfQuery9; /* spec is not clear, it says this can be 1 OR 3 bytes. 1 works...*/
    struct {
      uint8_t data0:1;
      uint8_t data1:1;
      uint8_t data2:1;
      uint8_t data3:1;
      uint8_t data4:1;
      uint8_t data5:1;
      uint8_t data6:1;
      uint8_t data7:1;
    } byte1;
    struct {
      uint8_t data8:1;
      uint8_t data9:1;
      uint8_t data10:1;
      uint8_t data11:1;
      uint8_t data12:1;
      uint8_t data13:1;
      uint8_t data14:1;
      uint8_t data15:1;
    } byte2;
  } dataRegisterPresense;
  uint8_t data[3];
} F12_2D_Query8;

/* 10.2.9. F12_2D_Ctrl8/: sensor tuning */
typedef union {
  struct {
    uint8_t MaximumXCoordLSB;
    uint8_t MaximumXCoordMSB;
    uint8_t MaximumYCoordLSB;
    uint8_t MaximumYCoordMSB;
    uint8_t unused[10]; /* we don't care */
  } sensorTuning;
  uint8_t data[14];
} F12_2D_Ctrl8;

/* 10.2.24. F12_2D_Ctrl23/: object report enable */
typedef union {
  struct {
    uint8_t ObjectTypeEnable; /* we don't care */
    uint8_t MaxNumberOfReportedObjects;
  } objectReportTable;
  uint8_t data[2];
} F12_2D_Ctrl23;

/* F12_2D_Data1_sensed_object/: sensed objects */
struct F12_2D_Data1_sensed_object {
  uint8_t ObjectTypeAndStatus;
  uint8_t XLSB;
  uint8_t XMSB;
  uint8_t YLSB;
  uint8_t YMSB;
  uint8_t Z;
  uint8_t Wx; /* We read it, but we don't currently use it */
  uint8_t Wy; /* We read it, but we don't currently use it */
};

/* Function Description for F12, i.e. what we care about F12 */
struct F12_desc {
  /* addresses of F12 registers */
  uint8_t queryBase;
  uint8_t controlBase;
  uint8_t dataBase;
  /* offsets for the data we read in F12, i.e. fingers */
  uint8_t data1_offset; /* here fingers start */
  /* Sensor info, info we have but don't really care about now */
  int sensorMaxX;
  int sensorMaxY;
  /* Supported number of fingers */
  uint8_t numberOfFingers;
  uint8_t interruptRegisterNumber;
  uint8_t interruptMask;
  unsigned int fingerDataSize;
};

struct synFingers{
  uint32_t code[MAX_NUMBER_OF_FINGERS];
  uint32_t x[MAX_NUMBER_OF_FINGERS];
  uint32_t y[MAX_NUMBER_OF_FINGERS];
};

// globals
static struct functionDescriptor     g_fdF01;
static struct F12_desc   g_fdF12;
static struct device_data g_info;
static uint32_t opened = 0;

static struct synFingers g_fingersLast = {0};


static struct i2cFunc g_i2cFunc = {
  synaptics_i2cReadReg,
  synaptics_i2cWriteReg,
  synaptics_i2cOpen,
  synaptics_i2cClose
};

// local functions
static int32_t synReadRegister(uint16_t address, uint8_t * buffer, uint16_t noOfBytes){
  int32_t rv = 0;
  uint8_t page;
  uint8_t offset;
  LOG_D("Reading register 0x%04X, %u bytes", address, noOfBytes);

  page    = ((address >> 8) & MASK_8BIT);
  offset  = (address & MASK_8BIT);
  // select the page
  rv = g_i2cFunc.i2cWriteReg(0xFF, &page, 1);
  if (rv) {
    LOG_E("I2C Page Section failed : 0x%08X", rv);
    return rv;
  }

  // read the register
  rv = g_i2cFunc.i2cReadReg( offset,
                   buffer,
                   noOfBytes);
  return rv;
}

static int32_t synWriteRegister(uint16_t address, uint8_t * buffer, uint16_t noOfBytes){
  int32_t rv = 0;
  uint8_t page;
  uint8_t offset;
  LOG_D("Writing register 0x%04X, %u bytes", address, noOfBytes);
  dumpHex("Writing:", buffer, noOfBytes);

  page    = ((address >> 8) & MASK_8BIT);
  offset  = (address & MASK_8BIT);
  // select the page
  rv = g_i2cFunc.i2cWriteReg(0xFF, &page, 1);
  if (rv) {
    LOG_E("I2C Page Section failed : 0x%08X", rv);
    return rv;
  }

  // write the register
  rv = g_i2cFunc.i2cWriteReg( offset,
                   buffer,
                   noOfBytes);
  return rv;
}

uint32_t synReadF12(struct synFingers *fingers)
{
  int retval;
  uint8_t touchCount = 0; /* number of touch points */
  uint8_t finger;
  uint8_t fingerStatus;
  uint8_t sizeOfFinger;
  int x;
  int y;

  struct F12_2D_Data1_sensed_object data[MAX_NUMBER_OF_FINGERS];
  struct F12_2D_Data1_sensed_object *fingerData;

  ENTER;

  if (!fingers) {
    LOG_E("NULL argument");
    EXITV(0);
  }

  sizeOfFinger = sizeof(struct F12_2D_Data1_sensed_object);

  retval = synReadRegister( g_fdF12.dataBase
                         + g_fdF12.data1_offset, /* fingers start here */
                         (uint8_t*)data,
                         sizeOfFinger*g_fdF12.numberOfFingers);
  if (retval) {
    LOG_E("I2C Read Error while fetching f12 finger status registers from the chip: 0x%08X", retval);
    EXITV(0);
  }

  /* Here we simply read all fingers, we don't check objectAttention (data15) */

  for ( finger = 0;
        (finger < g_fdF12.numberOfFingers) && (finger < MAX_NUMBER_OF_FINGERS);
        finger++) {

    fingerData = data + finger;
    fingerStatus = fingerData->ObjectTypeAndStatus & MASK_2BIT;

    /*
     * Each 2-bit finger status field represents the following:
     * 00 = finger not present
     * 01 = finger present and data accurate
     * 10 = finger present but data may be inaccurate
     * 11 = reserved
     */
    x  = (fingerData->XMSB << 8) | (fingerData->XLSB);
    y  = (fingerData->YMSB << 8) | (fingerData->YLSB);

    LOG_D("<-----(finger No = %d, status =%02x,x=%#x, y=%#x)--------->",finger,fingerStatus,x,y);
    if (fingerStatus) {
      fingers->x[finger] = x;
      fingers->y[finger] = y;
      fingers->code[finger] = TOUCH_EVENT_DOWN;
      touchCount++;
    } else if (g_fingersLast.code[finger] == TOUCH_EVENT_DOWN) {
      // finger was released
      fingers->x[finger] = g_fingersLast.x[finger];
      fingers->y[finger] = g_fingersLast.y[finger];
      fingers->code[finger] = TOUCH_EVENT_UP;
      touchCount++;
      LOG_D("<--- Finger %d released --->", finger);
    }
  }
  g_fingersLast = *fingers;

  EXITV(touchCount);
}

static uint32_t synReadFingers(struct synFingers *fingers)
{
  int32_t rv = 0;
  uint32_t touchCount = 0; /* number of touch points */
  uint8_t intReg[MAX_INTR_REQUESTS] = {0};

  if (!fingers) {
    LOG_E("NULL argument");
    return 0;
  }

  // determine the source of the interrupt
  rv = synReadRegister( g_fdF01.dataBase + 1,
                         intReg,
                         g_info.numberOfInterruptRegisters);
  if (rv) {
    LOG_E("I2C Read Error while fetching interrupt status registers from the chip: 0x%08X", rv);
    return 0;
  }

  //LOG_D("<-------------numberOfInterruptRegisters = %#x--------------->",g_info.numberOfInterruptRegisters);
  //LOG_D("<-------------intReg[%#x,%#x,%#x,%#x]-------------->",intReg[0],intReg[1],intReg[2],intReg[3]);

  // check source
  dumpHex("intReg", intReg, sizeof(intReg));
  if (intReg[g_fdF12.interruptRegisterNumber] & g_fdF12.interruptMask) {
    // F12
    touchCount = synReadF12(fingers);
  } else {
    // others - error condition
    LOG_D("No interrupt to process");
  }
  LOG_D("touchCount: %d  ",touchCount);
  return touchCount;
}

/** Open the I2C bus and initializes structures
 */
uint32_t synaptics_drTsOpen(const uint32_t width, const uint32_t height)
{
  int32_t rv = 0;
  uint8_t endOfTable = 0;
  uint8_t page_number;
  uint8_t entry_addr;
  uint8_t interruptCount = 0;
  unsigned short pdt_entry_addr;

  uint8_t f01_query   [F01_QUERY_LEN];

  struct functionDescriptor fd;

  ENTER;

  do {
    rv = g_i2cFunc.i2cOpen();
    if (rv != 0) {
      LOG_E("Error opening the I2C bus: 0x%08X", rv);
      break;
    }
    opened = 1;
    //now scan and collect all the Page descriptors
    for (page_number = 0; page_number < PAGES_TO_SERVICE; page_number++) {
      for (entry_addr = PDT_START; entry_addr > PDT_END; entry_addr -= PDT_ENTRY_SIZE) {

        pdt_entry_addr = ((unsigned short) entry_addr | (unsigned short)(page_number << 8));

        rv = synReadRegister( pdt_entry_addr,
                              (uint8_t *)&fd,
                               sizeof(fd));
        if (rv) {
          LOG_E("I2C Read Error while fetching PDT from the chip: 0x%08X", rv);
          break;
        }

        fd.interruptSourceCount &= 0x7;

        LOG_D("-------------PDT-----------------");
        LOG_D("queryBase            - %#x", fd.queryBase);
        LOG_D("controlBase          - %#x", fd.controlBase);
        LOG_D("dataBase             - %#x", fd.dataBase);
        LOG_D("interruptSourceCount - %#x", fd.interruptSourceCount);
        LOG_D("functionNumber       - %#x", fd.functionNumber);
        LOG_D("Page Number          - %#x", page_number);
        LOG_D("pdt_entry_addr       - %#x", pdt_entry_addr);

        switch (fd.functionNumber) {
          case F01: LOG_D("---------F01---------");
            g_fdF01.queryBase  = fd.queryBase;
            g_fdF01.controlBase   = fd.controlBase;
            g_fdF01.dataBase   = fd.dataBase;

            // collect all information regarding the chip
            rv = synReadRegister( g_fdF01.queryBase,
                                   f01_query,
                                   sizeof(f01_query));
            if (rv) {
              LOG_E("I2C Read Error while fetching f01 query register from the chip: 0x%08X", rv);
              break;
            }

            LOG_D("-Product ID        : %s ",  &f01_query[11]);
            LOG_D("-Serial            : %#x ", ((f01_query[9] & MASK_7BIT) << 8) |(f01_query[10] & MASK_7BIT));
            LOG_D("-Tester ID         : %#x ", ((f01_query[7] & MASK_7BIT) << 8) |(f01_query[8] & MASK_7BIT));
            LOG_D("-Info              : %#x %#x ", f01_query[3] & MASK_7BIT, f01_query[2] & MASK_7BIT);
            LOG_D("-Manufacturer ID   : %#x ", f01_query[0]);
            break;

          case F12: LOG_D("---------F12---------");
            {
              uint8_t sizeOfFinger;
              uint8_t sizeOfQuery8;
              uint8_t ctrl8Offset;
              uint8_t ctrl23Offset;
              uint8_t ctrl28Offset;
              uint8_t numberOfFingers;

              F12_2D_Query5     query5;
              F12_2D_Query8     query8;
              F12_2D_Ctrl8      ctrl8;
              F12_2D_Ctrl23     ctrl23;

              // collect all information regarding the sensors
              g_fdF12.queryBase  = fd.queryBase;
              g_fdF12.controlBase   = fd.controlBase;
              g_fdF12.dataBase   = fd.dataBase;

              sizeOfFinger = sizeof(struct F12_2D_Data1_sensed_object);

              rv = synReadRegister( g_fdF12.queryBase + 5,
                                     query5.data,
                                     sizeof(query5.data));
              if (rv) {
                LOG_E("Error in synReadRegister (query5): 0x%08X", rv);
                break;
              }

              ctrl8Offset = query5.controlRegisterPresense.byte1.ctrl0 +
                              query5.controlRegisterPresense.byte1.ctrl1 +
                              query5.controlRegisterPresense.byte1.ctrl2 +
                              query5.controlRegisterPresense.byte1.ctrl3 +
                              query5.controlRegisterPresense.byte1.ctrl4 +
                              query5.controlRegisterPresense.byte1.ctrl5 +
                              query5.controlRegisterPresense.byte1.ctrl6 +
                              query5.controlRegisterPresense.byte1.ctrl7;

              ctrl23Offset = ctrl8Offset +
                              query5.controlRegisterPresense.byte2.ctrl8  +
                              query5.controlRegisterPresense.byte2.ctrl9  +
                              query5.controlRegisterPresense.byte2.ctrl10 +
                              query5.controlRegisterPresense.byte2.ctrl11 +
                              query5.controlRegisterPresense.byte2.ctrl12 +
                              query5.controlRegisterPresense.byte2.ctrl13 +
                              query5.controlRegisterPresense.byte2.ctrl14 +
                              query5.controlRegisterPresense.byte2.ctrl15 +
                              query5.controlRegisterPresense.byte3.ctrl16 +
                              query5.controlRegisterPresense.byte3.ctrl17 +
                              query5.controlRegisterPresense.byte3.ctrl18 +
                              query5.controlRegisterPresense.byte3.ctrl19 +
                              query5.controlRegisterPresense.byte3.ctrl20 +
                              query5.controlRegisterPresense.byte3.ctrl21 +
                              query5.controlRegisterPresense.byte3.ctrl22;

              ctrl28Offset = ctrl23Offset +
                              query5.controlRegisterPresense.byte3.ctrl23 +
                              query5.controlRegisterPresense.byte4.ctrl24 +
                              query5.controlRegisterPresense.byte4.ctrl25 +
                              query5.controlRegisterPresense.byte4.ctrl26 +
                              query5.controlRegisterPresense.byte4.ctrl27;

              rv = synReadRegister( g_fdF12.controlBase + ctrl23Offset,
                                     ctrl23.data,
                                     sizeof(ctrl23.data));
              if (rv) {
                LOG_E("Error in synReadRegister (ctrl23): 0x%08X", rv);
                break;
              }

              // maximum no of fingers supported
              numberOfFingers = min(ctrl23.objectReportTable.MaxNumberOfReportedObjects,
                                   (uint8_t)MAX_NUMBER_OF_FINGERS);
              g_fdF12.numberOfFingers = numberOfFingers;
              LOG_D("max no of fingures supported: 0x%08X", numberOfFingers);

              rv = synReadRegister( g_fdF12.queryBase + 7,
                                     &sizeOfQuery8,
                                     sizeof(sizeOfQuery8));
              if (rv) {
                LOG_E("Error in synReadRegister (query7): 0x%08X", rv);
                break;
              }

              rv = synReadRegister( g_fdF12.queryBase + 8,
                                     query8.data,
                                     sizeOfQuery8);
              if (rv) {
                LOG_E("Error in synReadRegister (query8): 0x%08X", rv);
                break;
              }

              /* Determine the presence of the Data0 register */
              g_fdF12.data1_offset = query8.dataRegisterPresense.byte1.data0;

              uint8_t report_enable;
              report_enable = REPORT_ALL;
              rv = synWriteRegister(g_fdF12.controlBase
                                     + ctrl28Offset,
                                     &report_enable,
                                     sizeof(report_enable)
                                     );
              if (rv) {
                LOG_E("Error in synWriteRegister (ctrl28): 0x%08X", rv);
                break;
              }
              rv = synReadRegister( g_fdF12.controlBase
                                     + ctrl8Offset,
                                     ctrl8.data,
                                     sizeof(ctrl8.data));
              if (rv) {
                LOG_E("Error in synReadRegister (ctrl18): 0x%08X", rv);
                break;
              }

              /* Maximum x and y */
              g_fdF12.sensorMaxX = ((unsigned short)ctrl8.sensorTuning.MaximumXCoordLSB << 0) |
                                   ((unsigned short)ctrl8.sensorTuning.MaximumXCoordMSB << 8);
              g_fdF12.sensorMaxY = ((unsigned short)ctrl8.sensorTuning.MaximumYCoordLSB << 0) |
                                   ((unsigned short)ctrl8.sensorTuning.MaximumYCoordMSB << 8);

              g_fdF12.interruptRegisterNumber = (interruptCount + 7) / 8;
              if (g_fdF12.interruptRegisterNumber != 0)
                g_fdF12.interruptRegisterNumber -= 1;

              /* Set an enable bit for each data source */
              uint8_t ii = 0;
              uint8_t intr_offset = 0;
              intr_offset = interruptCount % 8;
              g_fdF12.interruptMask = 0;
              for ( ii = intr_offset;
                    ii < ((fd.interruptSourceCount & MASK_3BIT) + intr_offset);
                    ii++)
                g_fdF12.interruptMask |= 1 << ii;

              /* Allocate memory for finger data storage space */
              g_fdF12.fingerDataSize = numberOfFingers * sizeOfFinger;

              LOG_D("-sensorMaxX              : %u  ", g_fdF12.sensorMaxX);
              LOG_D("-sensorMaxY              : %u  ", g_fdF12.sensorMaxY);
              LOG_D("-numberOfFingers         : %u  ", g_fdF12.numberOfFingers);
              LOG_D("-interruptRegisterNumber : %u  ", g_fdF12.interruptRegisterNumber);
              LOG_D("-interruptMask           : %x  ", g_fdF12.interruptMask);
              LOG_D("-fingerDataSize          : %u  ", g_fdF12.fingerDataSize);
            }
            break;
          case END_OF_TABLE:
            LOG_D("---------END OF TABLE---------");
            endOfTable = 1;
            break;
          default:
            LOG_D("---------F# %x ignored ---------", fd.functionNumber);
            break;
        } //end of switch
        // Accumulate the interrupt count
        if (endOfTable) {
          break;
        } else {
          interruptCount += (fd.interruptSourceCount & MASK_3BIT);
        }
      }
      if (rv || endOfTable) {
        break;
      }
    }
    if (rv || endOfTable) {
      break;
    }
  } while (0);

  //update total number of interrupt source
  g_info.numberOfInterruptRegisters = interruptCount;
  LOG_D("-numberOfInterruptRegisters            : %d  ", g_info.numberOfInterruptRegisters);
  // cleanup if needed
  if (opened && (rv != 0)) {
    g_i2cFunc.i2cClose();
    opened = 0;
  }
  EXITV((uint32_t)rv);
}

/** Release the I2C bus
 */
uint32_t synaptics_drTsClose(void)
{
  int32_t rv = 0;
  ENTER;
  do {
    if (!opened) {
      LOG_E("Device not opened.");
      rv = -1;
      break;
    }
    rv = g_i2cFunc.i2cClose();
    if (rv != 0) {
      LOG_E("Error closing the I2C bus: 0x%08X", rv);
      break;
    }
  } while (0);
  EXITV((uint32_t)rv);
}

/** Process all available touch events
 */
uint32_t synaptics_drTsProcessEvents(void)
{
  int32_t rv = 0;
  int32_t touchCount = 0;
  struct synFingers fingers = {0};

  ENTER;
  do {
    if (!opened) {
      LOG_E("Device not opened.");
      rv = -1;
      break;
    }
    do {
      touchCount = synReadFingers(&fingers);
      if (touchCount == 0) {
        LOG_D("No touch data avaliable");
        //rv = -1;
        break;
      } else {
        for (size_t i=0; i<MAX_NUMBER_OF_FINGERS; i++) {
          if (fingers.code[i] != TOUCH_EVENT_NONE) {
            drNotifyEvent(fingers.x[i], fingers.y[i], fingers.code[i], i);
          }
        }
      }
    } while (0);
    if (rv) {
      LOG_E("Failed to read data from the TS controller: 0x%08X");
      break;
    }
  } while (0);
  EXITV((uint32_t)rv);
}
