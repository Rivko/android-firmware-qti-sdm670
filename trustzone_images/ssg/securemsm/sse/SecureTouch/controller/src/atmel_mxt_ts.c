/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "sselog.h"
#include "drTsController.h"
#include "i2c_stub.h"
#include "atmel_mxt_ts.h"
#include "atmel_mxt_ts_i2c.h"
#include "SecureTouchError.h"

/* Registers in the Memory Map, Figure 2-1*/
#define REG_FAMILY_ID      0x00
#define REG_VARIANT_ID    0x01
#define REG_VERSION        0x02
#define REG_BUILD          0x03
#define REG_MATRIX_X_SIZE  0x04
#define REG_MATRIX_Y_SIZE  0x05
#define REG_OBJECT_NUM    0x06
#define REG_OBJECT_START  0x07 /* After this objects start, 6 bytes each */

#define MXT_OBJECT_SIZE 6

/* Object types, only the ones we're interested in */
#define T5_GEN_MESSAGEPROCESSOR      5
#define T9_TOUCH_MULTITOUCHSCREEN    9
#define T15_TOUCH_KEYARRAY          15
#define T42_PROCI_TOUCHSUPPRESSION  42
#define T61_SPT_TIMER               61
#define T100_TOUCH_MULTITOUCHSCREEN 100

/* Config parameters to read from T100 */
#define T100_XRANGELSB      13
#define T100_XRANGEMSB      14
#define T100_YRANGELSB      24
#define T100_YRANGEMSB      25
#define T100_CFG1           1
#define T100_CFG1_SWITCHXY  (1 << 5)
#define T100_CFG1_INVERTY   (1 << 6)
#define T100_CFG1_INVERTX   (1 << 7)
#define T100_SIZE           54

/* Largest configuration object we read, now T9 */
#define MXT_CONFIG_SIZE_MAX   50
/* T100 could support more, we don't */
#define MAX_NUMBER_OF_FINGERS 10

/** Maintain the cunfiguration for the current screen layout
 */
struct panel_config {
  uint32_t width; /**< Width of the touch panel area */
  uint32_t height; /**< Height of the touch panel area */
};

/** Represents the MXT elements as returned in the object table
 */
struct object_table_element {
  uint8_t   type; /**< Object type, i.e. T5, T6, etc.*/
  uint16_t  start_address; /**< Start address when reading its messages */
  uint8_t   size; /**< Size - 1*/
  uint8_t   instances; /**< Instances - 1*/
  uint8_t   num_report_ids; /**< Number of reportIDs per instance */

  uint8_t   reportid_min; /**< this will be the minimum index for the report id */
  uint8_t   reportid_max; /**< this will be the maximum index for the report id */
};

struct config_object {
  struct object_table_element *element;
  uint32_t len;
  uint8_t *value;
};

#pragma pack(push, mxt, 1)
/** Message from the controller.
 * The message member is supposed to be variable in size, but the largest we're
 * going to read comes from T9, and it's 7 bytes.
 * This structure goes on the bus, so it needs to be packed.
 */
struct message {
  uint8_t reportid; /**< where it comes from */
  uint8_t message[9]; /**< the data, T100 is the largest with 9 bytes */
  uint8_t checksum; /**< we'll never check it...*/
};
#pragma pack(pop, mxt)

/** Placeholder where we keep track of the elements we care of.
 * They are just a small subset of the whole object table, but here we assume
 * the driver has already been configured, so we don't need any more.
 * Actually, T5 and T9 would be enough...
 */
struct mxt_data {
  // we don't need a full object table, just the objects we use
  struct object_table_element t5; /**< We read it */
  struct object_table_element t9; /**< We read it */
  //struct object_table_element t15; /**< Might read it in the future */
  //struct object_table_element t42; /**< Will read it in the future */
  struct object_table_element t61; /**< We read it */
  struct object_table_element t100; /**< We read it */
  /* T100 */
  uint32_t t100_max_x;
  uint32_t t100_max_y;
  uint32_t t100_switch_x_y;
  uint32_t t100_invert_x;
  uint32_t t100_invert_y;
};

/** T9 message, with data on the touch event for a single finger.
 */
