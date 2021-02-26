#ifndef QDI_DEBUG_H
#define QDI_DEBUG_H

#ifdef QDI_OFFTARGET
#include <sys/types.h>
#endif

#ifdef FEATURE_DATA_LOG_QXDM
  #include "msg.h"
#endif

#include "ds_string.h"


#ifdef QDI_OFFTARGET
  #define SA_FAMILY(addr)         (addr).sa_family
  #define SA_DATA(addr)           (addr).sa_data
  #define SASTORAGE_FAMILY(addr)  (addr).ss_family
  #define SASTORAGE_DATA(addr)    (addr).__ss_padding
#else
  #define SA_FAMILY(addr)         (addr).sa_family
  #define SA_DATA(addr)           (addr).sa_data
  #define SASTORAGE_FAMILY(addr)  (addr).ss_family
  #define SASTORAGE_DATA(addr)    (addr).__data
#endif

/* define to enable function level trace logging */
#define QDI_TRACE

#if defined(FEATURE_DATA_LOG_OFFTARGET)
  #include "tf_log.h"
  #define QDI_LOG_LOW    TF_MSG_INFO
  #define QDI_LOG_MED    TF_MSG_MED
  #define QDI_LOG_HIGH   TF_MSG_HIGH
  #define QDI_LOG_ERROR  TF_MSG_ERROR
  #define QDI_LOG_FATAL  TF_MSG_ERROR

#elif defined(FEATURE_DATA_LOG_QXDM) && defined(FEATURE_DATA_LOG_ADB)

  #define QDI_MAX_DIAG_LOG_MSG_SIZE 512
  #include "msg.h"


  #define DSI_L2S_PNAME_SIZE   50

  extern char l2s_proc_name[DSI_L2S_PNAME_SIZE];
  extern int  l2s_pid;
  extern int ds_log_mask;

  #define QDI_LOG_TAG "QC-QDI-LIB"
  #undef LOG_TAG
  #define LOG_TAG QDI_LOG_TAG

  #define QDI_LOG(level, ...)                                        \
  do{                                                                \
      if(ds_log_mask & DS_LOG_MASK_QXDM)                             \
      {                                                              \
        char buf[QDI_MAX_DIAG_LOG_MSG_SIZE];                         \
        std_strlprintf(buf, QDI_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__); \
        if (!strncasecmp(buf,"L2S",3))                               \
          MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, level, "%s", buf);      \
        else                                                         \
          MSG_SPRINTF_3(MSG_SSID_LINUX_DATA, level, "%s[%d] %s",l2s_proc_name,l2s_pid,buf); \
      }                                                              \
    } while (0);

  #define DATA_LOGD(...)    \
     if(ds_log_mask & DS_LOG_MASK_ADB)\
     { \
       LOGD(__VA_ARGS__);\
     }

  #define DATA_LOGE(...)    \
     if(ds_log_mask & DS_LOG_MASK_ADB)\
     { \
       LOGE(__VA_ARGS__);\
     }

  #define DATA_LOGI(...)    \
     if(ds_log_mask & DS_LOG_MASK_ADB)\
     { \
       LOGI(__VA_ARGS__);\
     }

    #define QDI_LOG_LOW(...)    \
    {                           \
      QDI_LOG(MSG_LEGACY_LOW, __VA_ARGS__);    \
      DATA_LOGD(__VA_ARGS__);                  \
    }

    #define QDI_LOG_MED(...)    \
    {                           \
    QDI_LOG(MSG_LEGACY_MED, __VA_ARGS__);      \
    DATA_LOGI(__VA_ARGS__);                    \
    }

    #define QDI_LOG_HIGH(...)   \
    {                           \
      QDI_LOG(MSG_LEGACY_HIGH, __VA_ARGS__);   \
      DATA_LOGI(__VA_ARGS__);                  \
    }

    #define QDI_LOG_ERROR(...)   \
    {                            \
      QDI_LOG(MSG_LEGACY_ERROR, __VA_ARGS__);   \
      DATA_LOGE(__VA_ARGS__);                   \
    }

    #define QDI_LOG_FATAL(...)   \
    {                            \
      QDI_LOG(MSG_LEGACY_FATAL, __VA_ARGS__);   \
      DATA_LOGE(__VA_ARGS__);                   \
    }

