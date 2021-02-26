/*===========================================================================
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
 07/17/16    dr     Move SUI sample code into a separate source file
===========================================================================*/

#include "sselog.h"
#include <comdef.h>
#include "qsee_fs.h"
#include <string.h>
#include "qsee_log.h"
#include "qsee_services.h"
#include "qsee_core.h"
#include "SecureUI.h"
#include <stdio.h>
#include "SecureUILib.h"
#include <stringl.h>
#include "qsee_heap.h"
#include "AccessControlHyp.h"
#include <stdbool.h>

/* commands supported from sample client */
#define SEC_UI_SAMPLE_CMD0_START_SEC_UI        0
#define SEC_UI_SAMPLE_CMD1_SHOW_IMAGE          1
#define SEC_UI_SAMPLE_CMD2_STOP_DISP           2
#define SEC_UI_SAMPLE_CMD3_SHOW_IMAGE_PTR      3
#define SEC_UI_SAMPLE_CMD4_TEST_APP_BUFFER     4
#define SECUREUILIB_CMD_AUTHENTICATE_FRAME     5

#define BYTES_PER_PIXEL                        4
#define MAX_FILENAME_LEN                   (256)
#define RED_PIXEL                     0xff0000ff
#define GREEN_PIXEL                   0xff00ff00
#define DOT_SIZE                              10
#define NO_TIMEOUT                          (-1)
#define DEFAULT_TIMEOUT               NO_TIMEOUT
#define TOP_EXIT_BAR                         100

// screen and panel sizes for FLUID and LIQUID
#define FLUID_PANEL_8974_W 760
#define FLUID_PANEL_8974_H 1424
#define FLUID_SCREEN_8974_W 720
#define FLUID_SCREEN_8974_H 1280

#define LIQUID_PANEL_8974_W 1920
#define LIQUID_PANEL_8974_H 1080
#define LIQUID_SCREEN_8974_W 1920
#define LIQUID_SCREEN_8974_H 1080

#define FLUID_PANEL_8084_W 1080
#define FLUID_PANEL_8084_H 2085
#define FLUID_SCREEN_8084_W 1080
#define FLUID_SCREEN_8084_H 1920

#define LIQUID_PANEL_8084_W 2560
#define LIQUID_PANEL_8084_H 1440
#define LIQUID_SCREEN_8084_W 2560
#define LIQUID_SCREEN_8084_H 1440

#define FLUID_PANEL_8994_W 1600
#define FLUID_PANEL_8994_H 2704
#define FLUID_SCREEN_8994_W 1600
#define FLUID_SCREEN_8994_H 2600
#define FLUID_INIT_SCREEN_8994_SZ 0x1040000

#define LIQUID_PANEL_8994_W 3840
#define LIQUID_PANEL_8994_H 2160
#define LIQUID_SCREEN_8994_W 3840
#define LIQUID_SCREEN_8994_H 2160

#define FLUID_PANEL_8996_W 1600
#define FLUID_PANEL_8996_H 2704
#define FLUID_SCREEN_8996_W 1600
#define FLUID_SCREEN_8996_H 2560
#define FLUID_INIT_SCREEN_8996_SZ 0xFA0000

#define MAX_UPDATE_EVENTS    40
#define MAX_BUFFERS           5

// Used by is_secure_boot_enabled 
#define SECBOOT_FUSE          0
#define SHK_FUSE              1
#define DEBUG_DISABLED_FUSE   2
#define ANTI_ROLLBACK_FUSE    3
#define FEC_ENABLED_FUSE      4
#define RPMB_ENABLED_FUSE     5
#define DEBUG_RE_ENABLED_FUSE 6

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

static uint32_t dispH    = 0;
static uint32_t dispW    = 0;
static uint32_t dispS    = 0;
static uint32_t pixelW   = 0;
static uint32_t rotation   = 0;
static uint8_t *g_screen = NULL;
static uint8_t *g_screen_last = NULL;
static uint8 red_dot[DOT_SIZE*BYTES_PER_PIXEL];
static uint8 green_dot[DOT_SIZE*BYTES_PER_PIXEL];

struct touch_event {
  uint32_t x;
  uint32_t y;
  uint8_t *dot;
};

struct event_queue {
  uint8_t *buffer;
  struct touch_event events[MAX_UPDATE_EVENTS];
};

static struct event_queue queue[MAX_BUFFERS] = {0};