struct T9_Message {
  struct {
    uint8_t UNGRIP : 1;
    uint8_t SUPPRESS : 1;
    uint8_t AMP : 1;
    uint8_t VECTOR : 1;
    uint8_t MOVE : 1;
    uint8_t RELEASE : 1;
    uint8_t PRESS : 1;
    uint8_t DETECT : 1;
  } STATUS;
  uint8_t XPOSMSB;
  uint8_t YPOSMSB;
  struct {
    uint8_t Y : 4;
    uint8_t X : 4;
  } XYPOSLSB;
  uint8_t TCHAREA;
  uint8_t TCHAMPLITUDE;
  struct {
    uint8_t Component2 : 4;
    uint8_t Component1 : 4;
  } TCHVECTOR;
};

/** T100 message
 */
struct T100_Message {
  struct {
    uint8_t EVENT : 4;
    uint8_t TYPE : 3;
    uint8_t DETECT : 1;
  } STATUS;
  uint8_t XPOSLSB;
  uint8_t XPOSMSB;
  uint8_t YPOSLSB;
  uint8_t YPOSMSB;
  /* AUXDATA */
  uint8_t VECT;
  uint8_t AMPL;
  uint8_t AREA;
  uint8_t PEAK;
};

struct mxt_finger {
  uint32_t code;
  uint32_t x;
  uint32_t y;
  uint32_t changed;
};

static struct mxt_data g_mxt_data = {0};

/* Configuration objects we verify */
static uint8_t T61_CONFIG[]={0,0,0,0,0};

static uint8_t T9_CONFIG_LIQUID[]={
            0xA4, 0x04, 0x11, 0xAB, 0x29, 0x48, 0x26, 0x25, 0xEE, 0x00,
            0x81, 0x00, 0x00, 0x2C, 0x70, 0x01, 0x00, 0x00, 0x00, 0x05,
            0x71, 0x01, 0x0A, 0x00, 0x00, 0x06, 0x7C, 0x01, 0x06, 0x00,
            0x01, 0x44, 0x83, 0x01, 0x48, 0x00, 0x01, 0x26, 0xCC, 0x01,
            0x3F, 0x00, 0x00, 0x47, 0x0C, 0x02, 0xC7};

static struct config_object config_table_liquid[] = {
  {&g_mxt_data.t9,  sizeof(T9_CONFIG_LIQUID), T9_CONFIG_LIQUID},
  {&g_mxt_data.t61, sizeof(T61_CONFIG), T61_CONFIG},
};

/** Panel configuration, with default values for MSM8974 Fluid
 */
static struct panel_config g_panel_config = {0, 0};

static uint32_t opened = 0;

static struct i2cFunc g_i2cFunc = {
  atmel_i2cReadReg,
  NULL,
  atmel_i2cOpen,
  atmel_i2cClose
};

static struct mxt_finger last_fingers[MAX_NUMBER_OF_FINGERS] = {0};

/** Wrapper to read a single MXT message
 */
static int32_t mxtReadMessage(struct message *mex)
{
  return g_i2cFunc.i2cReadReg(g_mxt_data.t5.start_address,mex,sizeof(struct message));
}

/** Verifies configuration integrity
 */
static int32_t mxtVerifyConfiguration(struct config_object const * const table, const size_t n)
{
  uint8_t buffer[MXT_CONFIG_SIZE_MAX] = {0};
  int32_t ret = -1;
  ENTER;
  for (size_t i = 0; i < n; i++) {
    LOG_D("Checking object %u", table[i].element->type);
    ret = g_i2cFunc.i2cReadReg(table[i].element->start_address, buffer, table[i].len);
    if (ret) {
      LOG_E("Error reading I2C bus: %d", ret);
      break;
    }
    ret = memcmp(buffer, table[i].value, table[i].len);
    if (ret) {
      LOG_E("Error reading object %u", table[i].element->type);
      DUMPHEX("Got", buffer, table[i].len);
      DUMPHEX("Expected", table[i].value, table[i].len);
      break;
    }
  }
  EXITV(ret);
}

/** Logging function, T9 message
 */
