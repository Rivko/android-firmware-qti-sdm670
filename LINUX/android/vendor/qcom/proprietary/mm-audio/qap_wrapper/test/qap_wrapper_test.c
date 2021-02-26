/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc
 */

#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cutils/properties.h>
#include <cutils/list.h>
#include <cutils/log.h>
#include <cutils/str_parms.h>
#include <system/audio.h>
#include "dts_m8.h"
#include "dolby_ms12.h"

#undef LOG_TAG
#define LOG_TAG "BD_WRAPPER_TEST"
#undef LOG_NDEBUG
#define LOG_NDEBUG 0

#define   MAX_KEYS 32 /* Make changes based on "valid_keys" array */
#define   MAX_STR_LEN 256

#define WAV_HEADER_LENGTH_MAX 46
#define WAVE_RIFF_FOURCC    0x46464952
#define WAVE_KEYWORD        0x45564157
#define FORMAT_CHUNK_MARKER 0x20746d66    /*fmt*/
#define CONTIGUOUS_TIMESTAMP 0x7fffffff

#define AUDIO_COMPRESSED_OUT_DD  QAP_AUDIO_FORMAT_AC3
#define AUDIO_COMPRESSED_OUT_DDP QAP_AUDIO_FORMAT_EAC3

#define QAC_LIB_MS12      "/usr/lib/libdolby_ms12_wrapper.so"
#define QAC_LIB_M8        "/usr/lib/libdts_m8_wrapper.so"
#define FRAME_SIZE_FILE   "data/frame_size_file.txt"

#define FRAME_SIZE 2048 * 16