struct send_cmd{
  uint32_t cmd_id;
  uint32_t height;
  uint32_t width;
  uint32_t x;
  uint32_t y;
  uint32_t timeout;
  char img_path[MAX_FILENAME_LEN];
};

struct send_cmd_rsp{
  int32_t status;
};

static bool is_secure_boot_enabled(void)
{
  qsee_secctrl_secure_status_t status = {0,0};
  bool retval = true;

  qsee_get_secure_state(&status);

  if(!CHECK_BIT(status.value[0], SECBOOT_FUSE) && !CHECK_BIT(status.value[0], SHK_FUSE)
        && !CHECK_BIT(status.value[0], DEBUG_DISABLED_FUSE) && !CHECK_BIT(status.value[0], RPMB_ENABLED_FUSE)
        && CHECK_BIT(status.value[0], DEBUG_RE_ENABLED_FUSE))
    retval = true;
  else
    retval = false;

  return retval;
}

static void copy_dot(uint8_t * const buffer, struct touch_event * ev) {
  size_t y = 0;
  size_t dot_width = 0;
  size_t buffer_size = dispS*dispH*pixelW;
  if (dispW - ev->x > DOT_SIZE) {
    dot_width = DOT_SIZE;
  } else {
    dot_width = dispW - ev->x;
  }
  qsee_log(QSEE_LOG_MSG_DEBUG, "%s: (%u,%u) onto buffer 0x%08X", __func__, ev->x, ev->y, buffer);
  for(y = ev->y; (y < ev->y + DOT_SIZE) && (y < dispH); y++) {
    memscpy(buffer + (y*dispS + ev->x)*pixelW, buffer_size - (y*dispS + ev->x)*pixelW, ev->dot, dot_width*BYTES_PER_PIXEL);
  }
}

static sec_touch_cmd_t draw_dot(sec_touch_err_t err, struct tsFingerData *fingers, int32_t *timeout)
{
  int32_t retval = -1;
  size_t i = 0;
  size_t j = 0;
  size_t k = 0;
  size_t current = 0;
  qsee_log(QSEE_LOG_MSG_DEBUG, "%s: err=  %d", __func__, err);

  if(SEC_TOUCH_ERR_STARTED == err){
    *timeout = DEFAULT_TIMEOUT;
    return SEC_TOUCH_CMD_CONTINUE;
  }

  if(SEC_TOUCH_ERR_STOPPED == err)
  {
    sec_ui_err_t status = SEC_UI_SUCCESS;

    if (g_screen == NULL) {
      return SEC_TOUCH_CMD_CANCELLED;
    }
	
    // terminate secure display
    status = sec_ui_stop_disp();
    if (status != SEC_UI_SUCCESS &&
        status != SEC_UI_IS_NOT_RUNNING &&
        status != SEC_UI_ABORTED) {
      return SEC_TOUCH_CMD_CANCELLED;
    }
    return SEC_TOUCH_CMD_COMPLETED;
  }
  if(SEC_TOUCH_ERR_DATA != err)
    return SEC_TOUCH_CMD_CANCELLED;

  // make sure current screen is in the queue
  for(i=0; i<MAX_BUFFERS; i++) {
    if (queue[i].buffer == NULL) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "%s: New buffer: 0x%08X [%u]", __func__, g_screen, i);
      queue[i].buffer = g_screen;
      current = i;
      break;
    }
    if (queue[i].buffer == g_screen) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "%s: Buffer 0x%08X already in queue [%u]", __func__, g_screen, i);
      current = i;
      break;
    }
  }
  if (i == MAX_BUFFERS) {
    qsee_log(QSEE_LOG_MSG_ERROR, "%s: Too many buffers!", __func__);
    return SEC_TOUCH_CMD_CANCELLED;
  }

  for (j = 0; j < MAX_FINGER_NUM; j++) {
    if (fingers->finger[j].event != TLAPI_TOUCH_EVENT_NONE) {
      struct touch_event ev = {0};
      qsee_log(QSEE_LOG_MSG_DEBUG, "Position: (%u, %u)", fingers->finger[j].x, fingers->finger[j].y);
      if (fingers->finger[j].y < TOP_EXIT_BAR) {
        return SEC_TOUCH_CMD_COMPLETED;
      }
      ev.x = fingers->finger[j].x;
      ev.y = fingers->finger[j].y;
      if (fingers->finger[j].event & TLAPI_TOUCH_EVENT_UP) {
        ev.dot = green_dot;
      } else {
        ev.dot = red_dot;
      }
      // add it to all buffers
      for (i = 0; i < MAX_BUFFERS; i++) {

        if (queue[i].buffer == NULL) break; // done
        for (k = 0; k < MAX_UPDATE_EVENTS; k++) {
          if (queue[i].events[k].dot == NULL) {
            queue[i].events[k] = ev;
            break;
          }
        }
      }
    }
  }

  // draw queue for current buffer
  for (k = 0; k < MAX_UPDATE_EVENTS; k++) {
    if (queue[current].events[k].dot != NULL) {
      copy_dot(queue[current].buffer,&queue[current].events[k]);
      queue[current].events[k].dot = NULL;
    } else {
      break;
    }
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "Succeeded drawing dot in secure buffer");
  g_screen_last = g_screen;
  retval = sec_ui_show_buf(FALSE, &g_screen);
  qsee_log(QSEE_LOG_MSG_DEBUG, "sec_ui_show_buf returned %d", retval);

  if(retval < 0) {
    return SEC_TOUCH_CMD_CANCELLED;
  }
  // make sure new screen is in the queue
  for(i=0; i<MAX_BUFFERS; i++) {
    if (queue[i].buffer == NULL) {
      qsee_log(QSEE_LOG_MSG_DEBUG, "%s: New buffer: 0x%08X [%u]", __func__, g_screen, i);
      queue[i].buffer = g_screen;
      memscpy(g_screen, dispH*dispS*pixelW, g_screen_last, dispH*dispS*pixelW);
      break;
    }
    if (queue[i].buffer == g_screen) break; // there already, nothing to do
  }
  if (i == MAX_BUFFERS) {
    qsee_log(QSEE_LOG_MSG_ERROR, "%s: Too many buffers!", __func__);
    return SEC_TOUCH_CMD_CANCELLED;
  }

  *timeout = DEFAULT_TIMEOUT;
  return SEC_TOUCH_CMD_CONTINUE;
}

