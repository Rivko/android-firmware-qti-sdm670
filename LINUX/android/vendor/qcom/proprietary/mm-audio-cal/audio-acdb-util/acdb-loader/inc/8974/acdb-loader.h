/* Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ACDB_LOADER_H

#include <stdbool.h>

enum {
	LISTEN_MODE_NO_TOPOLOGY = 0,
	LISTEN_MODE_TOPOLOGY,
	LISTEN_MAX_MODES
};

enum {
	LISTEN_HW_TYPE_CPE = 0,
	LISTEN_HW_TYPE_APE,
	LISTEN_HW_MAX_TYPE
};

enum {
	CAL_MODE_SEND		= 0x1,
	CAL_MODE_PERSIST	= 0x2,
	CAL_MODE_RTAC		= 0x4
};

int acdb_loader_init_v2(char *snd_card_name, char *cvd_version, int metaInfoKey);
int acdb_loader_init_v3(char *snd_card_name, char *cvd_version, struct listnode *metaKey_list);
int acdb_loader_init_ACDB(void);
int acdb_loader_get_default_app_type(void);
int acdb_loader_send_common_custom_topology(void);
void acdb_loader_deallocate_ACDB(void);
void acdb_loader_send_voice_cal_v2(int rxacdb_id, int txacdb_id, int feature_set);
void acdb_loader_send_voice_cal(int rxacdb_id, int txacdb_id);
int acdb_loader_reload_vocvoltable(int feature_set);
void acdb_loader_send_audio_cal(int acdb_id, int capability);
void acdb_loader_send_audio_cal_v2(int acdb_id, int capability, int app_id, int sample_rate);
void acdb_loader_send_audio_cal_v3(int acdb_id, int capability, int app_id, int sample_rate, int use_case);
void acdb_loader_send_listen_device_cal(int acdb_id, int type, int app_id, int sample_rate);
int acdb_loader_send_listen_lsm_cal(int acdb_id, int app_id, int mode, int type);
int acdb_loader_send_anc_cal(int acdb_id);
void send_tabla_anc_data(void);
int acdb_loader_get_aud_volume_steps(void);
int acdb_loader_send_gain_dep_cal(int acdb_id, int app_id,
	int capability, int mode, int vol_index);
int acdb_loader_get_remote_acdb_id(unsigned int native_acdb_id);
int acdb_loader_get_ecrx_device(int acdb_id);
int acdb_loader_get_calibration(char *attr, int size, void *data);
int acdb_loader_set_audio_cal_v2(void *caldata, void* data,
	unsigned int datalen);
int acdb_loader_get_audio_cal_v2(void *caldata, void* data,
	unsigned int *datalen);
int send_meta_info(int metaInfoKey, int buf_idx);
int send_meta_info_list(struct listnode *key_list);
int acdb_loader_set_codec_data(void *data, char *attr);
bool acdb_loader_is_initialized(void);

/*
 * acdb_loader_get_subgraph_list_size - Get size of subgraph list of a graph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @cal_size:    size of the subgraph list data, to be returned to caller
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_list_size(uint32_t u_id, uint32_t d_id,
                                           uint32_t *cal_size);

/*
 * acdb_loader_get_subgraph_list - Get subgraph list of a graph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @cal_buff:    pointer to the buffer to hold the subgraph list data
 * @cal_size:    size of the subgraph list data
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_list(uint32_t u_id, uint32_t d_id,
                                      uint8_t *cal_buff, size_t cal_size);

/*
 * acdb_loader_get_subgraph_mod_info_list_size - Get size for module list of
 *                                               a subgraph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @SG_ID:       subgraph ID for which the module list is queried for
 * @cal_size:    size of the module list data, to be returned to caller
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_mod_info_list_size(uint32_t u_id, uint32_t d_id,
                                                    uint32_t SG_ID,
                                                    uint32_t *cal_size);

/*
 * acdb_loader_get_subgraph_mod_info_list - Get module list of a subgraph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @SG_ID:       subgraph ID for which the module list is queried for
 * @cal_buff:    pointer to the buffer to hold the module list data
 * @cal_size:    size of the module list data
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_mod_info_list(uint32_t u_id, uint32_t d_id,
                                               uint32_t SG_ID,
                                               uint8_t *cal_buff, size_t cal_size);

/*
 * acdb_loader_get_subgraph_mod_conn_size - Get size of module connection data
 *                                          of a subgraph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @SG_ID:       subgraph ID for which the module connections are queried for
 * @cal_size:    size of the module connection data, to be returned to caller
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_mod_conn_size(uint32_t u_id, uint32_t d_id,
                                               uint32_t SG_ID,
                                               uint32_t *cal_size);

/*
 * acdb_loader_get_subgraph_mod_conn - Get module connection data of a subgraph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @SG_ID:       subgraph ID for which the module connections are queried for
 * @cal_buff:    pointer to the buffer to hold the module connection data
 * @cal_size:    size of the module connection data
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_mod_conn(uint32_t u_id, uint32_t d_id,
                                          uint32_t SG_ID,
                                          uint8_t *cal_buff, size_t cal_size);

/*
 * acdb_loader_get_subgraph_conn_size - Get size of connections between subgraphs
 *                                      of a graph
 *
 * @u_id:         unique ID of the graph
 * @d_id:         ACDB device ID of the input device used for the graph
 * @src_sg_id:    source subgraph ID for which connections is queried for
 * @dst_sg_id:    destination subgraph ID for which connections is queried for
 * @cal_size:     size of the connection data, to be returned to caller
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_conn_size(uint32_t u_id, uint32_t d_id,
                                           uint32_t src_sg_id, uint32_t dst_sg_id,
                                           uint32_t *cal_size);

/*
 * acdb_loader_get_subgraph_conn - Get connections between subgraphs of a graph
 *
 * @u_id:         unique ID of the graph
 * @d_id:         ACDB device ID of the input device used for the graph
 * @src_sg_id:    source subgraph ID for which connections is queried for
 * @dst_sg_id:    destination subgraph ID for which connections is queried for
 * @cal_buff:     pointer to the buffer to hold the subgraph connection data
 * @cal_size:     size of the connection data
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_conn(uint32_t u_id, uint32_t d_id,
                                      uint32_t src_sg_id, uint32_t dst_sg_id,
                                      uint8_t *cal_buff, size_t cal_size);

/*
 * acdb_loader_get_subgraph_mod_cal_size - Get size of calibration data of modules
 *                                         of a subgraph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @SG_ID:       subgraph ID for which module calibration is queried for
 * @cal_size:    size of the calibration data, to be returned to caller
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_mod_cal_size(uint32_t u_id, uint32_t d_id,
                                              uint32_t SG_ID, uint32_t *cal_size);

/*
 * acdb_loader_get_subgraph_mod_cal - Get calibration data of modules
 *                                    of a subgraph
 *
 * @u_id:        unique ID of the graph
 * @d_id:        ACDB device ID of the input device used for the graph
 * @SG_ID:       subgraph ID for which module calibration is queried for
 * @cal_buff:    pointer to the buffer to hold the subgraph module cal data
 * @cal_size:    size of the calibration data
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_subgraph_mod_cal(uint32_t u_id, uint32_t d_id,
                                         uint32_t SG_ID,
                                         uint8_t *cal_buff, size_t cal_size);

/*
 * acdb_loader_get_boot_time_mod_list_size - Get size of list of modules, that
 *                                           are loaded at device boot time
 *
 * @cal_size:    size of the module list data, to be returned to caller
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_boot_time_mod_list_size(uint32_t *cal_size);

/*
 * acdb_loader_get_boot_time_mod_list - Get list of modules, that are
 *                                      loaded at device boot time
 *
 * @cal_buff:    pointer to the buffer to hold the module list data
 * @cal_size:    size of the module list data
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_boot_time_mod_list(uint8_t *cal_buff, size_t cal_size);

/*
 * acdb_loader_get_mod_loading_meta_info_size - Get size of loading meta info, for
 *                                              modules that are loaded at boot time
 *                                              or dynamically at usecase boundary
 *
 * @mod_type:     type of module (boot time, dunamic, static etc.), for which
 *                meta info is queried for
 * @mod_count:    number of modules
 * @mod_list:     list of modules for which meta info is queried for
 * @cal_size:     size of loading meta info data, to be returned to caller
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_mod_loading_meta_info_size(uint32_t mod_type,
                                                   uint32_t mod_count,
                                                   uint8_t *mod_list,
                                                   uint32_t *cal_size);

/*
 * acdb_loader_get_mod_loading_meta_info - Get loading meta info, for modules that
 *                                         are loaded at boot time or dynamically
 *                                         at usecase boundary
 *
 * @mod_type:     type of module (boot time, dunamic, static etc.), for which
 *                meta info is queried for
 * @mod_count:    number of modules
 * @mod_list:     list of modules for which meta info is queried for
 * @cal_buff:     pointer to the buffer to hold the meta info data
 * @cal_size:     size of loading meta info data
 *
 * Returns 0 on success or error code otherwise
 */
int32_t acdb_loader_get_mod_loading_meta_info(uint32_t mod_type,
                                              uint32_t mod_count,
                                              uint8_t *mod_list,
                                              uint8_t *cal_buff,
                                              size_t cal_size);

#endif /* ACDB_LOADER_H */