#elif defined(FEATURE_DATA_LOG_QXDM)

#define QDI_MAX_DIAG_LOG_MSG_SIZE 512

#define QDI_QXDM_LOG(level, ...)                                 \
  do {                                                           \
    char buf[QDI_MAX_DIAG_LOG_MSG_SIZE];                         \
    std_strlprintf(buf, QDI_MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__); \
     if (!strncasecmp(buf,"L2S",3))  \
      MSG_SPRINTF_1(MSG_SSID_LINUX_DATA, level, "%s", buf); \
     else  \
      MSG_SPRINTF_3(MSG_SSID_LINUX_DATA, level, "%s[%d] %s",l2s_proc_name,l2s_pid,buf); \
  } while (0)

  #define QDI_LOG_LOW(...)    QDI_QXDM_LOG(MSG_LEGACY_LOW, __VA_ARGS__)
  #define QDI_LOG_MED(...)    QDI_QXDM_LOG(MSG_LEGACY_MED, __VA_ARGS__)
  #define QDI_LOG_HIGH(...)   QDI_QXDM_LOG(MSG_LEGACY_HIGH, __VA_ARGS__)
  #define QDI_LOG_ERROR(...)  QDI_QXDM_LOG(MSG_LEGACY_ERROR, __VA_ARGS__)
  #define QDI_LOG_FATAL(...)  QDI_QXDM_LOG(MSG_LEGACY_FATAL, __VA_ARGS__)

#elif defined(FEATURE_DATA_LOG_STDERR)

  #define QDI_LOG_LOW(...)    fprintf(stderr, "%s %d:", __FILE__, __LINE__);\
                              fprintf(stderr, __VA_ARGS__)
  #define QDI_LOG_MED(...)    fprintf(stderr, "%s %d:", __FILE__, __LINE__);\
                              fprintf(stderr, __VA_ARGS__)
  #define QDI_LOG_HIGH(...)   fprintf(stderr, "%s %d:", __FILE__, __LINE__);\
                              fprintf(stderr, __VA_ARGS__)
  #define QDI_LOG_ERROR(...)  fprintf(stderr, "%s %d:", __FILE__, __LINE__);\
                              fprintf(stderr, __VA_ARGS__)
  #define QDI_LOG_FATAL(...)  fprintf(stderr, "%s %d:", __FILE__, __LINE__);\
                              fprintf(stderr, __VA_ARGS__)

#elif defined(FEATURE_DATA_LOG_ADB)
  #define QDI_LOG_TAG "QC-QDI-LIB"
  #undef  LOG_TAG
  #define LOG_TAG QDI_LOG_TAG

  #define QDI_LOG_LOW(...)    LOGD(__VA_ARGS__)
  #define QDI_LOG_MED(...)    LOGI(__VA_ARGS__)
  #define QDI_LOG_HIGH(...)   LOGI(__VA_ARGS__)
  #define QDI_LOG_ERROR(...)  LOGE(__VA_ARGS__)
  #define QDI_LOG_FATAL(...)  LOGE(__VA_ARGS__)

#else

  #define QDI_LOG_LOW(...)
  #define QDI_LOG_MED(...)
  #define QDI_LOG_HIGH(...)
  #define QDI_LOG_ERROR(...)
  #define QDI_LOG_FATAL(...)

#endif

#ifdef QDI_TRACE
  #define QDI_LOG_TRACE_ENTRY            QDI_LOG_LOW("%s: enter", __FUNCTION__)
  #define QDI_LOG_TRACE_EXIT             QDI_LOG_LOW("%s: exit", __FUNCTION__)
  #define QDI_LOG_TRACE_RETURN(ret)      QDI_LOG_LOW("%s: exit %s=%d", __FUNCTION__, #ret, ret)
  #define QDI_LOG_TRACE_RETURN_HEX(ret)  QDI_LOG_LOW("%s: exit %s=0x%x", __FUNCTION__, #ret, ret)
  #define QDI_LOG_TRACE_RETURN_PTR(ret)  QDI_LOG_LOW("%s: exit %s=%p", __FUNCTION__, #ret, ret)