void sui_app_init(void) {
	int l_fd, bytes, i ,res;
	fs_stat logo_stat;

	for(i=0;i<DOT_SIZE;i++){
		((uint32 *)red_dot)[i] = RED_PIXEL;
		((uint32 *)green_dot)[i] = GREEN_PIXEL;
	}

}

static int32_t test_app_buffer_alloc(uint32_t buf_len)
{
  int32_t ret = 0;
  boolean prot = FALSE;
  uint8_t *buf;

  qsee_log(QSEE_LOG_MSG_DEBUG, " START: test App buffer: alloc on length=%u", buf_len);
  ret = sec_ui_alloc_app_buffer(buf_len, &buf);
  if (ret < 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, " failed to allocate app buffer. Returned: %d", ret);
    return 0;
  }
  prot = qsee_is_s_tag_area(AC_VM_CP_APP, (uint32_t)buf, (uint32_t)buf + buf_len);
  if (!prot) {
    qsee_log(QSEE_LOG_MSG_ERROR, " app buffer not correctly protected.");
    return 0;
  }
  ret = sec_ui_free_app_buffer();
  if (ret < 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, " failed to free the app buffer. Returned: %d", ret);
    return 0;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, " End: test App buffer SUCCESS");
  return 1;
}

static int32_t test_app_buffer_free_noalloc(void)
{
  int32_t ret = 0;

  qsee_log(QSEE_LOG_MSG_DEBUG, "START: test App buffer: free");
  ret = sec_ui_free_app_buffer();
  if (ret == 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, "sec_ui_free_app_buffer should fail. No previous allocations. Returned: %d", ret);
    return 0;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "End: test App buffer SUCCESS");
  return 1;
}

static int32_t test_app_buffer_multi_alloc(void)
{
  int32_t ret = 0;
  boolean prot = FALSE;
  uint8_t *buf;
  uint32_t buf_len = 0x10000;

  qsee_log(QSEE_LOG_MSG_DEBUG, "START: test App buffer: multi alloc");
  ret = sec_ui_alloc_app_buffer(buf_len, &buf);
  if (ret < 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, "failed to allocate app buffer. Returned: %d", ret);
    return 0;
  }
  ret = sec_ui_alloc_app_buffer(buf_len, &buf);
  if (ret == 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, "sec_ui_alloc_app_buffer should fail. Buffer already allocated.");
    return 0;
  }
  prot = qsee_is_s_tag_area(AC_VM_CP_APP, (uint32_t)buf, (uint32_t)buf + buf_len);
  if (!prot) {
    qsee_log(QSEE_LOG_MSG_ERROR, "app buffer not correctly protected.");
    return 0;
  }
  ret = sec_ui_free_app_buffer();
  if (ret < 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, "failed to free the app buffer. Returned: %d", ret);
    return 0;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "End: test App buffer SUCCESS");
  return 1;
}

