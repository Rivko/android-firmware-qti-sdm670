/*
 * Copyright (c) 2017, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#define LOG_TAG "mmiDiagJNIInterface"

#include "msg.h"
#include "diag_lsm.h"
#include "diagpkt.h"
#include "diagcmd.h"

#include <jni.h>
#include <JNIHelp.h>
#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include "MmiDiagJNIInterface.h"

static JavaVM *jvm;
static jobject gDiagJNIInterfaceObject, gDiagCommandObject;
const char *kInterfacePath = "com/qualcomm/qti/qmmi/service/diag/MmiDiagJNIInterface";
const char *kDataPath = "com/qti/diagservices/DiagCommand";

#define FTM_AP_C  52
static char ftm_ap_dir[PATH_MAX] = { 0 };


#ifdef __cplusplus
extern "C" {
#endif

    static int diag_callback_handler(int cmd, const char *s) {
        int jniError;
        JNIEnv *env;
        bool threadAttached = false;
          MMI_ALOGI("diag_callback_handler started to handle cmd=%d,param=%s \n", cmd, s);

        if(jvm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
        {
            if(jvm->AttachCurrentThread(&env, NULL) != JNI_OK) {
                LOGE("diag_callback_handler: AttachCurrentThread error");
                return JNI_ERR;
            }
            threadAttached = true;
        }

        jclass cls = env->GetObjectClass(gDiagJNIInterfaceObject);

        if(!cls) {
            if(threadAttached)
                jvm->DetachCurrentThread();
            LOGE("diag_callback_handler: GetObjectClass error");
            return JNI_ERR;
        }

        jmethodID callback = env->GetStaticMethodID(cls, "commandHandler", "(ILjava/lang/String;)I");

        if(!callback) {
            if(threadAttached)
                jvm->DetachCurrentThread();
            LOGE("diag_callback_handler: GetStaticMethodID error");
            return JNI_ERR;
        }

        /* Finally call the callback */
        jniError = env->CallStaticIntMethod(cls, callback, cmd, env->NewStringUTF(s));
        if(threadAttached)
            jvm->DetachCurrentThread();

        return jniError;
    }

    /*  FFBM related function */
    static bool is_file(const char *path) {
        struct stat st;

        if(lstat(path, &st) == 0)
            return S_ISREG(st.st_mode) != 0;
        return false;
    }

    /**Erase all files under FTM_AP */
    static int erase_all_files() {

        struct dirent *de;
        DIR *dir;
        char filepath[PATH_MAX];

        dir = opendir(ftm_ap_dir);
        if(dir != 0) {
            while((de = readdir(dir))) {
                if((strncmp(de->d_name, ".", 2) == 0)
                   || (strncmp(de->d_name, "..", 3) == 0))
                    continue;
                snprintf(filepath, sizeof(filepath), "%s%s", ftm_ap_dir, de->d_name);
                if(is_file(filepath)) {
                    remove(filepath);
                    MMI_ALOGI("file(%s) be removed", filepath);
                }
            }
            closedir(dir);
        } else {
            MMI_ALOGE("dir(%s) open fail, error=%s", ftm_ap_dir, strerror(errno));
        }

        return FTM_SUCCESS;
    }

    /*List the FTM_AP directory contents to a file directory.Txt,
       in the FTM_AP directory(see 3.3.1.1).  The directory.Txt file
       can later be retrieved to desktop with FTM_AP_READ_FILE command. */
    static int dir_list_filename_to_file(const char *filename) {
        struct dirent *de;
        DIR *dir;
        FILE *fp = NULL;
        char filepath[PATH_MAX];
        char buf[255];

        memset(buf, 0, 255);

        snprintf(filepath, sizeof(filepath), "%s%s", ftm_ap_dir, filename);
        MMI_ALOGI("List the dir(%s) contents to file(%s)", ftm_ap_dir, filepath);
        fp = fopen(filepath, "w");
        if(!fp) {
            MMI_ALOGI("file(%s) open fail, error=%s", filepath, strerror(errno));
            return FTM_FAIL;
        }

        dir = opendir(ftm_ap_dir);
        if(dir != 0) {
            while((de = readdir(dir))) {
                if(!strncmp(de->d_name, ".", 1) || !strcmp(de->d_name, ".."))
                    continue;

                MMI_ALOGI("%s \n", de->d_name);
                snprintf(buf, sizeof(buf), "%s\n", de->d_name);
                fwrite(buf, strlen(buf), 1, fp);
            }
            closedir(dir);
        } else {
            MMI_ALOGE("dir(%s) open fail, error=%s", ftm_ap_dir, strerror(errno));
        }

        fclose(fp);
        return FTM_SUCCESS;
    }

    PACKED void *ftm_app_status(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_ap_status_response *rsp = NULL;

        //unordered_map < string, string > params;
        /* Allocate the same length as the request. */
        rsp =
            (ftm_ap_status_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_ap_status_response));
        if(rsp == NULL) {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        rsp->ftm_error_code = FTM_SUCCESS;
        rsp->state = diag_callback_handler(FTM_AP_TEST_APP_STATUS, "");
        rsp->fail_count = diag_callback_handler(FTM_SUBCMD_GET_FAIL_COUNT, "");
        MMI_ALOGD("ftm app status info: state=%ld, fail_count=%d", rsp->state, rsp->fail_count);

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_TEST_APP_STATUS;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;

        return rsp;
    }

    PACKED void *ftm_select_sequence(PACK(void *)req_pkt, uint16 pkt_len) {

        ftm_select_sequence_response *rsp = NULL;
        ftm_select_seq_req *req = NULL;
        int iNumTests = 0;
        char filepath[PATH_MAX] = { 0 };

        //unordered_map < string, string > params;

        req = (ftm_select_seq_req *) req_pkt;

        /* Allocate the same length as the request. */
        rsp =
            (ftm_select_sequence_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C,
                                                                  sizeof(ftm_select_sequence_response));
        if(rsp == NULL) {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        rsp->ftm_error_code = diag_callback_handler(FTM_AP_SELECT_SEQUENCE, req->filename);
        rsp->iNumTests = diag_callback_handler(FTM_SUBCMD_GET_SEQUENCE_COUNT, "");

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_SELECT_SEQUENCE;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;

        return rsp;
    }

    PACKED void *ftm_clear_results(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;

        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp == NULL) {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        rsp->ftm_error_code = diag_callback_handler(FTM_AP_CLEAR_RESULTS, "");

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_CLEAR_RESULTS;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACKED void *ftm_execute_single_test(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;
        ftm_single_test_case_req *req = NULL;
        char case_name[NAME_MAX] = { 0 };

        req = (ftm_single_test_case_req *) req_pkt;
        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp == NULL) {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }
        //  snprintf(case_name, sizeof(case_name), "%s:%s", KEY_CASE_NAME, req->test_case);

        rsp->ftm_error_code = diag_callback_handler(FTM_AP_EXECUTE_SINGLE_TEST, req->test_case);

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_EXECUTE_SINGLE_TEST;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;

        return rsp;
    }


    PACKED void *ftm_execute_all_tests(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;

        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp == NULL) {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }
        rsp->ftm_error_code = diag_callback_handler(FTM_AP_EXECUTE_ALL_TESTS, "");

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_EXECUTE_ALL_TESTS;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;

        return rsp;
    }


    PACKED void *ftm_read_file(PACK(void *)req_pkt, uint16 pkt_len) {

        ftm_read_file_response *rsp = NULL;
        ftm_read_file_req *req = NULL;
        FILE *fp = NULL;
        char filepath[255] = { 0 };
        uint16_t iSize = 0;
        uint8_t Data[PACK_SIZE];

        req = (ftm_read_file_req *) req_pkt;

        /*parse parameter */
        char *pFileName = req->filename;
        uint32_t offset = req->offset;
        uint16_t max_size = req->max_size;

        /* Allocate the same length as the request. */
        rsp =
            (ftm_read_file_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_read_file_response));
        if(rsp == NULL) {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        /*get dir */
        memset(Data, 0, sizeof(Data));

        snprintf(filepath, sizeof(filepath), "%s%s", ftm_ap_dir, pFileName);

        fp = fopen(filepath, "rb");
        if(fp) {
            fseek(fp, 0, SEEK_END); /*non-portable */
            rsp->file_size = ftell(fp);

            MMI_ALOGI("ftm_read_file: offset=%d,filesize=%ld \n", offset, rsp->file_size);

            if(offset < rsp->file_size) {
                fseek(fp, offset, SEEK_SET);
                iSize = fread(Data, 1, max_size, fp);
                rsp->ftm_error_code = READ_SUCCESS;
            } else
                rsp->ftm_error_code = READ_BAD_OFFSET_ERR;

            fclose(fp);
            fp = NULL;
        } else {
            rsp->ftm_error_code = READ_FILE_NOT_EXIST_ERR;
            MMI_ALOGE("file(%s) open fail, error=%s\n", filepath, strerror(errno));
        }

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_READ_FILE;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;

        rsp->offset = offset;
        rsp->size = iSize;
        MMI_ALOGI("ftm_read_file: iSize=%d", iSize);
        memcpy(rsp->Data, Data, sizeof(rsp->Data));

        return rsp;
    }

    /* write file request struct
       CMD_CODE           Unsigned / 1 byte                 Command ID - Set CMD_CODE to 75
       SUB_SYS_ID             Unsigned / 1 byte                 Subsystem ID - FTM ID is 11
       SUBSYS_CMD_CODE        Unsigned / 2 bytes                FTM Mode ID - FTM_AP_C (52)
       FTM_CMD_ID             Unsigned / 2 bytes                6 - FTM_AP_WRITE_FILE
       FTM_DATA_LEN       Unsigned / 2 bytes                Unused, set to 0
       FTM_RSP_PKT_SIZE   Unsigned / 2 bytes                Unused, set to 0
       FILENAME           Variable length ASCII             Null terminated  The file to be read.
       The filename cannot contain any path "/"
       MORE_DATA              Unsigned / 1 bytes                0 = no more data   1 = more data
       SIZE               Unsigned / 2 bytes                The actual number of bytes transfer in DATA portion
       DATA               Variable length binary            The data stream
     */
    PACKED void *ftm_write_file(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;

        char filepath[255] = { 0 };
        static FILE *fp = NULL;
        uint8_t *pReq = (unsigned char *) req_pkt;
        char *filename = (char *) (pReq + WRITE_FILENAME_OFFSET);

        uint8_t append_data = *(unsigned char *) (pReq + WRITE_APPEND_DATA_OFFSET(filename));
        uint16_t i_size = *(unsigned short *) (pReq + WRITE_ISIZE_OFFSET(filename));
        unsigned char *pData = (unsigned char *) &pReq[WRITE_DATA_OFFSET(filename)];


        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        snprintf(filepath, sizeof(filepath), "%s%s", ftm_ap_dir, filename);

        if(!append_data)
            fp = fopen(filepath, "w");
        else
            fp = fopen(filepath, "a+");

        if(fp) {
            if(i_size > 0)
                fwrite(pData, 1, i_size, fp);

            MMI_ALOGI("write file: i_size=%d", i_size);
            rsp->ftm_error_code = WRITE_SUCCESS;
            fclose(fp);
            fp = NULL;
        } else {
            rsp->ftm_error_code = WRITE_OPEN_ERR;
        }

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_WRITE_FILE;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;

        return rsp;
    }


    PACKED void *ftm_test_list_to_file(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;
        char filepath[PATH_MAX];

        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        rsp->ftm_error_code = diag_callback_handler(FTM_AP_TEST_LIST_TO_FILE, "");

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_TEST_LIST_TO_FILE;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }


    PACKED void *ftm_erase_all_files(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;

        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        rsp->ftm_error_code = erase_all_files();    /*success */

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_ERASE_ALL_FILES;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACKED void *ftm_dir_to_file(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;

        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        rsp->ftm_error_code = dir_list_filename_to_file(DIRECTORY_FILENAME);    /*success */

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_DIR_TO_FILE;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACKED void *ftm_exit_test(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;
        ftm_exit_test_req *req = NULL;
        char params[256] = { 0 };

        req = (ftm_exit_test_req *) req_pkt;
        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }
        MMI_ALOGI("get test result from host: %d \n", req->result);
        snprintf(params, sizeof(params), "%s:%d", req->test_case, req->result);

        rsp->ftm_error_code = diag_callback_handler(FTM_AP_EXIT_TEST, params);

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_EXIT_TEST;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACKED void *ftm_append_to_res(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;
        ftm_append_to_res_req *req = NULL;
        char params[256] = { 0 };

        req = (ftm_append_to_res_req *) req_pkt;
        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        snprintf(params, sizeof(params), "%s:%s", req->test_case, req->line);
        rsp->ftm_error_code = diag_callback_handler(FTM_AP_APPEND_TO_RES, params);

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_APPEND_TO_RES;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACKED void *ftm_utility(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;
        ftm_ap_utility_req *req = NULL;
        char params[256] = { 0 };
        req = (ftm_ap_utility_req *) req_pkt;
        /* Allocate the same length as the request. */
        rsp = (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C, sizeof(ftm_common_response));

        if(req != NULL && rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        snprintf(params, sizeof(params), "%d:%d", req->utility_id, req->utility_ops);
        LOGI("ftm_utility called!,params:%s",params);
        rsp->ftm_error_code = diag_callback_handler(FTM_AP_UTILITY, params);

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_UTILITY;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACKED void *ftm_set_token(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_common_response *rsp = NULL;
        ftm_set_token_req *req = NULL;
        char params[256] = { 0 };

        req = (ftm_set_token_req *) req_pkt;
        /* Allocate the same length as the request. */
        rsp =
            (ftm_common_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C,
                                                               sizeof(ftm_common_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }

        snprintf(params, sizeof(params), "%s:%s", req->test_case, req->token);
        rsp->ftm_error_code = diag_callback_handler(FTM_AP_SET_TOKEN, params);

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_SET_TOKEN;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACKED void *ftm_read_app_version(PACK(void *)req_pkt, uint16 pkt_len) {
        ftm_read_version_response *rsp = NULL;

        /* Allocate the same length as the request. */
        rsp =
            (ftm_read_version_response *) diagpkt_subsys_alloc(DIAG_SUBSYS_FTM, FTM_AP_C,
                                                               sizeof(ftm_read_version_response));

        if(rsp != NULL) {
            MMI_ALOGI("diagpkt_subsys_alloc succeeded");
        } else {
            MMI_ALOGE("diagpkt_subsys_alloc failed");
            return NULL;
        }
        int version_code= diag_callback_handler(FTM_AP_READ_APP_VERSION, "");
        snprintf(rsp->Data, sizeof(rsp->Data), "%d", version_code);
        rsp->ftm_error_code = FTM_SUCCESS;

        rsp->sftm_header.cmd_code = 75;
        rsp->sftm_header.sub_sys_id = DIAG_SUBSYS_FTM;
        rsp->sftm_header.sub_sys_cmd_code = FTM_AP_C;
        rsp->sftm_header.ftm_cmd_id = FTM_AP_READ_APP_VERSION;
        rsp->sftm_header.ftm_data_len = 0;
        rsp->sftm_header.ftm_rsp_pkt_size = 0;
        return rsp;
    }

    PACK(void *) ftm_ap_dispatch(PACK(void *)req_pkt, uint16 pkt_len) {
        LOGD("ftm_ap_dispatch called!");
        PACK(void *) rsp_pkt = NULL;

        ftm_header *pheader = (ftm_header *) req_pkt;
        uint16_t iCmd = pheader->ftm_cmd_id;

        MMI_ALOGI("Start to process ftm cmd, cmd id=%d\n", iCmd);
        switch (iCmd) {
        case FTM_AP_TEST_APP_STATUS:
            rsp_pkt = ftm_app_status(req_pkt, pkt_len);
            break;
        case FTM_AP_SELECT_SEQUENCE:
            rsp_pkt = ftm_select_sequence(req_pkt, pkt_len);
            break;
        case FTM_AP_CLEAR_RESULTS:
            rsp_pkt = ftm_clear_results(req_pkt, pkt_len);
            break;
        case FTM_AP_EXECUTE_SINGLE_TEST:
            rsp_pkt = ftm_execute_single_test(req_pkt, pkt_len);
            break;
        case FTM_AP_EXECUTE_ALL_TESTS:
            rsp_pkt = ftm_execute_all_tests(req_pkt, pkt_len);
            break;
        case FTM_AP_READ_FILE:
            rsp_pkt = ftm_read_file(req_pkt, pkt_len);
            break;
        case FTM_AP_WRITE_FILE:
            rsp_pkt = ftm_write_file(req_pkt, pkt_len);
            break;
        case FTM_AP_TEST_LIST_TO_FILE:
            rsp_pkt = ftm_test_list_to_file(req_pkt, pkt_len);
            break;
        case FTM_AP_ERASE_ALL_FILES:
            rsp_pkt = ftm_erase_all_files(req_pkt, pkt_len);
            break;
        case FTM_AP_DIR_TO_FILE:
            rsp_pkt = ftm_dir_to_file(req_pkt, pkt_len);
            break;
        case FTM_AP_EXIT_TEST:
            rsp_pkt = ftm_exit_test(req_pkt, pkt_len);
            break;
        case FTM_AP_UTILITY:
            rsp_pkt = ftm_utility(req_pkt, pkt_len);
            break;
        case FTM_AP_APPEND_TO_RES:
            rsp_pkt = ftm_append_to_res(req_pkt, pkt_len);
            break;
        case FTM_AP_READ_APP_VERSION:
            rsp_pkt = ftm_read_app_version(req_pkt, pkt_len);
            break;
        case FTM_AP_SET_TOKEN:
            rsp_pkt = ftm_set_token(req_pkt, pkt_len);
            break;
        default:
            break;
        }
        MMI_ALOGI("ftm cmd(cmd id=%d) process finished\n", iCmd);

        return rsp_pkt;
    }

    static const diagpkt_user_table_entry_type ftm_mmi_diag_func_table[] = {
        {FTM_AP_C, FTM_AP_C, ftm_ap_dispatch},
    };

    static void com_qualcomm_qti_qmmi_service_diag_set_file_base_dir(JNIEnv * env, jobject clazz, jstring file_base_dir) {
        if(file_base_dir != NULL) {
            const char *base_dir = env->GetStringUTFChars(file_base_dir, 0);

            strlcpy(ftm_ap_dir, base_dir, sizeof(ftm_ap_dir));
            MMI_ALOGI("file base dir: %s", ftm_ap_dir);
        }
    }

    static JNINativeMethod gDiagMethods[] = {
        /* name, signature, funcPtr */
        {"native_set_file_base_dir", "(Ljava/lang/String;)V",
         (void *) com_qualcomm_qti_qmmi_service_diag_set_file_base_dir},
    };

    int register_DiagService(JNIEnv * env) {
        return jniRegisterNativeMethods(env, "com/qualcomm/qti/qmmi/service/diag/MmiDiagService", gDiagMethods,
                                        NELEM(gDiagMethods));
    }

    jint JNI_OnLoad(JavaVM * vm, void *reserved) {
        JNIEnv *env;
        int status;

        jvm = vm;
        if(vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
            MMI_ALOGE("Failed to get the environment using GetEnv()");
            return JNI_ERR;
        }

        if((status = register_DiagService(env)) < 0) {
            MMI_ALOGE("com.qualcomm.qti.qmmi.service.diag.MmiDiagService: jni registration failure: %d", status);
            return JNI_ERR;
        }

        jclass diagJNIInterface = env->FindClass(kInterfacePath);
        jmethodID diagJNIInit = env->GetMethodID(diagJNIInterface, "<init>", "()V");
        jobject diagJNIObject = env->NewObject(diagJNIInterface, diagJNIInit);

        gDiagJNIInterfaceObject = env->NewGlobalRef(diagJNIObject);


        /* Register for diag packets */
        boolean bInit_Success = FALSE;

        bInit_Success = Diag_LSM_Init(NULL);

        if(!bInit_Success) {
            MMI_ALOGE("Diag_LSM_Init call failed");
            return JNI_ERR;
        }

        MMI_ALOGI("Diag_LSM_Init call succeeded");

        DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_FTM, ftm_mmi_diag_func_table);
        return JNI_VERSION_1_4;
    }
#ifdef __cplusplus
}
#endif
