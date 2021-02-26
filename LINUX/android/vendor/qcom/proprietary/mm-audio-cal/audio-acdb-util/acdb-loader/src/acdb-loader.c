/*
 *
 * This library contains the API to load the audio calibration
 * data from database and push to the DSP
 *
 * Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <cutils/list.h>
#include <linux/msm_ion.h>
#include <linux/msm_audio.h>
#include <linux/msm_audio_calibration.h>
#include <linux/mfd/wcd9xxx/wcd9320_registers.h>

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t
#define CODEC_PACKED_REG_SIZE sizeof(u32)

#include <linux/mfd/msm-adie-codec.h>
#include <sys/mman.h>

#include <cutils/properties.h>
/* definitions for Android logging */
#include <utils/Log.h>
#include "common_log.h"

#include "acdb.h"
#include "acph.h"
#include "acdb-rtac.h"
#include "adie-rtac.h"
#ifdef _ANDROID_
#include "acdb-fts.h"
#include "acdb-mcs.h"
#endif /* _ANDROID_ */
#include "acdb-loader.h"
#include "acdb-anc-general.h"
#include "anc_map_api.h"
#include "acdb-id-mapper.h"
#include "vbat_map_api.h"
#undef LOG_NDDEBUG
#undef LOG_TAG
#define LOG_NDDEBUG 0
#define LOG_TAG "ACDB-LOADER"

#define INVALID_DATA	-1

#define MAX_INDEPENDANT_ACDB_FILES	1

#define APR_DOMAIN_ADSP     4
#define ADSP_ASM_SERVICE    0x07
#define ADSP_ADM_SERVICE    0x08
#define MAX_ACTIVE_COPP     8

#define TEMP_CAL_BUFSZ 1024 /* 1K should be plenty */
#define MAX_ACDB_FILES		20

/* Number of ANC device configurations */
#define NUM_OF_ANC_TUNING_CONFIG 2
#define NUM_OF_ANC_RX_CONFIG 6
#define NUM_OF_AANC_RX_CONFIG 1

#define EC_REF_RX_DEVS (sizeof(uint32_t) * 20)

#define round(val) ((val > 0) ? (val + 0.5) : (val - 0.5))

#ifdef AUDIO_USE_SYSTEM_HEAP_ID
#define HEAP_MASK	ION_HEAP(ION_SYSTEM_HEAP_ID)
#else
#define HEAP_MASK	ION_HEAP(ION_AUDIO_HEAP_ID)
#endif

#define MIN_BUFF_SIZE 4096 /* Incremental memory to accomodate header size for tables */

#define ACDB_SIMPLE_DEVICE_TYPE 0x00012A58
#define VSS_MAX_CHANNELS 8

static vbat_adc_data_t vbat_adc_data;

enum {
	DEFAULT_MAX_CAL_TABLE_SIZE = 4096,
	ANC_MAX_CODEC_CAL_TABLE_SIZE = 16384,
};

enum {
	BUFF_IDX_0 = 0,
	BUFF_IDX_1,
};

enum {
	NUM_VOICE_CAL_TABLES = 9,
};

enum {
        ERR_AUD_CAL_NOCOPP = 100,
        ERR_AUD_CAL_NODEVICE,
};

enum {
        AUDIO_DEVICE_CAL_TYPE = 0,
        AUDIO_STREAM_CAL_TYPE,
};

enum {
	AUDIO_VOL_STEPS,
	VOICE_VOL_STEPS
};

typedef struct acdb_audio_cal_cfg {
        uint32_t persist;
        uint32_t snd_dev_id;
        uint32_t dev_id;
        int32_t acdb_dev_id;
        uint32_t app_type;
        uint32_t topo_id;
        uint32_t sampling_rate;
        uint32_t cal_type;
        uint32_t module_id;
        uint32_t param_id;
} acdb_audio_cal_cfg_t;

struct param_data {
        int     use_case;
        int     acdb_id;
        int     get_size;
        int     buff_size;
        int     data_size;
        void    *buff;
};

struct aud_vol_idx_node {
	struct listnode		list;
	int			acdb_id;
	int			app_id;
	int			aud_vol_idx;
};

struct aud_vol_idx_node		aud_vol_idx_list;

struct cal_block {
	int		size;
	bool		is_mapped;
	int		*vaddr;
	int		map_handle;
	int		alloc_handle;
	int		version;
};

struct cal_node {
	int			buffer_number;
	struct cal_node		*next;
	struct cal_block	*cal_block;
};

struct cal_list {
	struct cal_node		*next;
	int			num_static_buffs;
};

struct meta_key_list {
	struct listnode		list;
	struct audio_cal_info_metainfo  cal_info;
};

#ifdef AFE_SIDETONE_IIR_CAL_TYPE
struct sidetone_iir_config {
	uint16_t	num_biquad_stages;
	uint16_t	pregain;
	uint8_t 	iir_config[MAX_SIDETONE_IIR_DATA_SIZE];
};
#endif

struct target_specific_info {
	uint16_t	major_version;
	uint16_t	minor_version;
	uint32_t	sample_rate_mask;
	uint32_t	channel_config;
	uint32_t	direction_mask;
	uint32_t	bits_per_sample_mask;
};

struct voice_channel_mapping {
	uint32_t	channel_map[VSS_MAX_CHANNELS];
};

struct cal_list		cal_data[MAX_CAL_TYPES] = {
	{0, 1},			/* CVP_VOC_RX_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* CVP_VOC_TX_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* CVP_VOCPROC_STATIC_CAL_TYPE */
	{0, 1},			/* CVP_VOCPROC_DYNAMIC_CAL_TYPE */
	{0, 1},			/* CVS_VOCSTRM_STATIC_CAL_TYPE */
	{0, 1},			/* CVP_VOCDEV_CFG_CAL_TYPE */
	{0, 1},			/* CVP_VOCPROC_STATIC_COL_CAL_TYPE */
	{0, 1},			/* CVP_VOCPROC_DYNAMIC_COL_CAL_TYPE */
	{0, 1},			/* CVS_VOCSTRM_STATIC_COL_CAL_TYPE */
	{0, 2},			/* ADM_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* ADM_CUST_TOPOLOGY_CAL_TYPE */
	{0, 2},			/* ADM_AUDPROC_CAL_TYPE */
	{0, 2},			/* ADM_AUDVOL_CAL_TYPE */
	{0, 1},			/* ASM_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* ASM_CUST_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* ASM_AUDSTRM_CAL_TYPE */
	{0, 1},			/* AFE_COMMON_RX_CAL_TYPE */
	{0, 1},			/* AFE_COMMON_TX_CAL_TYPE */
	{0, 1},			/* AFE_ANC_CAL_TYPE */
	{0, 1},			/* AFE_AANC_CAL_TYPE */
	{0, 1},			/* AFE_FB_SPKR_PROT_CAL_TYPE */
	{0, 2},			/* AFE_HW_DELAY_CAL_TYPE */
	{0, 1},			/* AFE_SIDETONE_CAL_TYPE */
	{0, 2},			/* AFE_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* AFE_CUST_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* LSM_CUST_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* LSM_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* LSM_CAL_TYPE */
	{0, 1},			/* ADM_RTAC_INFO_CAL_TYPE */
	{0, 1},			/* VOICE_RTAC_INFO_CAL_TYPE */
	{0, 1},			/* ADM_RTAC_APR_CAL_TYPE */
	{0, 1},			/* ASM_RTAC_APR_CAL_TYPE */
	{0, 1},			/* VOICE_RTAC_APR_CAL_TYPE */
	{0, 1},			/* MAD_CAL_TYPE */
	{0, 1},			/* ULP_AFE_CAL_TYPE */
	{0, 1},			/* ULP_LSM_CAL_TYPE */
	{0, 1},			/* DTS_EAGLE_CAL_TYPE */
	{0, 1},			/* AUDIO_CORE_METAINFO_CAL_TYPE */
        {0, 1},                 /* SRS_TRUMEDIA_CAL_TYPE */
	{0, 1},			/* CORE_CUSTOM_TOPOLOGIES_CAL_TYPE */
	{0, 1},			/* ADM_RTAC_AUDVOL_CAL_TYPE */
	{0, 1},			/* ULP_LSM_TOPOLOGY_ID_CAL_TYPE */
#ifdef MSM_SPKR_PROT_IN_FTM_MODE
	{0, 1},			/* AFE_FB_SPKR_PROT_TH_VI_CAL_TYPE */
	{0, 1},			/* AFE_FB_SPKR_PROT_EX_VI_CAL_TYPE */
#endif
#ifdef AFE_SIDETONE_IIR_CAL_TYPE
	{0, 1},         /* AFE_SIDETONE_IIR_CAL_TYPE */
#endif
#ifdef LSM_CAL_TYPES
	{0, 1},			/* AFE_LSM_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* AFE_LSM_TX_CAL_TYPE */
	{0, 1},			/* ADM_LSM_TOPOLOGY_CAL_TYPE */
	{0, 1},			/* ADM_LSM_AUDPROC_CAL_TYPE */
#endif
};

struct cal_block	mem_data[MAX_CAL_TYPES] = {
	{0, false, 0, -1, 0, VERSION_0_0},		/* CVP_VOC_RX_TOPOLOGY_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* CVP_VOC_TX_TOPOLOGY_CAL_TYPE */
	{245760, true, 0, -1, 0, VERSION_0_0},		/* CVP_VOCPROC_STATIC_CAL_TYPE */
	{61440, true, 0, -1, 0, VERSION_0_0},		/* CVP_VOCPROC_DYNAMIC_CAL_TYPE */
	{61440, true, 0, -1, 0, VERSION_0_0},		/* CVS_VOCSTRM_STATIC_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* CVP_VOCDEV_CFG_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* CVP_VOCPROC_STATIC_COL_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* CVP_VOCPROC_DYNAMIC_COL_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* CVS_VOCSTRM_STATIC_COL_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* ADM_TOPOLOGY_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* ADM_CUST_TOPOLOGY_CAL_TYPE */
	{16384, true, 0, -1, 0, VERSION_0_0},		/* ADM_AUDPROC_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* ADM_AUDVOL_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* ASM_TOPOLOGY_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* ASM_CUST_TOPOLOGY_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* ASM_AUDSTRM_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* AFE_COMMON_RX_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* AFE_COMMON_TX_CAL_TYPE */
	{4096, false, 0, -1, 0, VERSION_0_0},		/* AFE_ANC_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* AFE_AANC_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_FB_SPKR_PROT_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_HW_DELAY_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_SIDETONE_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_TOPOLOGY_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* AFE_CUST_TOPOLOGY_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* LSM_CUST_TOPOLOGY_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* LSM_TOPOLOGY_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* LSM_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* ADM_RTAC_INFO_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* VOICE_RTAC_INFO_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* ADM_RTAC_APR_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* ASM_RTAC_APR_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* VOICE_RTAC_APR_CAL_TYPE */
	{4096, false, 0, -1, 0, VERSION_0_0},		/* MAD_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* ULP_AFE_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* ULP_LSM_CAL_TYPE */
	{0, false, 0, -1, 0,VERSION_0_0},		/* DTS_EAGLE_CAL_TYPE */
	{4096, true, 0, -1, 0,VERSION_0_0},		/* AUDIO_CORE_METAINFO_CAL_TYPE */
	{0, false, 0, -1, 0,VERSION_0_0},		/* SRS_TRUMEDIA_CAL_TYPE */
	{4096, true, 0, -1, 0,VERSION_0_0},		/* CORE_CUSTOM_TOPOLOGIES_CAL_TYPE */
	{8192, true, 0, -1, 0, VERSION_0_0},		/* ADM_RTAC_AUDVOL_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* ULP_LSM_TOPOLOGY_ID_CAL_TYPE */
#ifdef MSM_SPKR_PROT_IN_FTM_MODE
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_FB_SPKR_PROT_TH_VI_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_FB_SPKR_PROT_EX_VI_CAL_TYPE */
#endif
#ifdef AFE_SIDETONE_IIR_CAL_TYPE
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_SIDETONE_IIR_CAL_TYPE */
#endif
#ifdef LSM_CAL_TYPES
	{0, false, 0, -1, 0, VERSION_0_0},		/* AFE_LSM_TOPOLOGY_CAL_TYPE */
	{4096, true, 0, -1, 0, VERSION_0_0},		/* AFE_LSM_TX_CAL_TYPE */
	{0, false, 0, -1, 0, VERSION_0_0},		/* ADM_LSM_TOPOLOGY_CAL_TYPE */
	{16384, true, 0, -1, 0, VERSION_0_0},		/* ADM_LSM_AUDPROC_CAL_TYPE */
#endif
};

enum {
	PER_VOCODER_VERSION = 0,
	MAX_CVD_VERSION
};

double cvd_version_table[] = {
	2.1
};

static int voice_cal_types[NUM_VOICE_CAL_TABLES] = {
	CVP_VOC_RX_TOPOLOGY_CAL_TYPE,
	CVP_VOC_TX_TOPOLOGY_CAL_TYPE,
	CVP_VOCPROC_STATIC_CAL_TYPE,
	CVP_VOCPROC_DYNAMIC_CAL_TYPE,
	CVS_VOCSTRM_STATIC_CAL_TYPE,
	CVP_VOCPROC_STATIC_COL_CAL_TYPE,
	CVP_VOCPROC_DYNAMIC_COL_CAL_TYPE,
	CVS_VOCSTRM_STATIC_COL_CAL_TYPE,
	CVP_VOCDEV_CFG_CAL_TYPE
};

static int		cal_driver_handle;
static int		ion_handle;
static bool		is_initialized;
static uint32_t		current_feature_set;
static uint32_t		current_voice_tx_acdb_id;
static uint32_t		current_voice_rx_acdb_id;
static bool			afe_topology_supported;
static unsigned int acdb_init_ref_cnt = 0;
pthread_mutex_t loader_mutex = PTHREAD_MUTEX_INITIALIZER;

static int allocate_cal_block(int cal_type,
				int buffer_number,
				struct audio_cal_alloc *audio_alloc,
				struct ion_allocation_data *alloc_data,
				struct cal_block *cal_block);
static int get_voctable_size(int rxacdb_id, int txacdb_id, int rxacdb_id_sample_rate, int txacdb_id_sample_rate);
static int get_vocvoltable_size(int rxacdb_id, int txacdb_id, int feature_set);
static int get_vocstrmtable_size();
static int get_audtable_size(int acdb_id, int app_id, int sample_rate);
static int get_audstrmtable_size(int app_id);
static int get_audvoltable_size(int acdb_dev_id, int app_type, int vol_index);
static int get_aanctable_size(int tx_dev_id);
static int get_afetable_size(int dev_id, int sample_rate);
static int get_lsmtable_size(int dev_id, int mad_app_type);
static int get_custom_topology_size();
int send_wcd9xxx_anc_cal(struct param_data *params);

bool acdb_loader_is_initialized(void)
{
    bool ret;

    pthread_mutex_lock(&loader_mutex);
    ret = is_initialized;
    pthread_mutex_unlock(&loader_mutex);

    return ret;
}

static bool cvd_version_supported(char *cvd_version, int version_id)
{
	bool ret = false;
	double cvd_version_num = 0;

	if (cvd_version == NULL) {
		goto done;
	} else if ((version_id >= MAX_CVD_VERSION) || (version_id < 0)) {
		LOGE("ACDB -> %s: CVD version %d not supported!", __func__, version_id);
		goto done;
	}

	/* format of version should be #.# */
	if (strstr(cvd_version, ".") == NULL)
		goto done;

	cvd_version_num = atof(cvd_version);
	if (cvd_version_num >= cvd_version_table[version_id])
		ret = true;
done:
	return ret;
}

static struct cal_node *create_cal_node(int cal_type, int buffer_number)
{
	struct cal_node		*cal_head = cal_data[cal_type].next;
	struct cal_node		*cal_node = NULL;

	cal_node = malloc(sizeof(*cal_node));
	if (cal_node == NULL) {
		LOGE("ACDB -> %s: Could not allocated cal_node for cal type %d!",
			__func__, cal_type);
		goto done;
	}

	cal_node->cal_block = malloc(sizeof(*cal_node->cal_block));
	if (cal_node->cal_block == NULL) {
		LOGE("ACDB -> %s: Could not allocated cal_block for cal type %d!",
			__func__, cal_type);
		goto done;
	}

	cal_node->buffer_number = buffer_number;
	*cal_node->cal_block = mem_data[cal_type];
	cal_node->next = NULL;

	if (cal_head == NULL) {
		cal_data[cal_type].next = cal_node;
	} else {
		while (cal_head->next != NULL) {
			cal_head = cal_head->next;
			LOGE("ACDB -> %s: New Node!", __func__);
		}
		cal_head->next = cal_node;
	}
done:
	return cal_node;
}

static void delete_cal_node(int cal_type, int buffer_number)
{
	struct cal_node		*cal_node = NULL;
	struct cal_node		*cal_head = cal_data[cal_type].next;

	while (cal_head != NULL) {
		if (cal_head->buffer_number == buffer_number) {
			cal_node = cal_head;
			cal_head = cal_head->next;
			break;
		}
		cal_head = cal_head->next;
	}
	if (cal_node == NULL)
		goto done;

	free(cal_node->cal_block);
	cal_node->cal_block = NULL;
	free(cal_node);
	cal_node = NULL;
done:
	return;
}

static void delete_cal_list(int cal_type)
{
	struct cal_node		*cal_node = NULL;
	struct cal_node		*cal_head = cal_data[cal_type].next;

	while (cal_head != NULL) {
		cal_node = cal_head;
		cal_head = cal_head->next;
		free(cal_node->cal_block);
		cal_node->cal_block = NULL;
		free(cal_node);
		cal_node = NULL;
	}
	cal_data[cal_type].next = NULL;
}

static struct cal_node *get_cal_node(int cal_type, int buffer_number)
{
	struct cal_node		*cal_node = cal_data[cal_type].next;

	while (cal_node != NULL) {
		if (cal_node->buffer_number == buffer_number)
			break;
		cal_node = cal_node->next;
	}
done:
	return cal_node;
}

static struct cal_node *create_node_and_alloc(int cal_type, int buffer_number)
{
	int ret;
	struct cal_node			*cal_node = NULL;
	struct ion_allocation_data	alloc_data;
	struct audio_cal_alloc		audio_alloc;

	cal_node = create_cal_node(cal_type, buffer_number);
	if (cal_node == NULL)
		goto done;

	audio_alloc.hdr.data_size = sizeof(audio_alloc);
	audio_alloc.hdr.version = VERSION_0_0;
	audio_alloc.hdr.cal_type_size = sizeof(audio_alloc.cal_type);
	audio_alloc.cal_type.cal_hdr.version = VERSION_0_0;

	alloc_data.align = 0x1000;
	alloc_data.heap_id_mask = HEAP_MASK;
	alloc_data.flags = 0;

	ret = allocate_cal_block(cal_type,
		cal_node->buffer_number,
		&audio_alloc,
		&alloc_data, cal_node->cal_block);
	if (ret < 0)
		LOGE("ACDB -> allocate_cal_block failed!\n");
done:
	return cal_node;
}


static struct cal_block *get_cal_block(int cal_type, int buffer_number)
{
	struct cal_node		*cal_node = NULL;
	struct cal_block	*cal_block = NULL;

	cal_node = get_cal_node(cal_type, buffer_number);
	if (cal_node == NULL)
		cal_node = create_node_and_alloc(cal_type, buffer_number);
	if (cal_node != NULL)
		cal_block = cal_node->cal_block;

	return cal_block;
}

static struct aud_vol_idx_node *get_aud_vol_node(int acdb_id, int app_id)
{
	struct listnode			*node;
	struct aud_vol_idx_node		*index_node;
	struct aud_vol_idx_node		*vol_node = NULL;

	list_for_each(node, &aud_vol_idx_list.list) {
		index_node = node_to_item(node, struct aud_vol_idx_node, list);
		if ((index_node->acdb_id == acdb_id) && (index_node->app_id == app_id)) {
			vol_node = index_node;
		}
	}
	return vol_node;
}

static void add_aud_vol_idx(int acdb_id, int app_id, int vol_index)
{
	struct aud_vol_idx_node		*index_node = NULL;

	index_node = get_aud_vol_node(acdb_id, app_id);
	if (index_node != NULL) {
		index_node->aud_vol_idx = vol_index;
		goto done;
	}

	index_node = malloc(sizeof(*index_node));
	if (index_node == NULL) {
		LOGE("ACDB -> %s: malloc failed, acdb_id %d, app_id %d",
			__func__, acdb_id, app_id);
		goto done;
	}

	list_init(&index_node->list);
	index_node->acdb_id = acdb_id;
	index_node->app_id = app_id;
	index_node->aud_vol_idx = vol_index;

	list_add_tail(&aud_vol_idx_list.list, &index_node->list);
done:
	return;
}

void destroy_vol_index_list(void)
{
	struct listnode			*node;
	struct aud_vol_idx_node		*index_node;
	struct listnode			*tmp_node;

	list_for_each_safe(node, tmp_node, &aud_vol_idx_list.list) {
		index_node = node_to_item(node, struct aud_vol_idx_node, list);
		list_remove(node);
		free(index_node);
	}
}

#ifdef _ANDROID_
static int get_files_from_properties(AcdbInitCmdType *acdb_init_cmd)
{
	int i = 0;
	int prop_len;
	char prop_name[24];

	for (i=0; i < MAX_ACDB_FILES; i++) {
		if (snprintf(prop_name, sizeof(prop_name), "persist.audio.calfile%d", i) < 0)
			goto done;

		prop_len = property_get(prop_name, acdb_init_cmd->acdbFiles[i].fileName, NULL);
		if (prop_len <= 0)
			goto done;

		acdb_init_cmd->acdbFiles[i].fileNameLen = strlen(acdb_init_cmd->acdbFiles[i].fileName);
		LOGD("ACDB -> Prop Load file: %s\n", acdb_init_cmd->acdbFiles[i].fileName);
	}
done:
	acdb_init_cmd->nNoOfFiles = i;
	return i;
}
#else
static int get_files_from_properties(AcdbInitCmdType *acdb_init_cmd) {return -ENODEV;}
#endif

static int send_common_custom_topology(void);
static int send_afe_custom_topology(void);
static int send_adm_custom_topology(void);
static int send_asm_custom_topology(void);
static int send_lsm_custom_topology(void);

static int get_acdb_files_in_directory(AcdbInitCmdType *acdb_init_cmd, char *dir_path)
{
	int result = 0;
	int i = 0;
	DIR *dir_fp = NULL;
	struct dirent *dentry;

	dir_fp = opendir(dir_path);
	if (dir_fp == NULL)
		goto done;

	/* search directory for .acdb files */
	while ((dentry = readdir(dir_fp)) != NULL) {
		if ((strstr(dentry->d_name, ".acdb") != NULL) ||
		    (strstr(dentry->d_name, ".qwsp") != NULL)) {
			if (acdb_init_cmd->nNoOfFiles >= MAX_ACDB_FILES) {
				LOGD("ACDB -> Maximum number of ACDB files hit, %d!\n", i);
				break;
			}

			result = snprintf(acdb_init_cmd->acdbFiles[acdb_init_cmd->nNoOfFiles].fileName,
					sizeof(acdb_init_cmd->acdbFiles[acdb_init_cmd->nNoOfFiles].fileName),
					"%s/%s", dir_path, dentry->d_name);
			if (result < 0) {
				LOGD("ACDB -> Error: snprintf load file failed: %s/%s, err %d\n",
					dir_path, acdb_init_cmd->acdbFiles[acdb_init_cmd->nNoOfFiles].fileName, result);
				continue;
			}

			acdb_init_cmd->acdbFiles[acdb_init_cmd->nNoOfFiles].fileNameLen =
				strlen(acdb_init_cmd->acdbFiles[acdb_init_cmd->nNoOfFiles].fileName);
			LOGD("ACDB -> Load file: %s\n", acdb_init_cmd->acdbFiles[acdb_init_cmd->nNoOfFiles].fileName);
			i++;
			acdb_init_cmd->nNoOfFiles++;
		}
	}

	if (i == 0)
		LOGD("ACDB -> No .acdb files found in %s!\n", dir_path);


	closedir(dir_fp);
done:
	return i;
}