static int32_t test_app_buffer_alloc_multi_length(void)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "START: test App buffer: alloc multi length");
  if (!test_app_buffer_alloc(0x1))
    return 0;
  if (!test_app_buffer_alloc(0x1000000))
    return 0;
  if (!test_app_buffer_alloc(0x1234567))
    return 0;
  if (!test_app_buffer_alloc(0x2000000))
    return 0;

  qsee_log(QSEE_LOG_MSG_DEBUG, "End: test App buffer SUCCESS");
  return 1;
}

static int32_t test_app_buffer_use_app_buffer(void)
{
  int32_t ret = 1;
  boolean prot = FALSE;
  uint8_t *buf = NULL;
  uint32_t buf_len = 0x1000000;
  uint8_t mem_tag_byte = 0xFA;
  int i;

  qsee_log(QSEE_LOG_MSG_DEBUG, "START: test App buffer: use buffer");

  do {
    if (sec_ui_alloc_app_buffer(buf_len, &buf) < 0) {
      qsee_log(QSEE_LOG_MSG_ERROR, "failed to allocate app buffer. Returned: %d", ret);
      return 0;
    }

    secure_memset((void *)buf, mem_tag_byte, buf_len);

    prot = qsee_is_s_tag_area(AC_VM_CP_APP, (uint32_t)buf, (uint32_t)buf + buf_len);
    if (!prot) {
      qsee_log(QSEE_LOG_MSG_ERROR, "app buffer not correctly protected.");
      ret = 0;
      break;
    }

    for (i = 0; i < buf_len; ++i) {
      if (buf[i] != mem_tag_byte) {
        qsee_log(QSEE_LOG_MSG_ERROR, "Wrong content read in the app buffer");
        ret = 0;
        break;
      }
    }
  } while(0);

  if (sec_ui_free_app_buffer() < 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, "failed to free the app buffer. Returned: %d", ret);
    return 0;
  }

  if (ret == 1)
    qsee_log(QSEE_LOG_MSG_DEBUG, "End: test App buffer SUCCESS");
  return ret;

}

static int32_t test_app_buffer(void)
{
  int32_t ret = 0;
  if (!test_app_buffer_alloc_multi_length()) {
      ret += -1;
  }
  if (!test_app_buffer_free_noalloc()) {
      ret += -1;
  }
  if (!test_app_buffer_multi_alloc()) {
      ret += -1;
  }
  if (!test_app_buffer_use_app_buffer()) {
      ret += -1;
  }
  return ret;
}

static int32_t alloc_init_screen(uint8_t **buf, uint32_t buf_len)
{
  int32_t retval = 0;
  uint8_t *tbuf;

  retval = sec_ui_alloc_app_buffer(buf_len, buf);
  if (retval < 0) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "failed to allocate app buffer %d", retval);
    return retval;
  }
  tbuf = *buf;
  if (!qsee_is_s_tag_area(AC_VM_CP_APP, (uint32_t)tbuf, (uint32_t)tbuf + buf_len)) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "buffer not tagged properly");
    return -1;
  }

  secure_memset((void *)tbuf, 0xAB, buf_len);

  return retval;

}

static void start_disp_fullscreen(struct send_cmd *cmd_ptr,
                                 struct send_cmd_rsp *rsp_ptr)
{
  int32_t retval = -1;
  uint8_t *initScreen = NULL;
  uint32_t initScreenLen = 0x0;

  retval = sec_ui_get_disp_properties(&dispH, &dispW);
  qsee_log(QSEE_LOG_MSG_DEBUG, "sec_ui_get_disp_properties returned  %d", retval);

