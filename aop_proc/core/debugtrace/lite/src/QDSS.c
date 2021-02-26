#include "qdss_lite.h"
#include "qdss_swevent.h"
#include "tracer_event_ids.h"
#include "rex.h"

boolean qdss_enable_aop_swevents = 1;//Turn this flag on to enable SW trace from AOP

boolean qdss_tpdm_sw_trace = 1; //Turn this flag off to use STM for trace when possible

int QDSSETBSwitchMode(int etb_id,int mode);

#include "STMTrace.c"    
#include "TPDMTrace.c"    
#include "STMCfg.c"
#include "TPDA.c"
#include "TMC.c"
#include "CSR.c"
#include "Replicator.c"

enum {
   QDSS_UNINITIALIZED=0,
   QDSS_BASE_INIT,
   QDSS_TRACE_ENABLED,
};

struct qdss_context {
   uint8 init_level;
   uint8 num_ports_in_use;
   STMPortAddrType port;
   rex_crit_sect_type crit_sect;
   int saved_swao_etb_mode;
   int saved_cx_etb_mode;
   boolean saved_stm_trace_flag;
};

static struct qdss_context qdss; //zero initialized



void QDSSTest(void) 
{
   int i;

   for (i=0; i < 5; i++) {
      QDSS_SWEVENT(QDSS_TEST_EVENT_0);
      QDSS_SWEVENT(QDSS_TEST_EVENT_1,10);
      QDSS_SWEVENT(QDSS_TEST_EVENT_2,10,20);
      QDSS_SWEVENT(QDSS_TEST_EVENT_3,10,20,30);
      QDSS_SWEVENT(QDSS_TEST_EVENT_4,10,20,30,40);
      QDSS_SWEVENT(QDSS_TEST_EVENT_1,10);
      QDSS_SWEVENT(QDSS_TEST_EVENT_2,20,20);
      QDSS_SWEVENT(QDSS_TEST_EVENT_3,30,30,30);
      QDSS_SWEVENT(QDSS_TEST_EVENT_4,40,40,40,40);
   }
}

#define QDSS_TS_FREQ_DEFAULT 19200000    //19.2 MHz