static int get_files_from_device_tree(AcdbInitCmdType *acdb_init_cmd, char *snd_card_name)
{
	int result = 0;
	char dir_path[300];
	char board_type[64] = DEFAULT_BOARD;
	FILE *fp = NULL;

	/* Get Board type */
	fp = fopen("/sys/devices/soc0/hw_platform","r");
	if (fp == NULL)
		fp = fopen("/sys/devices/system/soc/soc0/hw_platform","r");
	if (fp == NULL)
		LOGE("ACDB -> Error: Couldn't open hw_platform\n");
	else if (fgets(board_type, sizeof(board_type), fp) == NULL)
		LOGE("ACDB -> Error: Couldn't get board type\n");
	else if (board_type[(strlen(board_type) - 1)] == '\n')
		board_type[(strlen(board_type) - 1)] = '\0';
	if (fp != NULL)
		fclose(fp);

	/* get files from form factor & soundcard independant path */
	result = get_acdb_files_in_directory(acdb_init_cmd, ACDB_BIN_PATH);
	if (result >= 0)
		LOGD("ACDB -> found %d form factor & soundcard independant files\n",
			result);
	if (result > MAX_INDEPENDANT_ACDB_FILES)
		goto done;

	/* Try board directory with soundcard name */
	if (snd_card_name != NULL) {
		result = snprintf(dir_path, sizeof(dir_path), "%s/%s/%s", ACDB_BIN_PATH, board_type, snd_card_name);
		if (result < 0) {
			LOGE("ACDB -> Error: snprintf failed for snd card %s, error: %d\n", snd_card_name, result);
			result = -ENODEV;
			goto done;
		}
		result = get_acdb_files_in_directory(acdb_init_cmd, dir_path);
		if (result > 0)
			goto done;
	}

	/* Try board type directory */
	result = snprintf(dir_path, sizeof(dir_path), "%s/%s", ACDB_BIN_PATH, board_type);
	if (result < 0) {
		LOGE("ACDB -> Error: snprintf failed for board type dir, error: %d\n", result);
		result = -ENODEV;
		goto done;
	}
	result = get_acdb_files_in_directory(acdb_init_cmd, dir_path);
	if (result > 0)
		goto done;

	/* Try default directory with soundcard name */
	if ((snd_card_name != NULL) && (strcmp(DEFAULT_BOARD, board_type) != 0)) {
		result = snprintf(dir_path, sizeof(dir_path), "%s/%s/%s", ACDB_BIN_PATH, DEFAULT_BOARD, snd_card_name);
		if (result < 0) {
			LOGE("ACDB -> Error: snprintf failed for snd_card default dir, error: %d\n", result);
			result = -ENODEV;
			goto done;
		}
		result = get_acdb_files_in_directory(acdb_init_cmd, dir_path);
		if (result > 0)
			goto done;
	}

	/* Try default directory */
	if (strcmp(DEFAULT_BOARD, board_type) != 0) {
		result = snprintf(dir_path, sizeof(dir_path), "%s/%s", ACDB_BIN_PATH, DEFAULT_BOARD);
		if (result < 0) {
			LOGE("ACDB -> Error: snprintf failed for default dir, error: %d\n", result);
			result = -ENODEV;
			goto done;
		}
		result = get_acdb_files_in_directory(acdb_init_cmd, dir_path);
		if (result > 0)
			goto done;
	}

	/* Try etc directory */
	result = snprintf(dir_path, sizeof(dir_path), "%s", ETC_ROOT_PATH);
	if (result < 0) {
		LOGE("ACDB -> Error: snprintf failed for board type dir, error: %d\n", result);
		result = -ENODEV;
		goto done;
	}
	result = get_acdb_files_in_directory(acdb_init_cmd, dir_path);
	if (result > 0)
		goto done;

	LOGE("ACDB -> Error: Could not open any directory! Failed for %s\n", dir_path);
	result = -ENODEV;

done:
	return result;
}

static int acdb_load_files(AcdbInitCmdType *acdb_init_cmd, char * snd_card_name)
{
	int result = 0;

	result = get_files_from_properties(acdb_init_cmd);
	if (result > 0)
		goto done;

	result = get_files_from_device_tree(acdb_init_cmd, snd_card_name);
done:
	return result;
}

static void deallocate_cal_block(int cal_type,
				int buffer_number,
				struct audio_cal_dealloc *audio_dealloc,
				struct cal_block *cal_block)
{
	int				ret = 0;

	if (!cal_block->is_mapped) {
		if (cal_block->size != 0)
			free(cal_block->vaddr);
		goto done;
	} else if (cal_block->map_handle < 0)
		goto done;

	audio_dealloc->hdr.cal_type = cal_type;
	audio_dealloc->cal_type.cal_hdr.buffer_number = buffer_number;
	audio_dealloc->cal_type.cal_data.mem_handle = cal_block->map_handle;
	ret = ioctl(cal_driver_handle, AUDIO_DEALLOCATE_CALIBRATION,
		audio_dealloc);
	if (ret) {
		LOGE("ACDB -> Error: Sending AUDIO_DEALLOCATE_CALIBRATION, result = %d",
			ret);
	}

	munmap(cal_block->vaddr, cal_block->size);
	close(cal_block->map_handle);
	ret = ioctl(ion_handle, ION_IOC_FREE,
		&cal_block->alloc_handle);
	if (ret)
		LOGE("ACDB -> %s: ION_IOC_FREE, cal type %d, errno: %d",
			__func__, cal_type, ret);
done:
	return;
}

static void deallocate_memory(void)
{
	int				i;
	struct cal_node			*cal_node = NULL;
	struct audio_cal_dealloc	audio_dealloc;

	audio_dealloc.hdr.data_size = sizeof(audio_dealloc);
	audio_dealloc.hdr.version = VERSION_0_0;
	audio_dealloc.hdr.cal_type_size = sizeof(audio_dealloc.cal_type);
	audio_dealloc.cal_type.cal_hdr.version = VERSION_0_0;

	for (i=0; i<MAX_CAL_TYPES; i++) {
		cal_node = get_cal_node(i, BUFF_IDX_0);
		audio_dealloc.cal_type.cal_hdr.version = mem_data[i].version;
		while (cal_node != NULL) {
			deallocate_cal_block(i,
				cal_node->buffer_number,
				&audio_dealloc,
				cal_node->cal_block);
			cal_node = cal_node->next;
		}
		delete_cal_list(i);
	}
	close(ion_handle);
}

static void deallocate_cal_type(int cal_type_idx, int buffer_number)
{
        struct cal_node                 *cal_node = NULL;
        struct audio_cal_dealloc        audio_dealloc;

        audio_dealloc.hdr.data_size = sizeof(audio_dealloc);
        audio_dealloc.hdr.version = VERSION_0_0;
        audio_dealloc.hdr.cal_type_size = sizeof(audio_dealloc.cal_type);
        audio_dealloc.cal_type.cal_hdr.version = VERSION_0_0;

        cal_node = get_cal_node(cal_type_idx, buffer_number);
        audio_dealloc.cal_type.cal_hdr.version = mem_data[cal_type_idx].version;

        /* Deallocate cal block */
        deallocate_cal_block(cal_type_idx, cal_node->buffer_number,
                              &audio_dealloc, cal_node-> cal_block);
}

static int allocate_cal_block(int cal_type,
				int buffer_number,
				struct audio_cal_alloc *audio_alloc,
				struct ion_allocation_data *alloc_data,
				struct cal_block *cal_block)
{
	int				ret = 0;
	struct ion_fd_data		fd_data;

	if (!cal_block->is_mapped) {
		if (cal_block->size > 0) {
			cal_block->vaddr = malloc(cal_block->size);
			if (cal_block->vaddr == NULL) {
			LOGE("ACDB -> %s: malloc failed, cal type %d",
				__func__, cal_type);
				ret = -ENOMEM;
				goto done;
			}
		}
		goto done;
	} else if (cal_block->size <= 0) {
		LOGE("ACDB -> %s: Allocation request for cal type %d, but size is %d!",
			__func__, cal_type, cal_block->size);
			ret = -ENOMEM;
			goto done;
	}

	if (cal_block->map_handle > 0) {
		LOGE("ACDB -> %s: mmap", __func__);
		cal_block->vaddr = (int *)mmap(0, cal_block->size,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			cal_block->map_handle, 0);
		if (cal_block->vaddr != MAP_FAILED)
			goto done;
	}

	alloc_data->len = cal_block->size;
	ret = ioctl(ion_handle, ION_IOC_ALLOC, alloc_data);
	if (ret) {
		LOGE("ACDB -> %s: ION_ALLOC, cal type %d, errno: %d\n",
			__func__, cal_type, ret);
		ret = -ENOMEM;
		goto done;
	}

	fd_data.handle = alloc_data->handle;
	cal_block->alloc_handle = (int)alloc_data->handle;
	ret = ioctl(ion_handle, ION_IOC_SHARE, &fd_data);
	if (ret) {
		LOGE("ACDB -> %s: ION_IOC_SHARE, cal type %d, errno: %d\n",
			__func__, cal_type, ret);
		ret = -ENOMEM;
		goto done;
	}

	cal_block->map_handle = fd_data.fd;
	cal_block->vaddr = (int *)mmap(0, cal_block->size,
		PROT_READ | PROT_WRITE, MAP_SHARED,
		cal_block->map_handle, 0);
	if (cal_block->vaddr == MAP_FAILED) {
		LOGE("ACDB -> %s: Cannot allocate ION, cal type %d",
			__func__, cal_type);
		ret = -ENOMEM;
		goto done;
	}

	audio_alloc->hdr.cal_type = cal_type;
	audio_alloc->cal_type.cal_hdr.buffer_number = buffer_number;
	audio_alloc->cal_type.cal_data.mem_handle = cal_block->map_handle;
	ret = ioctl(cal_driver_handle, AUDIO_ALLOCATE_CALIBRATION,
		audio_alloc);
	if (ret) {
		LOGE("ACDB -> Error: Sending AUDIO_ALLOCATE_CALIBRATION, result = %d\n",
			ret);
		goto done;
	}

done:
	return ret;
}

static void add_initial_nodes(int cal_type)
{
	int			i;
	struct cal_node		*cal_node = NULL;

	for (i=0; i < cal_data[cal_type].num_static_buffs; i++) {
		if (create_cal_node(cal_type, i) == NULL) {
			LOGE("ACDB -> Error: Could not allocate cal node %d for cal type %d\n",
				i, cal_type);
			goto done;
		}
	}
done:
	return;
}

static int allocate_memory(void)
{
	int				ret = 0;
	int				i;
	struct cal_node			*cal_node = NULL;
	struct ion_allocation_data	alloc_data;
	struct audio_cal_alloc		audio_alloc;

	ion_handle = open("/dev/ion", O_RDONLY | O_DSYNC);
	if (ion_handle < 0) {
		LOGE("Cannot open /dev/ion errno: %d\n", ion_handle);
		ret = -ENODEV;
		goto done;
	}

	audio_alloc.hdr.data_size = sizeof(audio_alloc);
	audio_alloc.hdr.version = VERSION_0_0;
	audio_alloc.hdr.cal_type_size = sizeof(audio_alloc.cal_type);
	audio_alloc.cal_type.cal_hdr.version = VERSION_0_0;

	alloc_data.align = 0x1000;
	alloc_data.heap_id_mask = HEAP_MASK;
	alloc_data.flags = 0;

	for (i=0; i<MAX_CAL_TYPES; i++) {
		add_initial_nodes(i);
		cal_node = cal_data[i].next;
		audio_alloc.cal_type.cal_hdr.version = mem_data[i].version;
		while (cal_node != NULL) {
			ret = allocate_cal_block(i,
				cal_node->buffer_number,
				&audio_alloc,
				&alloc_data, cal_node->cal_block);
			if (ret < 0) {
				LOGE("ACDB -> allocate_cal_block failed!\n");
				goto err;
			}
			cal_node = cal_node->next;
		}
	}
done:
	return ret;
err:
	deallocate_memory();
	return ret;
}

static int allocate_cal_type(int cal_type_idx, int size, int buffer_number)
{
        int                             ret = 0;
        struct cal_node                 *cal_node = NULL;
        struct ion_allocation_data      alloc_data;
        struct audio_cal_alloc          audio_alloc;

        audio_alloc.hdr.data_size = sizeof(audio_alloc);
        audio_alloc.hdr.version = VERSION_0_0;
        audio_alloc.hdr.cal_type_size = sizeof(audio_alloc.cal_type);
        audio_alloc.cal_type.cal_hdr.version = VERSION_0_0;

        alloc_data.align = 0x1000;
        alloc_data.heap_id_mask = HEAP_MASK;
        alloc_data.flags = 0;

        cal_node = get_cal_node(cal_type_idx, buffer_number);
        cal_node->cal_block->size = size;
        audio_alloc.cal_type.cal_hdr.version = mem_data[cal_type_idx].version;

        /* Allocate cal block */
        ret = allocate_cal_block(cal_type_idx, cal_node->buffer_number, &audio_alloc,
                                  &alloc_data, cal_node->cal_block);
        if (ret < 0)
            LOGE("Error: ACDB -> allocate_cal_block failed!\n");

        return ret;
}

int acdb_loader_init_v3(char *snd_card_name,
			char *cvd_version,
			struct listnode *meta_key_list)
{
	int	ret = 0;
	int	i;
	int	result = 0;
	AcdbVocProcGainDepVolTblSizeV2CmdType	vocvoltablesize;
	AcdbSizeResponseType		response;
	AcdbInitCmdType			acdb_init_cmd;

        pthread_mutex_lock(&loader_mutex);
        if (acdb_init_ref_cnt != 0) {
            acdb_init_ref_cnt++;
            ALOGD("ACDB -> already initialized, exit");
            goto done;
        }

	list_init(&aud_vol_idx_list.list);

	if (acdb_load_files(&acdb_init_cmd, snd_card_name) <= 0) {
		LOGE("ACDB -> Could not load .acdb files!\n");
		ret = -ENODEV;
		goto done;
	}

	LOGD("ACDB -> ACDB_CMD_INITIALIZE_V2\n");
	ret = acdb_ioctl(ACDB_CMD_INITIALIZE_V2,
		(const uint8_t *)&acdb_init_cmd, sizeof(acdb_init_cmd), NULL, 0);
	if (ret) {
		LOGE("Error initializing ACDB returned = %d\n", ret);
		ret = -ENODEV;
		goto done;
	}

	LOGD("ACDB -> ACPH INIT\n");
	ret = acph_init();
	if (ret) {
		LOGE("Error initializing ACPH returned = %d\n", ret);
		ret = -ENODEV;
		goto done;
	}

	cal_driver_handle = open("/dev/msm_audio_cal", O_RDWR);
	if (cal_driver_handle < 0) {
		LOGE("ACDB -> Cannot open /dev/msm_audio_cal errno: %d\n", errno);
		ret = -ENODEV;
		goto done;
	}

	LOGD("ACDB -> RTAC INIT\n");
	acdb_rtac_init();

#ifdef _ANDROID_
	LOGD("ACDB -> MCS, FTS INIT\n");
	acdb_mcs_init(snd_card_name);
	acdb_fts_init();
#endif /* _ANDROID_ */

	LOGD("ACDB -> ADIE RTAC INIT\n");
	adie_rtac_init();

	if (cvd_version_supported(cvd_version, PER_VOCODER_VERSION)) {
		vocvoltablesize.nTxDeviceId = DEVICE_HANDSET_TX_ACDB_ID;
		vocvoltablesize.nRxDeviceId = DEVICE_HANDSET_RX_ACDB_ID;
		vocvoltablesize.nFeatureId = ACDB_VOCVOL_FID_DEFAULT;

		LOGD("ACDB -> ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE\n");
		result = acdb_ioctl(ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE,
				(const uint8_t *)&vocvoltablesize,
				sizeof(vocvoltablesize),(uint8_t *)&response,
				sizeof(response));
		if (result == 0)
			for (i=0; i<NUM_VOICE_CAL_TABLES; i++)
				mem_data[voice_cal_types[i]].version |= PER_VOCODER_CAL_BIT_MASK;
	}
	if (allocate_memory() < 0) {
		LOGE("ACDB -> Cannot allocate memory!");
		ret = -ENOMEM;
		goto done;
	}
	acdb_loader_send_common_custom_topology();

	/*meta info*/
	if (send_meta_info_list(meta_key_list) < 0)
		LOGD("ACDB -> send_meta_info_list failed");

	current_feature_set = ACDB_VOCVOL_FID_DEFAULT;
	is_initialized = true;

	parse_codec_type(snd_card_name);

	LOGD("ACDB -> init done!\n");
        acdb_init_ref_cnt++;

done:
        pthread_mutex_unlock(&loader_mutex);
	return ret;
}

int acdb_loader_init_v2(char *snd_card_name, char *cvd_version, int metaInfoKey)
{
	struct listnode key_list;
	struct meta_key_list key_info;

	key_info.cal_info.nKey = metaInfoKey;
	list_init(&key_list);
	list_add_tail(&key_list,&key_info.list);
	return acdb_loader_init_v3(snd_card_name, cvd_version, &key_list);
}

int acdb_loader_init_ACDB(void)
{
	return acdb_loader_init_v2(NULL, NULL,0);
}

int acdb_loader_get_default_app_type(void)
{
	return ACDB_APPTYPE_GENERAL_PLAYBACK;
}

int acdb_loader_send_common_custom_topology(void)
{
	int ret = -EINVAL;

	ret = send_common_custom_topology();
	if (ret < 0) {
		LOGD("ACDB -> %s: Common custom topology not in use",
			__func__);
		send_afe_custom_topology();
		send_adm_custom_topology();
		send_asm_custom_topology();
		send_lsm_custom_topology();
	} else {
		LOGD("ACDB -> %s: Common custom topology in use",
			__func__);
	}

	return ret;
}

static int get_audcal_path(uint32_t capability)
{
	int path;

	if (capability & MSM_SNDDEV_CAP_RX)
		path = RX_DEVICE;
	else if (capability & MSM_SNDDEV_CAP_TX)
		path = TX_DEVICE;
	else
		path = INVALID_DATA;

	return path;
}

int acdb_loader_set_codec_data(void *data, char *attr)
{
	int ret = 0;
	vbat_adc_data_t *param;

	if (strcmp("vbat_cal", attr) == 0) {
		param = (vbat_adc_data_t *) data;
		vbat_adc_data.dcp1 = param->dcp1;
		vbat_adc_data.dcp2 = param->dcp2;
	}

	return ret;
}

static void acdb_loader_get_codec_data(void **data, char *attr)
{

	if (strcmp("vbat_cal", attr) == 0)
		*data = &vbat_adc_data;

	return;
}

static uint32_t get_samplerate(int  acdb_id)
{
	uint32_t sample_rate = 48000;

	if (((uint32_t)acdb_id == DEVICE_BT_SCO_RX_ACDB_ID) ||
		((uint32_t)acdb_id == DEVICE_BT_SCO_TX_ACDB_ID) ||
		((uint32_t)acdb_id == DEVICE_BT_SCO_TX_NREC_ACDB_ID)) {
		sample_rate = 8000;
	} else if (((uint32_t)acdb_id == DEVICE_BT_SCO_RX_WB_ACDB_ID) ||
		((uint32_t)acdb_id == DEVICE_BT_SCO_TX_WB_ACDB_ID) ||
		((uint32_t)acdb_id == DEVICE_BT_SCO_TX_WB_NREC_ACDB_ID)) {
		sample_rate = 16000;
                /*To change to 16000HZ*/
        }

	return sample_rate;
}
static int get_afe_topology(struct audio_cal_afe_top *afe_top)
{
	int				result = 0;
	AcdbGetAfeTopIdCmdType	acdb_get_top;
	AcdbGetTopologyIdRspType	audio_top;

	acdb_get_top.nDeviceId = afe_top->cal_type.cal_info.acdb_id;

	LOGD("ACDB -> ACDB_CMD_GET_AFE_TOPOLOGY_ID\n");

	result = acdb_ioctl(ACDB_CMD_GET_AFE_TOPOLOGY_ID,
		(const uint8_t *)&acdb_get_top, sizeof(acdb_get_top),
		(uint8_t *)&audio_top, sizeof(audio_top));
	if (result) {
		LOGE("ACDB get afe topology for acdb id = %d, err = %d\n",
		     afe_top->cal_type.cal_info.acdb_id, result);
		afe_top->cal_type.cal_info.topology = 0;
		goto done;
	}
	afe_top->cal_type.cal_info.topology = audio_top.nTopologyId;
	LOGD("ACDB -> GET_AFE_TOPOLOGY_ID for adcd_id %d, Topology Id %x\n",
		acdb_get_top.nDeviceId, audio_top.nTopologyId);
done:
	return result;
}

static int get_adm_topology(struct audio_cal_adm_top *adm_top)
{
	int				result = 0;
	AcdbGetAudProcTopIdCmdType	acdb_get_top;
	AcdbGetTopologyIdRspType	audio_top;

	acdb_get_top.nDeviceId = adm_top->cal_type.cal_info.acdb_id;
	acdb_get_top.nApplicationType = adm_top->cal_type.cal_info.app_type;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_COMMON_TOPOLOGY_ID\n");

	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_COMMON_TOPOLOGY_ID,
		(const uint8_t *)&acdb_get_top, sizeof(acdb_get_top),
		(uint8_t *)&audio_top, sizeof(audio_top));
	if (result) {
		LOGE("Error: ACDB get adm topology for acdb id = %d, returned = %d\n",
		     adm_top->cal_type.cal_info.acdb_id, result);
		adm_top->cal_type.cal_info.topology = 0;
		goto done;
	}
	adm_top->cal_type.cal_info.topology = audio_top.nTopologyId;
done:
	return result;
}

static int get_asm_topology(struct audio_cal_asm_top *audstrm_top)
{
	int					result = 0;
	AcdbGetAudProcStrmTopIdCmdType		acdb_get_top;
	AcdbGetTopologyIdRspType		audio_top;

	acdb_get_top.nApplicationType = audstrm_top->cal_type.cal_info.app_type;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_STREAM_TOPOLOGY_ID\n");

	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_STREAM_TOPOLOGY_ID,
		(const uint8_t *)&acdb_get_top, sizeof(acdb_get_top),
		(uint8_t *)&audio_top, sizeof(audio_top));
	if (result) {
		LOGE("Error: ACDB get asm topology returned = %d\n",
		     result);
		audstrm_top->cal_type.cal_info.topology = 0;
		goto done;
	}
	audstrm_top->cal_type.cal_info.topology = audio_top.nTopologyId;
done:
	return 0;
}
static int get_audstrmtable(struct cal_block *block,
				struct audio_cal_audstrm *audstrm_cal)
{
	int				result = 0;
	AcdbAudStrmTableV2CmdType	audstrmtable;
	AcdbQueryResponseType		response;

	audstrmtable.nApplicationTypeId = audstrm_cal->cal_type.cal_info.app_type;
	audstrmtable.nBufferLength = block->size;
	audstrmtable.nBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_STREAM_TABLE_V2\n");

	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_STREAM_TABLE_V2,
		(const uint8_t *)&audstrmtable, sizeof(audstrmtable),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB get audstream table returned = %d\n", result);
		audstrm_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	audstrm_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
	LOGD("ACDB -> audstrm_cal->cal_type.cal_data.cal_size = %d\n",audstrm_cal->cal_type.cal_data.cal_size);

done:
	return result;
}

