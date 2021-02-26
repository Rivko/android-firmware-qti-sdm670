#include <jni.h>
#include <string>
#include "util.h"
#include "cmd.h"

extern "C" {

    class cmd androidCmd;

    uint32_t irExposureValue = 0;
    uint32_t irSetExposureValue = 0;
    uint32_t irGainValue = 0x60;

    jstring Java_com_qualcomm_qti_threedr_OrbecNative_stringFromJNI(
            JNIEnv *env,
            jobject /* this */) {
        std::string hello = "Hello from C++";
        return env->NewStringUTF(hello.c_str());
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_cmdInitFromJNI(
            JNIEnv *env,
            jobject clazz,
            jboolean isEnable) {
        LOGE("cmd init!!!");
        androidCmd.init();
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_enableLaserFromJNI(
            JNIEnv *env,
            jobject clazz,
            jboolean isEnable) {
        LOGE("set laser : %d", isEnable);
        androidCmd.emitter_set(isEnable);
    }

    jstring Java_com_qualcomm_qti_threedr_OrbecNative_getSerialNumberFromJNI(
            JNIEnv *env,
            jobject clazz) {
        LOGE("get serial number!");
        std::string sn = androidCmd.get_sn_number();
        return env->NewStringUTF(sn.c_str());
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_getCmosParamFromJNI(
            JNIEnv *env,
            jobject clazz) {
        LOGE("get cmos param!");
        androidCmd.get_cmos_params();
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_enableLdpFromJNI(
            JNIEnv *env,
            jobject clazz,
            jboolean isEnable) {
        LOGE("set ldp : %d\n", isEnable);
        androidCmd.ldp_set(isEnable);
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_enableIrFloodLedFromJNI(
            JNIEnv *env,
            jobject clazz,
            jboolean isEnable) {
        LOGE("set ir flood led : %d\n", isEnable);
        androidCmd.ir_flood_set(isEnable);
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_irGainAddFromJNI(
            JNIEnv *env,
            jobject clazz,
            jint changeValue) {
        LOGE("ir gain add!");
        irGainValue += changeValue;
        androidCmd.ir_gain_set(irGainValue);
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_irGainSubFromJNI(
            JNIEnv *env,
            jobject clazz,
            jint changeValue) {
        LOGE("ir gain sub!");
        irGainValue -= changeValue;
        androidCmd.ir_gain_set(irGainValue);
    }

    jint Java_com_qualcomm_qti_threedr_OrbecNative_getIrGainFromJNI(
            JNIEnv *env,
            jobject clazz) {
        return irGainValue;
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_setIrGainFromJNI(
            JNIEnv *env,
            jobject clazz,
            jint gain) {
        androidCmd.ir_gain_set((uint32_t) gain);
        irGainValue = (uint32_t) gain;
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_irExposureAddFromJNI(
            JNIEnv *env,
            jobject clazz,
            jint changeValue) {
        LOGE("ir exposure add!");
        androidCmd.ir_exposure_get(irExposureValue);
        irSetExposureValue += changeValue;
        irExposureValue = irSetExposureValue;
        androidCmd.ir_exposure_set(irExposureValue);
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_irExposureSubFromJNI(
            JNIEnv *env,
            jobject clazz,
            jint changeValue) {
        LOGE("ir exposure sub!");
        androidCmd.ir_exposure_get(irExposureValue);
        if (irExposureValue == 0x100)
            return;
        irSetExposureValue -= changeValue;
        irExposureValue = irSetExposureValue;
        androidCmd.ir_exposure_set(irExposureValue);
    }

    jint Java_com_qualcomm_qti_threedr_OrbecNative_getIrExposureFromJNI(
            JNIEnv *env,
            jobject clazz) {
        return irExposureValue;
    }

    void Java_com_qualcomm_qti_threedr_OrbecNative_setIrExposureFromJNI(
            JNIEnv *env,
            jobject clazz,
            jint exposure) {
        androidCmd.ir_exposure_set((uint32_t) exposure);
        irExposureValue = (uint32_t) exposure;
    }

    JNIEXPORT jobject JNICALL
    Java_com_qualcomm_qti_threedr_OrbecNative_getOBCmosParams(JNIEnv *env, jobject instance) {
        OBCameraParams params = androidCmd.get_cmos_params();

        /**
         * Create new instance of java class and fill it with data
         */
        jclass clazzOBCameraParams = env->FindClass("com/qualcomm/qti/threedr/orbbec/OBCameraParams");
        if(clazzOBCameraParams == NULL){
            LOGE("clazzOBCameraParams is null");
        }

        jmethodID constructorOBCameraParams = env->GetMethodID(clazzOBCameraParams, "<init>", "([F[F[F[F[F[FI)V");
        if(constructorOBCameraParams == NULL){
            LOGE("constructorOBCameraParams null");
        }


        //float l_intr_p[4];//[fx,fy,cx,cy]
        //float r_intr_p[4];//[fx,fy,cx,cy]
        //float r2l_r[9];//[r00,r01,r02;r10,r11,r12;r20,r21,r22]
        //float r2l_t[3];//[t1,t2,t3]
        //float l_k[5];//[k1,k2,k3,p1,p2]
        //float r_k[5];//[k1,k2,k3,p1,p2]
        //int is_m;
        jfloatArray l_intr_p_arr = env->NewFloatArray(4 /*num of items in array*/);
        env->SetFloatArrayRegion(l_intr_p_arr, 0, 4, params.l_intr_p);

        jfloatArray r_intr_p_arr = env->NewFloatArray(4 /*num of items in array*/);
        env->SetFloatArrayRegion(r_intr_p_arr, 0, 4, params.r_intr_p);

        jfloatArray r2l_r_arr = env->NewFloatArray(9 /*num of items in array*/);
        env->SetFloatArrayRegion(r2l_r_arr, 0, 9, params.r2l_r);

        jfloatArray r2l_t_arr = env->NewFloatArray(3 /*num of items in array*/);
        env->SetFloatArrayRegion(r2l_t_arr, 0, 3, params.r2l_t);

        jfloatArray l_k_arr = env->NewFloatArray(5 /*num of items in array*/);
        env->SetFloatArrayRegion(l_k_arr, 0, 5, params.l_k);

        jfloatArray r_k_arr = env->NewFloatArray(5 /*num of items in array*/);
        env->SetFloatArrayRegion(r_k_arr, 0, 5, params.r_k);

        jobject jobjectOBCameraParams = env->NewObject(clazzOBCameraParams,
                                                               constructorOBCameraParams,
                                                               l_intr_p_arr,
                                                               r_intr_p_arr,
                                                               r2l_r_arr,
                                                               r2l_t_arr,
                                                               l_k_arr,
                                                               r_k_arr,
                                                               params.is_m);
        if(jobjectOBCameraParams == NULL){
                LOGE("jobjectOBCameraParams object is null");
        }

        return jobjectOBCameraParams;
    }
}
