#ifndef _PLATFORM_QDI_H_
#define _PLATFORM_QDI_H_

#define PLATFORM_QDI_DRIVER_NAME "PLATFORM_QDI_DRIVER"

#define PLATFORM_HAP_POWER_REQUEST                  ( 1    + QDI_PRIVATE )
#define PLATFORM_HAP_POWER_REQUEST_ABS              ( 2    + QDI_PRIVATE )
#define PLATFORM_HAP_POWER_GET_MAX_SPEED            ( 3    + QDI_PRIVATE )
#define PLATFORM_HAP_GET_CHIP_FAMILY_ID             ( 4    + QDI_PRIVATE )
#define PLATFORM_HAP_DEBUG                          ( 5    + QDI_PRIVATE )
#define PLATFORM_SECHWIO_GETSERIALNUMBER            ( 6    + QDI_PRIVATE )
#define PLATFORM_SECHWIO_GETOEMID                   ( 7    + QDI_PRIVATE )
#define PLATFORM_SECHWIO_GETCHIPID                  ( 8    + QDI_PRIVATE )
#define PLATFORM_NPA_QUERY_BY_NAME                  ( 9    + QDI_PRIVATE )
#define PLATFORM_DALSYS_GETPROPERTYHANDLE           ( 10   + QDI_PRIVATE )
#define PLATFORM_DALSYS_GETPROPERTYSIZE             ( 11   + QDI_PRIVATE )
#define PLATFORM_DALSYS_GETPROPERTYVALUE            ( 12   + QDI_PRIVATE )
#define PLATFORM_HAP_DIAG                           ( 13   + QDI_PRIVATE )
#define PLATFORM_HAP_GET_PROCESS_LIST_LEN           ( 16   + QDI_PRIVATE )
#define PLATFORM_HAP_GET_PROCESS_LIST               ( 17   + QDI_PRIVATE )
#define PLATFORM_DYNCONFIG_GETCODETCGSSIZE          ( 18   + QDI_PRIVATE )
#define PLATFORM_DYNCONFIG_GETCODETCGS              ( 19   + QDI_PRIVATE )
#define PLATFORM_DYNCONFIG_GETTESTTCGSSIZE          ( 20   + QDI_PRIVATE )
#define PLATFORM_DYNCONFIG_GETTESTTCGS              ( 21   + QDI_PRIVATE )
#define PLATFORM_IS_TEST_ENABLED                    ( 22   + QDI_PRIVATE )
#define PLATFORM_HAP_POWER_HVX_REQUEST              ( 24   + QDI_PRIVATE )
#define PLATFORM_HAP_POWER_HVX_RELEASE              ( 25   + QDI_PRIVATE )
#define PLATFORM_HAP_POWER_SET                      ( 26   + QDI_PRIVATE )
#define PLATFORM_HAP_POWER_GET                      ( 27   + QDI_PRIVATE )
#define PLATFORM_HAP_RFS_WAIT                       ( 28   + QDI_PRIVATE )
#define PLATFORM_HAP_CLEAR_REGISTERS                ( 29   + QDI_PRIVATE )
#define PLATFORM_GET_PROCESS_ATTRS                  ( 30   + QDI_PRIVATE )
#define PLATFORM_GET_DSP_DOMAIN                     ( 31   + QDI_PRIVATE )
#define PLATFORM_HAP_TIMER_SLEEP                    ( 32   + QDI_PRIVATE )
#define PLATFORM_GET_PROCESS_NAME                   ( 33   + QDI_PRIVATE )
#define PLATFORM_POST_CRASH_SIGNATURE               ( 34   + QDI_PRIVATE )

/* These QDI methods are deprecated. So dont use these values in any new calls */
#define PLATFORM_DEPRECATED_0                       ( 101  + QDI_PRIVATE )
#define PLATFORM_DEPRECATED_1                       ( 14   + QDI_PRIVATE )
#define PLATFORM_DEPRECATED_2                       ( 15   + QDI_PRIVATE )
#define PLATFORM_DEPRECATED_3                       ( 23   + QDI_PRIVATE )

/**********************
 * Funtion prototypes *
 **********************/
int HAP_power_request(int clock, int bus, int latency);
int HAP_power_request_abs(int clock, int bus, int latency);
int HAP_power_get_max_speed(int* clock_max, int* bus_max);
uint32 HAP_DalChipInfo_ChipFamily(void);
void HAP_diag(int level, const char *filename, int line, int thread_id, int asid,
              const char *msg);
uint32 SecHWIO_GetSerialNumber(void);
uint16 SecHWIO_GetOEMID(void);
uint16 SecHWIO_GetChipID(void);
npa_query_status npa_query_by_name( const char *resource_name, uint32 query_id,
    npa_query_type  *query_result );
DALResult DALSYS_GetDALPropertyHandleStr(const char *pszDevName,
                                         DALSYSPropertyHandle hDALProps);
DALResult DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps,
    const char *pszName, uint32 dwId, DALSYSPropertyVar *pDALPropVar);
int HAP_get_process_list(uint32* num_processes,HAP_process** processes);
int* getOEMCodeTCGs(void);
int* getOEMTestTCGs(void);
int is_test_enabled(void);
boolean HAP_log_data_packet(unsigned short log_code_type, unsigned int dataLen,
                    byte* data);
int HAP_HVX_power_request(void);
int HAP_HVX_power_release(void);
int HVX_power_request(void);
int HVX_power_release(void);
int HAP_power_set(void* context, HAP_power_request_t* request);
int HAP_power_get(void* context, HAP_power_response_t* response);
int HAP_get_dsp_domain(void);

int HAP_rfs_wait(void);
int HAP_clear_registers(unsigned int num_registers, HAP_register_t* registers);
int is_process_debug_enabled(const char *sigfile, int siglen);
int HAP_timer_sleep(unsigned long long sleep_duration);

/******************************************************************
 *  Funtion prototypes. These are wrappers, and will only
    be implemented in the stub.*
 ******************************************************************/
int adsp_power_boost_on(void);
void adsp_power_boost_off(void);
void oemconfig_dtor(void);
extern int fastrpc_uprocess_group_wait(void);

#endif /*_PLATFORM_QDI_H_*/