void dumpT9(const struct message *mex) {
  LOG_D("ReportID=%u",mex->reportid);
  LOG_D("STATUS=%u %u %u %u %u %u %u %u",
    (mex->message[0] & 0x80) >> 7,
    (mex->message[0] & 0x40) >> 6,
    (mex->message[0] & 0x20) >> 5,
    (mex->message[0] & 0x10) >> 4,
    (mex->message[0] & 0x08) >> 3,
    (mex->message[0] & 0x04) >> 2,
    (mex->message[0] & 0x02) >> 1,
    (mex->message[0] & 0x01));
  LOG_D("XPOSMSB=0x%02X",mex->message[1]);
  LOG_D("YPOSMSB=0x%02X",mex->message[2]);
  LOG_D("XYPOSLSB= 0x%01X / 0x%01X",
    (mex->message[3] & 0xF0) >> 4,
    (mex->message[3] & 0x0F)
    );
  LOG_D("TCHAREA=0x%02X",mex->message[4]);
  LOG_D("TCHAMPLITUDE=0x%02X",mex->message[5]);
  LOG_D("TCHVECTOR= %d / %d",
    (mex->message[6] & 0xF0) >> 4,
    (mex->message[6] & 0x0F)
    );
}

/** Logging function, T100 message
 */
void dumpT100(const struct message *mex) {
  struct T100_Message *t100 = (struct T100_Message *)(mex->message);
  LOG_D("T100 reportId:%u, status:%02x (%u %u %u) x:%u y:%u vec:%02x amp:%02x area:%02x peak:%02x",
    mex->reportid,
    mex->message[0],
    t100->STATUS.DETECT,
    t100->STATUS.TYPE,
    t100->STATUS.EVENT,
    (mex->message[2] << 8) | mex->message[1],
    (mex->message[4] << 8) | mex->message[3],
    mex->message[5],
    mex->message[6],
    mex->message[7],
    mex->message[8]
    );
}

/** Logging function, T15 message
 */
void dumpT15(const struct message *mex) {
  LOG_D("ReportID=%u",mex->reportid);
  LOG_D("STATUS=DETECT=%u",
    (mex->message[0] & 0x80) >> 7);
  LOG_D("KEYSTATE 7-0  =%u %u %u %u %u %u %u %u",
    (mex->message[1] & 0x80) >> 7,
    (mex->message[1] & 0x40) >> 6,
    (mex->message[1] & 0x20) >> 5,
    (mex->message[1] & 0x10) >> 4,
    (mex->message[1] & 0x08) >> 3,
    (mex->message[1] & 0x04) >> 2,
    (mex->message[1] & 0x02) >> 1,
    (mex->message[1] & 0x01));
  LOG_D("KEYSTATE 15-8  =%u %u %u %u %u %u %u %u",
    (mex->message[2] & 0x80) >> 7,
    (mex->message[2] & 0x40) >> 6,
    (mex->message[2] & 0x20) >> 5,
    (mex->message[2] & 0x10) >> 4,
    (mex->message[2] & 0x08) >> 3,
    (mex->message[2] & 0x04) >> 2,
    (mex->message[2] & 0x02) >> 1,
    (mex->message[2] & 0x01));
  LOG_D("KEYSTATE 23-16=%u %u %u %u %u %u %u %u",
    (mex->message[3] & 0x80) >> 7,
    (mex->message[3] & 0x40) >> 6,
    (mex->message[3] & 0x20) >> 5,
    (mex->message[3] & 0x10) >> 4,
    (mex->message[3] & 0x08) >> 3,
    (mex->message[3] & 0x04) >> 2,
    (mex->message[3] & 0x02) >> 1,
    (mex->message[3] & 0x01));
  LOG_D("KEYSTATE 31-24=%u %u %u %u %u %u %u %u",
    (mex->message[4] & 0x80) >> 7,
    (mex->message[4] & 0x40) >> 6,
    (mex->message[4] & 0x20) >> 5,
    (mex->message[4] & 0x10) >> 4,
    (mex->message[4] & 0x08) >> 3,
    (mex->message[4] & 0x04) >> 2,
    (mex->message[4] & 0x02) >> 1,
    (mex->message[4] & 0x01));
}

/** Logging function, T42 message
 */
void dumpT42(const struct message *mex) {
  LOG_D("ReportID=%u",mex->reportid);
  LOG_D("status=TCHSUP=%u",
    (mex->message[0] & 0x01));
}

/** Process a T9 message, decoding the position in it
 */