#define print(format, args...) \
    fprintf (stdout, "\nBD_WRAPPER_TEST::  " format "\n", ##args)

#define FRAMES_BOUNDARIES_TIMESTAMPS_AC3        "/data/5ch_dd_25fps_channel_id.ac3_framebyframe.txt"
#define FRAMES_BOUNDARIES_TIMESTAMPS_EAC3       "/data/7ch_ddp_25fps_channel_id.ec3_framebyframe.txt"
#define FRAMES_BOUNDARIES_TIMESTAMPS_AAC        "/data/frame_size_AU3A.frame_by_frame.txt"
#define FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AC3  "/data/5ch_dd_25fps_channel_id.ac3_contiguous.txt"
#define FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_EAC3 "/data/7ch_ddp_25fps_channel_id.ec3_contiguous.txt"
#define FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AAC  "/data/frame_size_AU3A.contiguous_timestamps.txt"
#define FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AC3   "/data/5ch_dd_25fps_channel_id.ac3_jump.txt"
#define FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_EAC3  "/data/7ch_ddp_25fps_channel_id.ec3_jump.txt"
#define FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AAC   "/data/frame_size_AU3A.jump_timestamp.txt"
#define PARTIAL_FRAMES_TIMESTAMPS_AC3           "/data/5ch_dd_25fps_channel_id.ac3_partial.txt"
#define PARTIAL_FRAMES_TIMESTAMPS_EAC3          "/data/7ch_ddp_25fps_channel_id.ec3_partial.txt"
#define PARTIAL_FRAMES_TIMESTAMPS_AAC           "/data/frame_size_AU3A.partial_frames.txt"
#define PARTIAL_FRAMES_JUMP_TIMESTAMPS          "/data/7ch_ddp_25fps_channel_id.ec3_partial_jump.txt"
#define PARTIAL_FRAMES_JUMP_TIMESTAMPS_AAC      "/data/frame_size_AU3A.partial_jump_timestamp.txt"
#define FRAMES_BOUNDARIES_TIMESTAMPS_DTS        "/data/dts_perframe_timestamp.txt"
#define FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_DTS  "/data/dts_cont_timestamp.txt"
#define FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_DTS   "/data/dts_jump_timestamp.txt"
#define PARTIAL_FRAMES_TIMESTAMPS_DTS           "/data/dts_partialframes_timestamp.txt"

char input_file_name_main[MAX_STR_LEN];
char input_file_name_main2[MAX_STR_LEN];
char input_file_name_sys[MAX_STR_LEN];
char input_file_name_assoc[MAX_STR_LEN];
unsigned char data_buf[FRAME_SIZE];
uint32_t out_dev_id = 0;
uint32_t new_out_dev_id = 0;
uint32_t system_input_sample_rate = 0;
uint32_t main2_input_sample_rate = 0;
uint8_t system_input_channels = 0;
uint8_t main2_input_channels = 0;
bool encode = false;
int encode_out_format = 0;
uint8_t num_outputs = 0;
bool session_broadcast = false;
bool has_system_input = false;
bool has_assoc_input = false;
bool has_main2_input = false;
bool device_switch = false;
uint8_t device_switch_time = 0;
bool dolby_formats = false, dts_formats = false;
bool getmetadata = false;
bool no_timestamp_mode = false;
bool dual_dolby_bd_usecase = false;

FILE *fp_main_input = NULL;
FILE *fp_frame_size = NULL;
FILE *fp_output_writer = NULL;
FILE *fp_output_writer_hdmi = NULL;
FILE *fp_output_writer_dap = NULL;
FILE *fp_output_writer_hdmi_ddp = NULL;
FILE *fp_output_writer_hdmi_dd = NULL;
FILE *fp_output_writer_hdmi_dts = NULL;
FILE *fp_dolby_metadata = NULL;
FILE *fp_dts_metadata = NULL;
FILE *fp_input_file = NULL;
FILE *fp_output_timestamp_file = NULL;

qap_session_handle_t qap_session_handle_main2 = NULL;
FILE *fp_output_writer_secnd = NULL;
FILE *fp_output_writer_hdmi_secnd = NULL;
FILE *fp_output_writer_dap_secnd = NULL;
FILE *fp_output_writer_hdmi_ddp_secnd = NULL;
FILE *fp_output_writer_hdmi_dd_secnd = NULL;
FILE *fp_output_writer_hdmi_dts_secnd = NULL;
FILE *fp_dolby_metadata_secnd = NULL;
FILE *fp_dts_metadata_secnd = NULL;
pthread_cond_t main_eos_cond;
pthread_mutex_t main_eos_lock;
pthread_cond_t sec_eos_cond;
pthread_mutex_t sec_eos_lock;

pthread_t system_input_thread;
pthread_t assoc_input_thread;
pthread_t main2_input_thread;
void secondary_file_playback(void *ptr);
pthread_attr_t sec_inpt_thrd_attr;
char session_kvp[256] = {0};

char valid_keys[MAX_KEYS][MAX_STR_LEN] = {"im", "ia",
     "is","odp", "od", "o_device", "new_out_device",
     "ch", "smpl_rate", "broadcast",
     "device_switch",
     "dm_chs", "dm_smpl_rate",
     "sys_chs", "sys_smpl_rate",
     "render_format", "get_metadata", "dolby_bd"};

typedef struct secondary_input_params {
    char system_mix_file_name[MAX_STR_LEN];
    char assoc_input_file_name[MAX_STR_LEN];
    char main2_input_file_name[MAX_STR_LEN];
    qap_session_handle_t session_handle;
    qap_module_config_t input_config;
    unsigned char data_buf_secondary[13000];
    qap_session_outputs_config_t session_output_config;
    int8_t is_system_input;
    int8_t is_assoc_input;
    int8_t is_main2_input;
} secondary_input_params_t;

secondary_input_params_t second_input;

void show_usage(const char *prog)
{
    fprintf(stdout,
            "\nUSAGE: \n\n"
            "%s -im <input file> [options]\n"
            "                                OPTIONS\n"
            "              -broadcast         (specifies broadcast session must be exercise\n"
                                              "otherwise default BD will be exercised)\n"
            "              -o_device          <output device ids>\n"
            "              -odp               <DD+ output>\n"
            "              -od                <DD output>\n"
            "              -ch                <input channels>\n"
            "              -is                <system sound input>\n"
            "              -sys_smpl_rate     <system sound input sampling rate>\n"
            "              -sys_chs           <system sound input channels>\n"
            "              -is                <system sound input>\n"
            "              -ia                <associated input>\n"
            "              -im2               <second main input>\n"
            "              -dm_smpl_reate     <second main input sampling rate>\n"
            "              -dm_chs            <second main input channels>\n"
            "              -render_format     <specifies output format>\n"
            "              -enabletransencode <enabling encode mode for m8 wrapper>\n"
            "              -odts              <specifies DTS output>\n"
            "              -bidwidth          <specifies output bitwidth for m8 wrapper default is 24 bit>\n"
            "              -dolby_bd     <indicates BD usecase of only dolby formats>\n"
            , prog);
    fprintf(stdout,
            "For help on DAP options use: %s -hdap\n\n"
            , prog);
}

void show_dap_usage(const char *prog __unused)
{
    fprintf(stdout,
            "-dap_calibration_boost       <int> (0...192, def: 0)\n"
            "\n"
            "-dap_dmx                     <int> DAP Downmix mode\n"
            "                                     0 = Lt/Rt (Default) \n"
            "                                     1 = Lo/Ro \n"
            "\n"
            "-dap_gains                   <1 int> postgain (-2080...480, def: 0)\n"
            "\n"
            "-dap_surround_decoder_enable <int> DAP surround decoder enable flag (Default 1)\n"
            "\n"
            "-dap_surround_virtualizer    <5 int> Virtualizer Parameter\n"
            "                                     virtualizer_enable (0,1, def: 1)\n"
            "                                     headphone_reverb (-2080...192, def: 0)\n"
            "                                     speaker_angle (5..30, def: 10)\n"
            "                                     speaker_start (20...2000, def: 20)\n"
            "                                     surround_boost (0...96, def: 96)\n"
            "\n"
            "-dap_graphic_eq              <2+2*nb_bands int>\n"
            "                                     eq_enable (0,1, def: 0)\n"
            "                                     eq_nb_bands (1...20, def: 10)\n"
            "                                     eq_band_center (20...20000, def: {32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000})\n"
            "                                     eq_band_target (-576...576, def: {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}\n"
            "\n\n"
            "-dap_optimizer               <2+nb_bands+MAX_CHANNELS*nb_bands int>\n"
            "                                     optimizer_enable (0,1, def: 0)\n"
            "                                     opt_nb_bands (1...20, def: 10)\n"
            "                                     opt_band_center_freq (20...20000, def: {32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000})\n"
            "                                     opt_band_gains[MAX_CHANNELS] (-480...480, def: {{10*0, 10*0, 10*0, 10*0, 10*0, 10*0, 10*0, 10*0})\n"
            "\n"
            "-dap_bass_enhancer           <4 int>\n"
            "                                     bass_enable (0,1, def: 0)\n"
            "                                     bass_boost (0...384, def: 192)\n"
            "                                     bass_cutoff (20...20000, def: 200)\n"
            "                                     bass_width (2..64, def: 16)\n"
            "\n"
            "-dap_regulator               <6+4*nb_bands int>\n"
            "                                     regulator_enable (0,1, def: 1)\n"
            "                                     regulator_mode (0,1, def: 0)\n"
            "                                     regulator_overdrive (0...192, def: 0)\n"
            "                                     regulator_timbre (0...16, def: 16)\n"
            "                                     regulator_distortion (0...144, def: 96)\n"
            "                                     reg_nb_bands (1...20, def: 2)\n"
            "                                     reg_band_center (20...20000, def: {20,20000}\n"
            "                                     reg_low_thresholds (-2080...0, def: {-192, -192})\n"
            "                                     reg_high_thresholds (-2080...0, def: {0, 0})\n"
            "                                     reg_isolated_bands (0,1 def: {0,0})\n"
            "\n"
            "-dap_virtual_bass            <10>\n"
            "                                     virtual_bass_mode (0...3, def: 0)\n"
            "                                     virtual_bass_low_src_freq (30...90, def: 35)\n"
            "                                     virtual_bass_high_src_freq (90...270, def: 160)\n"
            "                                     virtual_bass_overall_gain(-480...0, def: 0)\n"
            "                                     virtual_bass_slope_gain (-3...0, def: 0)\n"
            "                                     virtual_bass_subgains[3] (-480...0, def: {-32,-144,-192})\n"
            "                                     virtual_bass_low_mix_freq (0...375, def: 94)\n"
            "                                     virtual_bass_high_mix_freq (281...938, def: 469)\n"
            "\n"
            "-dap_regulator               <6+4*nb_bands int>\n"
            "                                     regulator_enable (0,1, def: 1)\n"
            "                                     regulator_mode (0,1, def: 0)\n"
            "                                     regulator_overdrive (0...192, def: 0)\n"
            "                                     regulator_timbre (0...16, def: 16)\n"
            "                                     regulator_distortion (0...144, def: 96)\n"
            "                                     reg_nb_bands (1...20, def: 2)\n"
            "                                     reg_band_center (20...20000, def: {20,20000}\n"
            "                                     reg_low_thresholds (-2080...0, def: {-192, -192})\n"
            "                                     reg_high_thresholds (-2080...0, def: {0, 0})\n"
            "                                     reg_isolated_bands (0,1 def: {0,0})\n"
            "\n"
            "-dap_mi_steering             <4 int> * Media Intelligence Steering\n"
            "                                     mi_ieq_enable (0,1, def: 0)\n"
            "                                     mi_dv_enable (0,1, def: 0)\n"
            "                                     mi_de_enable (0,1, def: 0)\n"
            "                                     mi_surround_enable (0,1, def: 0)\n"
            "\n"
            "-dap_leveler                 <3 int>\n"
            "                                     leveler_enable (0,1, def: 0)\n"
            "                                     leveler_amount (0...10, def: 7)\n"
            "                                     leveler_ignore_il (0, 1, def: 0)\n"
            "\n"
            "-dap_ieq                     <3+2*nb_bands int>\n"
            "                                     ieq_enable (0,1, def: 0)\n"
            "                                     ieq_amount (0...16, def: 10)\n"
            "                                     ieq_nb_bands (1...20, def: 10)\n"
            "                                     ieq_band_center (20...20000, def: {32, 64, 125, 250, 500, 1000, 2000, 4000, 8000, 16000})\n"
            "                                     ieq_band_target (-480...480, def: {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}\n"
            "\n"
            "-dap_dialogue_enhancer       <3 int>\n"
            "                                     de_enable (0,1, def: 0)\n"
            "                                     de_amount (0...16, def: 0)\n"
            "                                     de_ducking (0...16, def: 0)\n"
            "\n");
}

void set_out_device(char *id)
{
    int dev_id = 0;
    dev_id = strtol(id, NULL, 0);
    if (id == NULL) {
        return;
    }
    if (num_outputs>=MAX_SUPPORTED_OUTPUTS) {
        print("Max output reached");
        return;
    }
    switch(dev_id) {
        case 1: out_dev_id |= AUDIO_DEVICE_OUT_SPEAKER;
                print("Setting output device: SPEAKER");
                break;
        case 2: out_dev_id |= AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
                print("Setting output device: AUDIO_DEVICE_OUT_WIRED_HEADPHONE");
                break;
        case 3: out_dev_id |= AUDIO_DEVICE_OUT_WIRED_HEADSET;
                print("Setting output device: AUDIO_DEVICE_OUT_WIRED_HEADSET");
                break;
        case 8: out_dev_id |= AUDIO_DEVICE_OUT_HDMI;
                print("Setting device HDMI %x\n", AUDIO_DEVICE_OUT_HDMI);
                break;
        default: out_dev_id |= AUDIO_DEVICE_OUT_SPEAKER;
                 print("DEVICE IN DEFAULT:: %d\n",dev_id);
    }
    num_outputs++;
}

uint32_t get_new_device_id() {
    switch(new_out_dev_id) {
        case 1:
            return AUDIO_DEVICE_OUT_SPEAKER;
            break;
        case 2:
            return AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
            break;
        case 3:
            return AUDIO_DEVICE_OUT_WIRED_HEADSET;
            break;
        case 8:
            return AUDIO_DEVICE_OUT_HDMI;
            break;
        default:
            return AUDIO_DEVICE_OUT_SPEAKER;
            break;
    }
}

static void set_output_config(qap_module_config_t *input_config, qap_session_outputs_config_t *session_output_conf)
{
    uint8_t i;

    if (input_config == NULL)
        return;

    for (i=0;i<num_outputs;i++) {
        if (out_dev_id & AUDIO_DEVICE_OUT_HDMI) {
            session_output_conf->output_config[i].channels = input_config->channels;
            session_output_conf->output_config[i].sample_rate = input_config->sample_rate;
            session_output_conf->output_config[i].id = AUDIO_DEVICE_OUT_HDMI;
            session_output_conf->output_config[i].bit_width = input_config->bit_width;
            session_output_conf->output_config[i].is_interleaved = input_config->is_interleaved;
        } else if (out_dev_id & AUDIO_DEVICE_OUT_WIRED_HEADPHONE) {
            session_output_conf->output_config[i].channels = input_config->channels;
            session_output_conf->output_config[i].sample_rate = input_config->sample_rate;
            session_output_conf->output_config[i].id = AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
            session_output_conf->output_config[i].bit_width = input_config->bit_width;
            session_output_conf->output_config[i].is_interleaved = input_config->is_interleaved;
        } else if (out_dev_id & AUDIO_DEVICE_OUT_WIRED_HEADSET) {
            session_output_conf->output_config[i].channels = input_config->channels;
            session_output_conf->output_config[i].sample_rate = input_config->sample_rate;
            session_output_conf->output_config[i].id = AUDIO_DEVICE_OUT_WIRED_HEADSET;
            session_output_conf->output_config[i].bit_width = input_config->bit_width;
            session_output_conf->output_config[i].is_interleaved = input_config->is_interleaved;
        } else {
            session_output_conf->output_config[i].channels = input_config->channels;
            session_output_conf->output_config[i].sample_rate = input_config->sample_rate;
            session_output_conf->output_config[i].id = AUDIO_DEVICE_OUT_SPEAKER;
            session_output_conf->output_config[i].bit_width = input_config->bit_width;
            session_output_conf->output_config[i].is_interleaved = input_config->is_interleaved;
        }
    }
    session_output_conf->num_output = num_outputs;
}

struct wave_hdr_s {
    uint32_t riff_fourcc;
    uint32_t chunk_size;
    uint32_t format;
};

struct subchunk_s {
    uint32_t id;
    uint32_t sz;
};

struct fmt_s {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

void parse_wave_header(char * file_name, qap_audio_format_t *input_format)
{
    int subchunk_size = 0, wav_header_len = 0, ret;
    char header[WAV_HEADER_LENGTH_MAX] = {0};
    uint8_t bit_width;
    FILE *fp = NULL;

    fp = fopen(file_name, "rb");
    fseek(fp, 16, SEEK_SET);
    if(fread(&subchunk_size, 4, 1, fp) != 1) {
        print("Unable to read subchunk:\n");
        exit (1);
    }
    if(subchunk_size < 16) {
        print("This is not a valid wav file \n");
    } else {
          switch (subchunk_size) {
          case 16:
              print("44-byte wav header \n");
              wav_header_len = 44;
              break;
          case 18:
              print("46-byte wav header \n");
              wav_header_len = 46;
              break;
          default:
              print("Header contains extra data and is larger than 46 bytes: subchunk_size=%d \n", subchunk_size);
              wav_header_len = subchunk_size;
              break;
          }
    }
    if (wav_header_len <= 0) {
        print("wav header length is invalid:%d\n", wav_header_len);
        exit (1);
    }
    fseek(fp, 0, SEEK_SET);
    ret = fread (header, wav_header_len , 1, fp);
    if (ret != 1) {
        print("Error fread failed\n");
        exit(1);
    }
    /*Check the wave FOURCC format*/
    if (strncmp (header, "RIFF", 4) && strncmp (header+8, "WAVE", 4)) {
        print("Not a wave format\n");
        exit(1);
    }
    /*Read the wave file bitwidth from header*/
    memcpy (&bit_width, &header[34], 2);
    if (bit_width == 8)
        *input_format = QAP_AUDIO_FORMAT_PCM_8_24_BIT;
    else if (bit_width == 32)
        *input_format = QAP_AUDIO_FORMAT_PCM_32_BIT;
    else if (bit_width == 24)
        *input_format = QAP_AUDIO_FORMAT_PCM_24_BIT_PACKED;
    else
        *input_format = QAP_AUDIO_FORMAT_PCM_16_BIT;
    print("File Format is WAV %d bit_width %d\n", *input_format, bit_width);
}

int skip_wave_header(FILE *fp)
{
    struct wave_hdr_s wave_hdr;
    struct subchunk_s subchunk;
    struct fmt_s fmt;
    int subchunks_present = 1;


    fseek(fp, 0 ,SEEK_SET);
    fread(&wave_hdr, sizeof(wave_hdr), 1, fp);
    if ((wave_hdr.riff_fourcc != WAVE_RIFF_FOURCC) || (wave_hdr.format != WAVE_KEYWORD)) {
        print("Input file is not a wave file\n");
        return -1;
    }

    do {
        fread(&subchunk, sizeof(subchunk), 1, fp);

        if (subchunk.id == FORMAT_CHUNK_MARKER) {
            fread(&fmt, sizeof(fmt), 1, fp);
            if (subchunk.sz > sizeof(fmt))
                fseek(fp, subchunk.sz - sizeof(fmt), SEEK_CUR);
        } else if (subchunk.id == 0x61746164 /* data */) {
            subchunks_present = 0;
        } else {
            /* Unknown chunk, skip bytes */
            fseek(fp, subchunk.sz, SEEK_CUR);
        }
    } while (subchunks_present);
    print("Skipped the header of wave file\n");
    return 0;
}

char * get_cmd_string_from_arg_array(int argc, const char * argv[], int *status)
{
    char * kvps;
    int idx;
    int has_key = 0;
    int mem = 0;

    print("%s %d in", __func__, __LINE__);
    if (argc < 2 || NULL == argv) {
        print("%s %d returning EINVAL", __func__, __LINE__);
        *status = -EINVAL;
        return NULL;
    }

    for (idx = 0; idx < argc; idx++) {
        mem += (strlen(argv[idx]) + 2);     /* Extra byte to insert delim ';' */
    }

    if (mem > 0) {
        kvps = malloc(mem * sizeof(char));
    } else {
        *status = -EINVAL;
        print("%s %d returning EINVAL", __func__, __LINE__);
        return NULL;
    }

    if (NULL == kvps) {
        *status = -ENOMEM;
        print("%s %d returning EINVAL", __func__, __LINE__);
        return NULL;
    }

    for (idx = 1; idx < argc; idx++) {
        if (( argv[idx][0] == '-') &&
                (argv[idx][1] < '0' || argv[idx][1] > '9')) {
            if (has_key) {
                strcat(kvps, ";");
                has_key = 0;
            }
            strcat(kvps, argv[idx]+1);
            strcat(kvps, "=");
            has_key = 1;
        } else if (has_key) {
            strcat(kvps, argv[idx]);
            strcat(kvps, ";");
            has_key = 0;
        } else {
            *status = -EINVAL;
            if (kvps != NULL) {
                free(kvps);
                kvps = NULL;
            }
            print("%s %d returning EINVAL", __func__, __LINE__);
            return NULL;
        }
    }
    *status = mem;
    print("%s %d returning", __func__, __LINE__);
    return kvps;
}

static int test_validate_extension(const char *param, const char *ext)
{
    int counter = 0;
    int ext_len = strlen(ext);
    char temp[ext_len];
    memset(temp, 0, ext_len);

    if (strlen(param) > ext_len) {
        for (counter = ext_len; counter >= 0; counter--) {
            temp[ext_len - counter] = (char)tolower(param[strlen(param) - counter]);
        }
    }

    if (strcmp(temp, ext) == 0) {
        return 0;
    } else {
        return -EINVAL;
    }
}

void read_bytes_timestamps_from_file(qap_audio_buffer_t *buffer) {
     if (NULL != fp_input_file) {
         char tempstr[100];
         int seek_offset = 0;
         fgets(tempstr, sizeof(tempstr), fp_input_file);
         printf("%s and tempstr is %s \n", __FUNCTION__,  tempstr);
         char * token = strtok(tempstr, ",");
         if (token != NULL) {
             buffer->common_params.size = atoi(token);
             ALOGD("%s and bytes to be read is %d", __FUNCTION__,  buffer->common_params.size);
             if(token!= NULL) {
                 token = strtok(NULL, ",");
                 if (token!= NULL) {
                     buffer->common_params.timestamp = atoi(token);
                     printf("%s and timestamp to be pushed to queue is %u \n", __FUNCTION__, buffer->common_params.timestamp);
                 }
                 token = strtok(NULL, ",");
                 if (token != NULL) {
                     seek_offset = atoi(token);
                     if (fp_main_input && seek_offset > 0) {
                         fseek(fp_main_input, seek_offset, SEEK_CUR);
                     }
                 }
             }
         }
     }
}

void get_bytes_timestamps_wanted(int timestamp_mode, qap_audio_format_t format, qap_audio_buffer_t *buffer) {
    static uint32_t cnt = 0;
        switch(timestamp_mode) {
            case 1:
                if(format == QAP_AUDIO_FORMAT_AC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_TIMESTAMPS_AC3, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_TIMESTAMPS_AC3  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_TIMESTAMPS_AC3  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_EAC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_TIMESTAMPS_EAC3, "r")) == NULL)
                            printf("FRAMES_BOUNDARIES_TIMESTAMPS_EAC3  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf("FRAMES_BOUNDARIES_TIMESTAMPS_EAC3  file opened\n");
                         read_bytes_timestamps_from_file( buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_AAC || format == QAP_AUDIO_FORMAT_AAC_ADTS) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_TIMESTAMPS_AAC, "r")) == NULL)
                            printf("FRAMES_BOUNDARIES_TIMESTAMPS_AAC  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf("FRAMES_BOUNDARIES_TIMESTAMPS_AAC  file opened\n");
                         read_bytes_timestamps_from_file( buffer);
                    }
                } else if ((format == QAP_AUDIO_FORMAT_DTS_HD) || (format == QAP_AUDIO_FORMAT_DTS) ) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_TIMESTAMPS_DTS, "r")) == NULL)
                            printf("FRAMES_BOUNDARIES_TIMESTAMPS_DTS  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf("FRAMES_BOUNDARIES_TIMESTAMPS_DTS  file opened\n");
                         read_bytes_timestamps_from_file( buffer);
                    }
                }
            break;
            case 2:
                if(format == QAP_AUDIO_FORMAT_AC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AC3, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AC3  file opening failed");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AC3  file opened");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_EAC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_EAC3, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_EAC3  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_EAC3  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_AAC || QAP_AUDIO_FORMAT_AAC_ADTS) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AAC, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AAC  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_AAC  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if ((format == QAP_AUDIO_FORMAT_DTS_HD) || (format == QAP_AUDIO_FORMAT_DTS) ) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_DTS, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_DTS  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_CONTG_TIMESTAMPS_DTS  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                }
            break;
            case 3:
                if(format == QAP_AUDIO_FORMAT_AC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AC3, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AC3  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AC3  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_EAC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_EAC3, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_EAC3  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_EAC3  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_AAC || format == QAP_AUDIO_FORMAT_AAC_ADTS) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AAC, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AAC  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_AAC  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if ((format == QAP_AUDIO_FORMAT_DTS_HD) || (format == QAP_AUDIO_FORMAT_DTS) ) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_DTS, "r")) == NULL)
                            printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_DTS  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" FRAMES_BOUNDARIES_JUMP_TIMESTAMPS_DTS  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                }
            break;
            case 4:
                if(format == QAP_AUDIO_FORMAT_AC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(PARTIAL_FRAMES_TIMESTAMPS_AC3, "r")) == NULL)
                            printf("PARTIAL_FRAMES_TIMESTAMPS_AC3  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf("PARTIAL_FRAMES_TIMESTAMPS_AC3  file opened\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_EAC3) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(PARTIAL_FRAMES_TIMESTAMPS_EAC3, "r")) == NULL)
                            printf(" PARTIAL_FRAMES_TIMESTAMPS_EAC3  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" PARTIAL_FRAMES_TIMESTAMPS_EAC3  file opend\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if(format == QAP_AUDIO_FORMAT_AAC || format == QAP_AUDIO_FORMAT_AAC_ADTS) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(PARTIAL_FRAMES_TIMESTAMPS_AAC, "r")) == NULL)
                            printf(" PARTIAL_FRAMES_TIMESTAMPS_AAC  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" PARTIAL_FRAMES_TIMESTAMPS_AAC  file opend\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                } else if ((format == QAP_AUDIO_FORMAT_DTS_HD) || (format == QAP_AUDIO_FORMAT_DTS) ) {
                    if (NULL == fp_input_file){
                        if ((fp_input_file = fopen(PARTIAL_FRAMES_TIMESTAMPS_DTS, "r")) == NULL)
                            printf(" PARTIAL_FRAMES_TIMESTAMPS_DTS  file opening failed\n");
                    }
                    if(NULL != fp_input_file){
                         printf(" PARTIAL_FRAMES_TIMESTAMPS_DTS  file opend\n");
                         read_bytes_timestamps_from_file(buffer);
                    }
                }
            break;
            case QAP_BUFFER_NO_TSTAMP:
            default:
                buffer->common_params.size = 1024;
                buffer->common_params.timestamp = QAP_BUFFER_NO_TSTAMP;
            break;
        }
        cnt++;
        return;
}

