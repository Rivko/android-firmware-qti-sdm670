/*
@file app_secure_ui.c
@brief Contains test code for most of the QSEE Secure UI APIs.

*/
/*===========================================================================
   Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#include "qsee_log.h"
#include "qsee_core.h"
#include "SecureUI.h"
#include "SecureUILib.h"
#include <stringl.h>
#include <qsee_heap.h>
#include "ACCommon.h"

#ifdef ENABLE_QSEE_LOG_MSG_DEBUG
#undef ENABLE_QSEE_LOG_MSG_DEBUG
#define ENABLE_QSEE_LOG_MSG_DEBUG 1
#endif

#define BYTES_PER_PIXEL                        4
#define RED_PIXEL                     0xff0000ff
#define GREEN_PIXEL                   0xff00ff00
#define DOT_SIZE                              10
#define NO_TIMEOUT                          (-1)
#define DEFAULT_TIMEOUT               NO_TIMEOUT
#define TOP_EXIT_BAR                         100

#define MAX_UPDATE_EVENTS    40
#define MAX_BUFFERS           5

#define IMAGE_POSITION      150
#define IMAGE_X_MAX         500
#define IMAGE_Y_MAX         100

#define APP_BUFF_TEST_ENABLE 0

uint32_t dispH    = 0;
uint32_t dispW    = 0;
uint32_t dispS    = 0;
uint32_t pixelW   = 0;
uint32_t rotation   = 0;
uint8_t *g_screen = NULL;
uint8_t *g_screen_last = NULL;
uint8 red_dot[DOT_SIZE*BYTES_PER_PIXEL];
uint8 green_dot[DOT_SIZE*BYTES_PER_PIXEL];
uint8 image_dot[DOT_SIZE*BYTES_PER_PIXEL];

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

int32_t stop_disp(void)
{
  return sec_ui_stop_disp( );
}

void copy_dot(uint8_t * const buffer, struct touch_event * ev) {
  size_t y           = 0;
  size_t dot_width   = 0;
  size_t buffer_size = dispS*dispH*pixelW;

  if( dispW - ev->x > DOT_SIZE )
  {
    dot_width = DOT_SIZE;
  }
  else
  {
    dot_width = dispW - ev->x;
  }

  qsee_log(QSEE_LOG_MSG_DEBUG, "%s: (%u,%u) onto buffer 0x%08X", __func__, ev->x, ev->y, buffer);

  for( y = ev->y; ( y < ev->y + DOT_SIZE ) && ( y < dispH ); y++ )
  {
    memscpy( buffer + ( y * dispS + ev->x ) * pixelW,
             buffer_size - ( y * dispS + ev->x ) * pixelW,
             ev->dot,
             dot_width * BYTES_PER_PIXEL );
  }
}

static sec_touch_cmd_t draw_dot( sec_touch_err_t err, struct tsFingerData *fingers, int32_t *timeout )
{
  int32_t retval         = -1;
  size_t i = 0, j = 0, k =  0;
  size_t current         =  0;
  struct touch_event ev  = { 0 };

  qsee_log(QSEE_LOG_MSG_DEBUG, "%s: err=  %d", __func__, err);

  if( SEC_TOUCH_ERR_STARTED == err )
  {
    *timeout = DEFAULT_TIMEOUT;
    return SEC_TOUCH_CMD_CONTINUE;
  }

  if(SEC_TOUCH_ERR_STOPPED == err)
  {
    retval = stop_disp( );
    if (retval != 0) {
      return SEC_TOUCH_CMD_CANCELLED;
    }
    if (secUiTouchStop( )) {
      return SEC_TOUCH_CMD_COMPLETED;
    } else {
      return SEC_TOUCH_CMD_CANCELLED;
    }
  }
  if(SEC_TOUCH_ERR_DATA != err)
  {
    return SEC_TOUCH_CMD_CANCELLED;
  }

  // make sure current screen is in the queue
  for( i = 0; i < MAX_BUFFERS; i++ )
  {
    if( queue[ i ].buffer == NULL )
    {
      qsee_log( QSEE_LOG_MSG_DEBUG, "%s: New buffer: 0x%08X [%u]", __func__, g_screen, i );
      queue[i].buffer = g_screen;
      current = i;
      break;
    }

    if( queue[ i ].buffer == g_screen )
    {
      qsee_log( QSEE_LOG_MSG_DEBUG, "%s: Buffer 0x%08X already in queue [%u]", __func__, g_screen, i );
      current = i;
      break;
    }
  }

  if( i == MAX_BUFFERS )
  {
    qsee_log( QSEE_LOG_MSG_ERROR, "%s: Too many buffers!", __func__ );
    return SEC_TOUCH_CMD_CANCELLED;
  }

  for( j = 0; j < MAX_FINGER_NUM; j++ )
  {
    if( fingers->finger[ j ].event != TLAPI_TOUCH_EVENT_NONE )
    {

      qsee_log( QSEE_LOG_MSG_DEBUG, "Position: ( %u, %u )", fingers->finger[ j ].x, fingers->finger[ j ].y );

      if( fingers->finger[ j ].y < TOP_EXIT_BAR )
      {
        return SEC_TOUCH_CMD_COMPLETED;
      }
      ev.x = fingers->finger[ j ].x;
      ev.y = fingers->finger[ j ].y;
      if( fingers->finger[j].event & TLAPI_TOUCH_EVENT_UP )
     {
        ev.dot = green_dot;
      } else {
        ev.dot = red_dot;
      }

      // add it to all buffers
      for( i = 0; i < MAX_BUFFERS; i++ )
      {
        if( queue[ i ].buffer == NULL )
          break; // done
        for( k = 0; k < MAX_UPDATE_EVENTS; k++ )
        {
          if( queue[ i ].events[ k ].dot == NULL )
          {
            queue[ i ].events[ k ] = ev;
            break;
          }
        }
      }
    }
  }

  // draw queue for current buffer
  for( k = 0; k < MAX_UPDATE_EVENTS; k++ )
  {
    if( queue[ current ].events[ k ].dot != NULL )
    {
      copy_dot( queue[ current ].buffer, &queue[ current ].events[ k ] );
      queue[ current ].events[ k ].dot = NULL;
    }
    else
    {
      break;
    }
  }

  qsee_log( QSEE_LOG_MSG_DEBUG, "Succeeded drawing dot in secure buffer" );
  g_screen_last = g_screen;
  retval = sec_ui_show_buf( FALSE, &g_screen );
  qsee_log( QSEE_LOG_MSG_DEBUG, "sec_ui_show_buf returned %d", retval );

  if( retval < 0 ) {
    return SEC_TOUCH_CMD_CANCELLED;
  }
  // make sure new screen is in the queue
  for( i=0; i < MAX_BUFFERS; i++ )
  {
    if( queue[ i ].buffer == NULL )
    {
      qsee_log( QSEE_LOG_MSG_DEBUG, "%s: New buffer: 0x%08X [%u]", __func__, g_screen, i );
      queue[ i ].buffer = g_screen;
      memscpy( g_screen,
               dispH * dispS * pixelW,
               g_screen_last,
               dispH * dispS * pixelW );
      break;
    }

    if( queue[ i ].buffer == g_screen )
      break; // there already, nothing to do
  }

  if( i == MAX_BUFFERS )
  {
    qsee_log( QSEE_LOG_MSG_ERROR, "%s: Too many buffers!", __func__ );
    return SEC_TOUCH_CMD_CANCELLED;
  }

  *timeout = DEFAULT_TIMEOUT;
  return SEC_TOUCH_CMD_CONTINUE;
}

int get_display_properties( uint32_t *initScreenLen )
{
  int retval = 0;

  if( ( retval = sec_ui_get_disp_properties( &dispH,  // can you retrieve the display properties?
                                             &dispW ) ) < 0 )
  {
    qsee_log( QSEE_LOG_MSG_ERROR, "sec_ui_get_disp_properties returned  %d", retval );
  }

  return retval;
}

int start_disp_fullscreen()
{
  int32_t  retval        = -1;
  uint32_t initScreenLen =  0;

  do{

    retval = get_display_properties( &initScreenLen );
    if( retval < 0 )
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "%d = get_display_properties( &initScreenLen ) failed", retval);
      break;
    }

    retval = sec_ui_start_disp_fullscreen( HAL_PIXEL_FORMAT_RGBA_8888,  // can you start the display fullscreen?
                                           NULL,
                                           initScreenLen,
                                          &g_screen,
                                          &dispS,
                                          &pixelW,
                                          &rotation );
    if( retval < 0 )
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "sec_ui_start_disp_fullscreen returned  %d", retval);
      break;
    }

    if( pixelW != BYTES_PER_PIXEL )  // is pixel width different from bytes per pixel?
    {
      qsee_log(QSEE_LOG_MSG_ERROR, "unsupported pixel width %d", pixelW);  // no, unsupported pixel width
      retval = -1;
      break;
    }
  }
  while( 0 );

  return retval;
}

int show_image( )
{
  size_t x           = IMAGE_POSITION;
  size_t y           = IMAGE_POSITION;
  size_t dot_width   =   0;
  size_t buffer_size = dispS*dispH*pixelW;
  int32_t retval     =  -1;
  uint8_t *color     = image_dot;
  volatile int spin  = 1;

  if (dispW - x > DOT_SIZE)
  {
    dot_width = DOT_SIZE;
  }
  else
  {
    dot_width = dispW - x;
  }

  memset( g_screen, 0, buffer_size );

  for( uint32_t j = RED_PIXEL; j < GREEN_PIXEL; j+=0x1fa )
  {

    for( int i = 0; i < DOT_SIZE; i++ )
    {
      ( ( uint32 * ) image_dot ) [ i ] = j;
    }

    for( x = IMAGE_POSITION; ( x < IMAGE_POSITION + IMAGE_X_MAX ) && ( x < dispW ); x++ )
    {
      for( y = IMAGE_POSITION; ( y < IMAGE_POSITION + IMAGE_Y_MAX ) && ( y < dispH ); y++ )
      {
        if( memscpy( g_screen + ( y*dispS + x ) * pixelW,
                     ( buffer_size - ( y*dispS + x ) * pixelW ) / sizeof( uint32_t ),
                     color,
                     dot_width*BYTES_PER_PIXEL ) < dot_width*BYTES_PER_PIXEL )
        {
          qsee_log( QSEE_LOG_MSG_ERROR, "memscpy failed when drawing" );
          break;
        }
      }
    }

    if( ( retval = sec_ui_show_buf( TRUE, &g_screen ) ) != 0 )
    {
      qsee_log( QSEE_LOG_MSG_ERROR, "sec_ui_show_buf returned %d", retval );
      return retval;
    }

  }

  return retval;
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
  prot = qsee_is_s_tag_area(AC_VM_CP_APP, (uintnt)buf, (uintnt)buf + buf_len);
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
  prot = qsee_is_s_tag_area(AC_VM_CP_APP, (uintnt)buf, (uintnt)buf + buf_len);
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
  int32_t retval = 1;
  qsee_log(QSEE_LOG_MSG_DEBUG, "START: test App buffer: alloc multi length");
  if (!test_app_buffer_alloc(0x1)) retval = 0;
  if (!test_app_buffer_alloc(0x1000000)) retval = 0;
  if (!test_app_buffer_alloc(0x1234567)) retval = 0;
  if (!test_app_buffer_alloc(0x2000000)) retval = 0;

  if (retval) {
    qsee_log(QSEE_LOG_MSG_DEBUG, "End: test App buffer SUCCESS");
  }
  else {
    qsee_log(QSEE_LOG_MSG_ERROR, "End: test App buffer FAILED");
  }
  return retval;
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

    memset((void *)buf, mem_tag_byte, buf_len);

    prot = qsee_is_s_tag_area(AC_VM_CP_APP, (uintnt)buf, (uintnt)buf + buf_len);
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

int32_t test_app_buffer(void)
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

int run_app_buff_test( void )
{
  int retval;
  /* test app cache functionalities */
  if ((retval = test_app_buffer()) < 0)
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "=========================");
    qsee_log(QSEE_LOG_MSG_ERROR, "test_app_buffer FAILED %d", retval);
    qsee_log(QSEE_LOG_MSG_ERROR, "=========================");
  }
  else
  {
    qsee_log(QSEE_LOG_MSG_DEBUG, "======================================");
    qsee_log(QSEE_LOG_MSG_DEBUG, "test_app_buffer : SUCCESS on all tests");
    qsee_log(QSEE_LOG_MSG_DEBUG, "======================================");
  }

  return retval;
}