void processT9(const struct T9_Message *mex, uint32_t *x, uint32_t *y, uint32_t *code)
{
  *code = TOUCH_EVENT_NONE;
  do {
    if (g_panel_config.width < 1024 ) { // 10-bit format
      *x = (mex->XPOSMSB << 2) | (mex->XYPOSLSB.X >> 2);
    } else { // 12-bit format
      *x = (mex->XPOSMSB << 4) | (mex->XYPOSLSB.X);
    }
    if (g_panel_config.height < 1024 ) { // 10-bit format
      *y = (mex->YPOSMSB << 2) | (mex->XYPOSLSB.Y >> 2);
    } else { // 12-bit format
      *y = (mex->YPOSMSB << 4) | (mex->XYPOSLSB.Y);
    }
    if (mex->STATUS.DETECT == 0) {
      if (mex->STATUS.RELEASE == 1) {
        *code |= TOUCH_EVENT_UP;
      }
    }
    if (mex->STATUS.MOVE) {
      *code |= TOUCH_EVENT_MOVE;
    }
    if (mex->STATUS.PRESS) {
      *code |= TOUCH_EVENT_DOWN;
    }
  } while (0);
}

/** Process a T100 message, decoding the position in it
 */
void processT100(const struct T100_Message *mex, uint32_t *x, uint32_t *y, uint32_t *code, struct mxt_finger *last)
{
  *code = TOUCH_EVENT_NONE;
  uint32_t x_read;
  uint32_t y_read;
  do {
    if (mex->STATUS.DETECT) {
      *code = TOUCH_EVENT_DOWN;
      x_read = (mex->XPOSMSB << 8) | mex->XPOSLSB;
      y_read = (mex->YPOSMSB << 8) | mex->YPOSLSB;
      /* rescale them */
      x_read = (x_read * g_panel_config.width) / g_mxt_data.t100_max_x;
      y_read = (y_read * g_panel_config.height) / g_mxt_data.t100_max_y;
      *x = x_read;
      *y = y_read;
      last->x = *x;
      last->y = *y;
      last->code = *code;
    } else if (mex->STATUS.EVENT == 5) {
      *code = TOUCH_EVENT_UP;
      x_read = (mex->XPOSMSB << 8) | mex->XPOSLSB;
      y_read = (mex->YPOSMSB << 8) | mex->YPOSLSB;
      /* rescale them */
      x_read = (x_read * g_panel_config.width) / g_mxt_data.t100_max_x;
      y_read = (y_read * g_panel_config.height) / g_mxt_data.t100_max_y;
      *x = x_read;
      *y = y_read;
      /* forget about it */
      last->x = 0;
      last->y = 0;
      last->code = TOUCH_EVENT_NONE;
    }
  } while (0);
}

/** Open the I2C bus and initializes our MXT structures
 */