void qap_session_callback_secondary(qap_session_handle_t session_handle __unused, void* priv_data __unused, qap_callback_event_t event_id, int size __unused, void *data)
{
    int ret;
    switch (event_id) {
        case QAP_CALLBACK_EVENT_EOS:
        case QAP_CALLBACK_EVENT_MAIN_2_EOS:
            printf("%s %d Received EOS\n", __func__, __LINE__);
            pthread_mutex_lock(&sec_eos_lock);
            pthread_cond_signal(&sec_eos_cond);
            pthread_mutex_unlock(&sec_eos_lock);
            break;
        case QAP_CALLBACK_EVENT_ERROR:
            break;
        case QAP_CALLBACK_EVENT_SUCCESS:
            break;
        case QAP_CALLBACK_EVENT_METADATA:
        case QAP_CALLBACK_EVENT_OUTPUT_CFG_CHANGE:
        case QAP_CALLBACK_EVENT_DATA:
            if (data != NULL) {
                qap_audio_buffer_t *buffer = (qap_audio_buffer_t *) data;
                if (getmetadata && buffer && buffer->buffer_parms.output_buf_params.metadata) {
                    if (dolby_formats) {
                        dlb_mixing_metadata_t *metadata = (dlb_mixing_metadata_t *) buffer->buffer_parms.output_buf_params.metadata;
                        if (fp_dolby_metadata_secnd == NULL) {
                            fp_dolby_metadata_secnd =
                                       fopen("/sdcard/dolby_metadata.txt","wb");
                        }
                        if (fp_dolby_metadata_secnd && metadata) {
                            fprintf(fp_dolby_metadata_secnd, "mixmdate %d\n", metadata->mixmdate);
                            fprintf(fp_dolby_metadata_secnd, "extpgmscle %d\n", metadata->extpgmscle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmscl %d\n", metadata->extpgmscl);
                            fprintf(fp_dolby_metadata_secnd, "mixdef %d\n", metadata->mixdef);
                            fprintf(fp_dolby_metadata_secnd, "extpgmlscle %d\n", metadata->extpgmlscle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmlscl %d\n", metadata->extpgmlscl);
                            fprintf(fp_dolby_metadata_secnd, "extpgmcscle %d\n", metadata->extpgmcscle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmcscl %d\n", metadata->extpgmcscl);
                            fprintf(fp_dolby_metadata_secnd, "extpgmrscle %d\n", metadata->extpgmrscle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmrscl %d\n", metadata->extpgmrscl);
                            fprintf(fp_dolby_metadata_secnd, "extpgmlsscle %d\n", metadata->extpgmlsscle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmlsscl %d\n", metadata->extpgmlsscl);
                            fprintf(fp_dolby_metadata_secnd, "extpgmrsscle %d\n", metadata->extpgmrsscle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmrsscl %d\n", metadata->extpgmrsscl);
                            fprintf(fp_dolby_metadata_secnd, "extpgmlfescle %d\n", metadata->extpgmlfescle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmlfescl %d\n", metadata->extpgmlfescl);
                            fprintf(fp_dolby_metadata_secnd, "dmixscle %d\n", metadata->dmixscle);
                            fprintf(fp_dolby_metadata_secnd, "addche %d\n", metadata->addche);
                            fprintf(fp_dolby_metadata_secnd, "extpgmaux1scle %d\n", metadata->extpgmaux1scle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmaux1scl %d\n", metadata->extpgmaux1scl);
                            fprintf(fp_dolby_metadata_secnd, "extpgmaux2scle %d\n", metadata->extpgmaux2scle);
                            fprintf(fp_dolby_metadata_secnd, "extpgmaux2scl %d\n", metadata->extpgmaux2scl);
                            fprintf(fp_dolby_metadata_secnd, "paninfoe %d\n", metadata->paninfoe);
                            fprintf(fp_dolby_metadata_secnd, "panmean %d\n", metadata->panmean);
                        }
                    } else if (dts_formats) {
                        dts_m8_gain_metadata_t *metadata = (dts_m8_gain_metadata_t *)buffer->buffer_parms.output_buf_params.metadata;
                        if (fp_dts_metadata_secnd == NULL) {
                            fp_dts_metadata_secnd =
                                       fopen("/sdcard/dts_metadata.txt","wb");
                        }
                        if (fp_dts_metadata_secnd && metadata) {
                            int i = 0;
                            fprintf(fp_dts_metadata_secnd, "primary_audio_channel_mask 0x%x\n", metadata->primary_audio_channel_mask);
                            fprintf(fp_dts_metadata_secnd, "is_metadata_enable %d\n", metadata->is_metadata_enable);
                            fprintf(fp_dts_metadata_secnd, "is_primary_audio_gain_valid %d\n", metadata->is_primary_audio_gain_valid);
                            fprintf(fp_dts_metadata_secnd, "is_secondary_pann_gain_valid %d\n", metadata->is_secondary_pann_gain_valid);
                            fprintf(fp_dts_metadata_secnd, "is_post_mix_gain_valid %d\n", metadata->is_post_mix_gain_valid);

                            for (i = 0; i < DTS_MAX_CHANNELS; i++) {
                                 fprintf(fp_dts_metadata_secnd, "ch %d, primary gain %d, secondary gain %d\n",
                                     i,
                                     metadata->primary_audio_gain[i],
                                     metadata->secondary_pann_gain[i]);
                            }
                            fprintf(fp_dts_metadata_secnd, "post mix gain %d\n", metadata->post_mix_gain);
                        }
                    }
                }

                if (buffer && buffer->common_params.data) {
                    if (buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_SPEAKER) {
                        if (fp_output_writer_secnd == NULL) {
                            char ch[4] = {0};
                            fp_output_writer_secnd =
                                         fopen("/sdcard/output_speaker_secnd.dump","wb");
                            if(fp_output_writer_secnd) {
                                print("output file :: "
                                      "/sdcard/output_speaker_secnd.dump"
                                      " has been generated.");
                                if (!dts_formats) {
                                    ret = fwrite((unsigned char *)&ch, sizeof(unsigned char),
                                                  4, fp_output_writer_secnd);
                                }
                            }
                        }
                        if (fp_output_writer_secnd) {
                            ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                          buffer->common_params.size, fp_output_writer_secnd);
                            fflush(fp_output_writer_secnd);
                        }
                    }
                    if (buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_HDMI) {
                        if (encode) {
                            if (encode_out_format == AUDIO_FORMAT_DTS) {
                                if (fp_output_writer_hdmi_dts_secnd == NULL) {
                                    fp_output_writer_hdmi_dts_secnd =
                                        fopen("/sdcard/output_hdmi_dts_secnd.dts","wb");
                                    if (fp_output_writer_hdmi_dts)
                                        print("output file :: "
                                              "/sdcard/output_hdmi_dts_secnd.dts"
                                              " has been generated.");
                                }
                                if (fp_output_writer_hdmi_dts_secnd) {
                                    ret = fwrite((unsigned char *)buffer->common_params.data,
                                                  sizeof(unsigned char), buffer->common_params.size, fp_output_writer_hdmi_dts_secnd);
                                    fflush(fp_output_writer_hdmi_dts_secnd);
                                }
                            } else if (encode_out_format == AUDIO_COMPRESSED_OUT_DDP) {
                                if (fp_output_writer_hdmi_ddp == NULL) {
                                    fp_output_writer_hdmi_ddp_secnd =
                                        fopen("/sdcard/output_hdmi_ddp_secnd.dump","wb");
                                    if (fp_output_writer_hdmi_ddp_secnd)
                                        print("output file :: "
                                              "/sdcard/output_hdmi_ddp_secnd.dump"
                                              " has been generated.");
                                }
                                if (fp_output_writer_hdmi_ddp_secnd) {
                                    ret = fwrite((unsigned char *)buffer->common_params.data,
                                                  sizeof(unsigned char), buffer->common_params.size, fp_output_writer_hdmi_ddp_secnd);
                                    fflush(fp_output_writer_hdmi_ddp_secnd);
                                }
                            } else {
                                if (!fp_output_writer_hdmi_dd_secnd) {
                                    fp_output_writer_hdmi_dd_secnd =
                                        fopen("/sdcard/output_hdmi_dd_secnd.dump","wb");
                                    if (fp_output_writer_hdmi_dd_secnd)
                                        print("output file :: "
                                              "/sdcard/output_hdmi_dd_secnd.dump"
                                              " has been generated.");
                                }
                                if (fp_output_writer_hdmi_dd_secnd) {
                                    ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                                  buffer->common_params.size, fp_output_writer_hdmi_dd_secnd);
                                    fflush(fp_output_writer_hdmi_dd_secnd);
                                }
                            }
                        } else {
                            if (fp_output_writer_hdmi_secnd == NULL) {
                            fp_output_writer_hdmi_secnd =
                                         fopen("/sdcard/output_hdmi_secnd.dump","wb");
                            print("output file :: "
                                  "/sdcard/output_hdmi_secnd.dump"
                                  " has been generated.");
                            }
                            if(fp_output_writer_hdmi_secnd) {
                                ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                              buffer->common_params.size, fp_output_writer_hdmi_secnd);
                                fflush(fp_output_writer_hdmi_secnd);
                            }
                        }
                    }
                    if ((buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_WIRED_HEADPHONE) ||
                        (buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_WIRED_HEADSET)) {
                        if (fp_output_writer_dap_secnd == NULL) {
                        char ch[4] = {0};
                        fp_output_writer_dap_secnd =
                                     fopen("/sdcard/output_hp_secnd.dump","wb");
                        if(fp_output_writer_dap_secnd) {
                            ret = fwrite((unsigned char *)&ch, sizeof(unsigned char),
                                          4, fp_output_writer_dap_secnd);
                            print("output file :: "
                                   "/sdcard/output_hp_secnd.dump"
                                   " has been generated.");
                            }
                        }
                        if (fp_output_writer_dap_secnd) {
                            ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                          buffer->common_params.size, fp_output_writer_dap_secnd);
                            fflush(fp_output_writer_dap_secnd);
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}

void qap_session_callback(qap_session_handle_t session_handle __unused, void* priv_data __unused, qap_callback_event_t event_id, int size __unused, void *data)
{
    int ret;
    switch (event_id) {
        case QAP_CALLBACK_EVENT_EOS:
            ALOGV("%s %d Received Main Input EOS", __func__, __LINE__);
            pthread_mutex_lock(&main_eos_lock);
            pthread_cond_signal(&main_eos_cond);
            pthread_mutex_unlock(&main_eos_lock);
            break;
        case QAP_CALLBACK_EVENT_EOS_ASSOC:
        case QAP_CALLBACK_EVENT_MAIN_2_EOS:
            if (!has_system_input){
                ALOGV("%s %d Received Secondary Input EOS", __func__, __LINE__);
                pthread_mutex_lock(&sec_eos_lock);
                pthread_cond_signal(&sec_eos_cond);
                pthread_mutex_unlock(&sec_eos_lock);
            }
            break;
        case QAP_CALLBACK_EVENT_ERROR:
            break;
        case QAP_CALLBACK_EVENT_SUCCESS:
            break;
        case QAP_CALLBACK_EVENT_METADATA:
        case QAP_CALLBACK_EVENT_OUTPUT_CFG_CHANGE:
        case QAP_CALLBACK_EVENT_DATA:
            if (data != NULL) {
                qap_audio_buffer_t *buffer = (qap_audio_buffer_t *) data;
                if (getmetadata && buffer && buffer->buffer_parms.output_buf_params.metadata) {
                    if (dolby_formats) {
                        dlb_mixing_metadata_t *metadata = (dlb_mixing_metadata_t *)buffer->buffer_parms.output_buf_params.metadata;
                        if (fp_dolby_metadata == NULL) {
                            fp_dolby_metadata =
                                       fopen("/sdcard/dolby_metadata.txt","wb");
                        }
                        if (fp_dolby_metadata && metadata) {
                            fprintf(fp_dolby_metadata, "mixmdate %d\n", metadata->mixmdate);
                            fprintf(fp_dolby_metadata, "extpgmscle %d\n", metadata->extpgmscle);
                            fprintf(fp_dolby_metadata, "extpgmscl %d\n", metadata->extpgmscl);
                            fprintf(fp_dolby_metadata, "mixdef %d\n", metadata->mixdef);
                            fprintf(fp_dolby_metadata, "extpgmlscle %d\n", metadata->extpgmlscle);
                            fprintf(fp_dolby_metadata, "extpgmlscl %d\n", metadata->extpgmlscl);
                            fprintf(fp_dolby_metadata, "extpgmcscle %d\n", metadata->extpgmcscle);
                            fprintf(fp_dolby_metadata, "extpgmcscl %d\n", metadata->extpgmcscl);
                            fprintf(fp_dolby_metadata, "extpgmrscle %d\n", metadata->extpgmrscle);
                            fprintf(fp_dolby_metadata, "extpgmrscl %d\n", metadata->extpgmrscl);
                            fprintf(fp_dolby_metadata, "extpgmlsscle %d\n", metadata->extpgmlsscle);
                            fprintf(fp_dolby_metadata, "extpgmlsscl %d\n", metadata->extpgmlsscl);
                            fprintf(fp_dolby_metadata, "extpgmrsscle %d\n", metadata->extpgmrsscle);
                            fprintf(fp_dolby_metadata, "extpgmrsscl %d\n", metadata->extpgmrsscl);
                            fprintf(fp_dolby_metadata, "extpgmlfescle %d\n", metadata->extpgmlfescle);
                            fprintf(fp_dolby_metadata, "extpgmlfescl %d\n", metadata->extpgmlfescl);
                            fprintf(fp_dolby_metadata, "dmixscle %d\n", metadata->dmixscle);
                            fprintf(fp_dolby_metadata, "addche %d\n", metadata->addche);
                            fprintf(fp_dolby_metadata, "extpgmaux1scle %d\n", metadata->extpgmaux1scle);
                            fprintf(fp_dolby_metadata, "extpgmaux1scl %d\n", metadata->extpgmaux1scl);
                            fprintf(fp_dolby_metadata, "extpgmaux2scle %d\n", metadata->extpgmaux2scle);
                            fprintf(fp_dolby_metadata, "extpgmaux2scl %d\n", metadata->extpgmaux2scl);
                            fprintf(fp_dolby_metadata, "paninfoe %d\n", metadata->paninfoe);
                            fprintf(fp_dolby_metadata, "panmean %d\n", metadata->panmean);
                        }
                    } else if (dts_formats) {
                        dts_m8_gain_metadata_t *metadata = (dts_m8_gain_metadata_t *)buffer->buffer_parms.output_buf_params.metadata;
                        if (fp_dts_metadata == NULL) {
                            fp_dts_metadata =
                                       fopen("/sdcard/dts_metadata.txt","wb");
                        }
                        if (fp_dts_metadata && metadata) {
                            int i = 0;
                            fprintf(fp_dts_metadata, "primary_audio_channel_mask 0x%x\n", metadata->primary_audio_channel_mask);
                            fprintf(fp_dts_metadata, "is_metadata_enable %d\n", metadata->is_metadata_enable);
                            fprintf(fp_dts_metadata, "is_primary_audio_gain_valid %d\n", metadata->is_primary_audio_gain_valid);
                            fprintf(fp_dts_metadata, "is_secondary_pann_gain_valid %d\n", metadata->is_secondary_pann_gain_valid);
                            fprintf(fp_dts_metadata, "is_post_mix_gain_valid %d\n", metadata->is_post_mix_gain_valid);

                            for (i = 0; i < DTS_MAX_CHANNELS; i++) {
                                 fprintf(fp_dts_metadata, "ch %d, primary gain %d, secondary gain %d\n",
                                     i,
                                     metadata->primary_audio_gain[i],
                                     metadata->secondary_pann_gain[i]);
                            }
                            fprintf(fp_dts_metadata, "post mix gain %d\n", metadata->post_mix_gain);
                        }
                    }
                }

                if(buffer && buffer->common_params.timestamp && !no_timestamp_mode) {
                    char ch[100] = {0};
                    if (fp_output_timestamp_file == NULL) {
                        fp_output_timestamp_file =
                                 fopen("/sdcard/output_timestamp_file.txt","w");
                        if(fp_output_timestamp_file) {
                            print("output file :: "
                                   "/sdcard/output_file_timestamp.txt"
                                   " has been generated.");
                            }
                    }
                    if (fp_output_timestamp_file) {
                        ALOGD(" %s size is %d and timestamp is %lld\n", __FUNCTION__,  buffer->common_params.size,  buffer->common_params.timestamp);
                        sprintf(ch, "%d,%lld\n", buffer->common_params.size, buffer->common_params.timestamp);
                        ret = fwrite((char *)&ch, sizeof(char),
                                     strlen(ch), fp_output_timestamp_file);
                        fflush(fp_output_timestamp_file);
                    }
                }

                if (buffer && buffer->common_params.data) {
                    if (buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_SPEAKER) {
                        if (fp_output_writer == NULL) {
                            char ch[4] = {0};
                            fp_output_writer =
                                         fopen("/sdcard/output_speaker.dump","wb");
                            if(fp_output_writer) {
                                print("output file :: "
                                      "/sdcard/output_speaker.dump"
                                      " has been generated.");
                                if (!dts_formats) {
                                    ret = fwrite((unsigned char *)&ch, sizeof(unsigned char),
                                                  4, fp_output_writer);
                                }
                            }
                        }
                        if (fp_output_writer) {
                            ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                          buffer->common_params.size, fp_output_writer);
                            fflush(fp_output_writer);
                        }
                    }
                    if (buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_HDMI) {
                        if (encode) {
                            if (encode_out_format == AUDIO_FORMAT_DTS) {
                                if (fp_output_writer_hdmi_dts == NULL) {
                                    fp_output_writer_hdmi_dts =
                                        fopen("/sdcard/output_hdmi_dts.dts","wb");
                                    if (fp_output_writer_hdmi_dts)
                                        print("output file :: "
                                              "/sdcard/output_hdmi_dts.dts"
                                              " has been generated.");
                                }
                                if (fp_output_writer_hdmi_dts) {
                                    ret = fwrite((unsigned char *)buffer->common_params.data,
                                                  sizeof(unsigned char), buffer->common_params.size, fp_output_writer_hdmi_dts);
                                    fflush(fp_output_writer_hdmi_dts);
                                }
                            } else if (encode_out_format == AUDIO_COMPRESSED_OUT_DDP) {
                                if (fp_output_writer_hdmi_ddp == NULL) {
                                    fp_output_writer_hdmi_ddp =
                                        fopen("/sdcard/output_hdmi_ddp.dump","wb");
                                    if (fp_output_writer_hdmi_ddp)
                                        print("output file :: "
                                              "/sdcard/output_hdmi_ddp.dump"
                                              " has been generated.");
                                }
                                if (fp_output_writer_hdmi_ddp) {
                                    ret = fwrite((unsigned char *)buffer->common_params.data,
                                                  sizeof(unsigned char), buffer->common_params.size, fp_output_writer_hdmi_ddp);
                                    fflush(fp_output_writer_hdmi_ddp);
                                }
                            } else {
                                if (!fp_output_writer_hdmi_dd) {
                                    fp_output_writer_hdmi_dd =
                                        fopen("/sdcard/output_hdmi_dd.dump","wb");
                                    if (fp_output_writer_hdmi_dd)
                                        print("output file :: "
                                              "/sdcard/output_hdmi_dd.dump"
                                              " has been generated.");
                                }
                                if (fp_output_writer_hdmi_dd) {
                                    ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                                  buffer->common_params.size, fp_output_writer_hdmi_dd);
                                    fflush(fp_output_writer_hdmi_dd);
                                }
                            }
                        } else {
                            if (fp_output_writer_hdmi == NULL) {
                            fp_output_writer_hdmi =
                                         fopen("/sdcard/output_hdmi.dump","wb");
                            print("output file :: "
                                  "/sdcard/output_hdmi.dump"
                                  " has been generated.");
                            }
                            if(fp_output_writer_hdmi) {
                                ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                              buffer->common_params.size, fp_output_writer_hdmi);
                                fflush(fp_output_writer_hdmi);
                            }
                        }
                    }
                    if ((buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_WIRED_HEADPHONE) ||
                        (buffer->buffer_parms.output_buf_params.output_id & AUDIO_DEVICE_OUT_WIRED_HEADSET)) {
                        if (fp_output_writer_dap == NULL) {
                        char ch[4] = {0};
                        fp_output_writer_dap =
                                     fopen("/sdcard/output_hp.dump","wb");
                        if(fp_output_writer_dap) {
                            ret = fwrite((unsigned char *)&ch, sizeof(unsigned char),
                                          4, fp_output_writer_dap);
                            print("output file :: "
                                   "/sdcard/output_hp.dump"
                                   " has been generated.");
                            }
                        }
                        if (fp_output_writer_dap) {
                            ret = fwrite((unsigned char *)buffer->common_params.data, sizeof(unsigned char),
                                          buffer->common_params.size, fp_output_writer_dap);
                            fflush(fp_output_writer_dap);
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}

void parse_input_format(char *input_file_name, qap_audio_format_t *format)
{
    if (0 == test_validate_extension(input_file_name, ".ac3")) {
        *format = QAP_AUDIO_FORMAT_AC3;
        print("File Format is AC3!\n");
    } else if (0 == test_validate_extension(input_file_name, ".ec3")) {
        *format = QAP_AUDIO_FORMAT_EAC3;
        print("File Format is EAC3!\n");
    } else if (0 == test_validate_extension(input_file_name, ".dts")) {
        *format = QAP_AUDIO_FORMAT_DTS;
        print("File Format is DTS!\n");
    } else if (0 == test_validate_extension(input_file_name, ".dtshd")) {
        *format = QAP_AUDIO_FORMAT_DTS_HD;
        print("File Format is DTS_HD!\n");
    } else if (0 == test_validate_extension(input_file_name, ".wav")) {
        parse_wave_header(input_file_name, format);
        print("File Format is WAV!\n");
    } else if (( 0 == test_validate_extension(input_file_name, ".aac")) ||
            ( 0 == test_validate_extension(input_file_name, "loas")) ||
            ( 0 == test_validate_extension(input_file_name, "adts"))) {
        *format = QAP_AUDIO_FORMAT_AAC_ADTS;
        print("File Format is AAC!\n");
    } else {
        print("File Format not supported!\n");
    }
}

int parse_argv(int argc,const char *argv[],qap_module_config_t *input_conf)
{

    int count;
    int i;
    int got_input = 0;
    int got_device = 0;
    const char *key;
    const char *value = NULL;
    uint8_t out_channels = 0;

    if (argc > 1) {
        count = argc;
        for ( i = 1; i < argc;) {

            if (( argv[i][0] == '-') &&( argv[i][1] < '0' || argv[i][1] > '9')) {
                key = argv[i] + 1;

                if (strcmp(key,"im") == 0) {
                    value = argv[i + 1];
                    if ( value == NULL ) {
                        print("No Main Input File Provided!");
                        return -EINVAL;
                    } else {
                        print("Main Input File  = %s", value);
                        strcpy(input_file_name_main, value);
                        got_input = 1;
                        input_conf->flags = QAP_MODULE_FLAG_PRIMARY;
                    }
                    i += 2;
                } else if (strcmp(key,"im2") == 0) {
                    value = argv[i + 1];
                    if ( value == NULL ) {
                        print("No Second Main Input File Provided!");
                        return -EINVAL;
                    } else {
                        print("Second Main Input File  = %s", value);
                        strcpy(input_file_name_main2, value);
                        got_input = 1;
                        has_main2_input = true;
                    }
                    i += 2;
                } else if (strcmp(key,"is") == 0) {
                    value = argv[i + 1];
                    if ( value == NULL ) {
                        print("No System Input File Provided!");
                        return -EINVAL;
                    } else {
                        print("System Input File  = %s", value);
                        strcpy(input_file_name_sys, value);
                        got_input = 1;
                        has_system_input = true;
                    }
                    i += 2;
                } else if (strcmp(key,"ia") == 0) {
                    value = argv[i + 1];
                    if ( value == NULL ) {
                        print("No Associcated Input File Provided!");
                        return -EINVAL;
                    } else {
                        print("Associated Input File  = %s", value);
                        strcpy(input_file_name_assoc, value);
                        got_input = 1;
                        has_assoc_input = true;
                    }
                    i += 2;
                } else if (strncmp(key, "odp", 3) == 0) {
                    encode_out_format = AUDIO_COMPRESSED_OUT_DDP;
                    encode = true;
                    i += 1;
                } else if (strncmp(key, "od", 2) == 0) {
                    encode_out_format = AUDIO_COMPRESSED_OUT_DD;
                    encode = true;
                    i += 1;
                } else if (strcmp(key, "render_format") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No Render Format Specified");
                        return -EINVAL;
                    } else {
                        print("Render Format = %s", value);
                        if (strncmp(value, "odts", 4) == 0) {
                            encode_out_format = AUDIO_FORMAT_DTS;
                            encode = true;
                        }
                    }
                    i += 2;
                } else if (strcmp(key, "get_metadata") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No GetMetadata value Specified");
                        return -EINVAL;
                    } else {
                        print("GetMetadata value = %s", value);
                    }
                    getmetadata = true;
                    i += 2;
                } else if (strcmp(key, "broadcast") == 0) {
                    session_broadcast = true;
                    i += 1;
                } else if (strcmp(key, "device_switch") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No Device switch time specified");
                        return -EINVAL;
                    } else {
                        print("Device Switch time = %s", value);
                        device_switch = true;
                        device_switch_time = strtol(value, NULL, 0);
                    }
                    i += 2;
                } else if (strcmp(key, "new_out_device") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No New Output Device Specified");
                        return -EINVAL;
                    } else {
                        print("New Output Device = %s", value);
                        new_out_dev_id = strtol(value, NULL, 0);
                    }
                    i += 2;
                } else if (strcmp(key, "smpl_rate") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No Sampling Rate Provided!");
                        return -EINVAL;
                    } else {
                        print("Input Sampling Rate = %s", __func__, __LINE__, value);
                        input_conf->sample_rate = strtol(value, NULL, 0);
                    }
                    i += 2;
                } else if (strcmp(key, "sys_smpl_rate") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No System Input Sampling Rate Provided!");
                        return -EINVAL;
                    } else {
                        print("System Input Sampling Rate = %s", __func__, __LINE__, value);
                        system_input_sample_rate = strtol(value, NULL, 0);
                    }
                    i += 2;
                } else if (strcmp(key, "dm_smpl_rate") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No Main2 Input Sampling Rate Provided!");
                        return -EINVAL;
                    } else {
                        print("Main2 Input Sampling Rate = %s", value);
                        main2_input_sample_rate = strtol(value, NULL, 0);
                    }
                    i += 2;
                } else if (strcmp(key, "sys_chs") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No System Input Channels Not Specified!");
                        return -EINVAL;
                    } else {
                        print("System Input Channels = %s", value);
                        system_input_channels = strtol(value, NULL, 0);
                        print("System Input Channels = %d", system_input_channels);
                    }
                    i += 2;
                } else if (strcmp(key, "dm_chs") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No Main2 Input Channels Not Specified!");
                        return -EINVAL;
                    } else {
                        print("Main2 Input Channels = %s", value);
                        main2_input_channels = strtol(value, NULL, 0);
                    }
                    i += 2;
                } else if(strcmp(key, "ch") == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("Output Channel Not Specified!");
                        return -EINVAL;
                    } else {
                        out_channels = *value;
                        if (out_channels == '2') {
                            print("Stereo Out Channel Selected");
                            input_conf->channels = 2;
                        } else if ( out_channels == '6') {
                            print("'6' Channel out Selected");
                            input_conf->channels = 6;
                        } else if ( out_channels == '8') {
                            print("'8' Channel out Selected");
                            input_conf->channels = 8;
                        }else {
                            print("Unsupported channel count: %c provided!"
                                    " Supported values are 2, 6 and 8", out_channels);
                            return -EINVAL;
                        }
                    }
                    i += 2;
                } else if (strncmp(key, "o_device", 8) == 0) {
                    value = argv[i + 1];
                    if (value == NULL) {
                        print("No value given for Device ID");
                        return -EINVAL;
                    } else {
                        got_device = 1;
                    }
                    char * tempstr = strdup(value);
                    char * token = strtok(tempstr, ",");
                    printf("tempstr = %s", tempstr);
                    if (token != NULL) {
                        set_out_device(token);
                        while(token!= NULL) {
                            token = strtok(NULL, ",");
                            if (token!= NULL) {
                                set_out_device(token);
                            }
                        }
                    } else {
                        set_out_device(tempstr);
                    }
                    i += 2;
                } else if (strncmp(key, "hdap", 4) == 0) {
                    show_dap_usage(argv[0]);
                    return -EINVAL;
                } else if (strncmp(key, "dolby_bd", 8) == 0) {
                    dual_dolby_bd_usecase = 1;
                    i++;
                } else {
                    i += 1;
                }
            } else {
                i += 1;
            }
        }
        if (1 != got_input) {
            print("Input file argument is mandatory!");
            show_usage(argv[0]);
            return -EINVAL;
        }
        if (1 != got_device) {
            print("Output device argument is mandatory!");
            show_usage(argv[0]);
            return -EINVAL;
        }
    } else {
        show_usage(argv[0]);
        return -EINVAL;
    }

    parse_input_format(input_file_name_main, &input_conf->format);
    if (has_system_input) {
        strncpy(second_input.system_mix_file_name, input_file_name_sys, MAX_STR_LEN);
        second_input.is_system_input = 1;
        second_input.input_config.flags = QAP_MODULE_FLAG_SYSTEM_SOUND;
        parse_input_format(second_input.system_mix_file_name, &second_input.input_config.format);
        printf("system_mix_file_name, = %s format %d\n", second_input.system_mix_file_name, second_input.input_config.format);
    }

    if (has_assoc_input) {
        second_input.is_assoc_input = 1;
        strncpy(second_input.assoc_input_file_name, input_file_name_assoc, MAX_STR_LEN);
        second_input.input_config.flags = QAP_MODULE_FLAG_SECONDARY;
        parse_input_format(second_input.assoc_input_file_name, &second_input.input_config.format);
        printf("assoc_input_file_name, = %s format %d\n", second_input.assoc_input_file_name, second_input.input_config.format);
    }

    if (has_main2_input) {
        second_input.is_main2_input = 1;
        strncpy(second_input.main2_input_file_name, input_file_name_main2, MAX_STR_LEN);
        second_input.input_config.flags = QAP_MODULE_FLAG_PRIMARY;
        parse_input_format(second_input.main2_input_file_name, &second_input.input_config.format);
        printf("input_file_name_main2, = %s format %d\n", second_input.main2_input_file_name, second_input.input_config.format);
    }

    return 0;
}

char *get_string_value(const char *kvp, int *status)
{
    char *tempstr1 = NULL;
    char *tempstr2 = NULL;
    char *l1;
    //char *l2 __unused;
    char *ctx1;

    if (kvp == NULL) {
        return NULL;
    }

    tempstr1 = strdup(kvp);
    l1 = strtok_r(tempstr1, "=", &ctx1);
    if (l1 != NULL) {
        /* jump from key to value */
        l1 = strtok_r(NULL, "=", &ctx1);
        if (l1 != NULL) {
            tempstr2 = strdup(l1);
        }
    }
    free(tempstr1);
    return tempstr2;
}

void secondary_file_playback(void *ptr)
{
    FILE *fp_sec_inpt = NULL;
    int bytes_read, ret, bytes_consumed = 0;
    qap_module_handle_t qap_module_handle = NULL;
    secondary_input_params_t *second_input;
    int8_t first_read = 1;
    int bytes_wanted, status = 0;
    int is_buffer_available = 0;
    char *temp_str = NULL;
    void *reply_data;
    char* temp_ptr = NULL;

    second_input = (secondary_input_params_t *)ptr;
    qap_audio_buffer_t *buffer;

    print("Secondary Thread started ...");

    if (second_input == NULL) {
        print("Mixer:: Input Structure NULL!");
        return;
    }

    if (has_main2_input && !session_broadcast) {
        ret = qap_session_set_callback(second_input->session_handle, &qap_session_callback_secondary);
        if (ret != QAP_STATUS_OK) {
            print("!!!! Please specify appropriate Session");
            ALOGD("%d Please specify appropriate Session\n", __LINE__);
            return;
        }
        ret = qap_session_cmd(second_input->session_handle, QAP_SESSION_CMD_SET_OUTPUTS, sizeof(second_input->session_output_config), &second_input->session_output_config, NULL, NULL);
        if (ret != QAP_STATUS_OK) {
            print("Output config failed\n");
            ALOGD("%d Output config failed\n", __LINE__);
            return;
        }
        ret = qap_session_cmd(second_input->session_handle, QAP_SESSION_CMD_SET_KVPAIRS, sizeof(session_kvp), session_kvp, NULL, NULL);
        if (ret != QAP_STATUS_OK) {
            print("Session set params failed\n");
            ALOGD("%d Session set params failed %s\n", __LINE__, session_kvp);
        }
    }

    if (second_input->is_assoc_input) {
        fp_sec_inpt = fopen(second_input->assoc_input_file_name, "rb");
        if (fp_sec_inpt == NULL) {
            print("Open File Failed for %s :%d\n", second_input->assoc_input_file_name, __LINE__);
            return;
        }
        ret = qap_module_init(second_input->session_handle, &second_input->input_config, &qap_module_handle);
        if (qap_module_handle == NULL) {
            print("Assoc Module Handle is Null\n");
        }
    } else if (second_input->is_system_input) {
        fp_sec_inpt = fopen(second_input->system_mix_file_name, "rb");
        if (fp_sec_inpt == NULL) {
            print("Open File Failed for %s %d\n", second_input->system_mix_file_name, __LINE__);
            return;
        }
        ret = qap_module_init(second_input->session_handle, &second_input->input_config, &qap_module_handle);
        if (qap_module_handle == NULL) {
            print("Assoc Module Handle is Null\n");
        }
    } else if (second_input->is_main2_input) {
        fp_sec_inpt = fopen(second_input->main2_input_file_name, "rb");
        if (fp_sec_inpt == NULL) {
            print("Open File Failed for %s %d\n", second_input->main2_input_file_name, __LINE__);
            return;
        }
        ret = qap_module_init(second_input->session_handle, &second_input->input_config, &qap_module_handle);
        if (qap_module_handle == NULL) {
            print("Assoc Module Handle is Null\n");
        }
    }


    ret = qap_module_cmd(qap_module_handle, QAP_MODULE_CMD_START, sizeof(QAP_MODULE_CMD_START), NULL, NULL, NULL);
    if (ret != QAP_STATUS_OK) {
        printf("START failed\n");
        return;
    }

    buffer = (qap_audio_buffer_t *) calloc(1, sizeof(qap_audio_buffer_t));
    if (buffer == NULL) {
        print("Memory Alloc Error 1\n");
        return;
    }
    buffer->common_params.data = calloc(1, FRAME_SIZE);
    if (buffer->common_params.data == NULL) {
       print("Memory Alloc Error 2\n");
       return;
    }
    buffer->buffer_parms.output_buf_params.output_id = out_dev_id;

    do {
        if (second_input->is_system_input) {
            if (first_read) {
                first_read = 0;
                if(skip_wave_header(fp_sec_inpt) != 0)
                {
                    qap_module_deinit(qap_module_handle);
                    print("%s : %d , Invalid WAV file input! \n", __func__, __LINE__);
                    return;
                }
            }
        }
        get_bytes_timestamps_wanted(QAP_BUFFER_NO_TSTAMP, 0, buffer);
        bytes_wanted = 2 * buffer->common_params.size;
        bytes_read = fread(second_input->data_buf_secondary, sizeof(unsigned char), bytes_wanted, fp_sec_inpt);

        buffer->common_params.offset = 0;
        buffer->common_params.size = bytes_read;
        memcpy(buffer->common_params.data, second_input->data_buf_secondary, bytes_read);
        if (bytes_read == 0) {
            buffer->buffer_parms.input_buf_params.flags = QAP_BUFFER_EOS;
            bytes_consumed = qap_module_process(qap_module_handle, buffer);
            ret = qap_module_cmd(qap_module_handle, QAP_MODULE_CMD_STOP, sizeof(QAP_MODULE_CMD_STOP), NULL, NULL, NULL);
            print("Stopped feeding secondary input");
            break;
        }
        buffer->buffer_parms.input_buf_params.flags = QAP_BUFFER_NO_TSTAMP;
        reply_data = (char*) calloc(1, 100);
        is_buffer_available = 0;
        temp_ptr = buffer->common_params.data;
        do {
            print("feeding secondary input of size %d \n", bytes_read);
            bytes_consumed = qap_module_process(qap_module_handle, buffer);
            if (bytes_consumed > 0) {
                buffer->common_params.data += bytes_consumed;
                buffer->common_params.size -= bytes_consumed;
            }
            if ((second_input->input_config.format == QAP_AUDIO_FORMAT_DTS) ||
                 (second_input->input_config.format == QAP_AUDIO_FORMAT_DTS_HD)) {
                  if (bytes_consumed < 0) {
                      while (!is_buffer_available) {
                          usleep(1000);
                          ret = qap_module_cmd(qap_module_handle, QAP_MODULE_CMD_GET_PARAM,
                                      sizeof(QAP_MODULE_CMD_GET_PARAM), "buf_available", NULL, reply_data);
                          if (reply_data)
                              temp_str = get_string_value((const char*)reply_data, &status);
                           if (temp_str) {
                               is_buffer_available = atoi(temp_str);
                               free(temp_str);
                           }
                       }
                  }
             }
        } while(buffer->common_params.size > 0);
        if (reply_data)
            free(reply_data);
        buffer->common_params.data = temp_ptr;
    } while(1);

    pthread_mutex_lock(&sec_eos_lock);
    printf("Waiting for EOS event for sec\n");
    pthread_cond_wait(&sec_eos_cond, &sec_eos_lock);
    pthread_mutex_unlock(&sec_eos_lock);
    pthread_cond_destroy(&sec_eos_cond);
    pthread_mutex_destroy(&sec_eos_lock);
    printf("Received EOS event for sec\n");

    print("Closing Mixer Stream");
    if(NULL != buffer &&  NULL != buffer->common_params.data) {
     free( buffer->common_params.data);
     buffer->common_params.data = NULL;
     free( buffer);
     buffer = NULL;
    }
    fclose(fp_sec_inpt);
    qap_module_deinit(qap_module_handle);

    return;
}

static bool is_formats_supported_for_bluray_usecase(qap_audio_format_t format) {
    switch (format) {
        case QAP_AUDIO_FORMAT_AC3:
        case QAP_AUDIO_FORMAT_EAC3:
        case QAP_AUDIO_FORMAT_DTS:
        case QAP_AUDIO_FORMAT_DTS_HD:
            return true;
            break;
       default:
            break;
    }
    return false;
}

static bool is_dolby_formats(qap_audio_format_t format) {
    switch (format) {
        case QAP_AUDIO_FORMAT_AC3:
        case QAP_AUDIO_FORMAT_EAC3:
        case QAP_AUDIO_FORMAT_AAC:
        case QAP_AUDIO_FORMAT_AAC_ADTS:
        case QAP_AUDIO_FORMAT_PCM_16_BIT:
        case QAP_AUDIO_FORMAT_PCM_8_24_BIT:
        case QAP_AUDIO_FORMAT_PCM_24_BIT_PACKED:
        case QAP_AUDIO_FORMAT_PCM_32_BIT:
            return true;
            break;
       default:
            break;
    }
    return false;
}

static bool is_dts_formats(qap_audio_format_t format) {
    switch (format) {
        case QAP_AUDIO_FORMAT_DTS:
        case QAP_AUDIO_FORMAT_DTS_HD:
            return true;
       default:
            break;
    }
    return false;
}

int main(int argc, const char *argv[])
{
    qap_module_config_t *input_config = NULL;
    qap_session_outputs_config_t session_output_config;
    char command_line[3000] = {0};
    int i = 0, status;
    qap_session_handle_t qap_session_handle = NULL;
    qap_module_handle_t qap_module_handle = NULL;
    qap_audio_buffer_t *buffer;
    qap_status_t ret;
    qap_lib_handle_t ms12_lib_handle = NULL;
    qap_lib_handle_t m8_lib_handle = NULL;
    int bytes_wanted;
    int bytes_read, bytes_consumed = 0;
    char *cmd_str = NULL;
    time_t begin,end;
    char *kvp_string = NULL;
    int8_t first_read = 1;
    bool wave_main_input = false;
    int timestamp_mode = 1;
    char temp[5] = {0};
    void *reply_data ;
    char *temp_str = NULL;
    char* temp_ptr = NULL;
    int is_buffer_available = 0;

    ALOGI("Command Line: ");
    for (i = 0; i< argc; i++) {
        strcat(command_line, argv[i]);
        strcat(command_line, " ");
    }
    ALOGI("%s", command_line);
    memset(&session_output_config, 0, sizeof(session_output_config));

    input_config = (qap_module_config_t *) calloc(1, sizeof(qap_module_config_t));
    if (input_config == NULL)
        return -1;

    if (parse_argv(argc, argv, input_config) != 0) {
        return -1;
    }
    cmd_str = get_cmd_string_from_arg_array(argc, argv, &status);
    if (status > 0) {
        kvp_string = (char *) malloc(status + strlen("standalone_exec=; ") + strlen(" manual_mixer_ctrl=; "));
        if (kvp_string != NULL) {
            if (!dual_dolby_bd_usecase)
                strcat(kvp_string, "standalone_exec=;manual_mixer_ctrl=;");
            strcat(kvp_string, cmd_str);
        }
    }
    if (cmd_str != NULL) {
        free(cmd_str);
        cmd_str = NULL;
    }

    if (kvp_string == NULL) {
        show_usage(argv[0]);
        return -1;
    }
    print("KVPAIR string %s\n", kvp_string);
    strcpy(session_kvp, kvp_string);

    if (!session_broadcast) {
        if (!is_formats_supported_for_bluray_usecase(input_config->format) && !has_main2_input) {
            print("%d Format %d is not supported for BD usecase\n ", __LINE__, input_config->format);
            return -1;
        }
        if (has_main2_input && !is_formats_supported_for_bluray_usecase(second_input.input_config.format)) {
            print("%d Format %d is not supported for BD usecase\n ", __LINE__, second_input.input_config.format);
            return -1;
        }
    }

    if ((ms12_lib_handle == NULL) && is_dolby_formats(input_config->format)) {
        ms12_lib_handle = (qap_session_handle_t) qap_load_library(QAC_LIB_MS12);
        if (ms12_lib_handle == NULL) {
            print("Failed to load MS12 library");
            return -1;
        }
        dolby_formats = true;
    } else if ((ms12_lib_handle == NULL) && has_main2_input && is_dolby_formats(second_input.input_config.format)) {
        ms12_lib_handle = (qap_session_handle_t) qap_load_library(QAC_LIB_MS12);
        if (ms12_lib_handle == NULL) {
            print("Failed to load MS12 library");
            return -1;
        }
        dolby_formats = true;
    }

    if ((m8_lib_handle == NULL) && is_dts_formats(input_config->format)) {
        m8_lib_handle = (qap_session_handle_t) qap_load_library(QAC_LIB_M8);
        if (m8_lib_handle == NULL) {
            print("Failed to load M8 library");
            return -1;
        }
        dts_formats = true;
    } else if ((m8_lib_handle == NULL) && has_main2_input && is_dts_formats(second_input.input_config.format)) {
        m8_lib_handle = (qap_session_handle_t) qap_load_library(QAC_LIB_M8);
        if (m8_lib_handle == NULL) {
            print("Failed to load M8 library");
            return -1;
        }
        dts_formats = true;
    }

    pthread_mutex_init(&main_eos_lock, (const pthread_mutexattr_t *)NULL);
    pthread_cond_init(&main_eos_cond, (const pthread_condattr_t *) NULL);
    if (has_assoc_input || has_system_input || has_main2_input) {
        pthread_attr_init(&sec_inpt_thrd_attr);
        pthread_attr_setdetachstate(&sec_inpt_thrd_attr, PTHREAD_CREATE_JOINABLE);
        pthread_mutex_init(&sec_eos_lock, (const pthread_mutexattr_t *)NULL);
        pthread_cond_init(&sec_eos_cond, (const pthread_condattr_t *) NULL);
    }

    if (session_broadcast && dolby_formats)
        qap_session_handle = (qap_module_handle_t) qap_session_open(QAP_SESSION_BROADCAST, ms12_lib_handle);
    else if (session_broadcast && dts_formats)
        qap_session_handle = (qap_module_handle_t) qap_session_open(QAP_SESSION_BROADCAST, m8_lib_handle);
    else {
        if (!encode) {
            if ((input_config->format == QAP_AUDIO_FORMAT_AC3) ||
                (input_config->format == QAP_AUDIO_FORMAT_EAC3)) {
                qap_session_handle = (qap_module_handle_t) qap_session_open(QAP_SESSION_DECODE_ONLY, ms12_lib_handle);
            }
            if ((input_config->format == QAP_AUDIO_FORMAT_DTS) ||
                (input_config->format == QAP_AUDIO_FORMAT_DTS_HD)) {
                qap_session_handle = (qap_module_handle_t) qap_session_open(QAP_SESSION_DECODE_ONLY, m8_lib_handle);
            }
            if ( has_main2_input && (second_input.input_config.format == QAP_AUDIO_FORMAT_AC3) ||
                                    (second_input.input_config.format == QAP_AUDIO_FORMAT_EAC3)) {
                qap_session_handle_main2 = (qap_module_handle_t) qap_session_open(QAP_SESSION_DECODE_ONLY, ms12_lib_handle);
            }
            if ( has_main2_input && (second_input.input_config.format == QAP_AUDIO_FORMAT_DTS) ||
                                    (second_input.input_config.format == QAP_AUDIO_FORMAT_DTS_HD)) {
                qap_session_handle_main2 = (qap_module_handle_t) qap_session_open(QAP_SESSION_DECODE_ONLY, m8_lib_handle);
            }
        } else {
            if ((input_config->format == QAP_AUDIO_FORMAT_AC3) ||
                (input_config->format == QAP_AUDIO_FORMAT_EAC3)) {
                qap_session_handle = (qap_module_handle_t) qap_session_open(QAP_SESSION_ENCODE_ONLY, ms12_lib_handle);
            }
            if ((input_config->format == QAP_AUDIO_FORMAT_DTS) ||
                (input_config->format == QAP_AUDIO_FORMAT_DTS_HD)) {
                qap_session_handle = (qap_module_handle_t) qap_session_open(QAP_SESSION_ENCODE_ONLY, m8_lib_handle);
            }
            if ( has_main2_input && (second_input.input_config.format == QAP_AUDIO_FORMAT_AC3) ||
                                    (second_input.input_config.format == QAP_AUDIO_FORMAT_EAC3)) {
                qap_session_handle_main2 = (qap_module_handle_t) qap_session_open(QAP_SESSION_ENCODE_ONLY, ms12_lib_handle);
            }
            if ( has_main2_input && (second_input.input_config.format == QAP_AUDIO_FORMAT_DTS) ||
                                    (second_input.input_config.format == QAP_AUDIO_FORMAT_DTS_HD)) {
                qap_session_handle_main2 = (qap_module_handle_t) qap_session_open(QAP_SESSION_ENCODE_ONLY, m8_lib_handle);
            }
        }
    }

    if (qap_session_handle)
        ALOGD("%s %d main session handle %p\n", __func__, __LINE__, qap_session_handle);
    if (qap_session_handle_main2)
        ALOGD("%s %d second main session handle %p\n", __func__, __LINE__, qap_session_handle_main2);

    ret = qap_session_set_callback(qap_session_handle, &qap_session_callback);
    if (ret != QAP_STATUS_OK) {
        print("!!!! Please specify appropriate Session");
        return -1;
    }

    print("%s, out_dev_id = %x", __func__, out_dev_id);
    set_output_config(input_config, &session_output_config);

    ret = qap_session_cmd(qap_session_handle, QAP_SESSION_CMD_SET_OUTPUTS, sizeof(session_output_config), &session_output_config, NULL, NULL);
    if (ret != QAP_STATUS_OK) {
        print("Output config failed\n");
        return -1;
    }

    ret = qap_session_cmd(qap_session_handle, QAP_SESSION_CMD_SET_KVPAIRS, sizeof(kvp_string), kvp_string, NULL, NULL);
    if (ret != QAP_STATUS_OK) {
        print("Output config failed\n");
        ALOGD("%d Session set params failed %s\n", __LINE__, kvp_string);
    }

    if (kvp_string != NULL) {
        free(kvp_string);
        kvp_string = NULL;
    }
    if (!encode)
        input_config->module_type = QAP_MODULE_DECODER;
    else
        input_config->module_type = QAP_MODULE_ENCODER;


    if (0 == test_validate_extension(input_file_name_main, ".wav")) {
        input_config->flags = QAP_MODULE_FLAG_SYSTEM_SOUND;
        wave_main_input = true;
    }

    if (!session_broadcast && !has_main2_input && getmetadata) {
        input_config->flags = QAP_MODULE_FLAG_SECONDARY;
    }

    begin= time(NULL);
    printf("Select one of the below choice for timestamp modes \n"
           "1: Buffer With Frame Boundaries With Timestamps \n"
           "2: Buffer With Frame Boundaries With Cointiguous Timestamps \n"
           "3: Buffer With Frame Boundaries With Jump In Timestamps \n"
           "4: Buffer With Partail Frames With Contiguous Timestamps \n"
           "default: Buffer With Frame Boundaries With NO Timestamps \n"
           );
    fgets(temp, 5, stdin);
    sscanf(temp, "%d", &timestamp_mode);
    printf("Entered timestap mode choice: %d \n", timestamp_mode);

    ret = qap_module_init(qap_session_handle, input_config, &qap_module_handle);
    if (qap_module_handle == NULL) {
        print("Module Handle is Null\n");
    }

    if (session_broadcast && (has_assoc_input || has_system_input || has_main2_input) ) {
        second_input.session_handle =  qap_session_handle;
        second_input.session_output_config = session_output_config;
        second_input.input_config.module_type = input_config->module_type;
        if (has_system_input) {
            second_input.input_config.channels = system_input_channels;
            second_input.input_config.sample_rate = system_input_sample_rate;
            pthread_create(&system_input_thread, &sec_inpt_thrd_attr, (void *)secondary_file_playback, (void *)&second_input);
        } else if(has_main2_input) {
            second_input.input_config.channels = main2_input_channels;
            second_input.input_config.sample_rate = main2_input_sample_rate;
            pthread_create(&main2_input_thread, &sec_inpt_thrd_attr, (void *)secondary_file_playback, (void *)&second_input);
        } else {
            second_input.input_config.channels = input_config->channels;
            second_input.input_config.sample_rate = input_config->sample_rate;
            pthread_create(&assoc_input_thread, &sec_inpt_thrd_attr, (void *)secondary_file_playback, (void *)&second_input);
        }
    } else if (has_main2_input && !session_broadcast) {
        second_input.session_handle =  qap_session_handle_main2;
        second_input.input_config.channels = main2_input_channels;
        second_input.input_config.sample_rate = main2_input_sample_rate;
        second_input.input_config.module_type = input_config->module_type;
        if (getmetadata)
            second_input.input_config.flags = QAP_MODULE_FLAG_SECONDARY;
        set_output_config(&second_input.input_config, &second_input.session_output_config);
        pthread_create(&main2_input_thread, &sec_inpt_thrd_attr, (void *)secondary_file_playback, (void *)&second_input);
    }

    fp_main_input = fopen(input_file_name_main, "rb");
    if (fp_main_input == NULL) {
        print("Open File Failed for %s\n", input_file_name_main);
        return 0;
    }

    ret = qap_module_cmd(qap_module_handle, QAP_MODULE_CMD_START, sizeof(QAP_MODULE_CMD_START), NULL, NULL, NULL);
    if (ret != QAP_STATUS_OK) {
        printf("START failed\n");
        return -1;
    }

    buffer = (qap_audio_buffer_t *) calloc(1, sizeof(qap_audio_buffer_t));
    if (buffer == NULL) {
        print("Memory Alloc Error 1\n");
        return -1;
    }
    buffer->common_params.data = calloc(1, FRAME_SIZE);
    if (buffer->common_params.data == NULL) {
       print("Memory Alloc Error 2\n");
       return -1;
    }
    buffer->buffer_parms.output_buf_params.output_id = out_dev_id;


    do {
        if (wave_main_input) {
            if (first_read) {
                first_read = 0;
                if(skip_wave_header(fp_main_input) != 0)
                {
                    qap_module_deinit(qap_module_handle);
                    print("%s : %d , Invalid WAV file input! \n", __func__, __LINE__);
                    return;
                }
            }
        }
        if((timestamp_mode > 0) && (timestamp_mode < 5)) {
            buffer->buffer_parms.input_buf_params.flags = QAP_BUFFER_TSTAMP;
        }
        else {
            buffer->buffer_parms.input_buf_params.flags = QAP_BUFFER_NO_TSTAMP;
            no_timestamp_mode = true;
        }
        get_bytes_timestamps_wanted(timestamp_mode, input_config->format, buffer);
        if (buffer->common_params.timestamp == CONTIGUOUS_TIMESTAMP) {
            buffer->buffer_parms.input_buf_params.flags = QAP_BUFFER_TSTAMP_CONTINUE;
        }
        bytes_wanted = buffer->common_params.size;
        bytes_read = fread(data_buf, sizeof(unsigned char), bytes_wanted, fp_main_input);

        buffer->common_params.offset = 0;
        buffer->common_params.size = bytes_read;
        memcpy(buffer->common_params.data, data_buf, bytes_read);
        if (bytes_read == 0) {
            buffer->buffer_parms.input_buf_params.flags = QAP_BUFFER_EOS;
            bytes_consumed = qap_module_process(qap_module_handle, buffer);
            ret = qap_module_cmd(qap_module_handle, QAP_MODULE_CMD_STOP, sizeof(QAP_MODULE_CMD_STOP), NULL, NULL, NULL);
            print("Stopped feeding main input \n");
            break;
        }

        reply_data = (char*) calloc(1, 100);
        is_buffer_available = 0;
        temp_ptr = buffer->common_params.data;
        do {
            bytes_consumed = qap_module_process(qap_module_handle, buffer);
            if (bytes_consumed > 0) {
                buffer->common_params.data += bytes_consumed;
                buffer->common_params.size -= bytes_consumed;
            }
            print("%s %d feeding Main input of size %d  and bytes_cosumed is %d \n",
                      __FUNCTION__, __LINE__,bytes_read, bytes_consumed);
            if ((input_config->format == QAP_AUDIO_FORMAT_DTS) ||
                 (input_config->format == QAP_AUDIO_FORMAT_DTS_HD)) {
                  if (bytes_consumed < 0) {
                      while (!is_buffer_available) {
                          usleep(1000);
                          ret = qap_module_cmd(qap_module_handle, QAP_MODULE_CMD_GET_PARAM,
                                 sizeof(QAP_MODULE_CMD_GET_PARAM), "buf_available", NULL, reply_data);
                          if (reply_data)
                              temp_str = get_string_value(reply_data, &status);
                           if (temp_str) {
                               is_buffer_available = atoi(temp_str);
                               free(temp_str);
                           }
                           print("%s : %d, dts clip reply_data is %d buffer availabale is %d",
                                 __FUNCTION__, __LINE__, reply_data, is_buffer_available);
                       }
                  }
             }
        } while(buffer->common_params.size > 0);
        if (reply_data)
            free(reply_data);
        buffer->common_params.data = temp_ptr;
        end = time(NULL);
        if ((difftime(end, begin) >= device_switch_time) && device_switch) {
            for (i=0;i<num_outputs;i++) {
                session_output_config.output_config[i].id = get_new_device_id();
            }
            ret = qap_session_cmd(qap_session_handle, QAP_SESSION_CMD_SET_OUTPUTS, sizeof(session_output_config), &session_output_config, NULL, NULL);
            if (ret != QAP_STATUS_OK) {
                print("Output config failed\n");
            } else {
                device_switch = false;
            }
        }
        if (has_system_input || has_assoc_input) {
            usleep(5000); //To swtich between main and secondary threads incase of dual input
        }
    } while(1);

    if (!wave_main_input) {
        pthread_mutex_lock(&main_eos_lock);
        printf("Waiting for EOS event for main\n");
        pthread_cond_wait(&main_eos_cond, &main_eos_lock);
        pthread_mutex_unlock(&main_eos_lock);
        pthread_cond_destroy(&main_eos_cond);
        pthread_mutex_destroy(&main_eos_lock);
        ALOGV("Received EOS event for main input\n");
        printf("Received EOS event for main\n");
    }

    if(NULL != buffer &&  NULL != buffer->common_params.data) {
        free( buffer->common_params.data);
        buffer->common_params.data = NULL;
        free( buffer);
        buffer = NULL;
    }
    if (fp_main_input) {
        fclose(fp_main_input);
        fp_main_input = NULL;
    }
    if (fp_input_file) {
        fclose(fp_input_file);
        fp_input_file = NULL;
    }
    if (fp_output_timestamp_file) {
        fclose(fp_output_timestamp_file);
        fp_output_timestamp_file = NULL;
    }

    if (has_main2_input) {
        printf("Waiting for 2nd main EOS event\n");
        pthread_join(main2_input_thread, NULL);
    }  else if (has_assoc_input) {
        pthread_join(assoc_input_thread, NULL);
    }
    qap_module_deinit(qap_module_handle);
    qap_session_close(qap_session_handle); //close the session
    printf("%s : %d , THREAD EXIT \n", __func__, __LINE__);

    return 0;
}