int QDSSSetTimestampFreq(uint32 ts_counter_freq)
{
   if (qdss.init_level < QDSS_BASE_INIT) {
      return QDSS_ERROR;
   }
   STMSetTimestampFreq(ts_counter_freq);
   TPDASetTimestampFreq(ts_counter_freq);

   return QDSS_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
void QDSSInit(void)
{

   if (QDSS_UNINITIALIZED != qdss.init_level ) {
      return;
   }

   if (QDSS_SUCCESS != STMTrace_AllocateWindow()) {
      return;
   }

   qdss.init_level=QDSS_BASE_INIT;


   if (QDSS_SUCCESS != QDSSAllocPort(&qdss.port)) {
      return;
   }
   rex_init_crit_sect(&qdss.crit_sect);
   STMCfgInit();
   CSRInit();
   TPDAInit();
   ReplicatorInit();
}


/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSAllocPort(STMPortAddrType *pPort)
{
   int retval;

   if (qdss.init_level < QDSS_BASE_INIT) {
      return QDSS_ERROR;
   }
   
   retval = STMTrace_NewPort(pPort);
   if (QDSS_SUCCESS == retval) {
      qdss.num_ports_in_use++;
   }
   return retval;
}

/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSFreePort(STMPortAddrType port)
{
   int retval;

   if (qdss.init_level < QDSS_BASE_INIT) {
      return QDSS_ERROR;
   }

   retval=STMTrace_FreePort(port);

   if (QDSS_SUCCESS == retval) {
      qdss.num_ports_in_use--;
   }

   return retval;
}


static __inline uint32 endian_swap32(uint32 x)
{
    return (x>>24) | 
       ((x<<8) & 0x00FF0000) |
       ((x>>8) & 0x0000FF00) |
       (x<<24);
}


static __inline int ost_start_packet(STMPortAddrType port,uint32 ost_header)
{ 
   STMTrace_Data32(port,                  
                   TRACE_DATA_MARKED,
                   endian_swap32(ost_header));
   return QDSS_SUCCESS;
}

static __inline int ost_end_packet(STMPortAddrType port)
{ 

   STMTrace_Flag(port,                  
                 TRACE_DATA_TIMESTAMPED);
     
   return QDSS_SUCCESS;
}


static __inline int ost_write_data(STMPortAddrType port, byte *pCurrent, int nRemaining)
{
   //Make pCurrent 32 bit aligned first
   while (( (uint32)pCurrent & 0x3) && (nRemaining > 0 )) {                                      
      STMTrace_Data8(port,                              
                     TRACE_DATA_NONETYPE,
                     *pCurrent);
      pCurrent++;
      nRemaining--;
   }

   
#define OST_WRITE_DATA_WORDS(_nBytes,_nBits)                            \
   while (nRemaining >= _nBytes) {                                      \
      STMTrace_Data##_nBits(port,                                       \
                            TRACE_DATA_NONETYPE,                        \
                            (*(uint##_nBits *)pCurrent)); \
      pCurrent += _nBytes;                                              \
      nRemaining -=_nBytes;                                             \
   }

   OST_WRITE_DATA_WORDS(4,32);
   OST_WRITE_DATA_WORDS(2,16);
   OST_WRITE_DATA_WORDS(1,8);

   return QDSS_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/

#define OST_TOKEN_QDSS_SWEVENT      0x80
#define PACK_EVENTID(_id) ((((uint32)(_id)) << 8 ) | OST_TOKEN_QDSS_SWEVENT)

int QDSSEvent(STMPortAddrType port, int event_id)
{
   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return QDSS_ERROR;
   }


   STMTrace_Data32(port,                                     
                   (TRACE_DATA_MARKED | TRACE_DATA_TIMESTAMPED),
                   PACK_EVENTID(event_id));

   return QDSS_SUCCESS;
}



static int WriteOSTPacket(STMPortAddrType port, uint32 ost_header, byte *packet, int nLen)
{
   if (nLen < 1) {
      return -1;
   }

   ost_start_packet(port,ost_header);
   ost_write_data(port,packet,nLen);
   ost_end_packet(port);
   return QDSS_SUCCESS;
}

#define TPDM_OST_PACKET_SIZE_MAX 4096
#define TPDM_OST_FRAME_START  0x57A67F6A
#define TPDM_OST_FRAME_END    0xEE55DDFF

static int TPDM_WriteOSTPacket(uint32 ost_header, byte *pCurrent, int nRemaining)
{
   if (!HAL_tpdm_IsAOPTraceEnabled()) {
      return 0;
   }
   if (((uint32)pCurrent & 0x3) || (nRemaining & 0x3)) {
      //not 32 bit aligned.
      return -1;
   }
   HAL_tpdm_AccessUnLock();
   nRemaining=(nRemaining < 0) ? 0: nRemaining;
   nRemaining=(nRemaining > TPDM_OST_PACKET_SIZE_MAX) ? 4096: nRemaining;

   TPDMTrace_Data32(TPDM_OST_FRAME_START);                                  

   TPDMTrace_Data32(endian_swap32(ost_header));                                  
   TPDMTrace_Data32((uint32)nRemaining);                                  

   while (nRemaining >= 4) {   
      TPDMTrace_Data32(*(uint32*)pCurrent);                                  
      pCurrent += 4;
      nRemaining -=4;
   }

   TPDMTrace_Data32(TPDM_OST_FRAME_END);                                  
   //Could lock TPDM access back, but avoiding this for couple reasons.
   //We don't have exclusive access to TPDM, So we don't want to
   //cause a potential config failure. Also results in performannce gain
   return QDSS_SUCCESS;
}


/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSWriteOSTPacket(STMPortAddrType port, uint32 ost_header, byte *packet, int nLen)
{
   int nErr = 0;

   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return QDSS_ERROR;
   }

   rex_enter_crit_sect(&qdss.crit_sect);
   if (qdss_tpdm_sw_trace) {
      nErr=TPDM_WriteOSTPacket(ost_header,packet,nLen);
   } 
   else {
      nErr=WriteOSTPacket(port,ost_header,packet,nLen);
   }
   rex_leave_crit_sect(&qdss.crit_sect);

   return nErr;
}

#define QDSS_SWEVT_ENTITY    2
#define QDSS_SWEVT_PROTOCOL  1  




/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSTraceEnable(void) 
{
   int nErr;

   if (qdss.init_level < QDSS_BASE_INIT) {
      return QDSS_ERROR;
   }

   CSRTraceEnable();
   TPDASetTimestampFreq(QDSS_TS_FREQ_DEFAULT);
   
   qdss.init_level = QDSS_TRACE_ENABLED;
   return QDSS_SUCCESS;
}


/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSTraceDisable(void) 
{
   if (qdss.init_level < QDSS_BASE_INIT) {
      return QDSS_ERROR;
   }

   qdss.init_level = QDSS_BASE_INIT;

   CSRTraceDisable();

   return QDSS_SUCCESS;
}

#include <stdarg.h>


int QDSSLogEventWithArgs(int id, int n, ...) 
{
   uint32 message[11];
   va_list vl;
   uint32 arg;
   int i = 0;
   int nLen = 0;

   if (!qdss_enable_aop_swevents) {
      return QDSS_SUCCESS;
   }

   n=(n>10) ? 10: n; //maximum 10 arugments

   message[0] = id;
   va_start(vl,n);
   for (i=1;i<(n+1);i++) {
      arg=va_arg(vl,uint32);
      message[i]=arg;
   }
  va_end(vl);

  nLen= (n+1) * sizeof(uint32);

  return QDSSWriteOSTPacket(qdss.port,
                            QDSSOSTHeader(QDSS_SWEVT_ENTITY,QDSS_SWEVT_PROTOCOL),
                            (byte *)message,
                            nLen);
}

void QDSSLogEvent(int id)
{
   if (!qdss_enable_aop_swevents) {
      return;
   }

   if (qdss_tpdm_sw_trace) {
      QDSSLogEventWithArgs(id,0);
   }
   else {
      QDSSEvent(qdss.port,id);
   }
}


int QDSSETBSwitchMode(int etb_id,int mode)
{
   int saved_mode = 0;
   if (qdss.init_level < QDSS_BASE_INIT) {
      return QDSS_ERROR;
   }

   if (QDSS_SWAO_ETB==etb_id) {
      saved_mode = ETBSwitchMode(QDSS_SWAO_TMC_BASE_PHYS,mode);
   }
   else if (QDSS_CX_ETB==etb_id) {
      saved_mode = ETBSwitchMode(QDSS_ETFETB_BASE_PHYS,mode);
   }

   return saved_mode;
}


/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSCXTraceSuspend(void) 
{
   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return QDSS_SUCCESS;
   }

   //Switch to using AO TPDM. Cannot use STM any more.
   if (!qdss_tpdm_sw_trace) {
      qdss.saved_stm_trace_flag = 1;
      qdss_tpdm_sw_trace = 1;
   }

   if (!ReplicatorIsCXArmEnabled()) {
      return QDSS_SUCCESS;
   }

   qdss.saved_swao_etb_mode = QDSSETBSwitchMode(QDSS_SWAO_ETB,QDSS_TMC_MODE_CBUF);
   if (QDSS_TMC_MODE_HWFIFO ==  qdss.saved_swao_etb_mode) {
      TPDAFreqReq();
   }

   return QDSS_SUCCESS;
}

/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSCXTraceResume(void) 
{
   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return QDSS_SUCCESS;
   }
   
   if (QDSS_TMC_MODE_HWFIFO ==  qdss.saved_swao_etb_mode) {
      QDSSETBSwitchMode(QDSS_SWAO_ETB,QDSS_TMC_MODE_HWFIFO);
      TPDAFreqReq();
   }
   
   if (qdss.saved_stm_trace_flag) {
      //revert to using STM
      qdss_tpdm_sw_trace = 0;
      qdss.saved_stm_trace_flag = 0;
   }

   //set it back to default
   qdss.saved_swao_etb_mode = 0;
   qdss.saved_cx_etb_mode = 0;
   //Resync timestamp
   QDSSTimestampResync();

   return QDSS_SUCCESS;

}

/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
boolean QDSSIsDDRTraceEnabled(void) 
{
  if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return FALSE;
   }

  if ((QDSS_TMC_MODE_HWFIFO ==  qdss.saved_cx_etb_mode) ||
      Is_CX_ETB_MODE_HWFIFO() ) {
     return TRUE;
  }
  return FALSE;
}



/*============================================================================
 * @brief QDSS Lite API function. See qdss_lite.h for details
 *==========================================================================*/
int QDSSTimestampResync(void) 
{
   if (qdss.init_level < QDSS_TRACE_ENABLED) {
      return QDSS_SUCCESS;
   }
   //HW team suggested reading timestamp counter value and reloading
   //the same value again to force a resync to workaound HW issues
   HAL_qdss_csr_AccessUnLock();
   HAL_qdss_csr_LoadTimestampCounter(HAL_qdss_csr_GetTimestampCounter());
   HAL_qdss_csr_AccessLock();
   return QDSS_SUCCESS;
}