uint32_t atmel_drTsOpen(const uint32_t width, const uint32_t height)
{
  int32_t rv = 0;
  uint8_t obj_num = 0;
  uint8_t i = 0;
  uint32_t reportid = 0;
  uint8_t min_id = 0;
  uint8_t max_id = 0;
  struct object_table_element element = {0};
  ENTER;
  do {
    LOG_D("Opening touch for panel %u x %u", width, height);
    rv = g_i2cFunc.i2cOpen();
    if (rv != 0) {
      LOG_E("Error opening the I2C bus: 0x%08X", rv);
      break;
    }
    opened = 1;

    // how many object is the object table made of
    rv = g_i2cFunc.i2cReadReg(REG_OBJECT_NUM, &obj_num, sizeof(obj_num));
    if (rv) break;

    LOG_D("Number of objects: %u",obj_num);
    // now we iterate through them to fill in the ones we actually care for
    reportid = 1; // start from 1, 0 is invalid
    for (i = 0; i < obj_num; i++) {
      uint8_t buffer[MXT_OBJECT_SIZE];
      rv = g_i2cFunc.i2cReadReg(REG_OBJECT_START + MXT_OBJECT_SIZE*i, &buffer, sizeof(buffer));
      if (rv) break;
      element.type = buffer[0];
      element.start_address = (buffer[2] << 8) | buffer[1];
      element.size = buffer[3]+1;
      element.instances = buffer[4]+1;
      element.num_report_ids = buffer[5];
      if (element.num_report_ids) {
        min_id = reportid;
        reportid += element.num_report_ids * element.instances;
        max_id = reportid - 1;
      } else {
        min_id = 0;
        max_id = 0;
      }
      element.reportid_min = min_id;
      element.reportid_max = max_id;
      LOG_D("T%u Start:%04X Size:%u Instances:%u Report IDs:%u-%u",
        element.type,element.start_address,element.size,
        element.instances,
        element.reportid_min,element.reportid_max);

      switch(element.type) {
        case T5_GEN_MESSAGEPROCESSOR: g_mxt_data.t5 = element; break;
        case T9_TOUCH_MULTITOUCHSCREEN: g_mxt_data.t9 = element; break;
        //case T15_TOUCH_KEYARRAY: g_mxt_data.t15 = element; break;
        //case T42_PROCI_TOUCHSUPPRESSION: g_mxt_data.t42 = element; break;
        case T61_SPT_TIMER: g_mxt_data.t61 = element; break;
        case T100_TOUCH_MULTITOUCHSCREEN: g_mxt_data.t100 = element; break;
      }
    }
    if (rv) break;
    LOG_D("T5: %u/%04X/%u/%u/%u/%u/%u",
      g_mxt_data.t5.type,g_mxt_data.t5.start_address,g_mxt_data.t5.size,
      g_mxt_data.t5.instances,g_mxt_data.t5.num_report_ids,
      g_mxt_data.t5.reportid_min,g_mxt_data.t5.reportid_max
    );
    LOG_D("T9: %u/%04X/%u/%u/%u/%u/%u",
      g_mxt_data.t9.type,g_mxt_data.t9.start_address,g_mxt_data.t9.size,
      g_mxt_data.t9.instances,g_mxt_data.t9.num_report_ids,
      g_mxt_data.t9.reportid_min,g_mxt_data.t9.reportid_max
    );
    LOG_D("T61: %u/%04X/%u/%u/%u/%u/%u",
      g_mxt_data.t61.type,g_mxt_data.t61.start_address,g_mxt_data.t61.size,
      g_mxt_data.t61.instances,g_mxt_data.t61.num_report_ids,
      g_mxt_data.t61.reportid_min,g_mxt_data.t61.reportid_max
    );
    LOG_D("T100: %u/%04X/%u/%u/%u/%u/%u",
      g_mxt_data.t100.type,g_mxt_data.t100.start_address,g_mxt_data.t100.size,
      g_mxt_data.t100.instances,g_mxt_data.t100.num_report_ids,
      g_mxt_data.t100.reportid_min,g_mxt_data.t100.reportid_max
    );
    // if we got a T100, read the associated configuration
    if (g_mxt_data.t100.size > 0) {
      LOG_D("Reading T100 configuration");
      uint8_t t100[T100_SIZE] = {0};
      uint32_t invert_x = 0;
      uint32_t invert_y = 0;
      uint32_t switch_x_y = 0;
      uint32_t max_x = 0;
      uint32_t max_y = 0;
      if (g_mxt_data.t100.size != T100_SIZE) {
        LOG_E("Unexpected T100 size: %u vs %u", g_mxt_data.t100.size, T100_SIZE);
        rv = E_ST_DRIVER_ERROR;
        break;
      }
      rv = g_i2cFunc.i2cReadReg(g_mxt_data.t100.start_address, &t100, sizeof(t100));
      if (rv) {
        LOG_E("Failed to read T100: %d", rv);
        break;
      }
      dumpHex("T100", t100, sizeof(t100));
      invert_x = t100[T100_CFG1] & T100_CFG1_INVERTX;
      invert_y = t100[T100_CFG1] & T100_CFG1_INVERTY;
      switch_x_y = (t100[T100_CFG1] & T100_CFG1_SWITCHXY);
      max_x = (t100[T100_XRANGEMSB] << 8) | t100[T100_XRANGELSB];
      max_y = (t100[T100_YRANGEMSB] << 8) | t100[T100_YRANGELSB];
      if (max_x == 0) max_x = 1023;
      if (max_y == 0) max_y = 1023;
      /* This is a bit twisted...
         - for the touch panel (0,0) is bottom left
         - for the OS (0,0) is top left
         so there are 2 ways of mathing them:
         - invert Y
         - or invert X and switch X/Y
         Both of these are ok. We don't allow any other inversions.
       * */
      if ((invert_y && !invert_x && !switch_x_y) ||
          (!invert_y && invert_x && switch_x_y)) {
        /* All good... */
      } else {
        LOG_E("Unauthorized invert/switch: %u %u %u", invert_x, invert_y, switch_x_y);
        rv = E_ST_DRIVER_ERROR;
        break;
      }
      g_mxt_data.t100_switch_x_y = switch_x_y;
      g_mxt_data.t100_invert_x = invert_x;
      g_mxt_data.t100_invert_y = invert_y;
      if (switch_x_y) {
        g_mxt_data.t100_max_x = max_y;
        g_mxt_data.t100_max_y = max_x;
      } else {
        g_mxt_data.t100_max_x = max_x;
        g_mxt_data.t100_max_y = max_y;
      }
      if (g_mxt_data.t100_max_x == 0) g_mxt_data.t100_max_x = 1023;
      if (g_mxt_data.t100_max_y == 0) g_mxt_data.t100_max_y = 1023;
      LOG_D("T100 max_x=%u, max_y=%u, switch=%u",
        g_mxt_data.t100_max_x, g_mxt_data.t100_max_y, g_mxt_data.t100_switch_x_y);
    }

    if (width != 0)
      g_panel_config.width = width;
    else
      g_panel_config.width = DEFAULT_ATMEL_PANEL_WIDTH;
    if (height != 0)
      g_panel_config.height = height;
    else
      g_panel_config.height = DEFAULT_ATMEL_PANEL_HEIGHT;

    if (g_panel_config.width > g_panel_config.height) {
      LOG_D("Checking for a Liquid device");
      rv = mxtVerifyConfiguration(config_table_liquid, LENGTH(config_table_liquid));
    } else {
      LOG_E("Atmel on 8994 is only on Liquids! Unknown device, cannot check");
      rv = E_ST_DRIVER_ERROR;
    }
    if (rv != 0) {
      LOG_E("Failed to verify configuration");
      break;
    }
  } while (0);
  // cleanup if needed
  if (opened && (rv != 0)) {
    g_i2cFunc.i2cClose();
    opened = 0;
  }
  EXITV((uint32_t)rv);
}