#else
  #define QDI_LOG_TRACE_ENTRY
  #define QDI_LOG_TRACE_EXIT
  #define QDI_LOG_TRACE_RETURN(ret)
  #define QDI_LOG_TRACE_RETURN_HEX(ret)
  #define QDI_LOG_TRACE_RETURN_PTR(ret)
#endif /*QDI_TRACE*/

/* ---------------------------------------------------------------------------
-----------------------L2S Entry and Exit macros -----------------------------
------------------------------------------------------------------------------*/
#define QDI_L2S_MSG_MAX_SIZE 256
#define QDI_L2S_PNAME_SIZE   50

extern char l2s_proc_name[QDI_L2S_PNAME_SIZE];
extern int  l2s_pid;

#define QDI_L2S_SETUP()                                                      \
  do{                                                                        \
    l2s_pid = -1;                                                            \
    memset(l2s_proc_name,0, QDI_L2S_PNAME_SIZE);                             \
    l2s_pid = getpid();                                                      \
    ds_get_proc_name(l2s_pid,l2s_proc_name,QDI_L2S_PNAME_SIZE);              \
    QDI_LOG_MED("L2S[pname:%s pid:%d tid:%d]",                               \
                 l2s_proc_name,l2s_pid, gettid() );                          \
  }while (0);

#define QDI_L2S_ENTRY_PARAMS(fmt,...)                                        \
  QDI_LOG_MED("L2S[pname:%s pid:%d tid:%d type:%s fname:%s params:" fmt "]", \
                 l2s_proc_name,                                              \
                 l2s_pid,                                                    \
                 gettid(),                                                   \
                 "FE",                                                       \
                 __func__,                                                   \
                 ##__VA_ARGS__);

#define QDI_L2S_ENTRY() QDI_L2S_ENTRY_PARAMS("iface_id:-1");

#define QDI_L2S_EXIT_WITH_STATUS(fmt,...)                                    \
  QDI_LOG_MED("L2S[pname:%s pid:%d tid:%d type:%s fname:%s status:%s params:" fmt "]",\
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "FX",                                                        \
                __func__,                                                    \
                (ret<QDI_SUCCESS)?"ERROR":"SUCCESS",                         \
                ##__VA_ARGS__);

#define QDI_L2S_EXIT()                                                       \
  QDI_LOG_MED("L2S[pname:%s pid:%d tid:%d type:%s fname:%s]",                \
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "FX",                                                        \
                __func__);

#define QDI_L2S_MSG(iface_id,msg)                                            \
  QDI_LOG_MED("L2S[pname:%s pid:%d tid:%d type:%s fname:%s iface_id:%d msg:\"%s\"]",  \
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "ME",                                                        \
                __func__,                                                    \
                iface_id,                                                    \
                msg);

#define QDI_L2S_MSG_SEQ(iface_id, msg)                                       \
  QDI_LOG_MED("L2S[pname:%s pid:%d tid:%d type:%s fname:%s iface_id:%d msg:\"%s\"]",  \
                l2s_proc_name,                                               \
                l2s_pid,                                                     \
                gettid(),                                                    \
                "MS",                                                        \
                __func__,                                                    \
                iface_id,                                                    \
                msg);

#define QDI_L2S_FORMAT_MSG(iface_id,msg,...)                                 \
  do{                                                                        \
    char l2s_msg[QDI_L2S_MSG_MAX_SIZE] = "\0" ;                              \
    memset(l2s_msg,0,QDI_L2S_MSG_MAX_SIZE);                                  \
    snprintf(l2s_msg, QDI_L2S_MSG_MAX_SIZE,msg, __VA_ARGS__);                \
    QDI_L2S_MSG(iface_id,l2s_msg);                                           \
  }while (0);

#endif /*QDI_DEBUG_H*/