static int get_common_custom_topology(struct cal_block *block,
				struct audio_cal_basic *common_top)
{
	int				result = 0;
	AcdbQueryCmdType		acdb_top;
	AcdbQueryResponseType		response;

	acdb_top.nBufferLength = block->size;
	acdb_top.pBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO\n");

	result = acdb_ioctl(ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO,
		(const uint8_t *)&acdb_top, sizeof(acdb_top),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB get common custom topologies returned = %d\n",
		     result);
		common_top->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	common_top->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	LOGD("ACDB -> ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO: size:0x%x ret=%d \n",
		common_top->cal_type.cal_data.cal_size, result);
	return result;
}

static int get_afe_custom_topology(struct cal_block *block,
				struct audio_cal_basic *afe_top)
{
	int				result = 0;
	AcdbQueryCmdType		acdb_top;
	AcdbQueryResponseType		response;

	acdb_top.nBufferLength = block->size;
	acdb_top.pBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AFE_TOPOLOGIES\n");

	result = acdb_ioctl(ACDB_CMD_GET_AFE_TOPOLOGIES,
		(const uint8_t *)&acdb_top, sizeof(acdb_top),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB get afe custom topologies returned = %d\n",
		     result);
		afe_top->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	afe_top->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	LOGD("ACDB -> ACDB_CMD_GET_AFE_TOPOLOGIES: size:0x%x ret=%d \n",
		afe_top->cal_type.cal_data.cal_size, result);
	return result;
}

static int get_adm_custom_topology(struct cal_block *block,
				struct audio_cal_basic *adm_top)
{
	int				result = 0;
	AcdbQueryCmdType		acdb_top;
	AcdbQueryResponseType		response;

	acdb_top.nBufferLength = block->size;
	acdb_top.pBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AUDIO_COPP_TOPOLOGIES\n");

	result = acdb_ioctl(ACDB_CMD_GET_AUDIO_COPP_TOPOLOGIES,
		(const uint8_t *)&acdb_top, sizeof(acdb_top),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB get adm topologies returned = %d\n",
		     result);
		adm_top->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	adm_top->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_asm_custom_topology(struct cal_block *block,
				struct audio_cal_basic *asm_top)
{
	int				result = 0;
	AcdbQueryCmdType		acdb_top;
	AcdbQueryResponseType		response;

	acdb_top.nBufferLength = block->size;
	acdb_top.pBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AUDIO_POPP_TOPOLOGIES\n");

	result = acdb_ioctl(ACDB_CMD_GET_AUDIO_POPP_TOPOLOGIES,
		(const uint8_t *)&acdb_top, sizeof(acdb_top),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB get asm topologies returned = %d\n",
		     result);
		asm_top->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	asm_top->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}


static int get_audtable(struct cal_block *block,
			struct audio_cal_audproc *audproc_cal)
{
	int				result = 0;
	AcdbAudProcTableCmdType		audtable;
	AcdbQueryResponseType		response;

	audtable.nDeviceId = audproc_cal->cal_type.cal_info.acdb_id;
	audtable.nDeviceSampleRateId = audproc_cal->cal_type.cal_info.sample_rate;
	audtable.nApplicationType = audproc_cal->cal_type.cal_info.app_type;
	audtable.nBufferLength = block->size;
	audtable.nBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_COMMON_TABLE\n");

	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_COMMON_TABLE,
		(const uint8_t *)&audtable, sizeof(audtable),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB audproc returned = %d\n", result);
		audproc_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	audproc_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_audvoltable(struct cal_block *block,
			struct audio_cal_audvol *audvol_cal)
{
	int					result = 0;
	AcdbAudProcGainDepVolTblStepCmdType	audvoltable;
	AcdbQueryResponseType			response;

	audvoltable.nDeviceId = audvol_cal->cal_type.cal_info.acdb_id;
	audvoltable.nApplicationType = audvol_cal->cal_type.cal_info.app_type;
	audvoltable.nVolumeIndex = audvol_cal->cal_type.cal_info.vol_index;
	audvoltable.nBufferLength = block->size;
	audvoltable.nBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_GAIN_DEP_STEP_TABLE, vol index %d\n", audvol_cal->cal_type.cal_info.vol_index);

	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_GAIN_DEP_STEP_TABLE,
		(const uint8_t *)&audvoltable, sizeof(audvoltable),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB AudProc vol returned = %d\n", result);
		audvol_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	audvol_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_hw_delay(struct cal_block *block,
			struct audio_cal_hw_delay *hw_delay_cal)
{
	int32_t			result = 0;
	AcdbQueryResponseType	response;
	AcdbDevPropCmdType	avsyncdelay;
	struct hw_delay		*delay;

	avsyncdelay.nDeviceId = hw_delay_cal->cal_type.cal_info.acdb_id;
	avsyncdelay.nPropID = hw_delay_cal->cal_type.cal_info.property_type;
	avsyncdelay.nBufferLength = sizeof(hw_delay_cal->cal_type.cal_info.data);
	avsyncdelay.pBufferPointer = (void *)&hw_delay_cal->cal_type.cal_info.data;

	LOGD("ACDB -> ACDB_AVSYNC_INFO: ACDB_CMD_GET_DEVICE_PROPERTY\n");

	result = acdb_ioctl(ACDB_CMD_GET_DEVICE_PROPERTY,
		(const uint8_t *)&avsyncdelay, sizeof(avsyncdelay),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB ACDB_CMD_GET_DEVICE_PROPERTY error = %d\n",
		     result);
		hw_delay_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	hw_delay_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_afetable(struct cal_block *block,
			struct audio_cal_afe *afe_cal)
{
	int					result = 0;
	AcdbAfeCommonTableCmdType		afetable;
	AcdbQueryResponseType			response;

	afetable.nDeviceId = afe_cal->cal_type.cal_info.acdb_id;
	/* Does not accept ACDB sample rate bit mask */
	afetable.nSampleRateId = afe_cal->cal_type.cal_info.sample_rate;
	afetable.nBufferLength = block->size;
	afetable.nBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_AFE_COMMON_TABLE\n");