/** Release the I2C bus
 */
uint32_t atmel_drTsClose(void)
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
uint32_t atmel_drTsProcessEvents(void)
{
  int32_t rv = 0;
  uint8_t reportid = 0;
  struct message mex;
  uint32_t x = 0;
  uint32_t y = 0;
  uint32_t code = 0;
  uint32_t finger = 0;
  uint32_t num_mex = 0;
  uint32_t num_scrstatus = 0;
  ENTER;
  do {
    if (!opened) {
      LOG_E("Device not opened.");
      rv = -1;
      break;
    }
    // now read all available events (=messages) from the controller
    do {
      rv = mxtReadMessage(&mex);
      if (rv) break;
      num_mex++;
      reportid = mex.reportid;
      LOG_D("reportid=%02u",reportid);

      if (reportid == 0) {
        // reserved to Atmel
        continue;
      }

      if ((reportid >= g_mxt_data.t9.reportid_min) &&
          (reportid <= g_mxt_data.t9.reportid_max)) {
        finger = reportid - g_mxt_data.t9.reportid_min - 1;
        dumpT9(&mex);
        x=y=code=0;
        processT9((struct T9_Message *)(mex.message), &x, &y, &code);
        drNotifyEvent(x, y, code, finger);
      } else if ((reportid >= g_mxt_data.t100.reportid_min) &&
                 (reportid <= g_mxt_data.t100.reportid_max)) {
        dumpT100(&mex);
        if (reportid - g_mxt_data.t100.reportid_min - 2 < 0) {
          num_scrstatus++;
          LOG_D("Ignore SCRSTATUS");
          continue;
        }
        finger = reportid - g_mxt_data.t100.reportid_min - 2;

        x=y=code=0;
        processT100((struct T100_Message *)(mex.message), &x, &y, &code,
                    &last_fingers[finger]);
        drNotifyEvent(x, y, code, finger);
      }

    } while (reportid != 0xFF); // 255 = invalid message
    if (rv) {
      LOG_E("Failed to read from MXT controller: 0x%08X");
      break;
    }
#ifdef MSM8994_V1
    if (num_mex == num_scrstatus + 1) {
      /* We only got 0xFF, interpret it as a release on any pressed fingers */
      for (finger = 0; finger < MAX_NUMBER_OF_FINGERS; finger++) {
        if ((last_fingers[finger].code == TOUCH_EVENT_DOWN) ||
            (last_fingers[finger].code == TOUCH_EVENT_MOVE)) {
          LOG_E("Releasing finger %u via hack", finger);
          drNotifyEvent(last_fingers[finger].x, last_fingers[finger].y, TOUCH_EVENT_UP, finger);
          last_fingers[finger].x = 0;
          last_fingers[finger].y = 0;
          last_fingers[finger].code = TOUCH_EVENT_NONE;
        }
      }
    }
#endif
  } while (0);
  EXITV((uint32_t)rv);
}
