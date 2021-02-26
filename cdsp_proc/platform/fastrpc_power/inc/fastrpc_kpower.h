#ifndef FASTRPC_KPOWER_H
#define FASTRPC_KPOWER_H

#include "npa.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    void* startAddress;
    uint32 size;
    UT_hash_handle    hh; /*Makes this hashable*/
}line_locked_address_node;

struct adsp_power {
    uint32 client;           // client id for MMPM
    uint32 client_dma;       // client id for MMPM DMA
    uint32 client_hcp;       // client id for MMPM HCP
    uint32 adsp_dcvs_client; // client id for adsp dcvs
    npa_client_handle lat_client;
    // Hashtable of line locked addresses (non-throttled)
    line_locked_address_node* line_locked_addresses;
};

// Definitions to support deprecated APIs
/**
 * Payload for HAP_power_abs and HAP_power_percentage. Set the boolean
 * flags appropriately to indicate if a particular value needs to be set / ignored.

 * @param set_clock - Set to TRUE to requst MIPS
 * @param clock - Absolute / percentage of target's maximum clock speed
 * @param set_bus_bw - Set to TRUE to request bus_bw
 * @param bus_bw - Absolute (MHz) /percentage of target's maximum bus speed
 * @param set_latency - Set to TRUE to set latency
 * @param latency - maximum hardware wakeup latency in microseconds.  The
 *                  higher the value the deeper state of sleep
 *                  that can be entered but the longer it may
 *                  take to awaken.
 */
typedef struct {
    boolean set_clock;
    int clock;
    boolean set_bus_bw;
    int bus_bw;
    boolean set_latency;
    int latency;
}HAP_power_abs_payload, HAP_power_percentage_payload;

/**
 * Identifies the HAP power request type
 * @param HAP_power_abs - Requests performance level by absolute values.
 *                    HAP_power_payload is set to HAP_power_abs_payload
 * @param HAP_power_percentage - Requests performance level by percentage values.
 *                    HAP_power_payload is set to HAP_power_percentage_payload
 */
typedef enum {
    HAP_power_set_abs = 100,
    HAP_power_set_percentage,
}HAP_power_deprecated_request_type;

/**
 * Data type to change power values on the ADSP
 * @param type - Identifies the request type.
 * @param absolute - Requests performance level by absolute values.
 * @param percentage - Requests performance level by percentage values.
*/
typedef struct {
    HAP_power_deprecated_request_type type;
    union{
        HAP_power_abs_payload absolute;
        HAP_power_percentage_payload percentage;
    };
}HAP_power_deprecated_request_t;


int power_ctor(void* ctx, void* data);
void power_dtor(void* data);
int VAPSS_power_ctor(void* ctx, void* data);
void VAPSS_power_dtor(void* data);
int HVX_power_ctor(void* ctx, void* data);
void HVX_power_dtor(void* data);
int fastrpc_kpower_set(struct adsp_power* me, void* request);
int fastrpc_kpower_free_linelocked_memory(struct adsp_power* me);

#ifdef __cplusplus
}
#endif
#endif //FASTRPC_KPOWER_H