int run_display_test( void )
{
  int retval = 0;
#if APP_BUFF_TEST_ENABLE
  uint8_t *cache;
  uint32_t cachelen = 0x100000;

  run_app_buff_test();
  /* check that we can have a cache while in SD session */
  sec_ui_alloc_app_buffer(cachelen, &cache);
#endif

  if( ( retval = start_disp_fullscreen( ) ) < 0 )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "sec_ui_start_disp_fullscreen returned  %d", retval);
  }
  else
  if( ( retval = show_image( ) ) < 0 )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "show_image( 1 ) returned  %d", retval);
    stop_disp( );
  }
  else
  if( ( retval = stop_disp( ) ) < 0 )
  {
	  qsee_log(QSEE_LOG_MSG_ERROR, "stop_disp( ) returned  %d", retval);
  }

#if APP_BUFF_TEST_ENABLE
  /* release the cache */
  sec_ui_free_app_buffer();
#endif

  return retval;
}

int run_touch_test( void )
{
  int retval = 0;

  for( int32_t i = 0; i < DOT_SIZE; i++ )
  {
    ( ( uint32 * ) red_dot   ) [ i ] = RED_PIXEL;
    ( ( uint32 * ) green_dot ) [ i ] = GREEN_PIXEL;
  }

  if( ( retval = start_disp_fullscreen( ) ) < 0 )
  {
    qsee_log(QSEE_LOG_MSG_ERROR, "sec_ui_start_disp_fullscreen returned  %d", retval);
  }
  else
  {
    secUiRegisterTouchCallback( &draw_dot );
  }

  return retval;
}