	result = acdb_ioctl(ACDB_CMD_GET_AFE_COMMON_TABLE,
		(const uint8_t *)&afetable, sizeof(afetable),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB AFE returned = %d\n", result);
		afe_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	afe_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_lsm_custom_topology(struct cal_block *block,
				struct audio_cal_basic *lsm_top)
{
	int				result = 0;
	AcdbQueryCmdType		acdb_top;
	AcdbQueryResponseType		response;

	acdb_top.nBufferLength = block->size;
	acdb_top.pBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_LSM_TOPOLOGIES\n");

	result = acdb_ioctl(ACDB_CMD_GET_LSM_TOPOLOGIES,
		(const uint8_t *)&acdb_top, sizeof(acdb_top),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB get lsm custom topologies returned = %d\n",
		     result);
		lsm_top->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	lsm_top->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_lsm_topology(struct audio_cal_lsm_top *lsm_top)
{
	int					result = 0;
	AcdbGetLsmTopIdCmdType			acdb_get_top;
	AcdbGetTopologyIdRspType		audio_top;

	acdb_get_top.nDeviceId = lsm_top->cal_type.cal_info.acdb_id;
	acdb_get_top.nLsmApplicationType = lsm_top->cal_type.cal_info.app_type;

	LOGD("ACDB -> ACDB_CMD_GET_LSM_TOPOLOGY_ID \n");

	result = acdb_ioctl(ACDB_CMD_GET_LSM_TOPOLOGY_ID,
		(const uint8_t *)&acdb_get_top, sizeof(acdb_get_top),
		(uint8_t *)&audio_top, sizeof(audio_top));
	if (result) {
		LOGE("Error: ACDB get lsm topology returned = %d\n",
		     result);
		lsm_top->cal_type.cal_info.topology = 0;
		result = -ENODATA;
		goto done;
	}
	lsm_top->cal_type.cal_info.topology = audio_top.nTopologyId;
done:
	return result;
}

static int get_lsm_table(struct cal_block *block,
				struct audio_cal_lsm *lsm_cal)
{
	int					result = 0;
	AcdbLsmTableCmdType			lsm_table;
	AcdbQueryResponseType			response;

	lsm_table.nDeviceId = lsm_cal->cal_type.cal_info.acdb_id;
	lsm_table.nMadApplicationType = lsm_cal->cal_type.cal_info.app_type;
	lsm_table.nBufferLength = block->size;
	lsm_table.nBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_LSM_TABLE\n");

	result = acdb_ioctl(ACDB_CMD_GET_LSM_TABLE,
		(const uint8_t *)&lsm_table, sizeof(lsm_table),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB LSM returned = %d\n", result);
		lsm_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	lsm_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

int acdb_loader_get_ecrx_device(int acdb_id)
{
	int32_t result = INVALID_DATA;
	uint32_t *pRxDevs;
	AcdbAudioRecRxListCmdType acdb_cmd;
	AcdbAudioRecRxListRspType acdb_cmd_response;

	acdb_cmd.nTxDeviceId = acdb_id;
	pRxDevs = malloc(EC_REF_RX_DEVS);
	if (pRxDevs == NULL) {
		LOGE("Error: %s Malloc Failed", __func__);
		return result;
	}
	acdb_cmd_response.pRxDevs = pRxDevs;
	result = acdb_ioctl(ACDB_CMD_GET_AUDIO_RECORD_RX_DEVICE_LIST,
				(const uint8_t *)&acdb_cmd, sizeof(acdb_cmd),
			(uint8_t *)&acdb_cmd_response, sizeof(acdb_cmd_response));
	if (result) {
		LOGE("Error: ACDB EC_REF_RX returned = %d\n", result);
		goto done;
	}

	if (acdb_cmd_response.nNoOfRxDevs) {
		result = acdb_cmd_response.pRxDevs[0];
	}

	done:
	free(pRxDevs);
	return result;
}

static int get_aanctable(struct cal_block *block,
				struct audio_cal_aanc *aanc_cal)
{
	int					result = 0;
	AcdbAANCConfigTableCmdType		aanctable;
	AcdbQueryResponseType			response;

	aanctable.nTxDeviceId = aanc_cal->cal_type.cal_info.acdb_id;
	aanctable.nBufferLength = block->size;
	aanctable.nBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE\n");

	result = acdb_ioctl(ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE,
		(const uint8_t *)&aanctable, sizeof(aanctable),
		(uint8_t *)&response, sizeof(response));
	LOGD("ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE result %d\n",
		result);

	if (result) {
		LOGE("Error: ACDB AANC returned = %d\n", result);
		aanc_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	aanc_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int send_afe_topology(int acdb_id, int path, int buff_num, int cal_type)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_afe_top	afe_top;

	block = get_cal_block(cal_type, buff_num);
	if (block == NULL) {
		LOGE("%s: no cal block for AFE_TOPOLOGY!\n", __func__);
		goto done;
	}
	LOGD("ACDB -> send_afe_topology\n");
	afe_top.hdr.data_size = sizeof(afe_top);
	afe_top.hdr.version = VERSION_0_0;
	afe_top.hdr.cal_type = cal_type;
	afe_top.hdr.cal_type_size = sizeof(afe_top.cal_type);
	afe_top.cal_type.cal_hdr.buffer_number = buff_num;
	afe_top.cal_type.cal_hdr.version = block->version;
	afe_top.cal_type.cal_info.acdb_id = acdb_id;
	afe_top.cal_type.cal_info.path = path;
	afe_top.cal_type.cal_data.mem_handle = block->map_handle;

	result = get_afe_topology(&afe_top);
	if (result) {
		LOGE("Error: get ACDB afe topology result = %d\n", result);
		afe_topology_supported = 0;
		goto done;
	}
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&afe_top);
	if (result) {
		LOGE("Error: Sending ACDB afe topology result = %d\n", result);
		goto done;
	}
	afe_topology_supported = 1;

done:
	return result;
}

static int get_meta_info_size(int metaInfoKey)
{
	AcdbGetMetaInfoSizeCmdType metaInfoSizeCmd = {0};
	AcdbSizeResponseType metaInfoSize = {0};
	int result = 0;

	metaInfoSizeCmd.nKey = metaInfoKey;
	result = acdb_ioctl( ACDB_CMD_GET_META_INFO_SIZE,
		(const uint8_t *) &metaInfoSizeCmd, sizeof(metaInfoSizeCmd),
		(uint8_t *) &metaInfoSize, sizeof(metaInfoSize));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_META_INFO_SIZE returned %d\n",
			result);
		goto done;
	}

	result = metaInfoSize.nSize;
	LOGD("ACDB -> Metainfo size:%d bytes for key:%d\n",
			metaInfoSize.nSize, metaInfoKey);
	if (metaInfoSize.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_META_INFO_SIZE returned 0 bytes\n");
		goto done;
	}
done:
        return result;
}

static int get_meta_info(struct cal_block *block,
				struct audio_core_metainfo *metaInfo,
				int metaInfoKey)
{
	AcdbGetMetaInfoCmdType metaInfoCmd;
	AcdbQueryResponseType response;
	int32_t result = 0;

	metaInfoCmd.nBufferPointer = (uint8_t *)block->vaddr;
	metaInfoCmd.nKey = metaInfoKey;
	metaInfoCmd.nBufferLength = block->size;

	if (metaInfoCmd.nBufferPointer == NULL) {
		LOGE("Fail to allocate memory for metainfo\n");
		result = -ENODEV;
		goto done;
        }

	result = acdb_ioctl( ACDB_CMD_GET_META_INFO,
		(const uint8_t *) &metaInfoCmd, sizeof(metaInfoCmd),
		(uint8_t *) &response, sizeof(response));

	metaInfo->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

int send_meta_info(int metaInfoKey, int key_idx)
{
	AcdbGetMetaInfoCmdType metaInfoCmd;
	AcdbQueryResponseType response;
	int 		metaInfoSize;
	int		reallocate_size = 0;
	int32_t		result = 0;
	uint8_t		*buf;
	struct audio_core_metainfo metaInfo;
	struct cal_block		*block;

	LOGD("ACDB -> send_meta_info enter, key:%d, index:%d\n",
			metaInfoKey, key_idx);

	block = get_cal_block(AUDIO_CORE_METAINFO_CAL_TYPE, key_idx);
	if (block == NULL) {
		LOGE("%s: ACDB -> Error:Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get meta info size from s/w and reallocate cal type in case of insufficient memory */
	metaInfoSize = get_meta_info_size(metaInfoKey);
	if(metaInfoSize > block->size) {
		if (metaInfoSize < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = metaInfoSize;
		LOGD("Reallocate memory for Meta Info to size: %d\n", reallocate_size);
		deallocate_cal_type(AUDIO_CORE_METAINFO_CAL_TYPE, key_idx);
		allocate_cal_type(AUDIO_CORE_METAINFO_CAL_TYPE, reallocate_size, key_idx);
	}

	metaInfo.hdr.data_size = sizeof(metaInfo);
	metaInfo.hdr.version = VERSION_0_0;
	metaInfo.hdr.cal_type = AUDIO_CORE_METAINFO_CAL_TYPE;
	metaInfo.hdr.cal_type_size = sizeof(metaInfo.cal_type);
	metaInfo.cal_type.cal_hdr.version = VERSION_0_0;
	metaInfo.cal_type.cal_hdr.buffer_number = key_idx;
	metaInfo.cal_type.cal_data.mem_handle = block->map_handle;
	metaInfo.cal_type.cal_info.nKey = metaInfoKey;

	result = get_meta_info(block, &metaInfo, metaInfoKey);

	if (result) {
		LOGE("ACDB -> get_meta_info returned error (%d), err:\n",
			result);
		goto done;
	}

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
			&metaInfo);
	if (result) {
		LOGE("Error in sending ACDB Meta Info, result = %d\n", result);
		goto done;
	}

	LOGD("ACDB -> send_meta_info Exit\n");
done:
	return result;
}

int send_meta_info_list(struct listnode *key_list)
{
	int i = BUFF_IDX_0;
	int32_t result = 0, retval = 0;
	struct listnode *node;
	struct meta_key_list *key_info;

	list_for_each(node, key_list) {
		key_info = node_to_item(node, struct meta_key_list, list);
		LOGD("ACDB -> send_meta_info_list key : %d",
			key_info->cal_info.nKey);
		result = send_meta_info(key_info->cal_info.nKey, i++);
		if (result)
			retval = result;
	}
	return retval;
}

static int send_adm_topology(int acdb_id, int path, int app_id, int buff_num,
			     int cal_type)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_adm_top	adm_top;

	block = get_cal_block(cal_type, buff_num);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	LOGD("ACDB -> send_adm_topology\n");
	adm_top.hdr.data_size = sizeof(adm_top);
	adm_top.hdr.version = VERSION_0_0;
	adm_top.hdr.cal_type = cal_type;
	adm_top.hdr.cal_type_size = sizeof(adm_top.cal_type);
	adm_top.cal_type.cal_hdr.buffer_number = buff_num;
	adm_top.cal_type.cal_hdr.version = block->version;
	adm_top.cal_type.cal_info.acdb_id = acdb_id;
	adm_top.cal_type.cal_info.path = path;
	adm_top.cal_type.cal_info.app_type = app_id;
	adm_top.cal_type.cal_data.mem_handle = block->map_handle;

	result = get_adm_topology(&adm_top);
	if (result < 0)
		goto done;

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&adm_top);
	if (result) {
		LOGE("Error: Sending ACDB adm topology result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_asm_topology(int app_id, int buff_num)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_asm_top	asm_top;

	block = get_cal_block(ASM_TOPOLOGY_CAL_TYPE, buff_num);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	LOGD("ACDB -> send_asm_topology\n");
	asm_top.hdr.data_size = sizeof(asm_top);
	asm_top.hdr.version = VERSION_0_0;
	asm_top.hdr.cal_type = ASM_TOPOLOGY_CAL_TYPE;
	asm_top.hdr.cal_type_size = sizeof(asm_top.cal_type);
	asm_top.cal_type.cal_hdr.buffer_number = buff_num;
	asm_top.cal_type.cal_hdr.version = block->version;
	asm_top.cal_type.cal_info.app_type = app_id;
	asm_top.cal_type.cal_data.mem_handle = block->map_handle;

	result = get_asm_topology(&asm_top);
	if (result < 0)
		goto done;

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&asm_top);
	if (result) {
		LOGE("Error: Sending ACDB asm topology result = %d\n", result);
		goto done;
	}
done:
	return result;
}
static int send_audstrmtable(int app_id, int buff_num)
{
	int				result = 0;
	int				audstrmtable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_audstrm	audstrm_cal;

	block = get_cal_block(ASM_AUDSTRM_CAL_TYPE, buff_num);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get audstrmtable size from s/w and reallocate cal type in case of insufficient memory */
	audstrmtable_size = get_audstrmtable_size(app_id);
	if (audstrmtable_size > block->size)
	{
		if (audstrmtable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = audstrmtable_size;
		LOGD("Reallocate memory for AudProc Stream Table to size: %d\n", reallocate_size);
		deallocate_cal_type(ASM_AUDSTRM_CAL_TYPE, buff_num);
		allocate_cal_type(ASM_AUDSTRM_CAL_TYPE, reallocate_size, buff_num);
	}

	LOGD("ACDB -> send_audstrmtable\n");
	audstrm_cal.hdr.data_size = sizeof(audstrm_cal);
	audstrm_cal.hdr.version = VERSION_0_0;
	audstrm_cal.hdr.cal_type = ASM_AUDSTRM_CAL_TYPE;
	audstrm_cal.hdr.cal_type_size = sizeof(audstrm_cal.cal_type);
	audstrm_cal.cal_type.cal_hdr.buffer_number = buff_num;
	audstrm_cal.cal_type.cal_hdr.version = block->version;
	audstrm_cal.cal_type.cal_info.app_type = app_id;
	audstrm_cal.cal_type.cal_data.mem_handle = block->map_handle;

	result = get_audstrmtable(block, &audstrm_cal);
	if (result < 0)
		goto done;

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&audstrm_cal);
	if (result) {
		LOGE("Error: Sending ACDB aud strm table result = %d\n", result);
		goto done;
		}
done:
	return result;
}

static int send_common_custom_topology(void)
{
	int				result = 0;
	int				result2 = 0;
	int				custom_topology_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_basic		common_top;
	LOGD("ACDB -> %s\n", __func__);

	block = get_cal_block(CORE_CUSTOM_TOPOLOGIES_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: no cal block CORE_CUSTOM_TOPOLOGIES!\n", __func__);
		result = -ENODEV;
		goto done;
	}

	/* Get custom topology size from s/w and reallocate cal type in case of insufficient memory */
	custom_topology_size = get_custom_topology_size();
	if (custom_topology_size > block->size)
	{
		if (custom_topology_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = custom_topology_size;
		LOGD("Reallocate memory for Custom Topology to size: %d\n", reallocate_size);
		deallocate_cal_type(CORE_CUSTOM_TOPOLOGIES_CAL_TYPE, BUFF_IDX_0);
		allocate_cal_type(CORE_CUSTOM_TOPOLOGIES_CAL_TYPE, reallocate_size, BUFF_IDX_0);
	}

	common_top.hdr.data_size = sizeof(common_top);
	common_top.hdr.version = VERSION_0_0;
	common_top.hdr.cal_type = CORE_CUSTOM_TOPOLOGIES_CAL_TYPE;
	common_top.hdr.cal_type_size = sizeof(common_top.cal_type);
	common_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	common_top.cal_type.cal_hdr.version = block->version;
	common_top.cal_type.cal_data.mem_handle = block->map_handle;

	result2 = get_common_custom_topology(block, &common_top);
	if (result2 < 0)
		result = result2;

	LOGD("ACDB -> CORE_CUSTOM_TOPOLOGIES\n");

	result2 = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&common_top);
	if (result2) {
		LOGE("AUDIO_SET_CALIBRATION, Error = %d\n", result2);
		result = -EINVAL;
		goto done;
	}
done:
	return result;
}


static int send_afe_custom_topology(void)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_basic		afe_top;
	LOGD("ACDB -> send_afe_custom_topology\n");

	block = get_cal_block(AFE_CUST_TOPOLOGY_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: no cal block AFE_CUST_TOPOLOGY!\n", __func__);
		goto done;
	}
	afe_top.hdr.data_size = sizeof(afe_top);
	afe_top.hdr.version = VERSION_0_0;
	afe_top.hdr.cal_type = AFE_CUST_TOPOLOGY_CAL_TYPE;
	afe_top.hdr.cal_type_size = sizeof(afe_top.cal_type);
	afe_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	afe_top.cal_type.cal_hdr.version = block->version;
	afe_top.cal_type.cal_data.mem_handle = block->map_handle;

	get_afe_custom_topology(block, &afe_top);

	LOGD("ACDB -> AUDIO_SET_AFE_CUSTOM_TOPOLOGY\n");

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&afe_top);
	if (result) {
		LOGE("AUDIO_SET_CALIBRATION, Error = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_adm_custom_topology(void)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_basic		adm_top;
	LOGD("ACDB -> send_adm_custom_topology\n");

	block = get_cal_block(ADM_CUST_TOPOLOGY_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	adm_top.hdr.data_size = sizeof(adm_top);
	adm_top.hdr.version = VERSION_0_0;
	adm_top.hdr.cal_type = ADM_CUST_TOPOLOGY_CAL_TYPE;
	adm_top.hdr.cal_type_size = sizeof(adm_top.cal_type);
	adm_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	adm_top.cal_type.cal_hdr.version = block->version;
	adm_top.cal_type.cal_data.mem_handle = block->map_handle;

	get_adm_custom_topology(block, &adm_top);

	LOGD("ACDB -> AUDIO_SET_ADM_CUSTOM_TOPOLOGY\n");

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&adm_top);
	if (result) {
		LOGE("Error: Sending ACDB ADM topology result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_asm_custom_topology(void)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_basic		asm_top;
	LOGD("ACDB -> send_asm_custom_topology\n");

	block = get_cal_block(ASM_CUST_TOPOLOGY_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	asm_top.hdr.data_size = sizeof(asm_top);
	asm_top.hdr.version = VERSION_0_0;
	asm_top.hdr.cal_type = ASM_CUST_TOPOLOGY_CAL_TYPE;
	asm_top.hdr.cal_type_size = sizeof(asm_top.cal_type);
	asm_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	asm_top.cal_type.cal_hdr.version = block->version;
	asm_top.cal_type.cal_data.mem_handle = block->map_handle;

	get_asm_custom_topology(block, &asm_top);

	LOGD("ACDB -> AUDIO_SET_ASM_CUSTOM_TOPOLOGY\n");

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&asm_top);
	if (result) {
		LOGE("Error: Sending ACDB ASM topology result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_audtable(int acdb_id, int path, int app_id, int sample_rate,
			 int buff_num, int cal_type)
{
	int				result = 0;
	int				audtable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_audproc	audproc_cal;
	LOGD("ACDB -> send_audtable\n");

	block = get_cal_block(cal_type, buff_num);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get audtable size from s/w and reallocate cal type in case of insufficient memory */
	audtable_size = get_audtable_size(acdb_id, app_id, sample_rate);
	if (audtable_size > block->size)
	{
		if (audtable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = audtable_size;

		LOGD("Reallocate memory for AudProc Table to size: %d\n", reallocate_size);
		deallocate_cal_type(cal_type, buff_num);
		allocate_cal_type(cal_type, reallocate_size, buff_num);
	}

	audproc_cal.hdr.data_size = sizeof(audproc_cal);
	audproc_cal.hdr.version = VERSION_0_0;
	audproc_cal.hdr.cal_type = cal_type;
	audproc_cal.hdr.cal_type_size = sizeof(audproc_cal.cal_type);
	audproc_cal.cal_type.cal_hdr.buffer_number = buff_num;
	audproc_cal.cal_type.cal_hdr.version = block->version;
	audproc_cal.cal_type.cal_info.acdb_id = acdb_id;
	audproc_cal.cal_type.cal_info.path = path;
	audproc_cal.cal_type.cal_info.app_type = app_id;
	audproc_cal.cal_type.cal_info.sample_rate = sample_rate;
	audproc_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_audtable(block, &audproc_cal);

	LOGD("ACDB -> AUDIO_SET_AUDPROC_CAL\n");

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&audproc_cal);
	if (result) {
		LOGE("Error: Sending ACDB audproc result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_audvoltable(int acdb_id, int path, int app_id, int vol_index, int cal_type, int buff_num)
{
	int				result = 0;
	int				audvoltable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_audvol		audvol_cal;
	LOGD("ACDB -> send_audvoltable\n");

	block = get_cal_block(cal_type, buff_num);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get audvoltable size from s/w and reallocate cal type in case of insufficient memory */
	audvoltable_size = get_audvoltable_size(acdb_id, app_id, vol_index);
	if (audvoltable_size > block->size)
	{
		if (audvoltable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = audvoltable_size;
		LOGD("Reallocate memory for AudProc Volume Table to size: %d\n", reallocate_size);
		deallocate_cal_type(cal_type, buff_num);
		allocate_cal_type(cal_type, reallocate_size, buff_num);
	}

	audvol_cal.hdr.data_size = sizeof(audvol_cal);
	audvol_cal.hdr.version = VERSION_0_0;
	audvol_cal.hdr.cal_type = cal_type;
	audvol_cal.hdr.cal_type_size = sizeof(audvol_cal.cal_type);
	audvol_cal.cal_type.cal_hdr.buffer_number = buff_num;
	audvol_cal.cal_type.cal_hdr.version = block->version;
	audvol_cal.cal_type.cal_info.acdb_id = acdb_id;
	audvol_cal.cal_type.cal_info.path = path;
	audvol_cal.cal_type.cal_info.app_type = app_id;
	/* 0 is max volume which is default Q6 COPP volume */
	audvol_cal.cal_type.cal_info.vol_index = vol_index;
	audvol_cal.cal_type.cal_data.mem_handle = block->map_handle;
	audvol_cal.cal_type.cal_data.cal_size = 0;

	get_audvoltable(block, &audvol_cal);

	LOGD("ACDB -> AUDIO_SET_VOL_CAL cal type = %d\n", cal_type);
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&audvol_cal);
	if (result) {
		LOGE("Error: Sending ACDB audproc vol result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_aanctable(int acdb_id)
{
	int				result = 0;
	int				aanctable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_aanc		aanc_cal;
	LOGD("ACDB -> send_aanctable\n");

	block = get_cal_block(AFE_AANC_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get aanctable size from s/w and reallocate cal type in case of insufficient memory */
	aanctable_size = get_aanctable_size(acdb_id);
	if (aanctable_size > block->size)
	{
		if (aanctable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = aanctable_size;
		LOGD("Reallocate memory for AANC Table to size: %d\n", reallocate_size);
		deallocate_cal_type(AFE_AANC_CAL_TYPE, BUFF_IDX_0);
		allocate_cal_type(AFE_AANC_CAL_TYPE, reallocate_size, BUFF_IDX_0);
	}

	aanc_cal.hdr.data_size = sizeof(aanc_cal);
	aanc_cal.hdr.version = VERSION_0_0;
	aanc_cal.hdr.cal_type = AFE_AANC_CAL_TYPE;
	aanc_cal.hdr.cal_type_size = sizeof(aanc_cal.cal_type);
	aanc_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	aanc_cal.cal_type.cal_hdr.version = block->version;
	aanc_cal.cal_type.cal_info.acdb_id = acdb_id;
	aanc_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_aanctable(block, &aanc_cal);

	LOGD("ACDB -> AUDIO_SET_AANC_TABLE\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&aanc_cal);
	if (result) {
		LOGE("Error: Sending ACDB AANC result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_afe_cal(int acdb_id, int path, int sample_rate, int cal_type, int buff_num)
{
	int				result = 0;
	int				afetable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_afe		afe_cal;
	LOGD("ACDB -> send_afe_cal\n");

	block = get_cal_block(cal_type, buff_num);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get afetable size from s/w and reallocate cal type in case of insufficient memory */
	afetable_size = get_afetable_size(acdb_id, sample_rate);
	if (afetable_size > block->size)
	{
		if (afetable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = afetable_size;
		LOGD("Reallocate memory for AFE Table to size: %d\n", reallocate_size);
		deallocate_cal_type(cal_type, buff_num);
		allocate_cal_type(cal_type, reallocate_size, buff_num);
	}

	afe_cal.hdr.data_size = sizeof(afe_cal);
	afe_cal.hdr.version = VERSION_0_0;
	afe_cal.hdr.cal_type = cal_type;
	afe_cal.hdr.cal_type_size = sizeof(afe_cal.cal_type);
	afe_cal.cal_type.cal_hdr.buffer_number = buff_num;
	afe_cal.cal_type.cal_hdr.version = block->version;
	afe_cal.cal_type.cal_info.acdb_id = acdb_id;
	afe_cal.cal_type.cal_info.path = path;
	afe_cal.cal_type.cal_info.sample_rate = sample_rate;
	afe_cal.cal_type.cal_data.mem_handle = block->map_handle;
	afe_cal.cal_type.cal_data.cal_size = 0;

	get_afetable(block, &afe_cal);

	LOGD("ACDB -> AUDIO_SET_AFE_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&afe_cal);
	if (result) {
		LOGE("Error: Sending ACDB AFE result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_hw_delay(int acdb_id, int path, int buff_num)
{
	int32_t				result = 0;
	struct cal_block		*block;
	struct audio_cal_hw_delay	hw_delay_cal;

	LOGD("ACDB -> send_hw_delay : acdb_id = %d path = %d\n",
	     acdb_id, path);

	block = get_cal_block(AFE_HW_DELAY_CAL_TYPE, buff_num);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	hw_delay_cal.hdr.data_size = sizeof(hw_delay_cal);
	hw_delay_cal.hdr.version = VERSION_0_0;
	hw_delay_cal.hdr.cal_type = AFE_HW_DELAY_CAL_TYPE;
	hw_delay_cal.hdr.cal_type_size = sizeof(hw_delay_cal.cal_type);
	hw_delay_cal.cal_type.cal_hdr.buffer_number = buff_num;
	hw_delay_cal.cal_type.cal_hdr.version = block->version;
	hw_delay_cal.cal_type.cal_info.acdb_id = acdb_id;
	hw_delay_cal.cal_type.cal_info.path = path;
	hw_delay_cal.cal_type.cal_info.property_type = ACDB_AVSYNC_INFO;
	hw_delay_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_hw_delay(block, &hw_delay_cal);

	if (hw_delay_cal.cal_type.cal_data.cal_size != 0) {
		hw_delay_cal.hdr.data_size = sizeof(hw_delay_cal) -
			sizeof(hw_delay_cal.cal_type.cal_info.data) +
			hw_delay_cal.cal_type.cal_data.cal_size;
		hw_delay_cal.hdr.cal_type_size = sizeof(hw_delay_cal.cal_type) -
			sizeof(hw_delay_cal.cal_type.cal_info.data) +
			hw_delay_cal.cal_type.cal_data.cal_size;
	}

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
				   &hw_delay_cal);
	if (result)
		LOGE("Error: Sending ACDB AFE result = %d errno=%d\n",
		      result, errno);
done:
	return result;
}

static int send_lsm_custom_topology(void)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_basic		lsm_top;
	LOGD("ACDB -> send_lsm_custom_topology\n");

	block = get_cal_block(LSM_CUST_TOPOLOGY_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	lsm_top.hdr.data_size = sizeof(lsm_top);
	lsm_top.hdr.version = VERSION_0_0;
	lsm_top.hdr.cal_type = LSM_CUST_TOPOLOGY_CAL_TYPE;
	lsm_top.hdr.cal_type_size = sizeof(lsm_top.cal_type);
	lsm_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	lsm_top.cal_type.cal_hdr.version = block->version;
	lsm_top.cal_type.cal_data.mem_handle = block->map_handle;

	get_lsm_custom_topology(block, &lsm_top);

	LOGD("ACDB -> AUDIO_SET_LSM_CUSTOM_TOPOLOGY\n");

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&lsm_top);
	if (result) {
		LOGE("Error: Sending ACDB LSM topology result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_lsm_topology(int acdb_id, int app_id, int mode, int cal_type)
{
	int				result = 0;
	int				result2 = 0;
	struct cal_block		*block;
	struct audio_cal_lsm_top	lsm_top;

	block = get_cal_block(cal_type, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	LOGD("ACDB -> send_lsm_topology\n");
	lsm_top.hdr.data_size = sizeof(lsm_top);
	lsm_top.hdr.version = VERSION_0_0;
	lsm_top.hdr.cal_type = cal_type;
	lsm_top.hdr.cal_type_size = sizeof(lsm_top.cal_type);
	lsm_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	lsm_top.cal_type.cal_hdr.version = block->version;
	lsm_top.cal_type.cal_info.acdb_id = acdb_id;
	lsm_top.cal_type.cal_info.app_type = app_id;
	lsm_top.cal_type.cal_data.mem_handle = block->map_handle;

	if (mode == LISTEN_MODE_NO_TOPOLOGY) {
                /* topology value 0 is not a valid topology. Driver can
                fallback to non-topology interface based on this value. */
		lsm_top.cal_type.cal_info.topology = 0;
	} else {
		result2 = get_lsm_topology(&lsm_top);
		if (result2 < 0) {
			/* return only failure code of get LSM topology to STHAL */
			/* so it knows to fallback to non-topology interface */
			result = result2;
			goto done;
		}
	}

	result2 = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&lsm_top);
	if (result2) {
		LOGE("Error: Sending ACDB LSM topology result = %d\n", result2);
		goto done;
	}
done:
	return result;
}

static int send_lsm_cal(int acdb_id, int app_id, int cal_type)
{
	int				result = 0;
	int				lsmtable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_lsm		lsm_cal;
	LOGD("ACDB -> get_lsm_table\n");

	block = get_cal_block(cal_type, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get lsmtable size from s/w and reallocate cal type in case of insufficient memory */
	lsmtable_size = get_lsmtable_size(acdb_id, app_id);
	if (lsmtable_size > block->size)
	{
		if (lsmtable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = lsmtable_size;
		LOGD("Reallocate memory for LSM Table to size: %d\n", reallocate_size);
		deallocate_cal_type(cal_type, BUFF_IDX_0);
		allocate_cal_type(cal_type, reallocate_size, BUFF_IDX_0);
	}

	lsm_cal.hdr.data_size = sizeof(lsm_cal);
	lsm_cal.hdr.version = VERSION_0_0;
	lsm_cal.hdr.cal_type = cal_type;
	lsm_cal.hdr.cal_type_size = sizeof(lsm_cal.cal_type);
	lsm_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	lsm_cal.cal_type.cal_hdr.version = block->version;
	lsm_cal.cal_type.cal_info.acdb_id = acdb_id;
	lsm_cal.cal_type.cal_info.app_type = app_id;
	lsm_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_lsm_table(block, &lsm_cal);

	LOGD("ACDB -> AUDIO_SET_LSM_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&lsm_cal);
	if (result) {
		LOGE("Error: Sending ACDB LSM result = %d\n", result);
		goto done;
	}
done:
	return result;
}

int send_codec_cal(struct param_data *params)
{
	int				result = 0;
	AcdbCodecCalDataCmdType		codec_table;
	AcdbQueryResponseType		response;

	LOGE("ACDB -> send_codec_cal\n");
	codec_table.nDeviceID = params->acdb_id;
	codec_table.nCodecFeatureType = ACDB_WCD9320_MAD;
	codec_table.nBufferLength = params->buff_size;
	codec_table.pBufferPointer = (uint8_t *)params->buff;

	LOGE("ACDB -> ACDB_CMD_GET_CODEC_CAL_DATA\n");

	result = acdb_ioctl(ACDB_CMD_GET_CODEC_CAL_DATA,
		(const uint8_t *)&codec_table, sizeof(codec_table),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB CODEC CAL returned = %d\n", result);
	}
	params->data_size = (int)response.nBytesUsedInBuffer;
done:
	return result;
}


void acdb_loader_send_audio_cal_v3(int acdb_id, int capability, int app_id, int sample_rate, int use_case)
{
	int				path;
	int				aud_vol_idx = 0;
	struct aud_vol_idx_node		*node;
	int				cal_type;
	int				buffer_idx = use_case;
	int				buffer_idx_w_path;

	if (!is_initialized) {
		LOGE("ACDB -> Not correctly initialized!\n");
		goto done;
	}

	path = get_audcal_path((uint32_t)capability);
	if (path == INVALID_DATA) {
		LOGE("ACDB -> Device is not RX or TX!"
			"acdb_id = %d\n", acdb_id);
		goto done;
	}
	buffer_idx_w_path = path + MAX_PATH_TYPE * use_case;

	LOGD("ACDB -> send_audio_cal, acdb_id = %d, path = %d, app id = 0x%x, sample rate = %d\n",
		acdb_id, path, app_id, sample_rate);

	send_asm_topology(app_id, buffer_idx);
	send_adm_topology(acdb_id, path, app_id, buffer_idx_w_path,
			  ADM_TOPOLOGY_CAL_TYPE);
	send_audtable(acdb_id, path, app_id, sample_rate, buffer_idx_w_path,
		      ADM_AUDPROC_CAL_TYPE);

	node = get_aud_vol_node(acdb_id, app_id);
	if ((node != NULL) && (node->aud_vol_idx >= 0))
		aud_vol_idx = node->aud_vol_idx;
	send_audvoltable(acdb_id, path, app_id, aud_vol_idx, ADM_AUDVOL_CAL_TYPE,
		buffer_idx_w_path);
	send_audstrmtable(app_id, buffer_idx_w_path);

	cal_type = (path == TX_DEVICE) ?
			AFE_COMMON_TX_CAL_TYPE : AFE_COMMON_RX_CAL_TYPE;
	send_afe_topology(acdb_id, path, buffer_idx_w_path, AFE_TOPOLOGY_CAL_TYPE);
	send_afe_cal(acdb_id, path, sample_rate, cal_type, buffer_idx);
	send_hw_delay(acdb_id, path, buffer_idx_w_path);
done:
	return;
}

void acdb_loader_send_audio_cal_v2(int acdb_id, int capability, int app_id, int sample_rate)
{
	acdb_loader_send_audio_cal_v3(acdb_id, capability, app_id, sample_rate, BUFF_IDX_0);
	return;
}

void acdb_loader_send_audio_cal(int acdb_id, int capability)
{
	acdb_loader_send_audio_cal_v2(acdb_id, capability, acdb_loader_get_default_app_type(), get_samplerate(acdb_id));
	return;
}

void acdb_loader_send_listen_device_cal(int acdb_id, int type, int app_id, int sample_rate)
{
	int cal_type;
	int buffer_idx_w_path = TX_DEVICE + MAX_PATH_TYPE * BUFF_IDX_0;

#ifdef LSM_CAL_TYPES
        /* send adm cal only for ADSP type */
        if (type != LISTEN_HW_TYPE_CPE) {
            send_adm_topology(acdb_id, TX_DEVICE, app_id, buffer_idx_w_path,
			      ADM_LSM_TOPOLOGY_CAL_TYPE);
            send_audtable(acdb_id, TX_DEVICE, app_id, sample_rate,
			  buffer_idx_w_path, ADM_LSM_AUDPROC_CAL_TYPE);
        }

	cal_type = (type == LISTEN_HW_TYPE_CPE) ?
			ULP_AFE_CAL_TYPE : AFE_LSM_TX_CAL_TYPE;
	send_afe_topology(acdb_id, TX_DEVICE, buffer_idx_w_path, AFE_LSM_TOPOLOGY_CAL_TYPE);
#else
        /* send adm cal only for ADSP type */
        if (type != LISTEN_HW_TYPE_CPE) {
            send_adm_topology(acdb_id, TX_DEVICE, app_id, buffer_idx_w_path,
			      ADM_TOPOLOGY_CAL_TYPE);
            send_audtable(acdb_id, TX_DEVICE, app_id, sample_rate,
			  buffer_idx_w_path, ADM_AUDPROC_CAL_TYPE);
        }

	cal_type = (type == LISTEN_HW_TYPE_CPE) ?
			ULP_AFE_CAL_TYPE : AFE_COMMON_TX_CAL_TYPE;
	send_afe_topology(acdb_id, TX_DEVICE, buffer_idx_w_path, AFE_TOPOLOGY_CAL_TYPE);
#endif
	send_afe_cal(acdb_id, TX_DEVICE, get_samplerate(acdb_id), cal_type, BUFF_IDX_0);
}

int acdb_loader_send_listen_lsm_cal(int acdb_id, int app_id, int mode, int type)
{
	int result = 0;
	int cal_type;

	cal_type = (type == LISTEN_HW_TYPE_CPE) ?
			ULP_LSM_TOPOLOGY_ID_CAL_TYPE : LSM_TOPOLOGY_CAL_TYPE;
	/* Only return Get LSM topology error code */
	/* so STHAL knows when to fallback to non-topology interface */
	result = send_lsm_topology(acdb_id, app_id, mode, cal_type);

	cal_type = (type == LISTEN_HW_TYPE_CPE) ?
			ULP_LSM_CAL_TYPE : LSM_CAL_TYPE;
	send_lsm_cal(acdb_id, app_id, cal_type);

	return result;
}

static int get_anc_table(int acdb_id, struct cal_block *block, int adie_pid)
{
	int result = -EINVAL;
	AcdbCodecANCSettingCmdType acdb_cmd;
	AcdbQueryResponseType response;

	uint32_t acdb_command_id = ACDB_CMD_GET_CODEC_ANC_SETTING;
	acdb_cmd.nRxDeviceId = acdb_id;
	acdb_cmd.nParamId = adie_pid;
	acdb_cmd.nBufferLength = block->size;
	acdb_cmd.nBufferPointer = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_ANC_SETTING\n");

	result = acdb_ioctl(acdb_command_id,
		(const uint8_t *)&acdb_cmd, sizeof(acdb_cmd),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB ANC returned = %d\n", result);
		goto done;
	}
done:
	return result;
}


void send_mbhc_data(struct param_data *params)
{
	int result;
	AcdbGblTblCmdType global_cmd;
	AcdbQueryResponseType   response;

	LOGD("send mbhc data\n");
	params->data_size = 0;

	global_cmd.nModuleId = ACDB_MID_MBHC;
	global_cmd.nParamId = ACDB_PID_GENERAL_CONFIG;
	global_cmd.nBufferLength = params->buff_size;
	global_cmd.nBufferPointer = (uint8_t *)params->buff;

	LOGD("ACDB -> MBHC ACDB_PID_GENERAL_CONFIG\n");
	result = acdb_ioctl(ACDB_CMD_GET_GLBTBL_DATA,
		(const uint8_t *) &global_cmd, sizeof(global_cmd),
		(uint8_t *) &response, sizeof(response));

	if (result)
	{
		LOGE("Error reading MBHC general config returned = %x\n", result);
		goto acdb_error;
	}

	params->data_size += response.nBytesUsedInBuffer;
	global_cmd.nBufferLength = params->buff_size - params->data_size;
	global_cmd.nBufferPointer = (uint8_t *)params->buff + params->data_size;
	global_cmd.nParamId = ACDB_PID_PLUG_REMOVAL_DETECTION;

	LOGD("ACDB -> MBHC ACDB_PID_PLUG_REMOVAL_DETECTION\n");
	result = acdb_ioctl(ACDB_CMD_GET_GLBTBL_DATA,
		(const uint8_t *) &global_cmd, sizeof(global_cmd),
		(uint8_t *) &response, sizeof(response));

	if (result)
	{
		LOGE("Error reading MBHC removal config returned = %x\n", result);
		goto acdb_error;
	}

	params->data_size += response.nBytesUsedInBuffer;
	global_cmd.nBufferLength = params->buff_size - params->data_size;
	global_cmd.nBufferPointer = (uint8_t *)params->buff + params->data_size;
	global_cmd.nParamId = ACDB_PID_PLUG_TYPE_DETECTION;

	LOGD("ACDB -> MBHC ACDB_PID_PLUG_TYPE_DETECTION\n");
	result = acdb_ioctl(ACDB_CMD_GET_GLBTBL_DATA,
		(const uint8_t *) &global_cmd, sizeof(global_cmd),
		(uint8_t *) &response, sizeof(response));

	if (result)
	{
		LOGE("Error reading MBHC plug type config returned = %x\n", result);
		goto acdb_error;
	}

	params->data_size += response.nBytesUsedInBuffer;
	global_cmd.nBufferLength = params->buff_size - params->data_size;
	global_cmd.nBufferPointer = (uint8_t *)params->buff + params->data_size;
	global_cmd.nParamId = ACDB_PID_BUTTON_PRESS_DETECTION;

	LOGD("ACDB -> MBHC ACDB_PID_BUTTON_PRESS_DETECTION\n");
	result = acdb_ioctl(ACDB_CMD_GET_GLBTBL_DATA,
		(const uint8_t *) &global_cmd, sizeof(global_cmd),
		(uint8_t *) &response, sizeof(response));

	if (result)
	{
		LOGE("Error reading MBHC button press config returned = %x\n", result);
		goto acdb_error;
	}

	params->data_size += response.nBytesUsedInBuffer;
	global_cmd.nBufferLength = params->buff_size - params->data_size;
	global_cmd.nBufferPointer = (uint8_t *)params->buff + params->data_size;
	global_cmd.nParamId = ACDB_PID_IMPEDANCE_DETECTION;

	LOGD("ACDB -> MBHC ACDB_PID_IMPEDANCE_DETECTION\n");
	result = acdb_ioctl(ACDB_CMD_GET_GLBTBL_DATA,
		(const uint8_t *) &global_cmd, sizeof(global_cmd),
		(uint8_t *) &response, sizeof(response));

	if (result)
	{
		LOGE("Error reading MBHC impedance config returned = %x\n", result);
		goto acdb_error;
	}

	params->data_size += response.nBytesUsedInBuffer;

acdb_error:
	return;
}


void send_wcd9xxx_anc_data(struct param_data *params)
{
	uint32_t anc_configurations = NUM_OF_ANC_RX_CONFIG +
					NUM_OF_AANC_RX_CONFIG +
					NUM_OF_ANC_TUNING_CONFIG;
	uint32_t anc_base_configuration = 26;
	uint32_t anc_reserved[3] = {0, 0, 0};
	int i;

	params->data_size = 0;

	memcpy(((uint8_t *)params->buff + params->data_size), anc_reserved, sizeof(uint32_t) * 3);
	params->data_size += sizeof(uint32_t) * 3;

	memcpy(((uint8_t *)params->buff + params->data_size), &anc_configurations, sizeof(uint32_t));
	params->data_size += sizeof(uint32_t);

	for (i = 0; i < NUM_OF_ANC_RX_CONFIG; i++) {
		params->acdb_id = i + anc_base_configuration;
		send_wcd9xxx_anc_cal(params);
	}

	/* Add the AANC Rx device */
	params->acdb_id = DEVICE_HANDSET_RX_AANC_ACDB_ID;
	send_wcd9xxx_anc_cal(params);

	/* Add AANC tuning Rx devices */
	params->acdb_id = DEVICE_ANC_TEST_S_PATH_HANDSET_SPKR_ANC_MONO_ACDB_ID;
	send_wcd9xxx_anc_cal(params);

	params->acdb_id = DEVICE_ANC_TEST_E_PATH_HANDSET_SPKR_ANC_MONO_ACDB_ID;
	send_wcd9xxx_anc_cal(params);
done:
	return;
}

static int convert_vbat_data_to_wcd9xxx(int *pVbatacdbCfg, struct param_data *params)
{
	vbat_monitor_reg_t vbat_reg_config;
	int result;
	void *Vbatadc_data;

	acdb_loader_get_codec_data(&Vbatadc_data, "vbat_cal");

	result = vbat_conversion(&vbat_reg_config, pVbatacdbCfg, (vbat_adc_data_t *) Vbatadc_data);

	memcpy(((uint8_t *)params->buff + params->data_size), &vbat_reg_config.size, sizeof(vbat_reg_config.size));
	params->data_size += sizeof(vbat_reg_config.size);

	memcpy(((uint8_t *)params->buff + params->data_size), vbat_reg_config.writes, (vbat_reg_config.size * CODEC_PACKED_REG_SIZE));
	params->data_size += vbat_reg_config.size * CODEC_PACKED_REG_SIZE;

	return result;
}

static void send_vbat_data(struct param_data *params)
{

	int result;
	AcdbGblTblCmdType global_cmd;
	AcdbQueryResponseType   response;
	int *vbat_temp_buff = malloc(sizeof(struct vbat_adc_cal_cfg_t) + sizeof(struct vbat_gain_proc_cfg_t));
	int vbat_buff_size = sizeof(struct vbat_adc_cal_cfg_t) + sizeof(struct vbat_gain_proc_cfg_t);
	int vbat_cal_size = 0;
	LOGD("send vbat data\n");
	params->data_size = 0;
	if ( vbat_temp_buff == NULL)
	{
		LOGE("Error allocating temp buffer \n");
		goto acdb_error;
	}
	global_cmd.nModuleId = ACDB_MID_VBAT_MONITOR;
	global_cmd.nParamId = ACDB_PID_ADC_CAL ;
	global_cmd.nBufferLength = vbat_buff_size;
	global_cmd.nBufferPointer = (uint8_t *)vbat_temp_buff;

	LOGD("ACDB -> VBAT ACDB_PID_ADC_CAL\n");
	result = acdb_ioctl(ACDB_CMD_GET_GLBTBL_DATA,
		(const uint8_t *) &global_cmd, sizeof(global_cmd),
		(uint8_t *) &response, sizeof(response));

	if (result)
	{
		LOGE("Error reading VBAT ADC CAL returned = %x\n", result);
		goto acdb_error;
	}

	vbat_cal_size += response.nBytesUsedInBuffer;
	global_cmd.nBufferLength = vbat_buff_size - vbat_cal_size;
	global_cmd.nBufferPointer = (uint8_t *)vbat_temp_buff + vbat_cal_size;
	global_cmd.nParamId = ACDB_PID_GAIN_PROC;

	LOGD("ACDB -> VBAT ACDB_PID_GAIN_PROC\n");
	result = acdb_ioctl(ACDB_CMD_GET_GLBTBL_DATA,
		(const uint8_t *) &global_cmd, sizeof(global_cmd),
		(uint8_t *) &response, sizeof(response));

	if (result)
	{
		LOGE("Error reading VBAT GAIN PROC returned = %x\n", result);
		goto acdb_error;
	}

	vbat_cal_size += response.nBytesUsedInBuffer;

	LOGD("send vbat data, calling convert_vbat_data\n");
	result = convert_vbat_data_to_wcd9xxx(vbat_temp_buff, params);
	if (result)
	{
		LOGE("Error converting vbat data to wcd9xxx data = %x\n", result);
	}

	LOGD("copied vbat cal size =%d\n",params->data_size);
	free(vbat_temp_buff);
acdb_error:
	return;
}

int convert_anc_data_to_wcd9xxx(int *pANCCfg,
				struct param_data *params, int anc_version)
{
	anc_reg_t anc_config;
	int result;

	result = anc_conversion(&anc_config, pANCCfg, anc_version);

	memcpy(((uint8_t *)params->buff + params->data_size), &anc_config.size, sizeof(anc_config.size));
	params->data_size += sizeof(anc_config.size);

	memcpy(((uint8_t *)params->buff + params->data_size), anc_config.writes, (anc_config.size * CODEC_PACKED_REG_SIZE));
	params->data_size += anc_config.size * CODEC_PACKED_REG_SIZE;

	return result;
}

int send_wcd9xxx_anc_cal(struct param_data *params)
{
	int					result;
	struct cal_block			*block;
	int					*ancCfg;
	int					anc_version;

	block = get_cal_block(AFE_ANC_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	result = get_anc_table(params->acdb_id, block, ACDB_PID_CODEC_ANC_DATA_HW_V5);
	if (!result) {
		anc_version = ANC_V2;
	} else {
		result = get_anc_table(params->acdb_id, block, ACDB_PID_CODEC_ANC_DATA_HW_V4);
		if (!result)
			anc_version = ANC_V1;
		else {
			result = get_anc_table(params->acdb_id, block,
					       ACDB_PID_CODEC_ANC_DATA_WCD9320);
			if (!result)
				anc_version =  ANC_V0;
			else
				return result;
		}
	}

	LOGD("%s: anc_version = %d\n", __func__, anc_version);
	ancCfg = block->vaddr;
	convert_anc_data_to_wcd9xxx(ancCfg, params, anc_version);
done:
	return 0;
}

static uint32_t get_voice_topology(struct audio_cal_voc_top *voc_top)
{
	int				result = 0;
	AcdbGetVocProcTopIdCmdType	acdb_get_top;
	AcdbGetTopologyIdRspType	audio_top;

	acdb_get_top.nDeviceId = voc_top->cal_type.cal_info.acdb_id;

	LOGD("ACDB -> ACDB_CMD_GET_VOCPROC_COMMON_TOPOLOGY_ID\n");

	result = acdb_ioctl(ACDB_CMD_GET_VOCPROC_COMMON_TOPOLOGY_ID,
		(const uint8_t *)&acdb_get_top, sizeof(acdb_get_top),
		(uint8_t *)&audio_top, sizeof(audio_top));
	if (result) {
		LOGE("Error: ACDB get voice rx topology for acdb id = %d, returned = %d\n",
		     voc_top->cal_type.cal_info.acdb_id, result);
		voc_top->cal_type.cal_info.topology = 0;
		goto err;
	}
	voc_top->cal_type.cal_info.topology = audio_top.nTopologyId;
err:
	return result;
}

static int get_voice_channel_config(struct audio_cal_voc_top *voc_top)
{
	int					result = 0;
#ifdef TOPOLOGY_SPECIFIC_CHANNEL_INFO
	AcdbDeviceInfoTargetSpecificCmdType	acdb_get_ts_info;
	AcdbQueryResponseType			response;
	struct target_specific_info		ts_info = {0};

	acdb_get_ts_info.nDeviceId = voc_top->cal_type.cal_info.acdb_id;
	acdb_get_ts_info.nDeviceType = ACDB_SIMPLE_DEVICE_TYPE;
	acdb_get_ts_info.nBufferLength = sizeof(ts_info);
	acdb_get_ts_info.pBufferPointer = (uint8_t *)&ts_info;

	LOGD("ACDB -> ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO\n");
	result = acdb_ioctl(ACDB_CMD_GET_TARGET_SPECIFIC_DEVICE_INFO,
			    (const uint8_t *)&acdb_get_ts_info, sizeof(acdb_get_ts_info),
			    (uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB get target specific info for acdb id = %d, returned = %d\n",
		     voc_top->cal_type.cal_info.acdb_id, result);
		result = -EINVAL;
		goto done;
	}
	voc_top->cal_type.cal_info.num_channels = ts_info.channel_config;
done:
#endif
	return result;
}

static int get_voice_channel_mapping(struct audio_cal_voc_top *voc_top)
{
	int				i, result = 0;
#ifdef TOPOLOGY_SPECIFIC_CHANNEL_INFO
	AcdbQueryResponseType		response;
	AcdbDevPropCmdType		voice_ch_map;
	struct voice_channel_mapping	ch_map = {0};

	voice_ch_map.nDeviceId = voc_top->cal_type.cal_info.acdb_id;
	voice_ch_map.nPropID = ACDB_CHANNEL_MAPPING_CONFIG;
	voice_ch_map.nBufferLength = sizeof(ch_map);
	voice_ch_map.pBufferPointer = (uint8_t *)&ch_map;

	LOGD("ACDB -> ACDB_CHANNEL_MAPPING_CONFIG: GET_DEVICE_PROPERTY\n");

	result = acdb_ioctl(ACDB_CMD_GET_DEVICE_PROPERTY,
		(const uint8_t *)&voice_ch_map, sizeof(voice_ch_map),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB ACDB_CMD_GET_DEVICE_PROPERTY error = %d\n",
		     result);
		result = -EINVAL;
		goto done;
	}

	for (i = 0; i < VSS_MAX_CHANNELS; i++) {
		voc_top->cal_type.cal_info.channel_mapping[i] = (uint8_t)ch_map.channel_map[i];
        }

done:
#endif
	return result;
}

static int get_sidetone(struct audio_cal_sidetone *sidetone_cal)
{
	int				result = 0;
	AcdbAfeDataCmdType		sidetone;
	AcdbQueryResponseType		response;

	sidetone.nTxDeviceId = sidetone_cal->cal_type.cal_info.tx_acdb_id;
	sidetone.nRxDeviceId = sidetone_cal->cal_type.cal_info.rx_acdb_id;
	sidetone.nModuleId = sidetone_cal->cal_type.cal_info.mid;
	sidetone.nParamId = sidetone_cal->cal_type.cal_info.pid;
	sidetone.nBufferLength = sizeof(sidetone_cal->cal_type.cal_info.enable) +
					sizeof(sidetone_cal->cal_type.cal_info.gain);
	sidetone.nBufferPointer = (uint8_t *)&sidetone_cal->cal_type.cal_info;

	LOGD("ACDB -> ACDB_CMD_GET_AFE_DATA\n");

	result = acdb_ioctl(ACDB_CMD_GET_AFE_DATA,
		(const uint8_t *)&sidetone, sizeof(sidetone),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB AFE DATA Returned = %d\n", result);
		sidetone_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	sidetone_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

#ifdef AFE_SIDETONE_IIR_CAL_TYPE
static int get_sidetone_iir_config(struct audio_cal_sidetone_iir *sidetone_iir_cal)
{
	int32_t				result = 0;
	AcdbAfeDataCmdType		sidetone;
	AcdbQueryResponseType		response;
	uint16_t			sidetone_iir_enable[2] = {0,0};
	struct sidetone_iir_config *psidetone_iir_config = NULL;

	sidetone.nTxDeviceId = sidetone_iir_cal->cal_type.cal_info.tx_acdb_id;
	sidetone.nRxDeviceId = sidetone_iir_cal->cal_type.cal_info.rx_acdb_id;
	sidetone.nModuleId = sidetone_iir_cal->cal_type.cal_info.mid;
	sidetone.nParamId = ACDB_PID_SIDETONE_IIR_ENABLE;
	sidetone.nBufferLength = sizeof(sidetone_iir_enable);
	sidetone.nBufferPointer = (uint8_t *)&sidetone_iir_enable;

	result = acdb_ioctl(ACDB_CMD_GET_AFE_DATA,
		(const uint8_t *)&sidetone, sizeof(sidetone),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("%s Error: ACDB AFE DATA Returned = %d\n", __func__, result);
		goto done;
	}

	/* Fetching iir_enable requires fields enable and reserved of size
	* 16bits each.
	*/
	sidetone_iir_cal->cal_type.cal_info.iir_enable = sidetone_iir_enable[0];

	psidetone_iir_config = (struct sidetone_iir_config *)malloc(sizeof(struct sidetone_iir_config));
	if(!psidetone_iir_config) {
		LOGE("%s Error: ACDB No memory", __func__);
		result = -ENOMEM;
		goto done;
	}

	sidetone.nParamId = ACDB_PID_SIDETONE_IIR_FILTER_CONFIG ;
	sidetone.nBufferLength = sizeof(struct sidetone_iir_config);
	sidetone.nBufferPointer = (uint8_t *)psidetone_iir_config;

	result = acdb_ioctl(ACDB_CMD_GET_AFE_DATA,
		(const uint8_t *)&sidetone, sizeof(sidetone),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB AFE DATA Returned = %d\n", result);
		goto done;
	}
	memcpy(sidetone_iir_cal->cal_type.cal_info.iir_config,
               psidetone_iir_config->iir_config,
               response.nBytesUsedInBuffer - 2 * sizeof(psidetone_iir_config->num_biquad_stages));
	sidetone_iir_cal->cal_type.cal_info.num_biquad_stages = psidetone_iir_config->num_biquad_stages;
	sidetone_iir_cal->cal_type.cal_info.pregain = psidetone_iir_config->pregain;
	sidetone_iir_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;

done:
	if(psidetone_iir_config)
		free(psidetone_iir_config);
	return result;
}
#endif

static int get_voice_columns(struct cal_block *block,
				struct audio_cal_voc_col *voc_col_cal)
{
	int				result = 0;
	AcdbVocColumnsInfoCmdType	voc_col;
	AcdbVocColumnsInfoCmdType_v2	voc_col_v2;
	AcdbQueryResponseType		response;

	if (block->version & PER_VOCODER_CAL_BIT_MASK) {
		voc_col_v2.nTableId = voc_col_cal->cal_type.cal_info.table_id;
		voc_col_v2.nTxDeviceId = voc_col_cal->cal_type.cal_info.tx_acdb_id;
		voc_col_v2.nRxDeviceId = voc_col_cal->cal_type.cal_info.rx_acdb_id;
		voc_col_v2.nBufferLength = sizeof(voc_col_cal->cal_type.cal_info.data);
		voc_col_v2.pBuff = (void *)&voc_col_cal->cal_type.cal_info.data;

		LOGD("ACDB -> ACDB_CMD_GET_VOC_COLUMNS_INFO_V2\n");

		result = acdb_ioctl(ACDB_CMD_GET_VOC_COLUMNS_INFO_V2,
			(const uint8_t *)&voc_col_v2, sizeof(voc_col_v2),
			(uint8_t *)&response, sizeof(response));
		if (result) {
			LOGE("Error: ACDB VOC COL V2 Returned = %d\n", result);
			voc_col_cal->cal_type.cal_data.cal_size = 0;
			goto done;
		}
	} else {
		voc_col.nTableId = voc_col_cal->cal_type.cal_info.table_id;
		voc_col.nBufferLength = block->size;
		voc_col.pBuff = (uint8_t *)block->vaddr;

		LOGD("ACDB -> ACDB_CMD_GET_VOC_COLUMNS_INFO\n");

		result = acdb_ioctl(ACDB_CMD_GET_VOC_COLUMNS_INFO,
			(const uint8_t *)&voc_col, sizeof(voc_col),
			(uint8_t *)&response, sizeof(response));
		if (result) {
			LOGE("Error: ACDB VOC COL Returned = %d\n", result);
			voc_col_cal->cal_type.cal_data.cal_size = 0;
			goto done;
		}
	}

	voc_col_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_col_cal_type(uint32_t table_id)
{
	int	table = CVP_VOCPROC_STATIC_COL_CAL_TYPE;

	switch (table_id) {
	case ACDB_VOC_PROC_TABLE_V2:
	case ACDB_VOC_PROC_STAT_TABLE_V2:
		table = CVP_VOCPROC_STATIC_COL_CAL_TYPE;
		break;
	case ACDB_VOC_PROC_VOL_TABLE_V2:
	case ACDB_VOC_PROC_DYN_TABLE_V2:
		table = CVP_VOCPROC_DYNAMIC_COL_CAL_TYPE;
		break;
	case ACDB_VOC_STREAM_TABLE_V2:
	case ACDB_VOC_STREAM2_TABLE_V2:
		table = CVS_VOCSTRM_STATIC_COL_CAL_TYPE;
		break;
	default:
		LOGE("%s: Error: %d not a valid table ID!\n",
			__func__, table_id);
	}
	return table;
}

static int send_voice_columns(uint32_t rxacdb_id,
	   uint32_t txacdb_id, uint32_t table_id)
{
	int		result = 0;
	struct cal_block		*block;
	struct audio_cal_voc_col	voc_col;
	LOGD("ACDB -> send_voice_columns, rxacdb_id %d, txacdb_id %d, table %d\n",
		   rxacdb_id, txacdb_id, table_id);

	block = get_cal_block(get_col_cal_type(table_id), BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	voc_col.hdr.data_size = sizeof(voc_col);
	voc_col.hdr.version = VERSION_0_0;
	voc_col.hdr.cal_type = get_col_cal_type(table_id);
	voc_col.hdr.cal_type_size = sizeof(voc_col.cal_type);
	voc_col.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	voc_col.cal_type.cal_hdr.version = block->version;
	voc_col.cal_type.cal_info.table_id = table_id;
	voc_col.cal_type.cal_info.rx_acdb_id = rxacdb_id;
	voc_col.cal_type.cal_info.tx_acdb_id = txacdb_id;
	voc_col.cal_type.cal_data.mem_handle = block->map_handle;

	get_voice_columns(block, &voc_col);

	if (voc_col.cal_type.cal_data.cal_size != 0) {
		voc_col.hdr.data_size = sizeof(voc_col) -
			sizeof(voc_col.cal_type.cal_info.data) +
			voc_col.cal_type.cal_data.cal_size;
		voc_col.hdr.cal_type_size = sizeof(voc_col.cal_type) -
			sizeof(voc_col.cal_type.cal_info.data) +
			voc_col.cal_type.cal_data.cal_size;
	}
	/* subtract 1 to map acdb.h defines to indexes */
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&voc_col);
	if (result) {
		LOGE("Error: Sending ACDB voice columns result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int get_vocproc_dev_cfg(struct cal_block *block,
				struct audio_cal_vocdev_cfg *vocdev_cal)
{
	int				result = 0;
	AcdbVocProcDevCfgCmdType	vocdevtable;
	AcdbQueryResponseType		response;

	vocdevtable.nTxDeviceId = vocdev_cal->cal_type.cal_info.tx_acdb_id;
	vocdevtable.nRxDeviceId = vocdev_cal->cal_type.cal_info.rx_acdb_id;
	vocdevtable.nBufferLength = block->size;
	vocdevtable.pBuff = (uint8_t *)block->vaddr;

	LOGD("ACDB -> ACDB_CMD_GET_VOC_PROC_DEVICE_CFG\n");

	result = acdb_ioctl(ACDB_CMD_GET_VOC_PROC_DEVICE_CFG,
		(const uint8_t *)&vocdevtable, sizeof(vocdevtable),
		(uint8_t *)&response, sizeof(response));
	if (result) {
		LOGE("Error: ACDB VocProc Dev Cfg Returned = %d\n",
			result);
		vocdev_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}

	vocdev_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}
static int send_vocproc_dev_cfg(int rxacdb_id, int txacdb_id)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_vocdev_cfg	vocdev_cal;

	block = get_cal_block(CVP_VOCDEV_CFG_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	vocdev_cal.hdr.data_size = sizeof(vocdev_cal);
	vocdev_cal.hdr.version = VERSION_0_0;
	vocdev_cal.hdr.cal_type = CVP_VOCDEV_CFG_CAL_TYPE;
	vocdev_cal.hdr.cal_type_size = sizeof(vocdev_cal.cal_type);
	vocdev_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	vocdev_cal.cal_type.cal_hdr.version = block->version;
	vocdev_cal.cal_type.cal_info.tx_acdb_id = txacdb_id;
	vocdev_cal.cal_type.cal_info.rx_acdb_id = rxacdb_id;
	vocdev_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_vocproc_dev_cfg(block, &vocdev_cal);

	LOGD("ACDB -> AUDIO_SET_VOCPROC_DEV_CFG_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION, &vocdev_cal);
	if (result) {
		LOGE("Error: Sending ACDB VocProc Dev Cfg data result = %d\n", result);
		goto done;
	}
done:
	return result;
}


static int get_voctable(struct cal_block *block,
				struct audio_cal_vocproc *vocproc_cal)
{
	int				result = 0;
	AcdbVocProcCmnTblCmdType	voctable;
	AcdbQueryResponseType		response;
	int	acdb_cmd[2] = {ACDB_CMD_GET_VOC_PROC_COMMON_TABLE,
				ACDB_CMD_GET_VOC_PROC_STATIC_TABLE};
	bool	per_vocoder_enabled = false;

	voctable.nTxDeviceId = vocproc_cal->cal_type.cal_info.tx_acdb_id;
	voctable.nRxDeviceId = vocproc_cal->cal_type.cal_info.rx_acdb_id;
	voctable.nTxDeviceSampleRateId = vocproc_cal->cal_type.cal_info.tx_sample_rate;
	voctable.nRxDeviceSampleRateId = vocproc_cal->cal_type.cal_info.rx_sample_rate;
	voctable.nBufferLength = block->size;
	voctable.nBufferPointer = (uint8_t *)block->vaddr;

	per_vocoder_enabled = !!(block->version & PER_VOCODER_CAL_BIT_MASK);

	LOGD("ACDB -> %s\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_PROC_STATIC_TABLE":
			"ACDB_CMD_GET_VOC_PROC_COMMON_TABLE");
	result = acdb_ioctl(acdb_cmd[per_vocoder_enabled],
			(const uint8_t *)&voctable, sizeof(voctable),
			(uint8_t *)&response, sizeof(response));

	if (result) {
		LOGE("Error: %s Returned = %d\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_PROC_STATIC_TABLE":
			"ACDB_CMD_GET_VOC_PROC_COMMON_TABLE",
			result);
		vocproc_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	vocproc_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_vocstrmtable(struct cal_block *block,
				struct audio_cal_basic *vocstrm_cal)
{
	int				result = 0;
	AcdbQueryCmdType		vocstrmtable;
	AcdbQueryResponseType		response;
	int	acdb_cmd[2] = {ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE,
				ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE};
	bool	per_vocoder_enabled = false;

	vocstrmtable.nBufferLength = block->size;
	vocstrmtable.pBufferPointer = (uint8_t *)block->vaddr;

	per_vocoder_enabled = !!(block->version & PER_VOCODER_CAL_BIT_MASK);

	LOGD("ACDB -> %s\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE":
			"ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE");
	result = acdb_ioctl(acdb_cmd[per_vocoder_enabled],
			(const uint8_t *)&vocstrmtable, sizeof(vocstrmtable),
			(uint8_t *)&response, sizeof(response));

	if (result) {
		LOGE("Error: %s Returned = %d\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE":
			"ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE",
			result);
		vocstrm_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	vocstrm_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int get_vocvoltable(struct cal_block *block,
				struct audio_cal_vocvol *vocvol_cal)
{
	AcdbVocProcGainDepVolTblV2CmdType	vocvoltable;
	int					result = 0;
	AcdbQueryResponseType			response;
	int	acdb_cmd[2] = {ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_V2,
				ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE};
	bool	per_vocoder_enabled = false;

	vocvoltable.nTxDeviceId = vocvol_cal->cal_type.cal_info.tx_acdb_id;
	vocvoltable.nRxDeviceId = vocvol_cal->cal_type.cal_info.rx_acdb_id;
	vocvoltable.nFeatureId = vocvol_cal->cal_type.cal_info.feature_set;
	vocvoltable.nBufferLength = block->size;
	vocvoltable.nBufferPointer = (uint8_t *)block->vaddr;

	per_vocoder_enabled = !!(block->version & PER_VOCODER_CAL_BIT_MASK);

	LOGD("ACDB -> %s\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE":
			"ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_V2");
	result = acdb_ioctl(acdb_cmd[per_vocoder_enabled],
			(const uint8_t *)&vocvoltable, sizeof(vocvoltable),
			(uint8_t *)&response, sizeof(response));

	if (result) {
		LOGE("Error: %s Returned = %d\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE":
			"ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_V2",
			result);
		vocvol_cal->cal_type.cal_data.cal_size = 0;
		goto done;
	}
	vocvol_cal->cal_type.cal_data.cal_size = response.nBytesUsedInBuffer;
done:
	return result;
}

static int send_voice_rx_topology(int acdb_id)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_voc_top	voc_top;
	LOGD("ACDB -> send_voice_rx_topology\n");

	block = get_cal_block(CVP_VOC_RX_TOPOLOGY_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	voc_top.hdr.data_size = sizeof(voc_top);
	voc_top.hdr.version = VERSION_0_0;
	voc_top.hdr.cal_type = CVP_VOC_RX_TOPOLOGY_CAL_TYPE;
	voc_top.hdr.cal_type_size = sizeof(voc_top.cal_type);
	voc_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	voc_top.cal_type.cal_hdr.version = block->version;
	voc_top.cal_type.cal_info.acdb_id = acdb_id;
	voc_top.cal_type.cal_data.mem_handle = block->map_handle;

	result = get_voice_topology(&voc_top);
	if (result < 0)
		goto done;

	result = get_voice_channel_config(&voc_top);
	if (result < 0)
		goto done;

	result = get_voice_channel_mapping(&voc_top);
	if (result < 0)
		goto done;

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&voc_top);
	if (result) {
		LOGE("Error: Sending ACDB voice rx topology result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_voice_tx_topology(int acdb_id)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_voc_top	voc_top;
	LOGD("ACDB -> send_voice_tx_topology\n");

	block = get_cal_block(CVP_VOC_TX_TOPOLOGY_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	voc_top.hdr.data_size = sizeof(voc_top);
	voc_top.hdr.version = VERSION_0_0;
	voc_top.hdr.cal_type = CVP_VOC_TX_TOPOLOGY_CAL_TYPE;
	voc_top.hdr.cal_type_size = sizeof(voc_top.cal_type);
	voc_top.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	voc_top.cal_type.cal_hdr.version = block->version;
	voc_top.cal_type.cal_info.acdb_id = acdb_id;
	voc_top.cal_type.cal_data.mem_handle = block->map_handle;

	result = get_voice_topology(&voc_top);
	if (result < 0)
		goto done;

	result = get_voice_channel_config(&voc_top);
	if (result < 0)
		goto done;

	result = get_voice_channel_mapping(&voc_top);
	if (result < 0)
		goto done;

	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&voc_top);
	if (result) {
		LOGE("Error: Sending ACDB voice tx topology result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_sidetone(int rxacdb_id, int txacdb_id)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_sidetone	sidetone_cal;

	block = get_cal_block(AFE_SIDETONE_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	sidetone_cal.hdr.data_size = sizeof(sidetone_cal);
	sidetone_cal.hdr.version = VERSION_0_0;
	sidetone_cal.hdr.cal_type = AFE_SIDETONE_CAL_TYPE;
	sidetone_cal.hdr.cal_type_size = sizeof(sidetone_cal.cal_type);
	sidetone_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	sidetone_cal.cal_type.cal_hdr.version = block->version;
	sidetone_cal.cal_type.cal_info.tx_acdb_id = txacdb_id;
	sidetone_cal.cal_type.cal_info.rx_acdb_id = rxacdb_id;
	sidetone_cal.cal_type.cal_info.mid = ACDB_MID_SIDETONE;
	sidetone_cal.cal_type.cal_info.pid = ACDB_PID_SIDETONE;
	sidetone_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_sidetone(&sidetone_cal);

	LOGD("ACDB -> AUDIO_SET_SIDETONE_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
			&sidetone_cal);
	if (result) {
		LOGE("Error: Sending ACDB sidetone data result = %d\n", result);
		goto done;
	}
done:
	return result;
}

#ifdef AFE_SIDETONE_IIR_CAL_TYPE
static int send_sidetone_iir(int rxacdb_id, int txacdb_id)
{
	int				result = 0;
	struct cal_block		*block;
	struct audio_cal_sidetone_iir	sidetone_iir_cal;

	memset((void*)&sidetone_iir_cal, 0, sizeof(struct audio_cal_sidetone_iir));
	block = get_cal_block(AFE_SIDETONE_IIR_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}
	sidetone_iir_cal.hdr.data_size = sizeof(sidetone_iir_cal);
	sidetone_iir_cal.hdr.version = VERSION_0_0;
	sidetone_iir_cal.hdr.cal_type = AFE_SIDETONE_IIR_CAL_TYPE;
	sidetone_iir_cal.hdr.cal_type_size = sizeof(sidetone_iir_cal.cal_type);
	sidetone_iir_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	sidetone_iir_cal.cal_type.cal_hdr.version = block->version;
	sidetone_iir_cal.cal_type.cal_info.tx_acdb_id = txacdb_id;
	sidetone_iir_cal.cal_type.cal_info.rx_acdb_id = rxacdb_id;
	sidetone_iir_cal.cal_type.cal_info.mid = ACDB_MID_SIDETONE_IIR_FILTER;
	sidetone_iir_cal.cal_type.cal_info.pid = ACDB_PID_SIDETONE_IIR_FILTER_CONFIG;
	sidetone_iir_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_sidetone_iir_config(&sidetone_iir_cal);

	LOGD("ACDB -> AUDIO_SET_SIDETONE_IIR_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
			&sidetone_iir_cal);
	if (result) {
		LOGE("Error: Sending ACDB sidetone data result = %d\n", result);
		goto done;
	}
done:
	return result;
}
#endif

static int send_voctable(int rxacdb_id, int txacdb_id)
{
	int				result = 0;
	int				voctable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_vocproc	vocproc_cal;

	block = get_cal_block(CVP_VOCPROC_STATIC_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get voctable size from s/w and reallocate cal type in case of insufficient memory */
	voctable_size = get_voctable_size(rxacdb_id, txacdb_id, get_samplerate(rxacdb_id), get_samplerate(txacdb_id));
	if (voctable_size > block->size)
	{
		if (voctable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = voctable_size;
		LOGD("Reallocate memory for VocProc Table with size: %d\n", reallocate_size);
		deallocate_cal_type(CVP_VOCPROC_STATIC_CAL_TYPE, BUFF_IDX_0);
		allocate_cal_type(CVP_VOCPROC_STATIC_CAL_TYPE, reallocate_size, BUFF_IDX_0);
	}

	vocproc_cal.hdr.data_size = sizeof(vocproc_cal);
	vocproc_cal.hdr.version = VERSION_0_0;
	vocproc_cal.hdr.cal_type = CVP_VOCPROC_STATIC_CAL_TYPE;
	vocproc_cal.hdr.cal_type_size = sizeof(vocproc_cal.cal_type);
	vocproc_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	vocproc_cal.cal_type.cal_hdr.version = block->version;
	vocproc_cal.cal_type.cal_info.tx_acdb_id = txacdb_id;
	vocproc_cal.cal_type.cal_info.rx_acdb_id = rxacdb_id;
	vocproc_cal.cal_type.cal_info.tx_sample_rate = get_samplerate(txacdb_id);
	vocproc_cal.cal_type.cal_info.rx_sample_rate = get_samplerate(rxacdb_id);
	vocproc_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_voctable(block, &vocproc_cal);

	LOGD("ACDB -> AUDIO_SET_VOCPROC_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION, &vocproc_cal);
	if (result) {
		LOGE("Error: Sending ACDB VocProc data result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int send_vocstrmtable(void) {
	int				result = 0;
	int				vocstrmtable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_basic		vocstrm_cal;

	block = get_cal_block(CVS_VOCSTRM_STATIC_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get vocstrmtable size from s/w and reallocate cal type in case of insufficient memory */
	vocstrmtable_size = get_vocstrmtable_size();
	if (vocstrmtable_size > block->size)
	{
		if (vocstrmtable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = vocstrmtable_size;
		LOGD("Reallocate memory for VocStrm Table with size: %d\n", reallocate_size);
		deallocate_cal_type(CVS_VOCSTRM_STATIC_CAL_TYPE, BUFF_IDX_0);
		allocate_cal_type(CVS_VOCSTRM_STATIC_CAL_TYPE, reallocate_size, BUFF_IDX_0);
	}

	vocstrm_cal.hdr.data_size = sizeof(vocstrm_cal);
	vocstrm_cal.hdr.version = VERSION_0_0;
	vocstrm_cal.hdr.cal_type = CVS_VOCSTRM_STATIC_CAL_TYPE;
	vocstrm_cal.hdr.cal_type_size = sizeof(vocstrm_cal.cal_type);
	vocstrm_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	vocstrm_cal.cal_type.cal_hdr.version = block->version;
	vocstrm_cal.cal_type.cal_data.mem_handle = block->map_handle;

	get_vocstrmtable(block, &vocstrm_cal);

	LOGD("ACDB -> AUDIO_SET_VOCPROC_STREAM_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
		&vocstrm_cal);
	if (result < 0) {
		LOGE("Error: Sending ACDB VOCPROC STREAM fail result %d\n",
			result);
		goto done;
	}
done:
	return result;
}

static int send_vocvoltable(int rxacdb_id, int txacdb_id, int feature_set)
{
	int				result = 0;
	int				vocvoltable_size = 0;
	int				reallocate_size = 0;
	struct cal_block		*block;
	struct audio_cal_vocvol		vocvol_cal;

	block = get_cal_block(CVP_VOCPROC_DYNAMIC_CAL_TYPE, BUFF_IDX_0);
	if (block == NULL) {
		LOGE("%s: Error: Could not get cal block!\n", __func__);
		goto done;
	}

	/* Get vocvoltable size from s/w and reallocate cal type in case of insufficient memory */
	vocvoltable_size = get_vocvoltable_size(rxacdb_id, txacdb_id, feature_set);
	if (vocvoltable_size > block->size)
	{
		if (vocvoltable_size < MIN_BUFF_SIZE)
			reallocate_size = MIN_BUFF_SIZE;
		else
			reallocate_size = vocvoltable_size;
		LOGD("Reallocate memory for VocVol Table with size: %d\n", reallocate_size);
		deallocate_cal_type(CVP_VOCPROC_DYNAMIC_CAL_TYPE, BUFF_IDX_0);
		allocate_cal_type(CVP_VOCPROC_DYNAMIC_CAL_TYPE, reallocate_size, BUFF_IDX_0);
	}

	vocvol_cal.hdr.data_size = sizeof(vocvol_cal);
	vocvol_cal.hdr.version = VERSION_0_0;
	vocvol_cal.hdr.cal_type = CVP_VOCPROC_DYNAMIC_CAL_TYPE;
	vocvol_cal.hdr.cal_type_size = sizeof(vocvol_cal.cal_type);
	vocvol_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;
	vocvol_cal.cal_type.cal_hdr.version = block->version;
	vocvol_cal.cal_type.cal_info.tx_acdb_id = txacdb_id;
	vocvol_cal.cal_type.cal_info.rx_acdb_id = rxacdb_id;
	vocvol_cal.cal_type.cal_info.feature_set = feature_set;
	vocvol_cal.cal_type.cal_data.mem_handle = block->map_handle;

	result = get_vocvoltable(block, &vocvol_cal);
	if (result < 0)
		LOGE("Error: getting VocVol Table = %d\n", result);

	LOGD("ACDB -> AUDIO_SET_VOCPROC_VOL_CAL\n");
	result = ioctl(cal_driver_handle, AUDIO_SET_CALIBRATION,
			&vocvol_cal);
	if (result) {
		LOGE("Error: Sending ACDB VocProc data result = %d\n", result);
		goto done;
	}
done:
	return result;
}

static int validate_voc_cal_dev_pair(int rxacdb_id, int txacdb_id)
{
	int result = 0;
	AcdbDevicePairType dev_pair;
	AcdbDevicePairingResponseType response;

	dev_pair.nTxDeviceId = txacdb_id;
	dev_pair.nRxDeviceId = rxacdb_id;

        result = acdb_ioctl(ACDB_CMD_IS_DEVICE_PAIRED,
			(const uint8_t *)&dev_pair, sizeof(dev_pair),
			(uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: failure to vaildate the device pair = %d\n",
			result);
		goto done;
	}

	result = (int)response.ulIsDevicePairValid;
done:
	return result;
}

static void send_voice_cal(int rxacdb_id, int txacdb_id, int feature_set)
{
	int	acdb_table_id[2][3] = {
				{ACDB_VOC_PROC_TABLE_V2,
				ACDB_VOC_PROC_VOL_TABLE_V2,
				ACDB_VOC_STREAM_TABLE_V2},
				{ACDB_VOC_PROC_STAT_TABLE_V2,
				ACDB_VOC_PROC_DYN_TABLE_V2,
				ACDB_VOC_STREAM2_TABLE_V2}
				};
	bool	per_vocoder_enabled = false;

	per_vocoder_enabled = !!(mem_data[voice_cal_types[0]].version &
					PER_VOCODER_CAL_BIT_MASK);

	LOGD("ACDB -> send_voice_cal, acdb_rx = %d, acdb_tx = %d, feature_set = %d\n",
		rxacdb_id, txacdb_id, feature_set);

	if (!is_initialized) {
		LOGE("ACDB -> Not correctly initialized!\n");
		goto done;
	}

	/* check if it is valid RX/TX device pair */
	if (validate_voc_cal_dev_pair(rxacdb_id, txacdb_id) != 1) {
		LOGE("ACDB -> Error: invalid device pair!");
		goto done;
	}

	current_voice_tx_acdb_id = txacdb_id;
	current_voice_rx_acdb_id = rxacdb_id;
	send_voice_rx_topology(rxacdb_id);
	send_voice_tx_topology(txacdb_id);
#ifdef AFE_SIDETONE_IIR_CAL_TYPE
	send_sidetone_iir(rxacdb_id, txacdb_id);
#endif
	send_sidetone(rxacdb_id, txacdb_id);

	send_voice_columns(rxacdb_id, txacdb_id, acdb_table_id[per_vocoder_enabled][0]);
	send_voctable(rxacdb_id, txacdb_id);
	send_vocproc_dev_cfg(rxacdb_id, txacdb_id);
	send_voice_columns(rxacdb_id, txacdb_id, acdb_table_id[per_vocoder_enabled][1]);
	if((send_vocvoltable(rxacdb_id, txacdb_id, feature_set) < 0) &&
			(feature_set != ACDB_VOCVOL_FID_DEFAULT)) {
		LOGD("ACDB -> feature set %d failed, using default feature set\n", feature_set);
		if (send_vocvoltable(rxacdb_id, txacdb_id, ACDB_VOCVOL_FID_DEFAULT) < 0)
			LOGE("ACDB -> Resend default vocvol unsuccessful!\n");
	}
	send_voice_columns(rxacdb_id, txacdb_id, acdb_table_id[per_vocoder_enabled][2]);
	send_vocstrmtable();
	send_afe_topology(txacdb_id, TX_DEVICE, (TX_DEVICE + MAX_PATH_TYPE * BUFF_IDX_0), AFE_TOPOLOGY_CAL_TYPE);
	send_afe_cal(txacdb_id, TX_DEVICE, get_samplerate(txacdb_id), AFE_COMMON_TX_CAL_TYPE, BUFF_IDX_0);
	send_afe_topology(rxacdb_id, RX_DEVICE, (RX_DEVICE + MAX_PATH_TYPE * BUFF_IDX_0), AFE_TOPOLOGY_CAL_TYPE);
	send_afe_cal(rxacdb_id, RX_DEVICE, get_samplerate(rxacdb_id), AFE_COMMON_RX_CAL_TYPE, BUFF_IDX_0);

	if (!afe_topology_supported)
		send_aanctable(txacdb_id);

	send_hw_delay(txacdb_id, TX_DEVICE, BUFF_IDX_1);
	send_hw_delay(rxacdb_id, RX_DEVICE, BUFF_IDX_0);

	LOGD("ACDB -> Sent VocProc Cal!\n");
done:
	return;
}

void acdb_loader_send_voice_cal_v2(int rxacdb_id, int txacdb_id, int feature_set)
{
	pthread_mutex_lock(&loader_mutex);
	send_voice_cal(rxacdb_id, txacdb_id, feature_set);
	pthread_mutex_unlock(&loader_mutex);
}

void acdb_loader_send_voice_cal(int rxacdb_id, int txacdb_id)
{
	pthread_mutex_lock(&loader_mutex);
	send_voice_cal(rxacdb_id, txacdb_id, current_feature_set);
	pthread_mutex_unlock(&loader_mutex);
}

static int get_vocvoltable_size(int rxacdb_id, int txacdb_id, int feature_set)
{
	int					result = 0;
	AcdbVocProcGainDepVolTblSizeV2CmdType	vocvoltablesize;
	AcdbSizeResponseType			response;
	int	acdb_cmd[2] = {ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_SIZE_V2,
				ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE};
	bool	per_vocoder_enabled = false;

	vocvoltablesize.nTxDeviceId = txacdb_id;
	vocvoltablesize.nRxDeviceId = rxacdb_id;
	vocvoltablesize.nFeatureId = feature_set;

	per_vocoder_enabled = !!(mem_data[voice_cal_types[0]].version &
					PER_VOCODER_CAL_BIT_MASK);

	LOGD("ACDB -> %s\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE":
			"ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_SIZE_V2");
	result = acdb_ioctl(acdb_cmd[per_vocoder_enabled],
			(const uint8_t *)&vocvoltablesize, sizeof(vocvoltablesize),
			(uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: %s Returned = %d\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE":
			"ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_SIZE_V2",
			result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: %s returned %d bytes\n", per_vocoder_enabled ?
			"ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE":
			"ACDB_CMD_GET_VOC_PROC_GAIN_DEP_VOLTBL_SIZE_V2",
			response.nSize);
		goto done;
	}

done:
	return result;
}

static int get_voctable_size(int rxacdb_id, int txacdb_id, int rxacdb_id_sample_rate, int txacdb_id_sample_rate)
{
        int                                     result = 0;
        AcdbVocProcCmnTblSizeCmdType            voctablesize;
        AcdbSizeResponseType                    response;
        int     acdb_cmd[2] = {ACDB_CMD_GET_VOC_PROC_COMMON_TABLE_SIZE,
                                ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE};
        bool    per_vocoder_enabled = false;

        voctablesize.nTxDeviceId = txacdb_id;
        voctablesize.nRxDeviceId = rxacdb_id;
        voctablesize.nTxDeviceSampleRateId = txacdb_id_sample_rate;
        voctablesize.nRxDeviceSampleRateId = rxacdb_id_sample_rate;

        per_vocoder_enabled = !!(mem_data[voice_cal_types[0]].version &
                                        PER_VOCODER_CAL_BIT_MASK);

        LOGD("ACDB -> %s\n", per_vocoder_enabled ?
                        "ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE":
                        "ACDB_CMD_GET_VOC_PROC_COMMON_TABLE_SIZE");
        result = acdb_ioctl(acdb_cmd[per_vocoder_enabled],
                        (const uint8_t *)&voctablesize, sizeof(voctablesize),
                        (uint8_t *)&response, sizeof(response));

        if (result < 0) {
                LOGE("Error: %s Returned = %d\n", per_vocoder_enabled ?
                        "ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE":
                        "ACDB_CMD_GET_VOC_PROC_COMMON_TABLE_SIZE",
                        result);
                goto done;
        }

        result = response.nSize;
        if (response.nSize == 0) {
                LOGE("Error: %s returned %d bytes\n", per_vocoder_enabled ?
                        "ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE":
                        "ACDB_CMD_GET_VOC_PROC_COMMON_TABLE_SIZE",
                        response.nSize);
                goto done;
        }

done:
        return result;
}

static int get_vocstrmtable_size()
{
        int                                     result = 0;
        AcdbSizeResponseType                    response;
        int     acdb_cmd[2] = {ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE_SIZE,
                                ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE};
        bool    per_vocoder_enabled = false;

        per_vocoder_enabled = !!(mem_data[voice_cal_types[0]].version &
                                        PER_VOCODER_CAL_BIT_MASK);

        LOGD("ACDB -> %s\n", per_vocoder_enabled ?
                        "ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE":
                        "ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE_SIZE");
        result = acdb_ioctl(acdb_cmd[per_vocoder_enabled], NULL, 0,
                             (uint8_t *)&response, sizeof(response));

        if (result < 0) {
                LOGE("Error: %s Returned = %d\n", per_vocoder_enabled ?
                        "ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE":
                        "ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE_SIZE",
                        result);
                goto done;
        }

        result = response.nSize;
        if (response.nSize == 0) {
                LOGE("Error: %s returned %d bytes\n", per_vocoder_enabled ?
                        "ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE":
                        "ACDB_CMD_GET_VOC_STREAM_COMMON_TABLE_SIZE",
                        response.nSize);
                goto done;
        }

done:
        return result;
}

static int get_audtable_size(int acdb_id, int app_id, int sample_rate)
{
	int					result = 0;
	AcdbAudProcTableSizeCmdType		audtablesize;
	AcdbSizeResponseType			response;

	audtablesize.nDeviceId = acdb_id;
	audtablesize.nApplicationType = app_id;
	audtablesize.nDeviceSampleRateId = sample_rate;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_COMMON_TABLE_SIZE\n");
	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_COMMON_TABLE_SIZE,
			(const uint8_t *)&audtablesize, sizeof(audtablesize),
			(uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_AUDPROC_COMMON_TABLE_SIZE Returned = %d\n",
			result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_AUDPROC_COMMON_TABLE_SIZE returned %d bytes\n",
			response.nSize);
		goto done;
	}
done:
	return result;
}

static int get_audstrmtable_size(int app_id)
{
	int result = 0;
	AcdbAudStrmTblSizeCmdType get_audstrm_table_size_cmd;
	AcdbSizeResponseType response;

	get_audstrm_table_size_cmd.nApplicationTypeId = app_id;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_STREAM_TABLE_SIZE\n");
	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_STREAM_TABLE_SIZE,
			    (const uint8_t *)&get_audstrm_table_size_cmd,
			    sizeof(get_audstrm_table_size_cmd),
			    (uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_AUDPROC_STREAM_TABLE_SIZE Returned = %d\n",
		     result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_AUDPROC_STREAM_TABLE_SIZE returned %d bytes\n",
		     response.nSize);
		goto done;
	}
done:
	return result;
}

static int get_audvoltable_size(int acdb_id, int app_type, int vol_index)
{
	int result = 0;
	AcdbAudProcGainDepVolTblStepSizeCmdType get_audvol_table_size_cmd;
	AcdbSizeResponseType response;

	get_audvol_table_size_cmd.nDeviceId = acdb_id;
	get_audvol_table_size_cmd.nApplicationType = app_type;
	get_audvol_table_size_cmd.nVolumeIndex = vol_index;

	LOGD("ACDB -> ACDB_CMD_GET_AUDPROC_VOL_STEP_TABLE_SIZE\n");
	result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_VOL_STEP_TABLE_SIZE,
			    (const uint8_t *)&get_audvol_table_size_cmd,
			    sizeof(get_audvol_table_size_cmd),
			    (uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_AUDPROC_VOL_STEP_TABLE_SIZE Returned = %d\n",
		     result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_AUDPROC_VOL_STEP_TABLE_SIZE returned %d bytes\n",
		     response.nSize);
		goto done;
	}
done:
	return result;
}

static int get_aanctable_size(int tx_dev_id)
{
	int result = 0;
	AcdbAANCConfigTableSizeCmdType get_aanc_table_size_cmd;
	AcdbSizeResponseType response;

	get_aanc_table_size_cmd.nTxDeviceId = tx_dev_id;

	LOGD("ACDB -> ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE_SIZE\n");
	result = acdb_ioctl(ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE_SIZE,
			    (const uint8_t *)&get_aanc_table_size_cmd,
			    sizeof(get_aanc_table_size_cmd),
			    (uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE_SIZE Returned = %d\n",
		     result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_ADAPTIVE_ANC_CONFIG_TABLE_SIZE returned %d bytes\n",
		     response.nSize);
		goto done;
	}
done:
	return result;
}

static int get_afetable_size(int acdb_id, int sample_rate)
{
	int result = 0;
	AcdbAfeCommonTableSizeCmdType get_afe_table_size_cmd;
	AcdbSizeResponseType response;

	get_afe_table_size_cmd.nDeviceId = acdb_id;
	get_afe_table_size_cmd.nSampleRateId = sample_rate;

	LOGD("ACDB -> ACDB_CMD_GET_AFE_COMMON_TABLE_SIZE\n");
	result = acdb_ioctl(ACDB_CMD_GET_AFE_COMMON_TABLE_SIZE,
			    (const uint8_t *)&get_afe_table_size_cmd,
			    sizeof(get_afe_table_size_cmd),
			    (uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_AFE_COMMON_TABLE_SIZE Returned = %d\n",
		     result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_AFE_COMMON_TABLE_SIZE returned %d bytes\n",
		     response.nSize);
		goto done;
	}
done:
	return result;
}

static int get_lsmtable_size(int acdb_id, int mad_app_type)
{
	int result = 0;
	AcdbLsmTableSizeCmdType get_lsm_table_size_cmd;
	AcdbSizeResponseType response;

	get_lsm_table_size_cmd.nDeviceId = acdb_id;
	get_lsm_table_size_cmd.nMadApplicationType = mad_app_type;

	LOGD("ACDB -> ACDB_CMD_GET_LSM_TABLE_SIZE\n");
	result = acdb_ioctl(ACDB_CMD_GET_LSM_TABLE_SIZE,
			    (const uint8_t *)&get_lsm_table_size_cmd,
			    sizeof(get_lsm_table_size_cmd),
			    (uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_LSM_TABLE_SIZE Returned = %d\n",
		     result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_LSM_TABLE_SIZE returned %d bytes\n",
		     response.nSize);
		goto done;
	}
done:
	return result;
}

static int get_custom_topology_size()
{
	int result = 0;
	AcdbSizeResponseType response;

	LOGD("ACDB -> ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO_SIZE\n");
	result = acdb_ioctl(ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO_SIZE,
			    NULL, 0, (uint8_t *)&response, sizeof(response));

	if (result < 0) {
		LOGE("Error: ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO_SIZE Returned = %d\n",
		     result);
		goto done;
	}

	result = response.nSize;
	if (response.nSize == 0) {
		LOGE("Error: ACDB_CMD_GET_AVCS_CUSTOM_TOPO_INFO_SIZE returned %d bytes\n",
		     response.nSize);
		goto done;
	}
done:
	return result;
}

int deregister_vocvoltable(void)
{
	int result = 0;
	struct audio_cal_basic		basic_cal;

	basic_cal.hdr.data_size = sizeof(basic_cal);
	basic_cal.hdr.version = VERSION_0_0;
	basic_cal.hdr.cal_type = CVP_VOCPROC_DYNAMIC_CAL_TYPE;
	basic_cal.hdr.cal_type_size = sizeof(basic_cal.cal_type);
	basic_cal.cal_type.cal_hdr.version = VERSION_0_0;
	basic_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;

	LOGD("ACDB -> AUDIO_DEREGISTER_VOCPROC_VOL_TABLE\n");
	result = ioctl(cal_driver_handle, AUDIO_PREPARE_CALIBRATION, &basic_cal);
	if (result < 0) {
		LOGE("Error: Deregister vocproc vol returned = %d\n",
			result);
		goto done;
	}
done:
	return result;
}

int register_vocvoltable(void)
{
	int result = 0;
	struct audio_cal_basic		basic_cal;

	basic_cal.hdr.data_size = sizeof(basic_cal);
	basic_cal.hdr.version = VERSION_0_0;
	basic_cal.hdr.cal_type = CVP_VOCPROC_DYNAMIC_CAL_TYPE;
	basic_cal.hdr.cal_type_size = sizeof(basic_cal.cal_type);
	basic_cal.cal_type.cal_hdr.version = VERSION_0_0;
	basic_cal.cal_type.cal_hdr.buffer_number = BUFF_IDX_0;

	LOGD("ACDB -> AUDIO_REGISTER_VOCPROC_VOL_TABLE\n");
	result = ioctl(cal_driver_handle, AUDIO_POST_CALIBRATION, &basic_cal);
	if (result < 0) {
		LOGE("Error: Register vocproc vol returned = %d\n",
			result);
		goto done;
	}
done:
	return result;
}

int acdb_loader_reload_vocvoltable(int feature_set)
{
	int result = 0;
	uint32_t txacdb_id;
	uint32_t rxacdb_id;

	pthread_mutex_lock(&loader_mutex);
	txacdb_id = current_voice_tx_acdb_id;
	rxacdb_id = current_voice_rx_acdb_id;

	LOGD("ACDB -> acdb_loader_reload_vocvoltable, acdb_rx = %d, acdb_tx = %d, feature_set = %d\n",
		rxacdb_id, txacdb_id, feature_set);

	result = get_vocvoltable_size(rxacdb_id, txacdb_id, feature_set);
	if (result < 0) {
		LOGE("ACDB -> No vocvol table to reload!\n");
		goto done;
	}

	result = deregister_vocvoltable();
	if (result < 0) {
		LOGE("ACDB -> Deregister vocvol table unsuccessful!\n");
		goto done;
	}

	result = send_vocvoltable(rxacdb_id, txacdb_id, feature_set);
	if (result < 0) {
		LOGE("ACDB -> Deregister vocvol table unsuccessful!\n");

		if (feature_set != ACDB_VOCVOL_FID_DEFAULT) {

			LOGE("ACDB -> Resend default vocvol table!\n");
			if (send_vocvoltable(rxacdb_id, txacdb_id, ACDB_VOCVOL_FID_DEFAULT) < 0)
				LOGE("ACDB -> Resend default vocvol unsuccessful!\n");
		}

		/* Even if second attempt to send vol table fails */
		/* Try to re-register. Memory should still contain */
		/* a previous valid volume table */
		LOGE("ACDB -> Reregister default vocvol table!\n");
		if(register_vocvoltable() < 0) {
			LOGE("ACDB -> Reregister default volume unsuccessful!\n");
			goto done;
		}
		LOGE("ACDB -> Resend default volume successful!\n");
		goto done;
	}

	result = register_vocvoltable();
	if (result < 0) {
		LOGE("ACDB -> Register vocvol table unsuccessful!\n");
		goto done;
	}

	/* Update current feature set after successful registration of the new tables */
	current_feature_set = feature_set;

done:
	pthread_mutex_unlock(&loader_mutex);
	return result;
}

void acdb_loader_deallocate_ACDB(void)
{
	int	result;

        pthread_mutex_lock(&loader_mutex);
        if (--acdb_init_ref_cnt != 0) {
            goto exit;
        }

	is_initialized = false;

	LOGD("ACDB -> deallocate_ADIE\n");
	adie_rtac_exit();
	LOGD("ACDB -> deallocate_ACDB\n");
	acdb_rtac_exit();
	acph_deinit();
	deallocate_memory();
	destroy_vol_index_list();
	close(cal_driver_handle);
	LOGD("ACDB -> deallocate_ACDB done!\n");

exit:
        pthread_mutex_unlock(&loader_mutex);
}

int acdb_loader_get_remote_acdb_id(unsigned int native_acdb_id)
{
	int				result;
	AcdbGetRmtCompDevIdCmdType	cmd;
	AcdbGetRmtCompDevIdRspType	response;

	LOGD("ACDB -> acdb_loader_get_remote_acdb_id, acdb_id = %d\n",
		native_acdb_id);

	if (!is_initialized) {
		LOGE("ACDB -> Not correctly initialized!\n");
		result = INVALID_DATA;
		goto done;
	}

	cmd.nNativeDeviceId = native_acdb_id;

	result = acdb_ioctl(ACDB_CMD_GET_COMPATIBLE_REMOTE_DEVICE_ID,
			(const uint8_t *)&cmd, sizeof(cmd),
			(uint8_t *)&response, sizeof(response));
	if (result < 0) {
		LOGE("Error: Remote ACDB ID lookup failed = %d\n",
			result);
		goto done;
	}

	result = response.nRmtDeviceId;
done:
	return result;
}

int process_attribute(char *attr, struct param_data *params)
{
	int ret = 0;

	if (params->get_size) {
		if (strcmp("anc_cal", attr) == 0)
			params->buff_size = ANC_MAX_CODEC_CAL_TABLE_SIZE;
		else
			params->buff_size = DEFAULT_MAX_CAL_TABLE_SIZE;
		goto done;
	}

	if (strcmp("mad_cal", attr) == 0) {
		ret = send_codec_cal(params);
	} else if (strcmp("mbhc_cal", attr) == 0) {
		send_mbhc_data(params);
	} else if (strcmp("anc_cal", attr) == 0) {
		send_wcd9xxx_anc_data(params);
	} else if (strcmp("vbat_cal", attr) == 0) {
		send_vbat_data(params);
	}
done:
	return ret;
}

int acdb_loader_get_calibration(char *attr, int size, void *data)
{
	int ret = 0;
	struct param_data *params = data;

	if (attr == NULL) {
		LOGE("%s: Error: attr is NULL!\n", __func__);
		ret = -EINVAL;
		goto done;
	} else if (size != sizeof(*params)) {
		LOGE("%s: Error: Invalid size %d, expected %zd\n",
			__func__, size, sizeof(*params));
		ret = -EINVAL;
		goto done;
	} else if (data == NULL) {
		LOGE("%s: Error: data is NULL!\n", __func__);
		ret = -EINVAL;
		goto done;
	}

	process_attribute(attr, params);
done:
	return ret;
}

int acdb_loader_store_set_audio_cal(acdb_audio_cal_cfg_t *cal, void *data, uint32_t datalen)
{
    int result = 0;
    if(data == NULL || cal == NULL) {
        ALOGE("[%s] received null pointer", __func__);
        return -EINVAL;
    }

    if(cal->cal_type == AUDIO_DEVICE_CAL_TYPE) {
        if(cal->module_id && cal->param_id) {
            AcdbAudProcCmdType cmd;
            cmd.nDeviceId = cal->acdb_dev_id;
            cmd.nDeviceSampleRateId = cal->sampling_rate;
            cmd.nApplicationType = cal->app_type;
            cmd.nModuleId = cal->module_id;
            cmd.nParamId = cal->param_id;
            cmd.nBufferLength = datalen;
            cmd.nBufferPointer = (uint8_t *)data;
            result = acdb_ioctl(ACDB_CMD_SET_AUDPROC_COMMON_DATA,
                                (const uint8_t *)&cmd, sizeof(cmd),
                                 NULL,0);
        } else {
            AcdbAudProcTableCmdType cmd;
            cmd.nDeviceId = cal->acdb_dev_id;
            cmd.nDeviceSampleRateId = cal->sampling_rate;
            cmd.nApplicationType = cal->app_type;
            cmd.nBufferLength = datalen;
            cmd.nBufferPointer = (uint8_t *)data;
            result = acdb_ioctl(ACDB_CMD_SET_AUDPROC_COMMON_TABLE,
                                (const uint8_t *)&cmd, sizeof(cmd),
                                 NULL,0);
        }
    } else if(cal->cal_type == AUDIO_STREAM_CAL_TYPE){
        if(cal->module_id && cal->param_id) {
            AcdbAudStrmV2CmdType scmd;
            scmd.nApplicationTypeId = cal->app_type;
            scmd.nModuleId = cal->module_id;
            scmd.nParamId = cal->param_id;
            scmd.nBufferLength = datalen;
            scmd.nBufferPointer = (uint8_t *)data;
            result = acdb_ioctl(ACDB_CMD_SET_AUDPROC_STREAM_DATA_V2,
                                (const uint8_t *)&scmd, sizeof(scmd),
                                 NULL,0);
        } else {
            AcdbAudStrmTableV2CmdType scmd;
            scmd.nApplicationTypeId = cal->app_type;
            scmd.nBufferLength = datalen;
            scmd.nBufferPointer = (uint8_t *)data;
            result = acdb_ioctl(ACDB_CMD_SET_AUDPROC_STREAM_TABLE_V2,
                                (const uint8_t *)&scmd, sizeof(scmd),
                                 NULL,0);
        }
    } else {
        ALOGE("[%s] unsupported caltype %d", __func__, cal->cal_type);
        result = -EINVAL;
    }
    if(result != ACDB_SUCCESS) {
        ALOGE("[%s] ACDB settting parameters failed %d\n", __func__, result);
    }
    return result;
}

int acdb_loader_adsp_set_audio_cal(acdb_audio_cal_cfg_t *cal, void *data, uint32_t datalen)
{
    ACPH_CMD_RTC_SET_CAL_DATA_req* req = NULL;
    uint32_t copp_id[MAX_ACTIVE_COPP+1] = {0};
    uint8_t* tptr;
    uint32_t i, req_len, resp_buf_filled;
    uint32_t service_id;
    uint32_t resp_buf_len = 64;
    uint8_t resp_buf[resp_buf_len];
    int result = 0;

    if(data == NULL || cal == NULL) {
        ALOGE("[%s] received null pointer", __func__);
        return -EINVAL;
    }

    if(cal->cal_type == AUDIO_DEVICE_CAL_TYPE) {
        result = get_audio_copp_id(cal->topo_id, cal->app_type,
                                   cal->acdb_dev_id, copp_id);
        service_id = ADSP_ADM_SERVICE;
    } else if(cal->cal_type == AUDIO_STREAM_CAL_TYPE) {
        result = get_audio_popp_id(cal->topo_id, cal->app_type,
                                   cal->acdb_dev_id, copp_id);
        service_id = ADSP_ASM_SERVICE;
    } else {
        result = -EINVAL;
        ALOGE("[%s] unsupported caltype %d", __func__, cal->cal_type);
        goto done;
    }

    if(result < 0) {
        result = -ERR_AUD_CAL_NOCOPP;
        ALOGE("[%s] active device/stream not found (result=%d) for topology 0x%x and apptype 0x%x\n",
               __func__, result, cal->topo_id, cal->app_type);
        goto done;
    }

    if(cal->module_id && cal->param_id) {
        req_len = sizeof(ACPH_CMD_RTC_SET_CAL_DATA_req) + datalen;
    } else {
        req_len = offsetof(ACPH_CMD_RTC_SET_CAL_DATA_req, module_id) + datalen;
    }
    req = (ACPH_CMD_RTC_SET_CAL_DATA_req*)malloc(req_len);
    if(NULL == req) {
        ALOGE("[%s] failed to allocate memory for length %d", __func__, req_len);
        result = -ENOMEM;
        goto done;
    }
    req->domain_id = APR_DOMAIN_ADSP;
    req->service_id = service_id;
    if(cal->module_id && cal->param_id) {
        req->module_id = cal->module_id;
        req->parameter_id = cal->param_id;
        req->length = datalen;
        req->reserved = 0;
        tptr = (uint8_t*)req;
        tptr += sizeof(ACPH_CMD_RTC_SET_CAL_DATA_req);
    } else {
        tptr = (uint8_t*)&(req->module_id);
    }

    memcpy(tptr, data, datalen);
    for(i = 1; i <= copp_id[0]; i++) {
        req->pp_id = copp_id[i];
        result = acdb_rtac_callback(ACPH_CMD_RTC_SET_CAL_DATA, (uint8_t*) req, req_len,
                                 resp_buf, resp_buf_len, &resp_buf_filled);
    }
done:
    if(result < 0) {
        ALOGE("[%s] set parameters failed with status %d", __func__, result);
    }
    if(req)
        free(req);
    return result;
}

int acdb_loader_set_audio_cal_v2(void *cfg, void *data, uint32_t datalen)
{
        acdb_audio_cal_cfg_t *cal = (acdb_audio_cal_cfg_t*) cfg;
        int result=0;
        if(cal == NULL) {
                result = -EINVAL;
                ALOGE("[%s] null pointer received", __func__);
                goto done;
        }
        if(cal->persist) {
                result = acdb_loader_store_set_audio_cal(cal, data, datalen);
        } else {
                result = acdb_loader_adsp_set_audio_cal(cal, data, datalen);
        }
done:
        ALOGV("[%s] Exiting %d", __func__, result);
        return result;
}

int acdb_loader_store_get_audio_cal(acdb_audio_cal_cfg_t *cal, void *data, uint32_t *datalen)
{
    int result=-1,i;
    AcdbQueryResponseType resp={0};

    if(datalen == NULL || data == NULL || cal == NULL) {
        ALOGE("[%s] received null pointer", __func__);
        return -EINVAL;
    }

    if(cal->cal_type == AUDIO_DEVICE_CAL_TYPE) {
        AcdbAudProcCmdType data_cmd;
        AcdbAudProcTableCmdType table_cmd;
        if(cal->module_id && cal->param_id) {
            data_cmd.nDeviceId = cal->acdb_dev_id;
            data_cmd.nDeviceSampleRateId = cal->sampling_rate;
            data_cmd.nApplicationType = cal->app_type;
            data_cmd.nModuleId = cal->module_id;
            data_cmd.nParamId = cal->param_id;
            data_cmd.nBufferLength = *datalen;
            data_cmd.nBufferPointer = (uint8_t*)data;
            result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_COMMON_DATA,
                                (const uint8_t *)&data_cmd, sizeof(data_cmd),
                                (uint8_t*)&resp,sizeof(resp));
        } else {
            table_cmd.nDeviceId = cal->acdb_dev_id;
            table_cmd.nDeviceSampleRateId = cal->sampling_rate;
            table_cmd.nApplicationType = cal->app_type;
            table_cmd.nBufferLength = *datalen;
            table_cmd.nBufferPointer = (uint8_t*)data;
            result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_COMMON_TABLE,
                                (const uint8_t *)&table_cmd, sizeof(table_cmd),
                                (uint8_t*)&resp,sizeof(resp));
        }
    } else if(cal->cal_type == AUDIO_STREAM_CAL_TYPE){
        AcdbAudStrmV2CmdType sdata_cmd;
        AcdbAudStrmTableV2CmdType stable_cmd;

        if(cal->module_id && cal->param_id) {
            sdata_cmd.nApplicationTypeId = cal->app_type;
            sdata_cmd.nModuleId = cal->module_id;
            sdata_cmd.nParamId = cal->param_id;
            sdata_cmd.nBufferLength = *datalen;
            sdata_cmd.nBufferPointer = (uint8_t*)data;
            result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_STREAM_DATA_V2,
                                (const uint8_t *)&sdata_cmd, sizeof(sdata_cmd),
                                (uint8_t*)&resp,sizeof(resp));
        } else {
            stable_cmd.nApplicationTypeId = cal->app_type;
            stable_cmd.nBufferLength = *datalen;
            stable_cmd.nBufferPointer = (uint8_t*)data;
            result = acdb_ioctl(ACDB_CMD_GET_AUDPROC_STREAM_TABLE_V2,
                                (const uint8_t *)&stable_cmd, sizeof(stable_cmd),
                                (uint8_t*)&resp,sizeof(resp));
        }
    } else if(cal->cal_type == AUDIO_CORE_METAINFO_CAL_TYPE){
        AcdbGetMetaInfoCmdType metaInfoCmd;

        LOGD("Reading meta_info\n");

        metaInfoCmd.nBufferPointer = (uint8_t*)data; //caller is expected to allocate memory
        metaInfoCmd.nKey = cal->acdb_dev_id; //Key
        metaInfoCmd.nBufferLength = *datalen;
        result = acdb_ioctl( ACDB_CMD_GET_META_INFO,
                 (const uint8_t *) &metaInfoCmd, sizeof(metaInfoCmd),
                 (uint8_t *) &resp, sizeof(resp));
    } else {
        result = -EINVAL;
    }

    if(ACDB_SUCCESS == result) {
        *datalen = resp.nBytesUsedInBuffer;
    } else {
        *datalen = 0;
        ALOGE("[%s] Error: ACDB get returned = %d length %d\n",
               __func__, result, resp.nBytesUsedInBuffer);
        goto done;
    }

done:
    return result;
}

int acdb_loader_adsp_get_audio_cal(acdb_audio_cal_cfg_t *cal, void *data, unsigned int *datalen)
{
        int result=0,i;
        ACPH_CMD_RTC_GET_CAL_DATA_req req_buf;
	uint32_t copp_id[MAX_ACTIVE_COPP+1]={0};
        uint32_t req_buf_len = 0;
        uint32_t resp_buf_len;
        uint16_t *tptr;
        uint32_t resp_buf_filled=0;
        uint8_t *resp_buf;
        uint32_t service_id;

        if(cal == NULL || data == NULL || datalen == NULL) {
                result = -EINVAL;
                ALOGE("[%s] null pointer received", __func__);
                goto done;
        }

        resp_buf = (uint8_t*)data;
        resp_buf_len = *datalen;
        if(cal->cal_type == AUDIO_DEVICE_CAL_TYPE) {
                result = get_audio_copp_id(cal->topo_id, cal->app_type,
                                            cal->acdb_dev_id, copp_id);
                service_id = ADSP_ADM_SERVICE;
        } else if(cal->cal_type == AUDIO_STREAM_CAL_TYPE) {
                result = get_audio_popp_id(cal->topo_id, cal->app_type,
                                            cal->acdb_dev_id, copp_id);
                service_id = ADSP_ASM_SERVICE;
        } else {
                result = -EINVAL;
                ALOGE("[%s] unsupported caltype %d", __func__, cal->cal_type);
                goto done;
        }

        if(result < 0) {
                result = -ERR_AUD_CAL_NOCOPP;
                ALOGE("[%s] active device/stream not found (result=%d) for topology 0x%x and apptype 0x%x\n",
                      __func__, result, cal->topo_id, cal->app_type);
                goto done;
        }

        req_buf.domain_id = APR_DOMAIN_ADSP;
        req_buf.pp_id = copp_id[1];
        req_buf.service_id = service_id;
        if(cal->module_id && cal->param_id) {
                req_buf.module_id = cal->module_id;
                req_buf.parameter_id = cal->param_id;
                result = acdb_rtac_callback(ACPH_CMD_RTC_GET_CAL_DATA, (uint8_t*)&req_buf, req_buf_len,
                                            resp_buf, resp_buf_len, &resp_buf_filled);

                *datalen = resp_buf_filled;
        } else {
                *datalen = 0;
                result = -EINVAL;
                ALOGE("[%s] module id (0x%x) and param id(0x%x) invalid",
                       __func__, cal->module_id, cal->param_id);
        }

done:
        return result;
}

int acdb_loader_get_audio_cal_v2(void *cfg, void *data, unsigned int *datalen)
{
        int result=0;
        acdb_audio_cal_cfg_t *cal = (acdb_audio_cal_cfg_t*)cfg;

        if(cal == NULL) {
                ALOGE("[%s] received null pointer", __func__);
                return -EINVAL;
        }
        if(cal->persist) {
                result = acdb_loader_store_get_audio_cal(cal, data, datalen);
        } else {
                result = acdb_loader_adsp_get_audio_cal(cal, data, datalen);
        }
        ALOGV("[%s] Exiting %d", __func__, result);
        return result;
}

static int get_volume_steps(int vol_step_type)
{
	int				ret = 0;
	AcdbVolTblStepSizeRspType	resp;

	LOGD("ACDB -> ACDB_CMD_GET_VOL_TABLE_STEP_SIZE\n");

	ret = acdb_ioctl(ACDB_CMD_GET_VOL_TABLE_STEP_SIZE, NULL, 0,
		(uint8_t *)&resp, sizeof(resp));
	if (ret) {
		LOGE("Error: ACDB get vol steps returned = %d\n", ret);
		goto err;
	}

	if (vol_step_type == VOICE_VOL_STEPS)
		ret = resp.VocProcVolTblStepSize;
	else
		ret = resp.AudProcVolTblStepSize;
err:
	return ret;
}

int acdb_loader_get_aud_vol_steps(void)
{
	return get_volume_steps(AUDIO_VOL_STEPS);
}

int acdb_loader_send_gain_dep_cal(int acdb_id, int app_id, int capability, int mode, int vol_index)
{
	int				ret = 0;
	int				path;
	struct aud_vol_idx_node		*node;


	path = get_audcal_path((uint32_t)capability);
	if (path == INVALID_DATA) {
		LOGE("ACDB -> Device is not RX or TX!"
			"acdb_id = %d\n", acdb_id);
		goto done;
	}

	node = get_aud_vol_node(acdb_id, app_id);
	if ((node != NULL) && (node->aud_vol_idx == vol_index))
		goto done;

	if (mode & CAL_MODE_RTAC) {
		send_audvoltable(acdb_id, path, app_id, vol_index, ADM_RTAC_AUDVOL_CAL_TYPE, BUFF_IDX_0);
		add_aud_vol_idx(acdb_id, app_id, vol_index);
	}
done:
	return ret;
}

int acdb_loader_reload_acdb_files_v2(char *new_acdb_file_path, char *snd_card_name __unused,
                                  char *cvd_version, struct listnode *meta_key_list)
{
	int				ret = 0;
	int				i = 0;
	int				result = 0;
	AcdbVocProcGainDepVolTblSizeV2CmdType	vocvoltablesize;
	AcdbSizeResponseType		response = {0};
	AcdbInitCmdType			acdb_init_cmd;

	memset((void*)&vocvoltablesize, 0, sizeof(AcdbVocProcGainDepVolTblSizeV2CmdType));
	memset((void*)&acdb_init_cmd, 0, sizeof(acdb_init_cmd));

        LOGD("ACDB -> RELOAD_ACDB\n");
        pthread_mutex_lock(&loader_mutex);

	if (new_acdb_file_path != NULL) {
		if (get_acdb_files_in_directory(&acdb_init_cmd, new_acdb_file_path) <= 0) {
			LOGE("ACDB -> Could not load .acdb files in %s path !\n", new_acdb_file_path);
			ret = -ENODEV;
			goto done;
		}
	}

        LOGD("ACDB -> ACDB_CMD_RESET\n");
	ret = acdb_ioctl(ACDB_CMD_RESET, (const uint8_t *)NULL, 0, NULL, 0);
	if (ret) {
		LOGE("Error reset ACDB returned = %d\n", ret);
		ret = -ENODEV;
		goto done;
	}

        LOGD("ACDB -> ACDB_CMD_INITIALIZE_V2\n");
	ret = acdb_ioctl(ACDB_CMD_INITIALIZE_V2,
		(const uint8_t *)&acdb_init_cmd, sizeof(acdb_init_cmd), NULL, 0);
	if (ret) {
		LOGE("Error initializing ACDB returned = %d\n", ret);
		ret = -ENODEV;
		goto done;
	}

	if (cvd_version_supported(cvd_version, PER_VOCODER_VERSION)) {
		vocvoltablesize.nTxDeviceId = DEVICE_HANDSET_TX_ACDB_ID;
		vocvoltablesize.nRxDeviceId = DEVICE_HANDSET_RX_ACDB_ID;
		vocvoltablesize.nFeatureId = ACDB_VOCVOL_FID_DEFAULT;

		LOGD("ACDB -> ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE\n");
		result = acdb_ioctl(ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE,
				(const uint8_t *)&vocvoltablesize,
				sizeof(vocvoltablesize),(uint8_t *)&response,
				sizeof(response));
		if (result == 0)
			for (i=0; i<NUM_VOICE_CAL_TABLES; i++)
				mem_data[voice_cal_types[i]].version |= PER_VOCODER_CAL_BIT_MASK;
	}

	acdb_loader_send_common_custom_topology();
	/*meta info*/
	if (send_meta_info_list(meta_key_list) < 0)
		LOGD("ACDB -> send_meta_info_list failed");

        LOGD("ACDB -> RELOAD_ACDB Done\n");
done:
        pthread_mutex_unlock(&loader_mutex);
	return ret;
}

int acdb_loader_reload_acdb_files(char *new_acdb_file_path, char *snd_card_name,
                                  char *cvd_version, int metaInfoKey)
{
	struct listnode key_list;
	struct meta_key_list key_info;

	key_info.cal_info.nKey = metaInfoKey;
	list_init(&key_list);
	list_add_tail(&key_list,&key_info.list);
	return acdb_loader_reload_acdb_files_v2(new_acdb_file_path, snd_card_name,
                                                cvd_version, &key_list);
}

int32_t acdb_loader_get_subgraph_list_size(uint32_t u_id, uint32_t d_id,
                                           uint32_t *cal_size)
{
    int32_t ret;
    WdspSubgraphListSizeCmdType subgraphlistsize;
    AcdbSizeResponseType response;

    if (cal_size == NULL) {
        ALOGE("%s: received null pointer for cal_size", __func__);
        return -EINVAL;
    }

    memset(&subgraphlistsize, 0, sizeof(subgraphlistsize));
    memset(&response, 0, sizeof(response));

    subgraphlistsize.nUsecaseId = u_id;
    subgraphlistsize.nDeviceId = d_id;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_LIST_SIZE\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_LIST_SIZE,
                     (const uint8_t *)&subgraphlistsize,
                     sizeof(subgraphlistsize), (uint8_t *)&response,
                     sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get size for Subgraph list, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        *cal_size = 0;
        goto done;
    }

    *cal_size = response.nSize;

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_list(uint32_t u_id, uint32_t d_id,
                                      uint8_t *cal_buff, size_t cal_size)
{
    int32_t ret;
    WdspSubgraphListCmdType subgraphlist;
    AcdbQueryResponseType response;

    if (cal_buff == NULL) {
        ALOGE("%s: received null pointer for cal_buff", __func__);
        return -EINVAL;
    }

    memset(&subgraphlist, 0, sizeof(subgraphlist));
    memset(&response, 0, sizeof(response));

    /* Prepare the command */
    subgraphlist.nUsecaseId = u_id;
    subgraphlist.nDeviceId = d_id;
    subgraphlist.nBufferLength = cal_size;
    subgraphlist.nBufferPointer = cal_buff;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_LIST\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_LIST,
                     (const uint8_t *)&subgraphlist, sizeof(subgraphlist),
                     (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get Subgraph list, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        goto done;
    }

    /* Compare the size of the data copied to the buffer */
    if (response.nBytesUsedInBuffer != cal_size) {
        ALOGE("%s: Subgraph list data size mismatch. Exiting\n", __func__);
        ret = -EINVAL;
        goto done;
    }

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_mod_info_list_size(uint32_t u_id,
                                                    uint32_t d_id,
                                                    uint32_t sg_id,
                                                    uint32_t *cal_size)
{
    int32_t ret;
    WdspDidUcidSubGraphIDSizeCmdType subgraphmodlistsize;
    AcdbSizeResponseType response;

    if (cal_size == NULL) {
        ALOGE("%s: received null pointer for cal_size", __func__);
        return -EINVAL;
    }

    memset(&subgraphmodlistsize, 0, sizeof(subgraphmodlistsize));
    memset(&response, 0, sizeof(response));

    subgraphmodlistsize.nUsecaseId = u_id;
    subgraphmodlistsize.nDeviceId = d_id;
    subgraphmodlistsize.nSubGraphId = sg_id;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_INFO_LIST_SIZE\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_INFO_LIST_SIZE,
                     (const uint8_t *)&subgraphmodlistsize,
                     sizeof(subgraphmodlistsize), (uint8_t *)&response,
                     sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get size for Subgraph Module list, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        *cal_size = 0;
        goto done;
    }

    *cal_size = response.nSize;

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_mod_info_list(uint32_t u_id, uint32_t d_id,
                                               uint32_t sg_id,
                                               uint8_t *cal_buff,
                                               size_t cal_size)
{
    int32_t ret;
    AcdbWdspDidUcidSubGraphIDType subgraphmodlist;
    AcdbQueryResponseType response;

    if (cal_buff == NULL) {
        ALOGE("%s: received null pointer for cal_buff", __func__);
        return -EINVAL;
    }

    memset(&subgraphmodlist, 0, sizeof(subgraphmodlist));
    memset(&response, 0, sizeof(response));

    /* Prepare the command */
    subgraphmodlist.nUsecaseId = u_id;
    subgraphmodlist.nDeviceId = d_id;
    subgraphmodlist.nSubGraphId = sg_id;
    subgraphmodlist.nBufferLength = cal_size;
    subgraphmodlist.nBufferPointer = cal_buff;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_INFO_LIST\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_INFO_LIST,
                     (const uint8_t *)&subgraphmodlist, sizeof(subgraphmodlist),
                     (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get Subgraph Module list, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        goto done;
    }

    /* Compare the size of the data copied to the buffer */
    if (response.nBytesUsedInBuffer != cal_size) {
        ALOGE("%s: Subgraph Module list data size mismatch. Exiting\n", __func__);
        ret = -EINVAL;
        goto done;
    }

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_mod_conn_size(uint32_t u_id, uint32_t d_id,
                                               uint32_t sg_id,
                                               uint32_t *cal_size)
{
    int32_t ret;
    WdspDidUcidSubGraphIDSizeCmdType subgraphmodconnsize;
    AcdbSizeResponseType response;

    if (cal_size == NULL) {
        ALOGE("%s: received null pointer for cal_size", __func__);
        return -EINVAL;
    }

    memset(&subgraphmodconnsize, 0, sizeof(subgraphmodconnsize));
    memset(&response, 0, sizeof(response));

    subgraphmodconnsize.nUsecaseId = u_id;
    subgraphmodconnsize.nDeviceId = d_id;
    subgraphmodconnsize.nSubGraphId = sg_id;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CONNECTION_SIZE\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CONNECTION_SIZE,
                     (const uint8_t *)&subgraphmodconnsize,
                     sizeof(subgraphmodconnsize), (uint8_t *)&response,
                     sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get size for Subgraph Module connections, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        *cal_size = 0;
        goto done;
    }

    *cal_size = response.nSize;

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_mod_conn(uint32_t u_id, uint32_t d_id,
                                          uint32_t sg_id,
                                          uint8_t *cal_buff, size_t cal_size)
{
    int32_t ret;
    AcdbWdspDidUcidSubGraphIDType subgraphmodconn;
    AcdbQueryResponseType response;

    if (cal_buff == NULL) {
        ALOGE("%s: received null pointer for cal_buff", __func__);
        return -EINVAL;
    }

    memset(&subgraphmodconn, 0, sizeof(subgraphmodconn));
    memset(&response, 0, sizeof(response));

    /* Prepare the command */
    subgraphmodconn.nUsecaseId = u_id;
    subgraphmodconn.nDeviceId = d_id;
    subgraphmodconn.nSubGraphId = sg_id;
    subgraphmodconn.nBufferLength = cal_size;
    subgraphmodconn.nBufferPointer = cal_buff;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CONNECTION\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CONNECTION,
                     (const uint8_t *)&subgraphmodconn, sizeof(subgraphmodconn),
                     (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get Subgraph Module connections, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        goto done;
    }

    /* Compare the size of the data copied to the buffer */
    if (response.nBytesUsedInBuffer != cal_size) {
        ALOGE("%s: Subgraph Module connections data size mismatch. Exiting\n",
             __func__);
        ret = -EINVAL;
        goto done;
    }

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_conn_size(uint32_t u_id, uint32_t d_id,
                                           uint32_t src_sg_id,
                                           uint32_t dst_sg_id,
                                           uint32_t *cal_size)
{
    int32_t ret;
    WdspSubgraphConnectionsSizeCmdType subgraphpairconnsize;
    AcdbSizeResponseType response;

    if (cal_size == NULL) {
        ALOGE("%s: received null pointer for cal_size", __func__);
        return -EINVAL;
    }

    memset(&subgraphpairconnsize, 0, sizeof(subgraphpairconnsize));
    memset(&response, 0, sizeof(response));

    subgraphpairconnsize.nUsecaseId = u_id;
    subgraphpairconnsize.nDeviceId = d_id;
    subgraphpairconnsize.nSubGraphIdSrc = src_sg_id;
    subgraphpairconnsize.nSubGraphIdDst = dst_sg_id;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_CONNECTIONS_SIZE\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_CONNECTIONS_SIZE,
                     (const uint8_t *)&subgraphpairconnsize,
                     sizeof(subgraphpairconnsize), (uint8_t *)&response,
                     sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get size for Subgraph connections, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        *cal_size = 0;
        goto done;
    }

    *cal_size = response.nSize;

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_conn(uint32_t u_id, uint32_t d_id,
                                      uint32_t src_sg_id, uint32_t dst_sg_id,
                                      uint8_t *cal_buff, size_t cal_size)
{
    int32_t ret;
    AcdbWdspDidUcidSubGraphIDPairType subgraphpairconn;
    AcdbQueryResponseType response;

    if (cal_buff == NULL) {
        ALOGE("%s: received null pointer for cal_buff", __func__);
        return -EINVAL;
    }

    memset(&subgraphpairconn, 0, sizeof(subgraphpairconn));
    memset(&response, 0, sizeof(response));

    /* Prepare the command */
    subgraphpairconn.nUsecaseId = u_id;
    subgraphpairconn.nDeviceId = d_id;
    subgraphpairconn.nSubGraphIdSrc = src_sg_id;
    subgraphpairconn.nSubGraphIdDst = dst_sg_id;
    subgraphpairconn.nBufferLength = cal_size;
    subgraphpairconn.nBufferPointer = cal_buff;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPHS_CONNECTION\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPHS_CONNECTION,
                     (const uint8_t *)&subgraphpairconn, sizeof(subgraphpairconn),
                     (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get Subgraph Pair connections, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        goto done;
    }

    /* Compare the size of the data copied to the buffer */
    if (response.nBytesUsedInBuffer != cal_size) {
        ALOGE("%s: Subgraph Pair connections data size mismatch. Exiting\n",
             __func__);
        ret = -EINVAL;
        goto done;
    }

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_mod_cal_size(uint32_t u_id, uint32_t d_id,
                                              uint32_t sg_id,
                                              uint32_t *cal_size)
{
    int32_t ret;
    WdspDidUcidSubGraphIDSizeCmdType subgraphmodcalsize;
    AcdbSizeResponseType response;

    if (cal_size == NULL) {
        ALOGE("%s: received null pointer for cal_size", __func__);
        return -EINVAL;
    }

    memset(&subgraphmodcalsize, 0, sizeof(subgraphmodcalsize));
    memset(&response, 0, sizeof(response));

    subgraphmodcalsize.nUsecaseId = u_id;
    subgraphmodcalsize.nDeviceId = d_id;
    subgraphmodcalsize.nSubGraphId = sg_id;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CALIBRATION_SIZE\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CALIBRATION_SIZE,
                     (const uint8_t *)&subgraphmodcalsize,
                     sizeof(subgraphmodcalsize), (uint8_t *)&response,
                     sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get size for Subgraph Module calibration, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        *cal_size = 0;
        goto done;
    }

    *cal_size = response.nSize;

done:
    return ret;
}

int32_t acdb_loader_get_subgraph_mod_cal(uint32_t u_id, uint32_t d_id,
                                         uint32_t sg_id,
                                         uint8_t *cal_buff, size_t cal_size)
{
    int32_t ret;
    AcdbWdspDidUcidSubGraphIDType subgraphmodcal;
    AcdbQueryResponseType response;

    if (cal_buff == NULL) {
        ALOGE("%s: received null pointer for cal_buff", __func__);
        return -EINVAL;
    }

    memset(&subgraphmodcal, 0, sizeof(subgraphmodcal));
    memset(&response, 0, sizeof(response));

    /* Prepare the command */
    subgraphmodcal.nUsecaseId = u_id;
    subgraphmodcal.nDeviceId = d_id;
    subgraphmodcal.nSubGraphId = sg_id;
    subgraphmodcal.nBufferLength = cal_size;
    subgraphmodcal.nBufferPointer = cal_buff;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CALIBRATION\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_SUBGRAPH_MODULE_CALIBRATION,
                     (const uint8_t *)&subgraphmodcal, sizeof(subgraphmodcal),
                     (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get Subgraph Module calibration, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        goto done;
    }

    /* Compare the size of the data copied to the buffer */
    if (response.nBytesUsedInBuffer != cal_size) {
        ALOGE("%s: Subgraph Module calibration data size mismatch. Exiting\n",
             __func__);
        ret = -EINVAL;
        goto done;
    }

done:
    return ret;
}

int32_t acdb_loader_get_boot_time_mod_list_size(uint32_t *cal_size)
{
    int32_t ret;
    AcdbSizeResponseType response;

    if (cal_size == NULL) {
        ALOGE("%s: received null pointer for cal_size", __func__);
        return -EINVAL;
    }

    memset(&response, 0, sizeof(response));

    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_BOOT_TIME_MODULE_LIST_SIZE\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_BOOT_TIME_MODULE_LIST_SIZE,
                     NULL, 0, (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get size of Boottime Load Module list, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        *cal_size = 0;
        goto done;
    }

    *cal_size = response.nSize;

done:
    return ret;
}

int32_t acdb_loader_get_boot_time_mod_list(uint8_t *cal_buff, size_t cal_size)
{
    int32_t ret;
    AcdbWdspBootTimeModuleReqType boottimemodlist;
    AcdbQueryResponseType response;

    if (cal_buff == NULL) {
        ALOGE("%s: received null pointer for cal_buff", __func__);
        return -EINVAL;
    }

    memset(&boottimemodlist, 0, sizeof(boottimemodlist));
    memset(&response, 0, sizeof(response));

    /* Prepare the command */
    boottimemodlist.nBufferLength = cal_size;
    boottimemodlist.nBufferPointer = cal_buff;

    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_BOOT_TIME_MODULE_LIST\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_BOOT_TIME_MODULE_LIST,
                     (const uint8_t *)&boottimemodlist,
                     sizeof(boottimemodlist),
                     (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get Boottime Module list, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        goto done;
    }

    /* Compare the size of the data copied to the buffer */
    if (response.nBytesUsedInBuffer != cal_size) {
        ALOGE("%s: Boottime Module list data size mismatch. Exiting\n",
             __func__);
        ret = -EINVAL;
        goto done;
    }

done:
    return ret;
}

int32_t acdb_loader_get_mod_loading_meta_info_size(uint32_t mod_type,
                                                   uint32_t mod_count,
                                                   uint8_t *mod_list,
                                                   uint32_t *cal_size)
{
    int32_t ret;
    WdspGetDynamicModuleInfoSizeCmdType dynamicmodmetainfosize;
    AcdbSizeResponseType response;

    if (cal_size == NULL) {
        ALOGE("%s: received null pointer for cal_size", __func__);
        return -EINVAL;
    }

    if (mod_list == NULL) {
        ALOGE("%s: received null pointer for mod_list", __func__);
        return -EINVAL;
    }

    memset(&dynamicmodmetainfosize, 0, sizeof(dynamicmodmetainfosize));
    memset(&response, 0, sizeof(response));

    dynamicmodmetainfosize.nModuleType = mod_type;
    dynamicmodmetainfosize.nNumOfModules = mod_count;
    dynamicmodmetainfosize.pBuff = mod_list;


    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_MODULE_LOADING_META_INFO_SIZE\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_MODULE_LOADING_META_INFO_SIZE,
                     (const uint8_t *)&dynamicmodmetainfosize,
                     sizeof(dynamicmodmetainfosize), (uint8_t *)&response,
                     sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get size for Dynamic Load Module Meta Info, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        *cal_size = 0;
        goto done;
    }

    *cal_size = response.nSize;

done:
    return ret;
}

int32_t acdb_loader_get_mod_loading_meta_info(uint32_t mod_type,
                                              uint32_t mod_count,
                                              uint8_t *mod_list,
                                              uint8_t *cal_buff,
                                              size_t cal_size)
{
    int32_t ret;
    AcdbWdspGetDynamicModuleReqType dynamicmodmetainfo;
    AcdbQueryResponseType response;

    if (cal_buff == NULL) {
        ALOGE("%s: received null pointer for cal_buff", __func__);
        return -EINVAL;
    }

    if (mod_list == NULL) {
        ALOGE("%s: received null pointer for mod_list", __func__);
        return -EINVAL;
    }

    memset(&dynamicmodmetainfo, 0, sizeof(dynamicmodmetainfo));
    memset(&response, 0, sizeof(response));

    /* Prepare the command */
    dynamicmodmetainfo.nModuleType = mod_type;
    dynamicmodmetainfo.nNumOfModules = mod_count;
    dynamicmodmetainfo.pBuff = mod_list;
    dynamicmodmetainfo.nBufferLength = cal_size;
    dynamicmodmetainfo.nBufferPointer = cal_buff;

    ALOGD("ACDB -> ACDB_WDSP_CMD_GET_MODULE_LOADING_META_INFO\n");
    ret = acdb_ioctl(ACDB_WDSP_CMD_GET_MODULE_LOADING_META_INFO,
                     (const uint8_t *)&dynamicmodmetainfo,
                     sizeof(dynamicmodmetainfo),
                     (uint8_t *)&response, sizeof(response));
    if (ret < 0) {
        ALOGE("%s: Failed to get Dynamic Load Module Meta Info, ret = %d\n",
             __func__, ret);
        ret = -EINVAL;
        goto done;
    }

    /* Compare the size of the data copied to the buffer */
    if (response.nBytesUsedInBuffer != cal_size) {
        ALOGE("%s: Dynamic Load Module Meta Info data size mismatch. Exiting\n",
             __func__);
        ret = -EINVAL;
        goto done;
    }

done:
    return ret;
}