  if(retval < 0){
    rsp_ptr->status = retval;
    return;
  }
  qsee_log(QSEE_LOG_MSG_DEBUG, "Display size: %u x %u", dispW, dispH);
  if (dispW == LIQUID_PANEL_8974_W) {
    secUiSetPanelSize(LIQUID_PANEL_8974_W, LIQUID_PANEL_8974_H);
  } else if (dispW == FLUID_PANEL_8974_W) {
    secUiSetPanelSize(FLUID_PANEL_8974_W, FLUID_PANEL_8974_H);
  } else if (dispW == LIQUID_PANEL_8084_W) {
    secUiSetPanelSize(LIQUID_PANEL_8084_W, LIQUID_PANEL_8084_H);
  } else if (dispW == FLUID_PANEL_8084_W) {
    secUiSetPanelSize(FLUID_PANEL_8084_W, FLUID_PANEL_8084_H);
  } else if (dispW == LIQUID_PANEL_8994_W) {
    secUiSetPanelSize(LIQUID_PANEL_8994_W, LIQUID_PANEL_8994_H);
  } else if (dispW == FLUID_SCREEN_8994_W && dispH == FLUID_SCREEN_8994_H) {
    secUiSetPanelSize(FLUID_PANEL_8994_W, FLUID_PANEL_8994_H);
    initScreenLen = FLUID_INIT_SCREEN_8994_SZ;
  } else if (dispW == FLUID_SCREEN_8996_W && dispH == FLUID_SCREEN_8996_H) {
	  secUiSetPanelSize(FLUID_PANEL_8996_W, FLUID_PANEL_8996_H);
	  initScreenLen = FLUID_INIT_SCREEN_8996_SZ;
  }
  else {
    qsee_log(QSEE_LOG_MSG_ERROR, "No matching panel for width: %u", dispW);
  }

  if (initScreenLen > 0) {
    retval = alloc_init_screen(&initScreen, initScreenLen);
    if(retval < 0){
      rsp_ptr->status = retval;
      return;
    }
  }

  retval = sec_ui_start_disp_fullscreen(HAL_PIXEL_FORMAT_RGBA_8888, initScreen, initScreenLen,
                                        &g_screen, &dispS, &pixelW, &rotation);
  qsee_log(QSEE_LOG_MSG_DEBUG, "sec_ui_start_disp_fullscreen returned  %d", retval);
  if(retval < 0){
    rsp_ptr->status = retval;
    return;
  }

  if(pixelW != BYTES_PER_PIXEL){
	  qsee_log(QSEE_LOG_MSG_DEBUG, "unsupported pixel width %d", pixelW);
	  rsp_ptr->status = -1;
	  return;
  }
  qsee_log(QSEE_LOG_MSG_DEBUG, "Detected rotation: %d", rotation);

}

static void show_image(struct send_cmd     *cmd_ptr,
                struct send_cmd_rsp *rsp_ptr)
{
  int32_t retval = -1;
  int l_fd;
  uint32_t row, h, w;
  struct input_to_copy in;
  qsee_log(QSEE_LOG_MSG_ERROR, "%s+", __func__);

  l_fd = open(cmd_ptr->img_path, O_RDONLY);
  if (l_fd < 0) {
    qsee_log(QSEE_LOG_MSG_ERROR, "Failed to open file %s", cmd_ptr->img_path);
    rsp_ptr->status = l_fd;
    return;
  }

  do {

    in.type = SEC_UI_FD_FS;
    in.input.u32Fd = l_fd;

    h = cmd_ptr->height;
    w = cmd_ptr->width;
    for (row = cmd_ptr->y; row < h + cmd_ptr->y; row++) {
      retval = sec_ui_copy_to_buffer(in, cmd_ptr->x,row, w*BYTES_PER_PIXEL);
      if (retval < 0) {
        rsp_ptr->status = retval;
        qsee_log(QSEE_LOG_MSG_ERROR, "copy to buffer failed; received  %d", retval);
        break;
      }
    }
    if (retval >= 0) {
      retval = sec_ui_show_buf(TRUE, &g_screen);
      if (retval < 0) {
        rsp_ptr->status = retval;
        qsee_log(QSEE_LOG_MSG_ERROR, "show buffer failed; received  %d", retval);
      }
    }
  } while (0);

  if (l_fd >= 0)
    close(l_fd);
}

static void show_image_ptr(struct send_cmd     *cmd_ptr,
                    struct send_cmd_rsp *rsp_ptr)
{
  boolean protected = FALSE;
  uint64_t long_res;

  qsee_log(QSEE_LOG_MSG_ERROR, "%s+", __func__);

  long_res = (uint64_t)dispH*dispS;
  if(long_res > UINT32_MAX){
    rsp_ptr->status = -1;
    qsee_log(QSEE_LOG_MSG_ERROR, "integer overflow in display properties calculation");
    return;
  }

