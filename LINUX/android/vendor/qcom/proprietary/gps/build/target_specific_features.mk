
#List of supported board platform targets
LOC_BOARD_PLATFORM_LIST := msm8960
LOC_BOARD_PLATFORM_LIST += msm8974
LOC_BOARD_PLATFORM_LIST += msm8226
LOC_BOARD_PLATFORM_LIST += msm8610
LOC_BOARD_PLATFORM_LIST += apq8084
LOC_BOARD_PLATFORM_LIST += msm8916
LOC_BOARD_PLATFORM_LIST += msm8994
LOC_BOARD_PLATFORM_LIST += msm8909
LOC_BOARD_PLATFORM_LIST += msm8952
LOC_BOARD_PLATFORM_LIST += msm8992
LOC_BOARD_PLATFORM_LIST += msm8996
LOC_BOARD_PLATFORM_LIST += msm8937
LOC_BOARD_PLATFORM_LIST += msm8953
LOC_BOARD_PLATFORM_LIST += msm8998
LOC_BOARD_PLATFORM_LIST += apq8098_latv
LOC_BOARD_PLATFORM_LIST += sdm660
LOC_BOARD_PLATFORM_LIST += sdm845
LOC_BOARD_PLATFORM_LIST += msmpeafowl
LOC_BOARD_PLATFORM_LIST += sdm670
LOC_BOARD_PLATFORM_LIST += qcs605
LOC_BOARD_PLATFORM_LIST += msmnile

GNSS_CFLAGS := \
    -Werror \
    -Wno-error=unused-parameter \
    -Wno-error=format \
    -Wno-error=macro-redefined \
    -Wno-error=reorder \
    -Wno-error=missing-braces \
    -Wno-error=self-assign \
    -Wno-error=enum-conversion \
    -Wno-error=logical-op-parentheses \
    -Wno-error=null-arithmetic \
    -Wno-error=null-conversion \
    -Wno-error=parentheses-equality \
    -Wno-error=undefined-bool-conversion \
    -Wno-error=tautological-compare \
    -Wno-error=switch \
    -Wno-error=date-time

ifneq (,$(filter $(LOC_BOARD_PLATFORM_LIST),$(TARGET_BOARD_PLATFORM)))
#default defines for all targets
GPS_LOC_API_V02_ENABLED := true
FEATURE_GNSS_BIT_API := true
GPS_FEATURES := \
        -D FEATURE_COARSE_POS_TIMESTAMP_AGE \
        -D FEATURE_LOC_MW_EFS_ACCESS \
        -D FEATURE_RPC_CHAR_ARRAY \
        -D FEATURE_GNSS_BIT_API
ifeq ($(TARGET_BOARD_PLATFORM),msm8960) # MSM8960 specific defines
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),msm8974) # MSM8974 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8226) # MSM8226 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8610) # MSM8610 specific defines
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),apq8084) # APQ8084 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8916) # MSM8916 specific defines
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),msm8994) # MSM8994 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8909) # MSM8909 specific defines
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),msm8952) # MSM8952 specific defines
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),msm8992) # MSM8992 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8996) # MSM8996 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8937) # msm8937 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8953) # MSM8953 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),msm8998) # MSM8998 specific defines
FEATURE_LOCTECH_SLIM_VNS := false
else ifeq ($(TARGET_BOARD_PLATFORM),apq8098_latv) # APQ8098_LATV specific defines
FEATURE_LOCTECH_SLIM_VNS := false
else ifeq ($(TARGET_BOARD_PLATFORM),sdm660) # SDM660 specific defines
else ifeq ($(TARGET_BOARD_PLATFORM),sdm845) # SDM845 specific defines
FEATURE_LOCTECH_SLIM_VNS := false
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),msmpeafowl) # msmpeafowl specific defines
FEATURE_LOCTECH_SLIM_VNS := false
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),sdm670) # sdm670 specific defines
FEATURE_LOCTECH_SLIM_VNS := false
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),qcs605) # qcs605 specific defines
FEATURE_LOCTECH_SLIM_VNS := false
FEATURE_LOCTECH_SLIM_DSPS := false
else ifeq ($(TARGET_BOARD_PLATFORM),msmnile) # msmnile specific defines
FEATURE_LOCTECH_SLIM_VNS := false
FEATURE_LOCTECH_SLIM_DSPS := false
endif
else #no supported target found
LOC_UNSUPPORTED_TARGET := true
endif #is-board-platform-in-list

ifeq ($(TARGET_DEVICE),apq8026_lw)
LW_FEATURE_SET := true
endif

ifeq ($(TARGET_SUPPORTS_ANDROID_WEAR),true)
LW_FEATURE_SET := true
endif