  long_res *= BYTES_PER_PIXEL;
  if(long_res > UINT32_MAX){
    rsp_ptr->status = -1;
    qsee_log(QSEE_LOG_MSG_ERROR, "integer overflow in display properties calculation");
    return;
  }

  secure_memset (g_screen, 0, long_res);

  long_res += (uint32_t)g_screen;
    if(long_res > UINT32_MAX){
    rsp_ptr->status = -1;
    qsee_log(QSEE_LOG_MSG_ERROR, "integer overflow in display properties calculation");
    return;
  }

  protected = qsee_is_s_tag_area(AC_VM_CP_SECDISP, (uint32_t)g_screen, (uint32_t)long_res);
  if (!protected) {
    rsp_ptr->status = -1;
    qsee_log(QSEE_LOG_MSG_ERROR, "Buffer not protected");
    return;
  }

  show_image(cmd_ptr, rsp_ptr);
}

void sui_app_cmd_handler(void* cmd, uint32_t cmdlen,
                        void* rsp, uint32_t rsplen)
{
  struct send_cmd *cmd_ptr = (struct send_cmd *)cmd;
  struct send_cmd_rsp *rsp_ptr = (struct send_cmd_rsp *)rsp;
  sec_ui_err_t retval = SEC_UI_GENERAL_ERROR;

  if ( NULL == cmd || NULL == rsp) {
	qsee_log(QSEE_LOG_MSG_ERROR, "cmd handler received null cmd/rsp buffers");
    return;
  }

  if (is_secure_boot_enabled()) {
        rsp_ptr->status = 1;
        qsee_log(QSEE_LOG_MSG_ERROR, "Secure-boot enabled, can not use test commands");
        return;        
  }

  if ((cmdlen < sizeof(struct send_cmd)) || (rsplen < sizeof(struct send_cmd_rsp))) {
	qsee_log(QSEE_LOG_MSG_ERROR, "cmd handler received too short cmd/rsp buffers %d vs %d, %d vs %d",cmdlen, sizeof(struct send_cmd),rsplen ,sizeof(struct send_cmd_rsp));
    return;
  }

  rsp_ptr->status = 0;
  switch (cmd_ptr->cmd_id) {
    case SEC_UI_SAMPLE_CMD0_START_SEC_UI:
      start_disp_fullscreen(cmd_ptr, rsp_ptr);
	  secUiRegisterTouchCallback(&draw_dot);
      break;

    case SEC_UI_SAMPLE_CMD1_SHOW_IMAGE:
      show_image(cmd_ptr,rsp_ptr);
      break;

    case SEC_UI_SAMPLE_CMD2_STOP_DISP:
      retval = sec_ui_stop_disp();
      if (retval < 0) {
        rsp_ptr->status = retval;
      }
      sec_ui_free_app_buffer();
      break;

    case SEC_UI_SAMPLE_CMD3_SHOW_IMAGE_PTR:
      show_image_ptr(cmd_ptr, rsp_ptr);
      break;

    case SEC_UI_SAMPLE_CMD4_TEST_APP_BUFFER:
      rsp_ptr->status = test_app_buffer();
      break;

    case SECUREUILIB_CMD_AUTHENTICATE_FRAME:
      qsee_log(QSEE_LOG_MSG_DEBUG, "SECUREUILIB_CMD_AUTHENTICATE_FRAME command rcvd!!");
      rsp_ptr->status = sec_ui_authenticate_frame();
      break;

    default:
      rsp_ptr->status = SEC_UI_NOT_SUPPORTED;
      qsee_log(QSEE_LOG_MSG_DEBUG, "Unsupported command\n\n");
      break;
  }
}

void sui_app_shutdown(void)
{
    bool touch_status = 0;
    sec_ui_err_t status = SEC_UI_SUCCESS;

    status = sec_ui_stop_disp();
    if (status != SEC_UI_SUCCESS &&
        status != SEC_UI_IS_NOT_RUNNING &&
        status != SEC_UI_ABORTED) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to stop display (%d), cant terminate touch", status);
        return;
    }

    touch_status = secUiTouchStop();
    if (!touch_status) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to stop touch");
    }

    status = sec_ui_free_app_buffer();
    if (status != SEC_UI_SUCCESS) {
        qsee_log(QSEE_LOG_MSG_ERROR, "failed to free app buffer (%d)", status);
    }
}
