#!/bin/bash

#/******************************************************************************
#  @file    perflock-test-suite.sh
#  @brief   Implementation for perflock verification.
#           Types of tests : Basic, Concurrency, Crash-Recovery, Hint, Profiles
#
#  DESCRIPTION
#
#  ---------------------------------------------------------------------------
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/

DEBUG="false"

#Perflock Verification native executable binary path
PERFLOCK_NATIVE_TEST_EXECUTABLE="/data/nativetest/perflock_native_test/perflock_native_test"
BOOST_CONFIG_XML_PARSER_EXECUTABLE="/data/nativetest/boostConfigParser/boostConfigParser"
PERFBOOST_CONFIG_PARSED_OUTPUT_FILE="/data/nativetest/boostConfigParser/perfHintConfig.txt"
PERFBOOST_CONFIG_XML="/vendor/etc/perf/perfboostsconfig.xml"
PERFLOCK_PROFILE_CONF_FILE_PATH="/vendor/etc/perf/*.conf"

#Macros to simulate Logging
RED=`tput setaf 1`
GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
BLUE=`tput setaf 4`
WHITE=`tput setaf 7`
RESET=`tput sgr0`

readonly SCRIPT_NAME=$(basename $0)

TAGI="Info: "
TAGE="Error: "
TAGW="Warning: "
TAGV="Verbose: "

function QLOGE() {
    echo "$RED $SCRIPT_NAME $TAGE $@ $RESET"
    logger -p user.notice -t $SCRIPT_NAME "$@"
}

function QLOGW() {
    echo "$YELLOW $SCRIPT_NAME $TAGW $@ $RESET"
    logger -p user.notice -t $SCRIPT_NAME "$@"
}

function QLOGI() {
    echo "$GREEN $SCRIPT_NAME $TAGI $@ $RESET"
    logger -p user.notice -t $SCRIPT_NAME "$@"
}

function QLOGV() {
    if [[ $DEBUG == "true" ]]
    then
        echo "$BLUE $SCRIPT_NAME $TAGV $@ $RESET"
        logger -p user.notice -t $SCRIPT_NAME "$@"
    fi
}

let storage_type=0;
let resolution=0;

# ------------------------------------------------------------------------

#***********************************************************
#*                                                         *
#*---------------- Perf Lock v2.0 Resource Namelist -------*
#*                                                         *
#***********************************************************

declare -a old_resources="
DISPLAY
POWER_COLLAPSE
CPU0_MIN_FREQ
CPU1_MIN_FREQ
CPU2_MIN_FREQ
CPU3_MIN_FREQ
UNSUPPORTED_0
CLUSTR_0_CPUS_ON
CLUSTR_0_MAX_CORES
UNSUPPORTED_2
UNSUPPORTED_3
SAMPLING_RATE
ONDEMAND_IO_IS_BUSY
ONDEMAND_SAMPLING_DOWN_FACTOR
INTERACTIVE_TIMER_RATE
INTERACTIVE_HISPEED_FREQ
INTERACTIVE_HISPEED_LOAD
SYNC_FREQ
OPTIMAL_FREQ
SCREEN_PWR_CLPS
THREAD_MIGRATION
CPU0_MAX_FREQ
CPU1_MAX_FREQ
CPU2_MAX_FREQ
CPU3_MAX_FREQ
ONDEMAND_ENABLE_STEP_UP
ONDEMAND_MAX_INTERMEDIATE_STEPS
IO_IS_BUSY_INTERACTIVE_CLUSTER_BIG
KSM_RUN_STATUS
KSM_PARAMS
SCHED_BOOST
CPU4_MIN_FREQ
CPU5_MIN_FREQ
CPU6_MIN_FREQ
CPU7_MIN_FREQ
CPU4_MAX_FREQ
CPU5_MAX_FREQ
CPU6_MAX_FREQ
CPU7_MAX_FREQ
CPU0_INTERACTIVE_ABOVE_HISPEED_DELAY
CPU0_INTERACTIVE_BOOST
CPU0_INTERACTIVE_BOOSTPULSE
CPU0_INTERACTIVE_BOOSTPULSE_DURATION
CPU0_INTERACTIVE_GO_HISPEED_LOAD
CPU0_INTERACTIVE_HISPEED_FREQ
CPU0_INTERACTIVE_IO_IS_BUSY
CPU0_INTERACTIVE_MIN_SAMPLE_TIME
CPU0_INTERACTIVE_TARGET_LOADS
CPU0_INTERACTIVE_TIMER_RATE
CPU0_INTERACTIVE_TIMER_SLACK
CPU4_INTERACTIVE_ABOVE_HISPEED_DELAY
CPU4_INTERACTIVE_BOOST
CPU4_INTERACTIVE_BOOSTPULSE
CPU4_INTERACTIVE_BOOSTPULSE_DURATION
CPU4_INTERACTIVE_GO_HISPEED_LOAD
CPU4_INTERACTIVE_HISPEED_FREQ
CPU4_INTERACTIVE_IO_IS_BUSY
CPU4_INTERACTIVE_MIN_SAMPLE_TIME
CPU4_INTERACTIVE_TARGET_LOADS
CPU4_INTERACTIVE_TIMER_RATE
CPU4_INTERACTIVE_TIMER_SLACK
CLUSTR_1_MAX_CORES
SCHED_PREFER_IDLE
SCHED_MIGRATE_COST
SCHED_SMALL_TASK
SCHED_MOSTLY_IDLE_LOAD
SCHED_MOSTLY_IDLE_NR_RUN
SCHED_INIT_TASK_LOAD
VIDEO_DECODE_PLAYBACK_HINT
DISPLAY_LAYER_HINT
VIDEO_ENCODE_PLAYBACK_HINT
CPUBW_HWMON_MIN_FREQ
CPUBW_HWMON_DECAY_RATE
CPUBW_HWMON_IO_PERCENT
CPU0_INTERACTIVE_MAX_FREQ_HYSTERESIS
CPU4_INTERACTIVE_MAX_FREQ_HYSTERESIS
GPU_DEFAULT_PWRLVL
CLUSTR_1_CPUS_ON
SCHED_UPMIGRATE
SCHED_DOWNMIGRATE
SCHED_MOSTLY_IDLE_FREQ
IRQ_BALANCER
INTERACTIVE_USE_SCHED_LOAD
INTERACTIVE_USE_MIGRATION_NOTIF
INPUT_BOOST_RESET
"

#***********************************************************
#*                                                         *
#*---------------- Perf Lock v3.0 Resource List -----------*
#*                                                         *
#***********************************************************

declare -a resource_group="
POWER_COLLAPSE;0x40400000;0x0;0x1;"/sys/module/lpm_levels/parameters/sleep_disabled";
MIN_FREQ_CLUSTER_BIG_CORE_0;0x40800000;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MIN_FREQ_CLUSTER_BIG_CORE_1;0x40800010;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MIN_FREQ_CLUSTER_BIG_CORE_2;0x40800020;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MIN_FREQ_CLUSTER_BIG_CORE_3;0x40800030;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MIN_FREQ_CLUSTER_LITTLE_CORE_0;0x40800100;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MIN_FREQ_CLUSTER_LITTLE_CORE_1;0x40800110;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MIN_FREQ_CLUSTER_LITTLE_CORE_2;0x40800120;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MIN_FREQ_CLUSTER_LITTLE_CORE_3;0x40800130;0x3e6;0x4e6;"/sys/module/msm_performance/parameters/cpu_min_freq";
MAX_FREQ_CLUSTER_BIG_CORE_0;0x40804000;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
MAX_FREQ_CLUSTER_BIG_CORE_1;0x40804010;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
MAX_FREQ_CLUSTER_BIG_CORE_2;0x40804020;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
MAX_FREQ_CLUSTER_BIG_CORE_3;0x40804030;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
MAX_FREQ_CLUSTER_LITTLE_CORE_0;0x40804100;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
MAX_FREQ_CLUSTER_LITTLE_CORE_1;0x40804110;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
MAX_FREQ_CLUSTER_LITTLE_CORE_2;0x40804120;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
MAX_FREQ_CLUSTER_LITTLE_CORE_3;0x40804130;0x510;0x708;"/sys/module/msm_performance/parameters/cpu_max_freq";
SCHED_BOOST;0x40C00000;0x1;0x0;"/proc/sys/kernel/sched_boost";
SCHED_PREFER_IDLE;0x40C04000;0x0;0x1;"/sys/devices/system/cpu/cpu%d/sched_prefer_idle";
SCHED_MIGRATE_COST;0x40C08000;0xf0;0xff;"/proc/sys/kernel/sched_migration_cost_ns";
SCHED_SMALL_TASK;0x40C0C000;0x10;0x20;"/proc/sys/kernel/sched_small_task";
SCHED_MOSTLY_IDLE_LOAD;0x40C10000;0x23;0x30;"/sys/devices/system/cpu/cpu%d/sched_mostly_idle_load";
SCHED_MOSTLY_IDLE_NR_RUN;0x40C14000;0x10;0x20;"/sys/devices/system/cpu/cpu%d/sched_mostly_idle_nr_run";
SCHED_INIT_TASK_LOAD;0x40C18000;0x1f;0x2f;"/proc/sys/kernel/sched_init_task_load";
SCHED_UPMIGRATE;0x40C1C000;0x40;0x62;"/proc/sys/kernel/sched_upmigrate";
SCHED_DOWNMIGRATE;0x40C20000;0x20;0x10;"/proc/sys/kernel/sched_downmigrate";
SCHED_MOSTLY_IDLE_FREQ;0x40C24000;0x4fe;0x5ff;"/sys/devices/system/cpu/cpu%d/sched_mostly_idle_freq";
SCHED_GROUP;0x40C28000;NA;NA;"NA";
SCHED_SPILL_NR_RUN;0x40C2C000;0x20;0x40;"/proc/sys/kernel/sched_spill_nr_run";
SCHED_STATIC_CPU_PWR_COST;0x40C30000;0x1;0x2;"/sys/devices/system/cpu/cpu%d/sched_static_cpu_pwr_cost";
SCHED_RESTRICT_CLUSTER_SPILL;0x40C34000;0x0;0x1;"/proc/sys/kernel/sched_restrict_cluster_spill";
SCHED_FREQ_AGGR_GROUP;0x40C38000;0x1;0x2;"NA";
SCHED_CPUSET_TOP_APP;0x40C3C000;0x1;0x2;"/dev/cpuset/top-app/cpus";
SCHED_CPUSET_FOREGROUND;0x40C40000;0x1;0x2;"/dev/cpuset/foreground/cpus";
SCHED_CPUSET_SYSTEM_BACKGROUND;0x40C44000;0x1;0x2;"/dev/cpuset/system-background/cpus";
SCHED_CPUSET_BACKGROUND;0x40C48000;0x1;0x2;"/dev/cpuset/background/cpus";
SCHED_SET_FREQ_AGGR;0x40C4C000;0x0;0x1;"/proc/sys/kernel/sched_freq_aggregate";
SCHED_ENABLE_THREAD_GROUPING;0x40C50000;0x1;0x0;"/proc/sys/kernel/sched_enable_thread_grouping";
SCHED_GROUP_UPMIGRATE;0x40C54000;0x50;0xC8;"/proc/sys/kernel/sched_group_upmigrate";
SCHED_GROUP_DOWNMIGRATE;0x40C58000;0x32;0x20;"/proc/sys/kernel/sched_group_downmigrate";
SCHED_FREQ_AGGR_THRESHOLD;0x40C5C000;0x300;0x500;"/proc/sys/kernel/sched_freq_aggregate_threshold";
MIN_ONLINE_CPU_CLUSTER_BIG;0x41000000;0x2;0x4;"/sys/devices/system/cpu/cpu*/core_ctl/min_cpus";
MIN_ONLINE_CPU_CLUSTER_LITTLE;0x41000100;0x1;0x2;"/sys/devices/system/cpu/cpu*/core_ctl/min_cpus";
MAX_ONLINE_CPU_CLUSTER_BIG;0x41004000;0x2;0x4;"/sys/devices/system/cpu/cpu*/core_ctl/max_cpus";
MAX_ONLINE_CPU_CLUSTER_LITTLE;0x41004100;0x1;0x2;"/sys/devices/system/cpu/cpu*/core_ctl/max_cpus";
ABOVE_HISPEED_DELAY_INTERACTIVE_CLUSTER_BIG;0x41400000;0x15;0x25;"/sys/devices/system/cpu/cpu4/cpufreq/interactive/above_hispeed_delay";
BOOST_INTERACTIVE_CLUSTER_BIG;0x41404000;0x1;0x2;"/sys/devices/system/cpu/cpu4/cpufreq/interactive/boost";
BOOSTPULSE_INTERACTIVE_CLUSTER_BIG;0x41408000;0x1;0x2;"/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse";
BOOSTPULSE_DURATION_INTERACTIVE_CLUSTER_BIG;0x4140C000;0x60;0x70;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/boostpulse_duration";
GO_HISPEED_LOAD_INTERACTIVE_CLUSTER_BIG;0x41410000;0x3B;0x4B;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/go_hispeed_load";
HISPEED_FREQ_INTERACTIVE_CLUSTER_BIG;0x41414000;0x2C0;0x4C0;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/hispeed_freq";
IO_IS_BUSY_INTERACTIVE_CLUSTER_BIG;0x41418000;0x0;0x1;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/io_is_busy";
MIN_SAMPLE_TIME_INTERACTIVE_CLUSTER_BIG;0x4141C000;0x10;0x20;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/min_sample_time";
TARGET_LOADS_INTERACTIVE_CLUSTER_BIG;0x41420000;0x30;0x40;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/target_loads";
TIMER_RATE_INTERACTIVE_CLUSTER_BIG;0x41424000;0x20;0x60;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/timer_rate";
TIMER_SLACK_INTERACTIVE_CLUSTER_BIG;0x41428000;0x5C4;0x9C4;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/timer_slack";
MAX_FREQ_HYSTERESIS_INTERACTIVE_CLUSTER_BIG;0x4142C000;0x45;0x60;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/max_freq_hysteresis";
USE_SCHED_LOAD_INTERACTIVE_CLUSTER_BIG;0x41430000;0x1;0x0;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/use_sched_load";
USE_MIGRATION_NOTIF_CLUSTER_BIG;0x41434000;0x0;0x1;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/use_migration_notif";
IGNORE_HISPEED_NOTIF_CLUSTER_BIG;0x41438000;0x0;0x1;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/ignore_hispeed_on_notif";
ABOVE_HISPEED_DELAY_INTERACTIVE_CLUSTER_LITTLE;0x41400100;0x20;0x40;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/above_hispeed_delay";
BOOST_INTERACTIVE_CLUSTER_LITTLE;0x41404100;0x1;0x2;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/boost";
BOOSTPULSE_INTERACTIVE_CLUSTER_LITTLE;0x41408100;0x1;0x2;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/boostpulse";
BOOSTPULSE_DURATION_INTERACTIVE_CLUSTER_LITTLE;0x4140C100;0x5C4;0x9C4;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/boostpulse_duration";
GO_HISPEED_LOAD_INTERACTIVE_CLUSTER_LITTLE;0x41410100;0x3B;0x4B;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/go_hispeed_load";
HISPEED_FREQ_INTERACTIVE_CLUSTER_LITTLE;0x41414100;0x2C0;0x4C0;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/hispeed_freq";
IO_IS_BUSY_INTERACTIVE_CLUSTER_LITTLE;0x41418100;0x0;0x1;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/io_is_busy";
MIN_SAMPLE_TIME_INTERACTIVE_CLUSTER_LITTLE;0x4141C100;0x10;0x20;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/min_sample_time";
TARGET_LOADS_INTERACTIVE_CLUSTER_LITTLE;0x41420100;0x30;0x40;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/target_loads";
TIMER_RATE_INTERACTIVE_CLUSTER_LITTLE;0x41424100;0x20;0x60;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/timer_rate";
TIMER_SLACK_INTERACTIVE_CLUSTER_LITTLE;0x41428100;0x5C4;0x9C4;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/timer_slack";
MAX_FREQ_HYSTERESIS_INTERACTIVE_CLUSTER_LITTLE;0x4142C100;0x45;0x60;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/max_freq_hysteresis";
USE_SCHED_LOAD_INTERACTIVE_CLUSTER_LITTLE;0x41430100;0x1;0x0;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/use_sched_load";
USE_MIGRATION_NOTIF_CLUSTER_LITTLE;0x41434100;0x0;0x1;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/use_migration_notif";
IGNORE_HISPEED_NOTIF_CLUSTER_LITTLE;0x41438100;0x0;0x1;"/sys/devices/system/cpu/cpu%d/cpufreq/interactive/ignore_hispeed_on_notif";
CPUBW_HWMON_MIN_FREQ;0x41800000;0xCA;0xFA;"/sys/class/devfreq/soc:qcom,cpubw*/min_freq";
CPUBW_HWMON_DECAY_RATE;0x41804000;0x2D;0x4D;"/sys/class/devfreq/soc:qcom,cpubw*/bw_hwmon/decay_rate";
CPUBW_HWMON_IO_PERCENT;0x41808000;0x14;0x20;"/sys/class/devfreq/soc:qcom,cpubw/bw_hwmon/io_percent";
CPUBW_HWMON_HYST_OPT;0x4180C000;0x2;0x5;"/sys/class/devfreq/soc:qcom,cpubw/bw_hwmon/hyst_trigger_count";
CPUBW_HWMON_LOW_POWER_CEIL_MBPS;0x41810000;0x10;0x20;"/sys/class/devfreq/soc:qcom,cpubw*/bw_hwmon/low_power_ceil_mbps";
CPUBW_HWMON_LOW_POWER_IO_PERCENT;0x41814000;0x14;0x25;"/sys/class/devfreq/soc:qcom,cpubw/bw_hwmon/low_power_io_percent";
CPUBW_HWMON_MAX_FREQ;0x41818000;0x10;0x35;"/sys/class/devfreq/soc:qcom,cpubw*/max_freq";
CPUBW_HWMON_POLLING_INTERVAL;0x4181C000;0x20;0x40;"/sys/class/devfreq/soc:qcom,cpubw/polling_interval";
CPUBW_HWMON_SAMPLE_MS;0x41820000;0x2;0x5;"/sys/class/devfreq/soc:qcom,cpubw/bw_hwmon/sample_ms";
CPUBW_HWMON_IDLE_MBPS;0x41824000;0x40;0x60;"/sys/class/devfreq/soc:qcom,cpubw/bw_hwmon/idle_mbps";
VIDEO_ENCODE_PB_HINT;0x41C00000;NA;NA;"NA";
VIDEO_DECODE_PB_HINT;0x41C04000;NA;NA;"NA";
VIDEO_DISPLAY_PB_HINT;0x41C08000;NA;NA;"NA";
KSM_RUN_STATUS;0x42000000;0x0;0x1;"/sys/kernel/mm/ksm/run";
KSM_PARAMS;0x42004000;0x05;0x10;"/sys/kernel/mm/ksm/sleep_millisecs";
KSM_PARAMS;0x42004000;0x30;0x50;"/sys/kernel/mm/ksm/pages_to_scan";
SAMPLING_RATE_ONDEMAND;0x42400000;NA;NA;"/sys/devices/system/cpu/cpufreq/ondemand/sampling_rate";
IO_IS_BUSY_ONDEMAND;0x42404000;NA;NA;"/sys/devices/system/cpu/cpufreq/ondemand/io_is_busy";
SAMPLING_DOWN_FACTOR_ONDEMAND;0x42408000;NA;NA;"/sys/devices/system/cpu/cpufreq/ondemand/sampling_down_factor";
SYNC_FREQ_ONDEMAND;0x4240C000;NA;NA;"/sys/devices/system/cpu/cpufreq/ondemand/sync_freq";
OPTIMAL_FREQ_ONDEMAND;0x42410000;NA;NA;"/sys/devices/system/cpu/cpufreq/ondemand/optimal_freq";
ENABLE_STEP_UP_ONDEMAND;0x42414000;NA;NA;"/sys/devices/system/cpu/cpufreq/ondemand/enable_stepup";
MAX_INTERMEDIATE_STEPS_ONDEMAND;0x42418000;NA;NA;"/sys/devices/system/cpu/cpufreq/ondemand/max_intermediate_steps";
NOTIFY_ON_MIGRATE;0x4241C000;0x1;0x2;"/dev/cpuctl/cpu.notify_on_migrate";
GPU_POWER_LEVEL;0x42800000;0x2;0x3;"/sys/class/kgsl/kgsl-3d0/default_pwrlevel";
GPU_MIN_POWER_LEVEL;0x42804000;0x1;0x0;"/sys/class/kgsl/kgsl-3d0/min_pwrlevel";
GPU_MAX_POWER_LEVEL;0x42808000;0x2;0x4;"/sys/class/kgsl/kgsl-3d0/max_pwrlevel";
GPU_MIN_FREQ;0x4280C000;0xB0;0x20E;"/sys/class/kgsl/kgsl-3d0/devfreq/min_freq";
GPU_MAX_FREQ;0x42810000;0x20C;0x32C;"/sys/class/kgsl/kgsl-3d0/devfreq/max_freq";
GPU_BUS_MIN_FREQ;0x42814000;0x23C;0x32C;"/sys/class/devfreq/soc:qcom,gpubw/min_freq";
GPU_BUS_MAX_FREQ;0x42818000;0xBE2;0xDB2;"/sys/class/devfreq/soc:qcom,gpubw/max_freq";
IRQ_BALANCER;0x42C04000;NA;NA;"/dev/socket/msm_irqbalance";
UNSUPPORTED;0x42C00000;NA;NA;"NA";
INPUT_BOOST_RESET;0x42C08000;0x2B6;0x320;"/sys/module/cpu_boost/parameters/input_boost_freq";
SWAP_RATIO;0x42C0C000;0x32;0x45;"/proc/sys/vm/swap_ratio";
STORAGE_CLK_SCALING_DISABLE;0x42C10000;0x1;0x0;"/sys/class/mmc_host/mmc0/clk_scaling/enable";
STORAGE_CLK_SCALING_DISABLE;0x42C10000;0x1;0x0;"/sys/class/scsi_host/host0/../../../clkscale_enable";
KEEP_ALIVE;0x42C14000;NA;NA;"NA"
"

#*************************************************
#*                                               *
#*---------------- rootAndremount ---------------*
#*                                               *
#*************************************************
function rootAndremount() {

    sleep 2
    adb wait-for-device root
    sleep 2
    adb wait-for-device remount
}

#**********************************************
#*                                            *
#*---------------- fileExists  ---------------*
#*                                            *
#**********************************************
function fileExists() {

    FILENAME=$1

    fExists=`adb shell ls $FILENAME`

    if [[ $fExists != *"No such"*  ]]
    then
        echo " "
        QLOGI "*****************************************************************************";
        QLOGI "$FILENAME exists... ";
        QLOGI "*****************************************************************************";
        echo " "
    else
        echo " "
        echo " "
        QLOGE "$FILENAME does not exist... Aborting....... !!!";
        exit 1
    fi
}

#*******************************************************
#*                                                     *
#*---------------- setStayAwakeOn ---------------------*
#*                                                     *
#*******************************************************
function setStayAwakeOn() {

    #Wakeup the device, if or not sleeping
    adb shell input keyevent KEYCODE_WAKEUP

    sleep 5

    #Keep Device Always Awake, during execution of test
    adb shell settings put global stay_on_while_plugged_in 3

    sleep 5
}

#*******************************************************
#*                                                     *
#*---------------- setStayAwakeOff --------------------*
#*                                                     *
#*******************************************************
function setStayAwakeOff() {

   #Set Stay Awake to 0
   adb shell settings put global stay_on_while_plugged_in 0

   sleep 5

   #Turn Off Power and turn it back on to reflect stay awake change
   adb shell input keyevent KEYCODE_POWER
   sleep 5
   adb shell input keyevent KEYCODE_POWER
}

#***********************************************************
#*                                                         *
#*---------------- Determine Device Storage Type ----------*
#*                                                         *
#***********************************************************
function getDeviceNameAndStorageType() {

    targetName=`adb shell getprop ro.board.platform`
    boot_device=`adb shell getprop ro.boot.bootdevice`

    density=`adb shell wm density | cut -d':' -f2`

    if [[ $density == *"560"* ]]
    then
        resolution=2560
    elif [[ $density == *"480"* ]]
    then
        resolution=1080
    elif [[ $density = *"320"* ]]
    then
        resolution=720
    fi

#    resolution=`adb shell wm size | cut -d'x' -f2`

    QLOGI "Device Resolution = " $resolution

    if [[ $boot_device == *"ufs"* ]]
    then
        storage_type="UFS"
    else
        storage_type="eMMC"
    fi

    QLOGI "Device Storage type is $storage_type"
    echo " "
}

total_test_case=0;
test_case_pass=0;
test_case_fail=0
test_case_false_alarm=0
test_case_NA=0;
failed_counter=0;

declare -a resource_list;
declare -a resource_list_final;
declare -a resource_name;
declare -a resource_opcode
declare -a resource_val;
declare -a resource_new_val;
declare -a resource_sysfs_node;
declare -a resource_supported;
declare -a old_resources_list=()

#**********************************************************************************
#*                                                                                *
#*--------------- Split Resource Group to Individual resource Array  -------------*
#*                                                                                *
#**********************************************************************************
function split_resource_array() {

    let resource_list_count=0;
    IFS=$'\n'
    for item in ${resource_group[@]};
    do
    resource_list[resource_list_count]=$item
    ((resource_list_count++))
    done
    unset IFS

    let resource_list_count=0;
    IFS=$'\n'
    for item in ${old_resources[@]};
    do
    old_resources_list[resource_list_count]=$item
    ((resource_list_count++))
    done
    unset IFS

    let resource_count=0;
    let array_counter=0;
    let my_count=0;
    IFS=';';
    for item in ${resource_list[@]};
        do
            array_counter=$((resource_count % 6));

            case $array_counter in
                   0)
                   resource_name[my_count]=$item;
                   ;;
                   1)
                   resource_opcode[my_count]=$item;
                   ;;
                   2)
                   resource_val[my_count]=$item;
                   ;;
                   3)
                   resource_new_val[my_count]=$item;
                   ;;
                   4)
                   resource_sysfs_node[my_count]=$item;
                   ;;
                   5)
                    let my_count++
                   ;;
            esac
        ((resource_count++))
        done
    unset IFS
}

#**********************************************************************************
#*                                                                                *
#*------------- Determine Power / Perf Clusters & Master Cores -------------------*
#*                                                                                *
#**********************************************************************************
function check_clusters_and_master_cores() {

    num_clusters=`adb shell cat "/sys/module/msm_performance/parameters/num_clusters"`
    managed_cpus=`adb shell cat "/sys/module/msm_performance/parameters/managed_cpus"`

    QLOGV "Num of Clusters =  $num_clusters"
    QLOGV "Managed_cpus =  $managed_cpus"

    if [[ $num_clusters == 2 ]]
    then
        IFS=':'
        read -r cluster0 cluster1 <<< "$managed_cpus"
        unset IFS

        cluster0=${cluster0%-*}
        cluster1=${cluster1%-*}

        freq0=`adb shell cat "/sys/devices/system/cpu/cpu$cluster0/cpufreq/scaling_max_freq"`
        freq1=`adb shell cat "/sys/devices/system/cpu/cpu$cluster1/cpufreq/scaling_max_freq"`

        if [[ freq1 -gt freq0 ]]
        then
            master_big_core=$cluster1
            master_little_core=$cluster0
        else
            master_big_core=$cluster0
            master_little_core=$cluster1
        fi
        QLOGV "Big Cluster =  $master_big_core"
        QLOGV "Little Cluster =  $master_little_core"
    elif [[ $num_clusters == 1 ]]
    then
        master_big_core=${managed_cpus%-*}
        QLOGV "Big Cluster =  $master_big_core"
    else
        QLOGE " Number of CLUSTER is NOT SET.... HALTING EXECUTION..... "
        exit 1
    fi
}

#*******************************************************************************
#*                                                                             *
#*----------------- COMPUTE INTERACTIVE GOVERNOR SYSFS NODES ----------------- *
#*                                                                             *
#*******************************************************************************
function compute_interactive_governor_nodes() {

    for (( i = 0 ; i < ${#resource_name[@]} ; i=$i+1 ));
    do
        if [[ $num_clusters == 1 ]]
        then
            if [[ ${resource_name[i]} == *"INTERACTIVE_CLUSTER_BIG"* ]]
            then
                temp=${resource_sysfs_node[i]}
                big_final=${temp/\cpu%d\//}
                resource_sysfs_node[i]=$big_final
            elif [[ ${resource_name[i]} == *"INTERACTIVE_CLUSTER_LITTLE"* ]]
            then
                temp=${resource_sysfs_node[i]}
                little_final=${temp/\%d/}
                resource_sysfs_node[i]=$little_final
            fi
        elif [[ $num_clusters == 2 ]]
        then
            if [[ ${resource_name[i]} == *"INTERACTIVE_CLUSTER_BIG"* ]]
            then
                temp=${resource_sysfs_node[i]}
                big_final=${temp/\%d/$master_big_core}
                resource_sysfs_node[i]=$big_final
            elif [[ ${resource_name[i]} == *"INTERACTIVE_CLUSTER_LITTLE"* ]]
            then
                temp=${resource_sysfs_node[i]}
                little_final=${temp/\%d/$master_little_core}
                resource_sysfs_node[i]=$little_final
            fi
        fi

        if [[ ${resource_name[i]} == "SCHED_PREFER_IDLE" || ${resource_name[i]} == "SCHED_MOSTLY_IDLE_LOAD" || ${resource_name[i]} == "SCHED_MOSTLY_IDLE_NR_RUN" ||
              ${resource_name[i]} == "SCHED_MOSTLY_IDLE_FREQ" || ${resource_name[i]} == "SCHED_STATIC_CPU_PWR_COST" ]]
        then
            temp=${resource_sysfs_node[i]}
            big_final=${temp/\%d/$master_big_core}
            resource_sysfs_node[i]=$big_final
        fi
    done
}

#**************************************************************************************
#*                                                                                    *
#*-------------------------- DETERMINE DEVICE SPECIFIC PERFLOCKS  ------------------- *
#*                                                                                    *
#**************************************************************************************
function validate_target_supported_perflocks() {

    for (( i = 0 ; i < ${#resource_name[@]} ; i=$i+1 ));
    do

        sysfsNodePresent=`adb shell cat ${resource_sysfs_node[i]}`

        if [[ $sysfsNodePresent == *"No such file"* || $sysfsNodePresent == "" ]]
        then
            resource_supported[i]="N"
        else
            resource_supported[i]="Y"
        fi

        if [[ ${resource_name[i]} == "MIN_ONLINE_CPU_CLUSTER_LITTLE" || ${resource_name[i]} == "MAX_ONLINE_CPU_CLUSTER_LITTLE" ]]
        then
            resource_supported[i]="N"
        fi

        if [[ $num_clusters == 1 ]]
        then
            if [[ ${resource_name[i]} == *"FREQ_CLUSTER_BIG_CORE_$master_big_core"* ]]
            then
                resource_supported[i]="Y"
            elif [[ ${resource_name[i]} == *"FREQ_CLUSTER"* ]]
            then
                resource_supported[i]="N"
            fi

            if [[ ${resource_name[i]} == "BOOSTPULSE_INTERACTIVE_CLUSTER_BIG" ]]
            then
                resource_supported[i]="N"
            fi
        elif [[ $num_clusters == 2 ]]
        then
            if [[ ${resource_name[i]} == *"FREQ_CLUSTER_BIG_CORE_0"* || ${resource_name[i]} == *"FREQ_CLUSTER_LITTLE_CORE_0"* ]]
            then
                resource_supported[i]="Y"
            elif [[ ${resource_name[i]} == *"FREQ_CLUSTER_BIG_CORE"* || ${resource_name[i]} == *"FREQ_CLUSTER_LITTLE_CORE"* ]]
            then
                resource_supported[i]="N"
            fi

            if [[ ${resource_name[i]} == "BOOSTPULSE_INTERACTIVE_CLUSTER_LITTLE" || ${resource_name[i]} == "BOOSTPULSE_INTERACTIVE_CLUSTER_BIG" ]]
            then
                resource_supported[i]="N"
            fi
        fi

        if [[ ${resource_name[i]} == *"STORAGE_CLK_SCALING_DISABLE"* ]]
        then
            if [[ ${resource_sysfs_node[i]} == *"mmc_host"* ]]
            then
                if [[ $storage_type == "UFS" ]]
                then
                    resource_sysfs_node[i]="NA"
                    resource_opcode[i]="NOT_AVAILABLE"
                    resource_supported[i]="N"
                fi
            fi

            if [[ ${resource_sysfs_node[i]} == *"scsi_host"* ]]
            then
                if [[ $storage_type == "eMMC" ]]
                then
                    resource_sysfs_node[i]="NA"
                    resource_opcode[i]="NOT_AVAILABLE"
                    resource_supported[i]="N"
                fi
            fi
        fi
    done
}

#**************************************************************
#*                                                            *
#*----------------- Get SysNode from OpCode ----------------- *
#*                                                            *
#**************************************************************
function getSysNodeFromOpCode() {

    VAL_TO_CHECK=$1

    for ((index=0; index<${#resource_opcode[@]}; index++)); do
        if [ "${resource_opcode[$index]}" = "$VAL_TO_CHECK" ]; then
            echo ${resource_sysfs_node[index]}
            return
        fi
    done
    echo 'Given OpCode is not valid....'
}

#**************************************************************
#*                                                            *
#*------- Get v3.0 Opcode from v2.0 Resource Name ----------- *
#*                                                            *
#**************************************************************
function getNewOpCodeFromOldResName() {

    VAL_TO_CHECK=$1

    for ((index=0; index<${#resource_name[@]}; index++)); do
        if [ "${resource_name[$index]}" = "$VAL_TO_CHECK" ]; then
            echo ${resource_opcode[index]}
            return
        fi
    done
    echo 'Given Resource Name Does not Exist in Old perflock Resource List....'
}

#*********************************************************
#*                                                       *
# ---------------- Generate HTML HEADER  --------------- *
#*                                                       *
#*********************************************************
function UTIL_htmlHeader() {

    echo "<!DOCTYPE html>" > $output_filename
    echo "<html>" >> $output_filename
    echo "<head>"  >> $output_filename

    echo "<style>"  >> $output_filename
    echo "table {"  >> $output_filename
    echo "    font-family: arial, sans-serif;"  >> $output_filename
    echo "    border-collapse: collapse;" >> $output_filename
    echo "    width: 100%;"  >> $output_filename
    echo "    table-layout: fixed;" >> $output_filename
    echo "    word-wrap: break-word;" >> $output_filename
    echo "}"  >> $output_filename

    echo "td, th {"  >> $output_filename
    echo "    border: 1px solid #dddddd;"  >> $output_filename
    echo "    text-align: center;"  >> $output_filename
    echo "    padding: 8px;"  >> $output_filename
    echo "}"  >> $output_filename

    echo "tr{border:2px solid blue}" >> $output_filename

    echo "tr:nth-child(even) {"  >> $output_filename
    echo "    background-color: #E0FFFF;"  >> $output_filename
    echo "}"  >> $output_filename

    echo "h1 {text-align:center; font-size: 60px; font-weight: bold; }" >> $output_filename
    echo "h2 {text-align:center; font-size: 30px; font-weight: italic; }" >> $output_filename
    echo "h3 {text-align:center; font-size: 20px; font-weight: bold; }" >> $output_filename
    echo "h4 {text-align:center; font-size: 10px; font-weight: bold; }" >> $output_filename

    echo "#pR {color: red; font-weight: bold;}" >> $output_filename
    echo "#pG {color: green; font-weight: bold;}" >> $output_filename
    echo "#pB {color: blue; font-weight: bold;}" >> $output_filename
    echo "#pDV {color: darkviolet; font-weight: bold;}" >> $output_filename
    echo "#pDate {color: blue; font-weight: bold; text-align:center;}" >> $output_filename

    echo "#myBtn {" >> $output_filename
    echo "  display: none;" >> $output_filename
    echo "  position: fixed;" >> $output_filename
    echo "  bottom: 20px;" >> $output_filename
    echo "  right: 30px;" >> $output_filename
    echo "  z-index: 99;" >> $output_filename
    echo "  border: none;" >> $output_filename
    echo "  outline: none;" >> $output_filename
    echo "  background-color: red;" >> $output_filename
    echo "  color: white;" >> $output_filename
    echo "  cursor: pointer;" >> $output_filename
    echo "  padding: 15px;" >> $output_filename
    echo "  border-radius: 10px;" >> $output_filename
    echo "}" >> $output_filename

    echo "#myBtn:hover {" >> $output_filename
    echo "  background-color: #555;" >> $output_filename
    echo "}" >> $output_filename

    echo "</style>"  >> $output_filename

    echo "<script>" >> $output_filename
    echo "function toggle(elementId)" >> $output_filename
    echo "{" >> $output_filename
    echo "var ele = document.getElementById(elementId);" >> $output_filename
    echo "   if(ele.style.display == \"block\") {" >> $output_filename
    echo "            ele.style.display = \"none\";" >> $output_filename
    echo "    }" >> $output_filename
    echo "    else {" >> $output_filename
    echo "        ele.style.display = \"block\";" >> $output_filename
    echo "    }" >> $output_filename
    echo "}" >> $output_filename

    #When the user scrolls down 50px from the top of the document, make the button visible.
    echo "window.onscroll = function() {scrollFunction()};" >> $output_filename

    echo "function scrollFunction() {" >> $output_filename
    echo "var btn_element = document.getElementById(\"myBtn\");" >> $output_filename
    echo "    if (document.body.scrollTop > 50 || document.documentElement.scrollTop > 50) {" >> $output_filename
    echo "        btn_element.style.display = \"block\";" >> $output_filename
    echo "    } else {" >> $output_filename
    echo "        btn_element.style.display = \"none\";" >> $output_filename
    echo "    }" >> $output_filename
    echo "}" >> $output_filename

    echo "function scrollToTop(scrollDuration) {" >> $output_filename
    echo "    var scrollStep = -window.scrollY / (scrollDuration / 15)," >> $output_filename
    echo "        scrollInterval = setInterval(function(){" >> $output_filename
    echo "        if ( window.scrollY != 0 ) {" >> $output_filename
    echo "            window.scrollBy( 0, scrollStep );" >> $output_filename
    echo "        }" >> $output_filename
    echo "        else clearInterval(scrollInterval); " >> $output_filename
    echo "    },15);" >> $output_filename
    echo "}" >> $output_filename

    echo "</script>" >> $output_filename

    echo "</head>"  >> $output_filename
}

#*********************************************************
#*                                                       *
# ---------------- Generate HTML FOOTER  --------------- *
#*                                                       *
#*********************************************************
function UTIL_htmlFooter() {

    echo "</body>" >> $output_filename
    echo "</html>" >> $output_filename
    echo " "

    setStayAwakeOff

    MYPWD=${PWD}

    QLOGI "Output File : file://$MYPWD/$output_filename"
    echo " "
    echo " "
}

#********************************************************
#*                                                      *
# ----------------- GET DEVICE INFO ------------------- *
#*                                                      *
#********************************************************
function UTIL_deviceInfo() {

    echo "<h3>" >> $output_filename
    au_info=`adb shell getprop | grep AU`

    if [[ $au_info == "" ]]
    then
        au_info="It's an eCRM Build"
    fi

    echo "AU Info   : $au_info" >> $output_filename
    echo "<br>" >> $output_filename

    meta_info=`adb shell cat /firmware/verinfo/ver_info.txt | grep -i meta_build_id`

    echo $meta_info >> $output_filename
    echo "<br>" >> $output_filename
    echo "<hr>" >> $output_filename

    if [[ $storage_type == "UFS" ]]
    then
        echo "Device Storage Type : UFS" >> $output_filename
    elif [[ $storage_type == "eMMC" ]]
    then
        echo "Device Storage Type : eMMC" >> $output_filename
    else
        echo "Device Storage Type : Unknown" >> $output_filename
    fi
    echo "<hr>" >> $output_filename

    echo "Num of Clusters = " $num_clusters >> $output_filename
    echo "<br>" >> $output_filename
    echo "Managed_cpus = " $managed_cpus >> $output_filename
    echo "<br>" >> $output_filename

    if [[ $num_clusters == 2 ]]
    then
        echo "Perf Cluster Master CPU = " $master_big_core >> $output_filename
        echo "<br>" >> $output_filename
        echo "Power Cluster Master CPU = " $master_little_core >> $output_filename
    fi

    echo "<br>" >> $output_filename
    echo "<hr>" >> $output_filename
    echo "</h3>" >> $output_filename
}

#**********************************************************
#*                                                        *
# --------------- PERFLOCK BASIC TEST ------------------- *
#*                                                        *
#**********************************************************
function startPerflockBasicTest() {

    echo ""
    QLOGI "*************************************"
    QLOGI "*Starting Perflock Basic Tests......*"
    QLOGI "*************************************"
    echo ""
    sleep 5

    UTIL_htmlHeader
    echo "<body>"  >> $output_filename

    echo "<button onclick=\"scrollToTop(1000)\" id=\"myBtn\" title=\"Go to top\">Top</button>" >> $output_filename

    echo "<h1>" >> $output_filename
    echo "Perf Lock Basic Test Report" >> $output_filename
    echo "</h1>" >> $output_filename

    UTIL_reportDate
    echo "<hr>" >> $output_filename
    UTIL_deviceInfo

    echo "<h3><a href=\"javascript:;\" onclick=toggle('detailed')>Perf Lock Execution Report</a></h3>" >> $output_filename

    echo "<div id=\"detailed\" style=\"display:none\">" >> $output_filename

    echo "<table>"  >> $output_filename
    echo "  <tr>"  >> $output_filename
    echo "    <th>Resource Name</th>"  >> $output_filename
    echo "    <th>Resource OpCode</th>"   >> $output_filename
    echo "    <th>Resource Sysfs Node</th>"   >> $output_filename
    echo "    <th>Resource Value</th>"   >> $output_filename
    echo "    <th>Perf Lock test command</th>"   >> $output_filename
    echo "    <th>Sysfs Node Value Before lock</th>"   >> $output_filename
    echo "    <th>Sysfs Node Value During lock</th>"   >> $output_filename
    echo "    <th>Sysfs Node Value After lock</th>"   >> $output_filename
    echo "    <th>Perflock Acquire Status</th>"   >> $output_filename
    echo "    <th>Perflock Release Status</th>"   >> $output_filename
    echo "  </tr>"   >> $output_filename

    for (( i = 0 ; i < ${#resource_name[@]} ; i=$i+1 ));
    do

    if [[ ${resource_supported[i]} == "Y" ]]
    then

        echo "<tr>" >> $output_filename
            echo "<td>" >> $output_filename
                echo ${resource_name[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo ${resource_opcode[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "${resource_sysfs_node[i]}" >> $output_filename
            echo "</td>" >> $output_filename

            #Get the Node value, before acquiring perflock
            before_node=`adb shell cat ${resource_sysfs_node[i]}`

            #Acquire perflock - 1st Attempt
            echo ""
            echo ""
            QLOGI "First try :: adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 5000 ${resource_opcode[i]},${resource_val[i]}"
            adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 5000 "${resource_opcode[i]},${resource_val[i]}" &
            sleep 2

            #Get Node value, when perflock is acquired - 1st Attempt
            during_node_first_run=`adb shell cat ${resource_sysfs_node[i]}`

            #First Attempt Perflock Failed. Check with different resource value in 2nd Attempt
            if [ "$before_node" == "$during_node_first_run" ]
            then
                sleep 10
                #Acquire perflock - 2nd Attempt
                QLOGE "First try to acquire lock Failed.... Modifying the value and trying again....."
                QLOGI "Second try :::: adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 5000 ${resource_opcode[i]},${resource_new_val[i]}"
                adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 5000 "${resource_opcode[i]},${resource_new_val[i]}" &
                sleep 2
                during_node_second_run=`adb shell cat ${resource_sysfs_node[i]}`
                if [ "$before_node" == "$during_node_second_run" ]
                then
                    QLOGE "Second try to acquire lock Also Failed.... Moving to next perflock....."
                else
                    QLOGI "Second Attempt Succeeded !!!"
                fi
                during_node=$during_node_second_run
                final_val=${resource_new_val[i]}
            else    #First Attempt Perflock Passed...
                during_node=$during_node_first_run
                final_val=${resource_val[i]}
                QLOGI "First Attempt Succeeded !!!"
            fi

            echo "<td>" >> $output_filename
                echo $final_val >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "$PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 5000" ${resource_opcode[i]}","$final_val >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                echo $before_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                echo $during_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            sleep 5

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                after_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $after_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $before_node == *"No such"* ]]
                then
                    echo "<p id=pB>" >> $output_filename
                    echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                    let "test_case_NA++"
                elif [[ "$before_node" == "$during_node" ]]
                then
                    if [[ "${resource_opcode[i]}" == "0x40400000" ]]
                    then
                        echo "<p id=pDV>" >> $output_filename
                        echo "False Alarm" >> $output_filename
                        echo "</p id=pDV>" >> $output_filename
                        acq_failed="FA"
                        let "test_case_false_alarm++"
                    else
                        echo "<p id=pR>" >> $output_filename
                        echo "Acquire Failed " >> $output_filename
                        echo "</p id=pR>" >> $output_filename
                        let "test_case_fail++"
                        failed_resource_name[failed_counter]=${resource_name[i]}
                        failed_resource_node[failed_counter]=${resource_sysfs_node[i]}
                        let "failed_counter++"
                        acq_failed="true"
                    fi
                else
                    echo "<p id=pG>" >> $output_filename
                    echo "Acquire Success " >> $output_filename
                    echo "</p id=pG>" >> $output_filename
                    let "test_case_pass++"
                fi
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $acq_failed != "true" ]]
                then
                    if [[ $before_node == *"No such"* ]]
                    then
                       echo "<p id=pB>" >> $output_filename
                       echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                       echo "</p id=pB>" >> $output_filename
                    elif [ "$before_node" == "$after_node" ]
                    then
                        if [[ $acq_failed == "FA" ]]
                        then
                            echo "<p id=pDV>" >> $output_filename
                            echo "Lock NOT Acquired and hence Release is N.A." >> $output_filename
                            echo "</p id=pDV>" >> $output_filename
                       else
                           echo "<p id=pG>" >> $output_filename
                           echo "Release Success " >> $output_filename
                           echo "</p id=pG>" >> $output_filename
                       fi
                    else
                       echo "<p id=pR>" >> $output_filename
                       echo "Release Failed " >> $output_filename
                       echo "</p id=pR>" >> $output_filename
                    fi
                else
                   echo "<p id=pB>" >> $output_filename
                   echo "Acquire Failed. Hence, Release is Not Applicable" >> $output_filename
                   echo "</p id=pB>" >> $output_filename
                fi

            echo "</td>" >> $output_filename
        echo "</tr>" >> $output_filename

    acq_failed="false"
    let "total_test_case++"
    fi
    done

    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    echo "<br>" >> $output_filename

    echo "<h3><a href=\"javascript:;\" onclick=toggle('summary')>Summary</a></h3>" >> $output_filename
    echo "<div id=\"summary\" style=\"display:block\">" >> $output_filename

    echo "<table>" >> $output_filename
        echo "<tr><td> Total Supported PerfLocks </td>" "<td> $total_test_case </td></tr>" >> $output_filename
        echo "<tr><td> PerfLocks Passed </td>" "<td> $test_case_pass </td></tr>" >> $output_filename
        echo "<tr><td> PerfLocks Failed </td>" "<td> $test_case_fail </td></tr>" >> $output_filename
        echo "<tr><td> PerfLocks False Alarm </td>" "<td> $test_case_false_alarm </td></tr>" >> $output_filename
    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    echo "<br>" >> $output_filename
    echo "<br>" >> $output_filename

    if [ "$test_case_fail" -ne 0 ]
    then
        echo "<h3><a href=\"javascript:;\" onclick=toggle('failed')>Failed Cases</a></h3>" >> $output_filename
        echo "<div id=\"failed\" style=\"display:none\">" >> $output_filename
        echo "<table>" >> $output_filename
        echo "<tr><td><b>Resource Name</b></td><td><b>Sysfs Node</b></td></tr>" >> $output_filename
            for (( i = 0 ; i < ${#failed_resource_name[@]} ; i=$i+1 ));
            do
                echo "<tr><td>" ${failed_resource_name[i]} "</td><td>" ${failed_resource_node[i]} "</td></tr>" >> $output_filename
            done
        echo "</table>" >> $output_filename
        echo "</div>" >> $output_filename
    fi

    echo " "
    echo " "
    QLOGI "************* Test Summary *******************"
    QLOGI "Total Supported Perflocks = $total_test_case"
    QLOGI "Total Perflocks Passed    = $test_case_pass"
    QLOGI "Total Perflocks Failed    = $test_case_fail"
    QLOGI "Perflocks False Alarm     = $test_case_false_alarm"
    QLOGI "**********************************************"

    echo " "

    UTIL_supportedLocks

    UTIL_htmlFooter
}

#****************************************************************
#*                                                              *
# --------------- PERFLOCK CONCURRENCY TEST ------------------- *
#*                                                              *
#****************************************************************
function startPerflockConcurrencyTest() {

    total_test_case=0;
    test_case_pass=0;
    test_case_fail=0
    test_case_NA=0;
    test_case_false_alarm=0;
    partial_test_case_fail=0;

    first_lock_fail=0;
    second_lock_fail=0;
    first_lock_false_alarm=0;
    second_lock_false_alarm=0;

    first_failed_counter=0;
    second_failed_counter=0;
    both_failed_counter=0;
    both_false_alarm_counter=0;
    partial_failed_counter=0;

    first_acq_failed="false"
    second_acq_failed="false"
    first_acq_false_alarm="false"
    second_acq_false_alarm="false"

    echo " "
    QLOGI "*******************************************"
    QLOGI "*Starting Perflock Concurrency Tests......*"
    QLOGI "*******************************************"
    echo " "
    sleep 5

    UTIL_htmlHeader
    echo "<body>"  >> $output_filename

    echo "<button onclick=\"scrollToTop(1000)\" id=\"myBtn\" title=\"Go to top\">Top</button>" >> $output_filename

    echo "<h1>" >> $output_filename
    echo "Perf Lock Concurrency Test Report" >> $output_filename
    echo "</h1>" >> $output_filename

    UTIL_reportDate
    echo "<hr>" >> $output_filename
    UTIL_deviceInfo

    echo "<h3><a href=\"javascript:;\" onclick=toggle('detailed')>Click Here for Detailed Perf Lock Report (Small value to Big value)</a></h3>" >> $output_filename
    echo "<div id=\"detailed\" style=\"display:none\">" >> $output_filename

    echo "<table>"  >> $output_filename
    echo "  <tr>"  >> $output_filename
    echo "    <th>Resource Name</th>"  >> $output_filename
    echo "    <th>Resource OpCode</th>"   >> $output_filename
    echo "    <th>Resource Sysfs Node</th>"   >> $output_filename
    echo "    <th>Value Before lock</th>"   >> $output_filename
    echo "    <th>First Perf Lock test command</th>"   >> $output_filename
    echo "    <th>Value During 1st lock, before 2nd lock</th>"   >> $output_filename
    echo "    <th>Second Perf Lock test command</th>"   >> $output_filename
    echo "    <th>Value During 1st & 2nd locks</th>"   >> $output_filename
    echo "    <th>Sysfs Node Value After 2nd lock Rel</th>"   >> $output_filename
    echo "    <th>Sysfs Node Value After 1st lock Rel</th>"   >> $output_filename
    echo "    <th>First lock Acquire Status</th>"   >> $output_filename
    echo "    <th>Second lock Acquire Status</th>"   >> $output_filename
    echo "    <th>Second lock Release Status</th>"   >> $output_filename
    echo "    <th>First lock Release Status</th>"   >> $output_filename
    echo "  </tr>"   >> $output_filename

    for (( i = 0 ; i < ${#resource_name[@]} ; i=$i+1 ));
    do

    if [[ ${resource_supported[i]} == "Y" ]]
    then

        echo "<tr>" >> $output_filename
            echo "<td>" >> $output_filename
                echo ${resource_name[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo ${resource_opcode[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "${resource_sysfs_node[i]}" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                before_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $before_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename


            echo "<td>" >> $output_filename
                echo "$PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 15000" ${resource_opcode[i]}","${resource_val[i]} >> $output_filename
            echo "</td>" >> $output_filename

            QLOGI "First Lock : adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 15000 ${resource_opcode[i]},${resource_val[i]}"
            adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 15000 "${resource_opcode[i]},${resource_val[i]}" &
            sleep 2

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                during_first_lock_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $during_first_lock_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "$PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 10000" ${resource_opcode[i]}","${resource_new_val[i]} >> $output_filename
            echo "</td>" >> $output_filename

            QLOGI "Concurrent Lock :: adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 10000 ${resource_opcode[i]},${resource_new_val[i]}"
            adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 10000 "${resource_opcode[i]},${resource_new_val[i]}" &
            sleep 2

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                during_both_lock_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $during_both_lock_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            sleep 8
            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                after_second_rel_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $after_second_rel_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename
            sleep 5

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                after_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $after_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $before_node == *"No such"* ]]
                then
                    echo "<p id=pB>" >> $output_filename
                    echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                    let "test_case_NA++"
                elif [ "$before_node" == "$during_first_lock_node" ]
                then
                    testFirstVal=`printf '%d' ${resource_val[i]}`
                    if [[ "${resource_opcode[i]}" == "0x40400000" ||
                           "$before_node" == "$testFirstVal" ]]
                    then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                    first_acq_false_alarm="true"
                    let "first_lock_false_alarm++"
                    else
                    echo "<p id=pR>" >> $output_filename
                    echo "First Lock Acquire Failed " >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                    let "first_lock_fail++"
                    first_failed_resource_name[first_failed_counter]=${resource_name[i]}
                    first_failed_resource_value[first_failed_counter]=${resource_val[i]}
                    first_failed_resource_node[first_failed_counter]=${resource_sysfs_node[i]}
                    let "first_failed_counter++"
                    first_acq_failed="true"
                    fi
                else
                    echo "<p id=pG>" >> $output_filename
                    echo "First Lock Acquire Success " >> $output_filename
                    echo "</p id=pG>" >> $output_filename
                fi
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $before_node == *"No such"* ]]
                then
                    echo "<p id=pB>" >> $output_filename
                    echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                elif ( [ $first_acq_failed != "true" ] && [ "$during_first_lock_node" == "$during_both_lock_node" ] && [ "$before_node" != "$during_both_lock_node" ] )
                then
                    echo "<p id=pG>" >> $output_filename
                    echo "Second Lock Acquire Success " >> $output_filename
                    echo "</p id=pG>" >> $output_filename
                    let "test_case_pass++"
                elif [ $first_acq_failed != "true" ] && [ "$during_first_lock_node" != "$during_both_lock_node" ]
                then
                    echo "<p id=pG>" >> $output_filename
                    echo "Second Lock Acquire Success " >> $output_filename
                    echo "</p id=pG>" >> $output_filename
                    let "test_case_pass++"
                elif [ "$during_first_lock_node" == "$during_both_lock_node" ]
                then
                    testSecondVal=`printf '%d' ${resource_new_val[i]}`
                    if [[ "${resource_opcode[i]}" == "0x40400000" ||
                           "$during_first_lock_node" == "$testSecondVal" ]]
                    then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                    second_acq_false_alarm="true"
                    let "second_lock_false_alarm++"
                    else
                    echo "<p id=pR>" >> $output_filename
                    echo "Second Lock Acquire Failed " >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                    if [[ $first_acq_failed != "true" ]]
                    then
                        let "second_lock_fail++"
                        second_failed_resource_name[second_failed_counter]=${resource_name[i]}
                        second_failed_resource_value[second_failed_counter]=${resource_new_val[i]}
                        second_failed_resource_node[second_failed_counter]=${resource_sysfs_node[i]}
                        let "second_failed_counter++"
                    fi
                    second_acq_failed="true"
                    fi
                else
                echo "<p id=pG>" >> $output_filename
                echo "Second Lock Acquire Success " >> $output_filename
                echo "</p id=pG>" >> $output_filename
                let "test_case_pass++"
                fi
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $second_acq_failed != "true" && $second_acq_false_alarm != "true" ]]
                then
                    if [[ $before_node == *"No such"* ]]
                    then
                        echo "<p id=pB>" >> $output_filename
                        echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                        echo "</p id=pB>" >> $output_filename
                    elif [ "$before_node" == "$after_node" ]
                    then
                        echo "<p id=pG>" >> $output_filename
                        echo "Second Lock Release Success " >> $output_filename
                        echo "</p id=pG>" >> $output_filename
                    else
                        echo "<p id=pR>" >> $output_filename
                        echo "Second Lock Release Failed " >> $output_filename
                        echo "</p id=pR>" >> $output_filename
                    fi
                elif [[ $second_acq_false_alarm == "true" ]]
                then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                else
                    echo "<p id=pB>" >> $output_filename
                    echo "Second Lock Acquire Failed. Hence, Release is Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                fi
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $first_acq_failed != "true" && $first_acq_false_alarm != "true" ]]
                then
                    if [[ $before_node == *"No such"* ]]
                    then
                        echo "<p id=pB>" >> $output_filename
                        echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                        echo "</p id=pB>" >> $output_filename
                    elif [ "$before_node" == "$after_node" ]
                    then
                        echo "<p id=pG>" >> $output_filename
                        echo "First Lock Release Success " >> $output_filename
                        echo "</p id=pG>" >> $output_filename
                    else
                        echo "<p id=pR>" >> $output_filename
                        echo "First Lock Release Failed " >> $output_filename
                        echo "</p id=pR>" >> $output_filename
                    fi
                elif [[ $first_acq_false_alarm == "true" ]]
                then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                else
                    echo "<p id=pB>" >> $output_filename
                    echo "First Lock Acquire Failed. Hence, Release is Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                fi
            echo "</td>" >> $output_filename
        echo "</tr>" >> $output_filename

        if [ $first_acq_failed == "true" ] && [ $second_acq_failed == "true" ]
        then
            failed_resource_name[both_failed_counter]=${resource_name[i]}
            failed_resource_value[both_failed_counter]=${resource_val[i]}
            failed_resource_node[both_failed_counter]=${resource_sysfs_node[i]}
            let "both_failed_counter++"
            let "test_case_fail++"
        fi

        if [ $first_acq_false_alarm == "true" ] && [ $second_acq_false_alarm == "true" ]
        then
            false_alarm_resource_name[both_false_alarm_counter]=${resource_name[i]}
            false_alarm_resource_value[both_false_alarm_counter]=${resource_val[i]}
            false_alarm_resource_node[both_false_alarm_counter]=${resource_sysfs_node[i]}
            let "both_false_alarm_counter++"
            let "test_case_false_alarm++"
        fi

        if ( [ $first_acq_failed == "true" ] && [ $second_acq_failed != "true" ] ) || ( [ $first_acq_failed != "true" ] && [ $second_acq_failed == "true" ] ) ||
           ( [ $first_acq_false_alarm == "true" ] && [ $second_acq_failed == "true" ] ) || ( [ $first_acq_failed == "true" ] && [ $second_acq_false_alarm == "true" ] )
        then
            partial_failed_resource_name[partial_failed_counter]=${resource_name[i]}
            partial_failed_resource_value[partial_failed_counter]=${resource_val[i]}
            partial_failed_resource_node[partial_failed_counter]=${resource_sysfs_node[i]}
            let "partial_failed_counter++"
            let "partial_test_case_fail++"
        fi

        first_acq_failed="false"
        second_acq_failed="false"
        first_acq_false_alarm="false"
        second_acq_false_alarm="false"
        let "total_test_case++"
        echo " "
    fi
    done

    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    test_case_pass=$(expr $total_test_case - $test_case_fail - $test_case_false_alarm - $partial_test_case_fail)

    echo "<h3><a href=\"javascript:;\" onclick=toggle('summary')>Summary</a></h3>" >> $output_filename

    echo "<div id=\"summary\" style=\"display:block\">" >> $output_filename
        echo "<table>" >> $output_filename
            echo "<tr><td> Total Concurrency Testcases </td>" "<td> $total_test_case </td></tr>" >> $output_filename
            echo "<tr><td> Total Concurrency Tests Passed </td>" "<td> $test_case_pass </td></tr>" >> $output_filename
            echo "<tr><td> Total Concurrency Tests Failed </td>" "<td> $test_case_fail </td></tr>" >> $output_filename
            echo "<tr><td> Total Concurrency Tests False Alarm </td>" "<td> $test_case_false_alarm </td></tr>" >> $output_filename
            echo "<tr><td> Partially Failed Cases </td>" "<td> $partial_test_case_fail </td></tr>" >> $output_filename
            echo "<tr><td> Not Applicable Cases </td>" "<td> $test_case_NA </td></tr>" >> $output_filename

        echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    if [[ "$both_failed_counter" -ne 0 ]]
    then
        echo "<h3><a href=\"javascript:;\" onclick=toggle('failed')>Failed Cases</a></h3>" >> $output_filename
        echo "<div id=\"failed\" style=\"display:none\">" >> $output_filename
            echo "<table>" >> $output_filename
                echo "<tr><td><b>Resource Name</b></td><td><b>Sysfs Node</b></td></tr>" >> $output_filename
                for (( i = 0 ; i < ${#failed_resource_name[@]} ; i=$i+1 ));
                do
                echo "<tr><td>" ${failed_resource_name[i]} "</td><td>" ${failed_resource_node[i]} "</td></tr>" >> $output_filename
                done
            echo "</table>" >> $output_filename
        echo "</div>" >> $output_filename
    fi

    if [[ "$second_lock_fail" -ne 0 ]]
    then
        echo "<h3><a href=\"javascript:;\" onclick=toggle('second_failed')>Second Lock Failed Cases</a></h3>" >> $output_filename
        echo "<div id=\"second_failed\" style=\"display:none\">" >> $output_filename
            echo "<table>" >> $output_filename
                echo "<tr><td><b>Resource Name</b></td><td><b>Sysfs Node</b></td></tr>" >> $output_filename
                for (( i = 0 ; i < ${#second_failed_resource_name[@]} ; i=$i+1 ));
                do
                echo "<tr><td>" ${second_failed_resource_name[i]} "</td><td>" ${second_failed_resource_node[i]} "</td></tr>" >> $output_filename
                done
            echo "</table>" >> $output_filename
        echo "</div>" >> $output_filename
    fi

    echo " "
    echo " "
    QLOGI "************* Test Summary **********************************"
    QLOGI "Total Concurrency Testcases           = $total_test_case"
    QLOGI "Total Concurrency Tests Passed        = $test_case_pass"
    QLOGI "Total Concurrency Tests Failed        = $test_case_fail"
    QLOGI "Total Concurrency Tests False Alarm   = $test_case_false_alarm"
    QLOGI "Partially Failed  Cases               = $partial_test_case_fail"
    QLOGI "Not Applicable Cases                  = $test_case_NA"
    QLOGI "*************************************************************"

    echo " "
    echo " "

    QLOGI " ************** Reverse Concurrency (Big Value to Small Value) *************************** "

    echo "<hr>" >> $output_filename
    echo "<hr>" >> $output_filename
    echo "<hr>" >> $output_filename

    total_test_case=0;
    test_case_pass=0;
    test_case_fail=0
    test_case_NA=0;
    test_case_false_alarm=0;
    partial_test_case_fail=0;

    first_lock_fail=0;
    second_lock_fail=0;
    first_lock_false_alarm=0;
    second_lock_false_alarm=0;

    first_failed_counter=0;
    second_failed_counter=0;
    both_failed_counter=0;
    both_false_alarm_counter=0;
    partial_failed_counter=0;

    first_acq_failed="false"
    second_acq_failed="false"
    first_acq_false_alarm="false"
    second_acq_false_alarm="false"

    first_failed_resource_name=()
    first_failed_resource_value=()
    first_failed_resource_node=()

    second_failed_resource_name=()
    second_failed_resource_value=()
    second_failed_resource_node=()

    failed_resource_name=()
    failed_resource_value=()
    failed_resource_node=()

    echo "<h3><a href=\"javascript:;\" onclick=toggle('detailed2')>Click Here for Detailed Perf Lock Report (Big Value to Small Value)</a></h3>" >> $output_filename
    echo "<div id=\"detailed2\" style=\"display:none\">" >> $output_filename

    echo "<table>"  >> $output_filename
        echo "  <tr>"  >> $output_filename
            echo "    <th>Resource Name</th>"  >> $output_filename
            echo "    <th>Resource OpCode</th>"   >> $output_filename
            echo "    <th>Resource Sysfs Node</th>"   >> $output_filename
            echo "    <th>Value Before lock</th>"   >> $output_filename
            echo "    <th>First Perf Lock test command</th>"   >> $output_filename
            echo "    <th>Value During 1st lock, before 2nd lock</th>"   >> $output_filename
            echo "    <th>Second Perf Lock test command</th>"   >> $output_filename
            echo "    <th>Value During 1st & 2nd locks</th>"   >> $output_filename
            echo "    <th>Sysfs Node Value After 2nd lock Rel</th>"   >> $output_filename
            echo "    <th>Sysfs Node Value After 1st lock Rel</th>"   >> $output_filename
            echo "    <th>First lock Acquire Status</th>"   >> $output_filename
            echo "    <th>Second lock Acquire Status</th>"   >> $output_filename
            echo "    <th>Second lock Release Status</th>"   >> $output_filename
            echo "    <th>First lock Release Status</th>"   >> $output_filename
    echo "  </tr>"   >> $output_filename

    for (( i = 0 ; i < ${#resource_name[@]} ; i=$i+1 ));
    do

    if [[ ${resource_supported[i]} == "Y" ]]
    then

        echo "<tr>" >> $output_filename
            echo "<td>" >> $output_filename
                echo ${resource_name[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo ${resource_opcode[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "${resource_sysfs_node[i]}" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                before_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $before_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "$PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 15000" ${resource_opcode[i]}","${resource_new_val[i]} >> $output_filename
            echo "</td>" >> $output_filename

            QLOGI "First Lock : adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 15000 ${resource_opcode[i]},${resource_new_val[i]}"
            adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 15000 "${resource_opcode[i]},${resource_new_val[i]}" &
            sleep 2

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                during_first_lock_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $during_first_lock_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "$PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 10000" ${resource_opcode[i]}","${resource_val[i]} >> $output_filename
            echo "</td>" >> $output_filename

            QLOGI "Concurrent Lock :: adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 10000 ${resource_opcode[i]},${resource_val[i]}"
            adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq  0 10000 "${resource_opcode[i]},${resource_val[i]}" &
            sleep 2

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                during_both_lock_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $during_both_lock_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            sleep 8
            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                after_second_rel_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $after_second_rel_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename
            sleep 5

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                after_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $after_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $before_node == *"No such"* ]]
                then
                    echo "<p id=pB>" >> $output_filename
                    echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                    let "test_case_NA++"
                elif [ "$before_node" == "$during_first_lock_node" ]
                then
                    testFirstVal=`printf '%d' ${resource_new_val[i]}`
                    if [[ "${resource_opcode[i]}" == "0x40400000" ||
                           "$before_node" == "$testFirstVal" ]]
                    then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                    first_acq_false_alarm="true"
                    let "first_lock_false_alarm++"
                    else
                    echo "<p id=pR>" >> $output_filename
                    echo "First Lock Acquire Failed " >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                    first_failed_resource_name[first_failed_counter]=${resource_name[i]}
                    first_failed_resource_value[first_failed_counter]=${resource_new_val[i]}
                    first_failed_resource_node[first_failed_counter]=${resource_sysfs_node[i]}
                    let "first_failed_counter++"
                    first_acq_failed="true"
                    fi
                else
                    echo "<p id=pG>" >> $output_filename
                    echo "First Lock Acquire Success " >> $output_filename
                    echo "</p id=pG>" >> $output_filename
                fi
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $before_node == *"No such"* ]]
                then
                    echo "<p id=pB>" >> $output_filename
                    echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                elif [ "$during_first_lock_node" == "$during_both_lock_node" ] && [ "$before_node" != "$during_both_lock_node" ]
                then
                    echo "<p id=pG>" >> $output_filename
                    echo "Second Lock Acquire Success " >> $output_filename
                    echo "</p id=pG>" >> $output_filename
                    let "test_case_pass++"
                elif [ "$during_first_lock_node" != "$during_both_lock_node" ]
                then
                    echo "<p id=pG>" >> $output_filename
                    echo "Second Lock Acquire Success " >> $output_filename
                    echo "</p id=pG>" >> $output_filename
                    let "test_case_pass++"
                elif [ "$during_first_lock_node" == "$during_both_lock_node" ]
                then
                    testSecondVal=`printf '%d' ${resource_val[i]}`
                    if [[ "${resource_opcode[i]}" == "0x40400000" ||
                           "$during_first_lock_node" == "$testSecondVal" ]]
                    then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                    second_acq_false_alarm="true"
                    let "second_lock_false_alarm++"
                    else
                    echo "<p id=pR>" >> $output_filename
                    echo "Second Lock Acquire Failed " >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                    if [[ $first_acq_failed != "true" ]]
                    then
                        let "second_lock_fail++"
                        second_failed_resource_name[second_failed_counter]=${resource_name[i]}
                        second_failed_resource_value[second_failed_counter]=${resource_new_val[i]}
                        second_failed_resource_node[second_failed_counter]=${resource_sysfs_node[i]}
                        let "second_failed_counter++"
                    fi
                    second_acq_failed="true"
                    fi
                fi
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $second_acq_failed != "true" && $second_acq_false_alarm != "true" ]]
                then
                    if [[ $before_node == *"No such"* ]]
                    then
                        echo "<p id=pB>" >> $output_filename
                        echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                        echo "</p id=pB>" >> $output_filename
                    elif [ "$before_node" == "$after_node" ]
                    then
                        echo "<p id=pG>" >> $output_filename
                        echo "Second Lock Release Success " >> $output_filename
                        echo "</p id=pG>" >> $output_filename
                    else
                        echo "<p id=pR>" >> $output_filename
                        echo "Second Lock Release Failed " >> $output_filename
                        echo "</p id=pR>" >> $output_filename
                    fi
                elif [[ $second_acq_false_alarm == "true" ]]
                then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                else
                    echo "<p id=pB>" >> $output_filename
                    echo "Second Lock Acquire Failed. Hence, Release is Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                fi
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $first_acq_failed != "true" && $first_acq_false_alarm != "true" ]]
                then
                    if [[ $before_node == *"No such"* ]]
                    then
                        echo "<p id=pB>" >> $output_filename
                        echo "Node Not Present. Hence, Not Applicable" >> $output_filename
                        echo "</p id=pB>" >> $output_filename
                    elif [ "$before_node" == "$after_node" ]
                    then
                        echo "<p id=pG>" >> $output_filename
                        echo "First Lock Release Success " >> $output_filename
                        echo "</p id=pG>" >> $output_filename
                    else
                        echo "<p id=pR>" >> $output_filename
                        echo "First Lock Release Failed " >> $output_filename
                        echo "</p id=pR>" >> $output_filename
                    fi
                elif [[ $first_acq_false_alarm == "true" ]]
                then
                    echo "<p id=pDV>" >> $output_filename
                    echo "False Alarm " >> $output_filename
                    echo "</p id=pDV>" >> $output_filename
                else
                    echo "<p id=pB>" >> $output_filename
                    echo "First Lock Acquire Failed. Hence, Release is Not Applicable" >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                fi
            echo "</td>" >> $output_filename
        echo "</tr>" >> $output_filename

        if [ $first_acq_failed == "true" ] && [ $second_acq_failed == "true" ]
        then
            failed_resource_name[both_failed_counter]=${resource_name[i]}
            failed_resource_value[both_failed_counter]=${resource_val[i]}
            failed_resource_node[both_failed_counter]=${resource_sysfs_node[i]}
            let "both_failed_counter++"
            let "test_case_fail++"
        fi

        if [ $first_acq_false_alarm == "true" ] && [ $second_acq_false_alarm == "true" ]
        then
            false_alarm_resource_name[both_false_alarm_counter]=${resource_name[i]}
            false_alarm_resource_value[both_false_alarm_counter]=${resource_val[i]}
            false_alarm_resource_node[both_false_alarm_counter]=${resource_sysfs_node[i]}
            let "both_false_alarm_counter++"
            let "test_case_false_alarm++"
        fi

        if ( [ $first_acq_failed == "true" ] && [ $second_acq_failed != "true" ] ) || ( [ $first_acq_failed != "true" ] && [ $second_acq_failed == "true" ] ) ||
           ( [ $first_acq_false_alarm == "true" ] && [ $second_acq_failed == "true" ] ) || ( [ $first_acq_failed == "true" ] && [ $second_acq_false_alarm == "true" ] )
        then
            partial_failed_resource_name[partial_failed_counter]=${resource_name[i]}
            partial_failed_resource_value[partial_failed_counter]=${resource_val[i]}
            partial_failed_resource_node[partial_failed_counter]=${resource_sysfs_node[i]}
            let "partial_failed_counter++"
            let "partial_test_case_fail++"
        fi

        first_acq_failed="false"
        second_acq_failed="false"
        first_acq_false_alarm="false"
        second_acq_false_alarm="false"
        let "total_test_case++"
        echo " "
    fi
    done

    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    test_case_pass=$(expr $total_test_case - $test_case_fail - $test_case_false_alarm - $partial_test_case_fail)

    echo "<h3><a href=\"javascript:;\" onclick=toggle('summary')>Summary</a></h3>" >> $output_filename

    echo "<div id=\"summary2\" style=\"display:block\">" >> $output_filename
        echo "<table>" >> $output_filename
            echo "<tr><td> Total Concurrency Testcases </td>" "<td> $total_test_case </td></tr>" >> $output_filename
            echo "<tr><td> Total Concurrency Tests Passed </td>" "<td> $test_case_pass </td></tr>" >> $output_filename
            echo "<tr><td> Total Concurrency Tests Failed </td>" "<td> $test_case_fail </td></tr>" >> $output_filename
            echo "<tr><td> Total Concurrency Tests False Alarm </td>" "<td> $test_case_false_alarm </td></tr>" >> $output_filename
            echo "<tr><td> Partially Failed Cases </td>" "<td> $partial_test_case_fail </td></tr>" >> $output_filename
            echo "<tr><td> Not Applicable Cases </td>" "<td> $test_case_NA </td></tr>" >> $output_filename
        echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    if [[ "$both_failed_counter" -ne 0 ]]
    then
        echo "<h3><a href=\"javascript:;\" onclick=toggle('failed2')>Failed Cases</a></h3>" >> $output_filename
        echo "<div id=\"failed2\" style=\"display:none\">" >> $output_filename
            echo "<table>" >> $output_filename
                echo "<tr><td><b>Resource Name</b></td><td><b>Sysfs Node</b></td></tr>" >> $output_filename
                for (( i = 0 ; i < ${#failed_resource_name[@]} ; i=$i+1 ));
                do
                    echo "<tr><td>" ${failed_resource_name[i]} "</td><td>" ${failed_resource_node[i]} "</td></tr>" >> $output_filename
                done
            echo "</table>" >> $output_filename
        echo "</div>" >> $output_filename
    fi

    if [[ "$second_lock_fail" -ne 0 ]]
    then
        echo "<h3><a href=\"javascript:;\" onclick=toggle('second_failed2')>Second Lock Failed Cases</a></h3>" >> $output_filename
        echo "<div id=\"second_failed2\" style=\"display:none\">" >> $output_filename
            echo "<table>" >> $output_filename
                echo "<tr><td><b>Resource Name</b></td><td><b>Sysfs Node</b></td></tr>" >> $output_filename
                for (( i = 0 ; i < ${#second_failed_resource_name[@]} ; i=$i+1 ));
                do
                    echo "<tr><td>" ${second_failed_resource_name[i]} "</td><td>" ${second_failed_resource_node[i]} "</td></tr>" >> $output_filename
                done
            echo "</table>" >> $output_filename
        echo "</div>" >> $output_filename
    fi

    echo " "
    echo " "
    QLOGI "************* Test Summary **********************************"
    QLOGI "Total Concurrency Testcases           = $total_test_case"
    QLOGI "Total Concurrency Tests Passed        = $test_case_pass"
    QLOGI "Total Concurrency Tests Failed        = $test_case_fail"
    QLOGI "Total Concurrency Tests False Alarm   = $test_case_false_alarm"
    QLOGI "Partially Failed Cases                = $partial_test_case_fail"
    QLOGI "Not Applicable Cases                  = $test_case_NA"
    QLOGI "*************************************************************"

    echo " "
    echo " "

    UTIL_supportedLocks
    UTIL_htmlFooter
}

#**********************************************************
#*                                                        *
# --------------- PERFLOCK CRASH TEST ------------------- *
#*                                                        *
#**********************************************************
function startPerflockCrashTest() {

    echo " "
    QLOGI "**********************************************"
    QLOGI "*Starting Perflock Crash-Recovery Tests......*"
    QLOGI "**********************************************"
    echo " "
    sleep 5

    UTIL_htmlHeader
    echo "<body>"  >> $output_filename

    echo "<button onclick=\"scrollToTop(1000)\" id=\"myBtn\" title=\"Go to top\">Top</button>" >> $output_filename

    echo "<h1>" >> $output_filename
        echo "Perf Lock Crash Test Report" >> $output_filename
    echo "</h1>" >> $output_filename

    UTIL_reportDate
    echo "<hr>" >> $output_filename
    UTIL_deviceInfo

    echo "<br>" >> $output_filename

    echo "<h3><a href=\"javascript:;\" onclick=toggle('detailed')>Perf Lock Execution Report</a></h3>" >> $output_filename

    echo "<div id=\"detailed\" style=\"display:none\">" >> $output_filename

    echo "<table>"  >> $output_filename
    echo "  <tr>"  >> $output_filename
        echo "    <th>Resource Name</th>"  >> $output_filename
        echo "    <th>Resource OpCode</th>"   >> $output_filename
        echo "    <th>Resource Sysfs Node</th>"   >> $output_filename
        echo "    <th>Resource Value</th>"   >> $output_filename
        echo "    <th>Perf Lock test command</th>"   >> $output_filename
        echo "    <th>Sysfs Node Value Before lock</th>"   >> $output_filename
        echo "    <th>Sysfs Node Value During lock</th>"   >> $output_filename
        echo "    <th>Sysfs Node Value After crash Recovery</th>"   >> $output_filename
        echo "    <th>Perflock Crash Recovery</th>"   >> $output_filename
    echo "  </tr>"   >> $output_filename

    for (( i = 0 ; i < ${#resource_name[@]} ; i=$i+1 ));
    do

    if [[ ${resource_supported[i]} == "Y" ]]
    then

        echo "<tr>" >> $output_filename
            echo "<td>" >> $output_filename
                echo ${resource_name[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo ${resource_opcode[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "${resource_sysfs_node[i]}" >> $output_filename
            echo "</td>" >> $output_filename

           #Get the Node value, before acquiring perflock
           before_node=`adb shell cat ${resource_sysfs_node[i]}`

           #Acquire perflock - 1st Attempt
            echo ""
            echo ""
            QLOGI "First try :: adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 15000 ${resource_opcode[i]},${resource_val[i]}"
            adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 15000 "${resource_opcode[i]},${resource_val[i]}" &
            sleep 5

            #Get Node value, when perflock is acquired - 1st Attempt
            during_node_first_run=`adb shell cat ${resource_sysfs_node[i]}`

            #First Attempt Perflock Failed. Check with different resource value in 2nd Attempt
            if [ "$before_node" == "$during_node_first_run" ]
            then
                sleep 10
                #Acquire perflock - 2nd Attempt
                QLOGE "First try to acquire lock Failed.... Modifying the value and trying again....."
                QLOGI "Second try :::: adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 15000 ${resource_opcode[i]},${resource_new_val[i]}"
                adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 15000 "${resource_opcode[i]},${resource_new_val[i]}" &
                sleep 5
                during_node_second_run=`adb shell cat ${resource_sysfs_node[i]}`
                if [ "$before_node" == "$during_node_second_run" ]
                then
                    QLOGE "Second try to acquire lock Also Failed.... Moving to next perflock....."
                else
                    QLOGI "Second Attempt Succeeded !!!"
                fi
                during_node=$during_node_second_run
                final_val=${resource_new_val[i]}
            else    #First Attempt Perflock Passed...
                during_node=$during_node_first_run
                final_val=${resource_val[i]}
                QLOGI "First Attempt Succeeded !!!"
            fi

            echo "<td>" >> $output_filename
                echo $final_val >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "$PERFLOCK_NATIVE_TEST_EXECUTABLE --acq 0 15000" ${resource_opcode[i]}","$final_val >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                echo $before_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                echo $during_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            QLOGI "Stopping perf-hal-1-0 ........ to simulate perfHal Crash"
            adb shell stop perf-hal-1-0
            sleep 3

            QLOGI "Starting perf-hal-1-0 ........ to restart perfHal Service"
            adb shell start perf-hal-1-0
            sleep 5

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                after_node=`adb shell cat ${resource_sysfs_node[i]}`
                echo $after_node >> $output_filename
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                if [[ $before_node == *"No such"* ]]
                then
                    echo "<p id=pB>" >> $output_filename
                    echo "Node Not Present. Hence, Test case N.A." >> $output_filename
                    echo "</p id=pB>" >> $output_filename
                    let "test_case_NA++"
                else
                    if [[ "$before_node" == "$during_node" ]]
                    then
                        echo "<p id=pB>" >> $output_filename
                        echo "Acquire Failed. Hence, Test case N.A." >> $output_filename
                        echo "</p id=pB>" >> $output_filename
                        let "test_case_NA++"
                    elif [ "$before_node" != "$after_node" ]
                    then
                        echo "<p id=pR>" >> $output_filename
                        echo "Recovery Failed " >> $output_filename
                        echo "</p id=pR>" >> $output_filename
                        let "test_case_fail++"
                        failed_resource_name[failed_counter]=${resource_name[i]}
                        failed_resource_node[failed_counter]=${resource_sysfs_node[i]}
                        let "failed_counter++"
                    else
                        echo "<p id=pG>" >> $output_filename
                        echo "Recovery Success " >> $output_filename
                        echo "</p id=pG>" >> $output_filename
                        let "test_case_pass++"
                    fi
                fi
            echo "</td>" >> $output_filename
        echo "</tr>" >> $output_filename

        let "total_test_case++"
        echo " "
    fi
    done
    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    echo "<br>" >> $output_filename

    echo "<h3><a href=\"javascript:;\" onclick=toggle('summary')>Summary</a></h3>" >> $output_filename

    echo "<div id=\"summary\" style=\"display:block\">" >> $output_filename
        echo "<table>" >> $output_filename
            echo "<tr><td> Total Test Cases </td>" "<td> $total_test_case </td></tr>" >> $output_filename
            echo "<tr><td> Test Case Pass </td>" "<td> $test_case_pass </td></tr>" >> $output_filename
            echo "<tr><td> Test Case Fail </td>" "<td> $test_case_fail </td></tr>" >> $output_filename
            echo "<tr><td> Test Case N.A </td>" "<td> $test_case_NA </td></tr>" >> $output_filename
        echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    echo "<br>" >> $output_filename
    echo "<br>" >> $output_filename

    if [ "$test_case_fail" -ne 0 ]
    then
        echo "<h3><a href=\"javascript:;\" onclick=toggle('failed')>Failed Cases</a></h3>" >> $output_filename
        echo "<div id=\"failed\" style=\"display:none\">" >> $output_filename
            echo "<table>" >> $output_filename
                echo "<tr><td><b>Resource Name</b></td><td><b>Sysfs Node</b></td></tr>" >> $output_filename
                for (( i = 0 ; i < ${#failed_resource_name[@]} ; i=$i+1 ));
                do
                echo "<tr><td>" ${failed_resource_name[i]} "</td><td>" ${failed_resource_node[i]} "</td></tr>" >> $output_filename
                done
            echo "</table>" >> $output_filename
        echo "</div>" >> $output_filename
    fi

    echo " "
    echo " "
    QLOGI "************* Test Summary *********************************"
    QLOGI "Total Supported Perflocks         = $total_test_case"
    QLOGI "Perflocks Crash-Recovery Passed   = $test_case_pass"
    QLOGI "Perflocks Crash-Recovery Failed   = $test_case_fail"
    QLOGI "Perflocks Crash-Recovery NA       = $test_case_NA"
    QLOGI "************************************************************"

    echo " "

    UTIL_supportedLocks
    UTIL_htmlFooter
}

#*********************************************************
#*                                                       *
# --------------- PERFLOCK HINT TEST ------------------- *
#*                                                       *
#*********************************************************
function startPerflockHintTest() {


    echo " "
    QLOGI "********************************"
    QLOGI "*Starting Perf Hint Tests......*"
    QLOGI "********************************"
    echo " "
    sleep 5

    UTIL_htmlHeader
    echo "<body>"  >> $output_filename

    echo "<button onclick=\"scrollToTop(1000)\" id=\"myBtn\" title=\"Go to top\">Top</button>" >> $output_filename

    echo "<h1>" >> $output_filename
        echo "Perf Hint Test Report" >> $output_filename
    echo "</h1>" >> $output_filename

    UTIL_reportDate
    echo "<hr>" >> $output_filename
    UTIL_deviceInfo

    echo "<br>" >> $output_filename

    echo "<h3><a href=\"javascript:;\" onclick=toggle('detailed')>Perf Hint Execution Report</a></h3>" >> $output_filename

    echo "<div id=\"detailed\" style=\"display:none\">" >> $output_filename

    echo "<table>"  >> $output_filename
    echo "  <tr>"  >> $output_filename
        echo "    <th>Hint Id</th>"  >> $output_filename
        echo "    <th>Perf Hint test command</th>"   >> $output_filename
        echo "    <th>Resource OpCodes</th>"   >> $output_filename
        echo "    <th>Resource Value</th>"   >> $output_filename
        echo "    <th>Resource Sysfs Node</th>"   >> $output_filename
        echo "    <th>Node Value Before Hint Acq</th>"   >> $output_filename
        echo "    <th>Node Value During Hint Acq</th>"   >> $output_filename
        echo "    <th>Node Value After Hint Release</th>"   >> $output_filename
        echo "    <th>PerfLock Acquire Status for Hint</th>"   >> $output_filename
        echo "    <th>PerfLock Release Status for Hint</th>"   >> $output_filename
    echo "  </tr>"   >> $output_filename

    let totalHintTests=0;
    let totalHintPass=0;
    let totalHintFail=0;

    for (( i = 0 ; i < ${#hint_id_array[@]} ; i=$i+1 ));
    do
        ((totalHintTests++))
        if [[ $i == 0 ]]
        then
            let start=0
        else
            let start=$start+${hint_id_resource_count_map_array[i-1]}
        fi
        let end=${hint_id_resource_count_map_array[i]}

        echo "<tr>" >> $output_filename
            echo "<td>" >> $output_filename
            echo ${hint_id_array[i]} >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "$PERFLOCK_NATIVE_TEST_EXECUTABLE --hint" ${hint_id_array[i]}" 15000 \"TestApp\"" ${hint_type_array[i]}  >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                    echo "<br/>" >> $output_filename
                    echo ${hint_resource_opcode_array[counter]} >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<hr>" >> $output_filename
                done
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                    echo "<br/>" >> $output_filename
                    echo ${hint_resource_val_array[counter]} >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<hr>" >> $output_filename
                done
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                    echo "<br/>" >> $output_filename
                    echo ${hint_resource_sysfs_node_array[counter]} >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<hr>" >> $output_filename
                done
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                    echo "<br/>" >> $output_filename
                    hint_before_node_val_array[b_index]=`adb shell cat ${hint_resource_sysfs_node_array[counter]}`
                    if [[ ${hint_before_node_val_array[b_index]} == *"No such"*  ]]
                    then
                        echo "Node Not Present" >> $output_filename
                    else
                        echo ${hint_before_node_val_array[b_index]} >> $output_filename
                    fi
                    echo "<br/>" >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<hr>" >> $output_filename
                    ((b_index++))
                done
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            QLOGI "adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --hint ${hint_id_array[i]} 15000 \"TestApp\" ${hint_type_array[i]}"
            adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --hint "${hint_id_array[i]}" 15000 \"TestApp\" "${hint_type_array[i]}" &

            sleep 5

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                    echo "<br/>" >> $output_filename
                    hint_during_node_val_array[d_index]=`adb shell cat ${hint_resource_sysfs_node_array[counter]}`
                    if [[ ${hint_during_node_val_array[d_index]} == *"No such"*  ]]
                    then
                        echo "Node Not Present" >> $output_filename
                    else
                        echo ${hint_during_node_val_array[d_index]} >> $output_filename
                    fi
                    echo "<br/>" >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<hr>" >> $output_filename
                    ((d_index++))
                done
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            sleep 15

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                    echo "<br/>" >> $output_filename
                    hint_after_node_val_array[a_index]=`adb shell cat ${hint_resource_sysfs_node_array[counter]}`
                    if [[ ${hint_after_node_val_array[a_index]} == *"No such"*  ]]
                    then
                        echo "Node Not Present" >> $output_filename
                    else
                        echo ${hint_after_node_val_array[a_index]} >> $output_filename
                    fi
                    echo "<br/>" >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<hr>" >> $output_filename
                    ((a_index++))
                done
                echo "</p>" >> $output_filename
            echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                let individual_counter=0;
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                if [[ "${hint_before_node_val_array[counter]}" != *"No such"* ]]
                then
                    if [[ "${hint_before_node_val_array[counter]}" == "${hint_during_node_val_array[counter]}" ]]
                    then
                        testVal=`printf '%d' ${hint_resource_val_array[counter]}`
                        if [[ "${hint_resource_opcode_array[counter]}" == "0x40400000" ||
                              "$testVal" == "${hint_before_node_val_array[counter]}" ]]
                        then
                            echo "<p id=pDV>" >> $output_filename
                                echo "False Alarm" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<hr>" >> $output_filename
                            echo "</p id=pDV>" >> $output_filename
                        else
                            echo "<p id=pR>" >> $output_filename
                                echo "Acquire Failed" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<hr>" >> $output_filename
                            echo "</p id=pR>" >> $output_filename
                            hint_lock_acq_result[$individual_counter]="Fail"
                            ((individual_counter++))
                        fi
                    else
                        echo "<p id=pG>" >> $output_filename
                            echo "Acquire Success" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                        echo "</p id=pG>" >> $output_filename
                        hint_lock_acq_result[$individual_counter]="Pass"
                        ((individual_counter++))
                    fi
                else
                        echo "<p id=pB>" >> $output_filename
                            echo "Node Not present..." >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                        echo "</p id=pB>" >> $output_filename
                fi
            done
            echo "</p>" >> $output_filename
        echo "</td>" >> $output_filename

            echo "<td>" >> $output_filename
                echo "<p>" >> $output_filename
                let individual_counter=0;
                for (( counter = $start ; counter < $start+$end ; counter=$counter+1 ));
                do
                if [[ "${hint_before_node_val_array[counter]}" != *"No such"* ]]
                then
                    if [[ "${hint_before_node_val_array[counter]}" == "${hint_after_node_val_array[counter]}" ]]
                    then
                        testVal=`printf '%d' ${hint_resource_val_array[counter]}`
                        if [[ "${hint_resource_opcode_array[counter]}" == "0x40400000" ||
                              "$testVal" == "${hint_before_node_val_array[counter]}" ]]
                        then
                            echo "<p id=pDV>" >> $output_filename
                                echo "False Alarm" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<hr>" >> $output_filename
                            echo "</p id=pDV>" >> $output_filename
                        else
                            echo "<p id=pG>" >> $output_filename
                                echo "Release Success" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<hr>" >> $output_filename
                                hint_lock_rel_result[$individual_counter]="Pass"
                                ((individual_counter++))
                            echo "</p id=pG>" >> $output_filename
                        fi
                    else
                        echo "<p id=pR>" >> $output_filename
                            echo "Release Fail" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                            hint_lock_rel_result[$individual_counter]="Fail"
                            ((individual_counter++))
                        echo "</p id=pR>" >> $output_filename
                    fi
                else
                        echo "<p id=pB>" >> $output_filename
                            echo "Node Not present..." >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                        echo "</p id=pB>" >> $output_filename
                fi
            done
            echo "</p>" >> $output_filename
        echo "</td>" >> $output_filename
    echo "</tr>" >> $output_filename

        for ((result_index = 0; result_index < ${#hint_lock_acq_result[@]}; result_index++))
        do
        if [[ ${hint_lock_acq_result[result_index]} == *"Fail"* || ${hint_lock_rel_result[result_index]} == *"Fail"* ]]
        then
            hint_result[$result_index]="Fail"
            break;
        fi
        done
        unset -v hint_lock_acq_result[*]
        unset -v hint_lock_rel_result[*]
    done

    echo "</table>"  >> $output_filename
    echo "</div>"  >> $output_filename

    for result in ${hint_result[*]}
    do
       if [[ $result == "Fail" ]]
       then
           ((totalHintFail++))
       fi
    done

    totalHintPass=$(expr $totalHintTests - $totalHintFail)

    echo "<br>" >> $output_filename

    echo "<h3><a href=\"javascript:;\" onclick=toggle('summary')>Summary</a></h3>" >> $output_filename
    echo "<div id=\"summary\" style=\"display:block\">" >> $output_filename

    echo "<table>" >> $output_filename
        echo "<tr><td> Total PerfHints Tested </td>" "<td> $totalHintTests </td></tr>" >> $output_filename
        echo "<tr><td> Total PerfHints Passed </td>" "<td> $totalHintPass </td></tr>" >> $output_filename
        echo "<tr><td> Total PerfHints Failed </td>" "<td> $totalHintFail </td></tr>" >> $output_filename
    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    echo "<br>" >> $output_filename
    echo "<br>" >> $output_filename

    echo " "
    echo " "
    QLOGI "************* Test Summary *******************"
    QLOGI "Total PerfHints Tested   = $totalHintTests"
    QLOGI "Total PerfHints Passed   = $totalHintPass"
    QLOGI "Total PerfHints Failed   = $totalHintFail"
    QLOGI "**********************************************"

    echo " "

    UTIL_supportedLocks
    UTIL_htmlFooter
}

#************************************************************
#*                                                          *
# --------------- PERFLOCK PROFILE TEST ------------------- *
#*                                                          *
#************************************************************
function startPerflockProfileTest() {

    echo " "
    QLOGI "***********************************"
    QLOGI "*Starting Perf Profile Tests......*"
    QLOGI "***********************************"
    echo " "
    sleep 5

    UTIL_htmlHeader

    echo "<body>"  >> $output_filename
    echo "<button onclick=\"scrollToTop(1000)\" id=\"myBtn\" title=\"Go to top\">Top</button>" >> $output_filename

    echo "<h1>" >> $output_filename
        echo "Perf Profiles Test Report" >> $output_filename
    echo "</h1>" >> $output_filename

    UTIL_reportDate
    echo "<hr>" >> $output_filename
    UTIL_deviceInfo

    echo "<br>" >> $output_filename

    echo "<h3><a href=\"javascript:;\" onclick=toggle('detailed')>Perf Profiles Execution Report</a></h3>" >> $output_filename

    echo "<div id=\"detailed\" style=\"display:none\">" >> $output_filename

    echo "<table>"  >> $output_filename
    echo "  <tr>"  >> $output_filename
    echo "    <th>Profile Name</th>"  >> $output_filename
    echo "    <th>Resource OpCodes</th>"   >> $output_filename
    echo "    <th>Resource Value</th>"   >> $output_filename
    echo "    <th>Resource Sysfs Node</th>"   >> $output_filename
    echo "    <th>Before Profile Acq</th>"   >> $output_filename
    echo "    <th>During Profile Acq</th>"   >> $output_filename
    echo "    <th>After Profile Release</th>"   >> $output_filename
    echo "    <th>Resource Acquire Status for Profile</th>"   >> $output_filename
    echo "    <th>Resource Release Status for Profile</th>"   >> $output_filename
    echo "  </tr>"   >> $output_filename

    let totalProfileTests=0;
    let totalProfilesPass=0;
    let totalProfilesFail=0;
    let totalProfilesNA=0;

    for (( i = 0 ; i < ${#profile_name[@]} ; i=$i+1 ));
    do
        ((totalProfileTests++))

        if [[ $i == 0 ]]
        then
            let start=0
        else
            let start=${profile_map[i-1]}+1
        fi
        let end=${profile_map[i]}

        if [[ $end -gt 0 ]]
        then
            echo "<tr>" >> $output_filename
                echo "<td>" >> $output_filename
                    echo ${profile_name[i]} >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        echo "<br/>" >> $output_filename
                        if [[ ${conf_resource_new_lock_op_code[counter]} != "0xDEAD" ]]
                        then
                            echo ${conf_resource_new_lock_op_code[counter]} >> $output_filename
                        elif [[ ${conf_resource_new_lock_op_code[counter]} == "0xDEAD" ]]
                        then
                            echo "-1" >> $output_filename
                        fi
                        echo "<br/>" >> $output_filename
                        echo "<br/>" >> $output_filename
                        echo "<hr>" >> $output_filename
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        echo "<br/>" >> $output_filename
                        if [[ ${conf_resource_val[counter]} == "-1" ]]
                        then
                            cat "target-load_"${profile_name[i]} >> $output_filename
                        else
                            echo ${conf_resource_val[counter]} >> $output_filename
                        fi
                        echo "<br/>" >> $output_filename
                        echo "<br/>" >> $output_filename
                        echo "<hr>" >> $output_filename
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        echo "<br/>" >> $output_filename
                        echo ${conf_resource_sysfs_map[counter]} >> $output_filename
                        echo "<hr>" >> $output_filename
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        echo "<br/>" >> $output_filename
                        profile_before_node_val_array[b_index]=`adb shell cat ${conf_resource_sysfs_map[counter]}`
                        if [[ ${profile_before_node_val_array[b_index]} == *"No such"*  ]]
                        then
                            echo "Node Not Present" >> $output_filename
                        else
                            echo ${profile_before_node_val_array[b_index]} >> $output_filename
                        fi
                        echo "<br/>" >> $output_filename
                        echo "<br/>" >> $output_filename
                        echo "<hr>" >> $output_filename
                        ((b_index++))
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

                profile=${profile_id[i]}

                echo "Profile = " $profile
                QLOGI "adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --profile 0 $profile 10000"
                adb shell $PERFLOCK_NATIVE_TEST_EXECUTABLE --profile 0 "$profile "10000 &

                sleep 5
                let index=0;

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        echo "<br/>" >> $output_filename
                        profile_during_node_val_array[d_index]=`adb shell cat ${conf_resource_sysfs_map[counter]}`
                        if [[ ${profile_during_node_val_array[d_index]} == *"No such"*  ]]
                        then
                            echo "Node Not Present" >> $output_filename
                        else
                            echo ${profile_during_node_val_array[d_index]} >> $output_filename
                        fi
                        echo "<br/>" >> $output_filename
                        echo "<br/>" >> $output_filename
                        echo "<hr>" >> $output_filename
                        ((d_index++))
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

                sleep 10
                let index=0;

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        echo "<br/>" >> $output_filename
                        profile_after_node_val_array[a_index]=`adb shell cat ${conf_resource_sysfs_map[counter]}`
                        if [[ ${profile_after_node_val_array[a_index]} == *"No such"*  ]]
                        then
                            echo "Node Not Present" >> $output_filename
                        else
                            echo ${profile_after_node_val_array[a_index]} >> $output_filename
                        fi
                        echo "<br/>" >> $output_filename
                        echo "<br/>" >> $output_filename
                        echo "<hr>" >> $output_filename
                        ((a_index++))
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    let individual_counter=0;
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        if [[ "${profile_before_node_val_array[counter]}" == "${profile_during_node_val_array[counter]}" ]]
                        then
                        if [[ "${conf_resource_new_lock_op_code[counter]}" == "0x40400000"  ||
                              "${conf_resource_val[counter]}" == "${profile_before_node_val_array[counter]}" ]]
                        then
                            echo "<p id=pDV>" >> $output_filename
                                echo "False Alarm" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<hr>" >> $output_filename
                            echo "</p id=pDV>" >> $output_filename
                        else
                            echo "<p id=pR>" >> $output_filename
                            echo "Acquire Fail" >> $output_filename
                            echo "</p id=pR>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                            profile_lock_acq_result[$individual_counter]="Fail"
                            ((individual_counter++))
                        fi
                        else
                            echo "<p id=pG>" >> $output_filename
                            echo "Acquire Success" >> $output_filename
                            echo "</p id=pG>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                            profile_lock_acq_result[$individual_counter]="Pass"
                            ((individual_counter++))
                        fi
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p>" >> $output_filename
                    let individual_counter=0;
                    for (( counter = $start ; counter <= $end ; counter=$counter+1 ));
                    do
                        if [[ "${profile_before_node_val_array[counter]}" == "${profile_after_node_val_array[counter]}" ]]
                        then
                        if [[ "${conf_resource_new_lock_op_code[counter]}" == "0x40400000"  ||
                              "${conf_resource_val[counter]}" == "${profile_before_node_val_array[counter]}" ]]
                        then
                            echo "<p id=pDV>" >> $output_filename
                                echo "False Alarm" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<br/>" >> $output_filename
                                echo "<hr>" >> $output_filename
                            echo "</p id=pDV>" >> $output_filename
                        else
                            echo "<p id=pG>" >> $output_filename
                            echo "Release Success" >> $output_filename
                            echo "</p id=pG>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                            profile_lock_rel_result[$individual_counter]="Pass"
                            ((individual_counter++))
                        fi
                        else
                            echo "<p id=pR>" >> $output_filename
                            echo "Release Failed" >> $output_filename
                            echo "</p id=pR>" >> $output_filename
                            echo "<br/>" >> $output_filename
                            echo "<hr>" >> $output_filename
                            profile_lock_rel_result[$individual_counter]="Fail"
                            ((individual_counter++))
                        fi
                    done
                    echo "</p>" >> $output_filename
                echo "</td>" >> $output_filename

            echo "</tr>" >> $output_filename
        else
            echo "<tr>" >> $output_filename
                echo "<td>" >> $output_filename
                    echo ${profile_name[i]} >> $output_filename
                    echo "<br/>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                    echo "No Profile Configuration present in conf file" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                    profile_result[$i]="NA"
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                        echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                        echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                        echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                        echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                        echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                    echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                    echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename

                echo "<td>" >> $output_filename
                    echo "<p id=pR>" >> $output_filename
                    echo "-" >> $output_filename
                    echo "</p id=pR>" >> $output_filename
                echo "</td>" >> $output_filename
            echo "</tr>" >> $output_filename
        fi

        for ((result_index = 0; result_index < ${#profile_lock_acq_result[@]}; result_index++))
        do
        if [[ ${profile_lock_acq_result[result_index]} == *"Fail"* || ${profile_lock_rel_result[result_index]} == *"Fail"* ]]
        then
           profile_result[$result_index]="Fail"
           break;
        fi
        done
        unset -v profile_lock_acq_result[*]
        unset -v profile_lock_rel_result[*]
    done

    echo "</table>"  >> $output_filename
    echo "</div>"  >> $output_filename

    for profile_result in ${profile_result[*]}
    do
    if [[ $profile_result == "Fail" ]]
    then
        (( totalProfilesFail++ ))
    elif [[ $profile_result == "NA" ]]
    then
        (( totalProfilesNA++ ))
    fi
    done

    totalProfilesPass=$(expr $totalProfileTests - $totalProfilesFail - $totalProfilesNA)


    echo "<br>" >> $output_filename

    echo "<h3><a href=\"javascript:;\" onclick=toggle('summary')>Summary</a></h3>" >> $output_filename
    echo "<div id=\"summary\" style=\"display:block\">" >> $output_filename

    echo "<table>" >> $output_filename
        echo "<tr><td> Total Profiles Tested </td>" "<td> $totalProfileTests </td></tr>" >> $output_filename
        echo "<tr><td> Total Profiles Passed </td>" "<td> $totalProfilesPass </td></tr>" >> $output_filename
        echo "<tr><td> Total Profiles Failed </td>" "<td> $totalProfilesFail </td></tr>" >> $output_filename
        echo "<tr><td> Total Profiles NA </td>" "<td> $totalProfilesNA </td></tr>" >> $output_filename
    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename

    echo "<br>" >> $output_filename
    echo "<br>" >> $output_filename

    echo " "
    echo " "
    QLOGI "************* Test Summary *******************"
    QLOGI "Total Profiles Tested   = $totalProfileTests"
    QLOGI "Total Profiles Passed   = $totalProfilesPass"
    QLOGI "Total Profiles Failed   = $totalProfilesFail"
    QLOGI "Total Profiles N.A.     = $totalProfilesNA"
    QLOGI "**********************************************"

    echo " "

    UTIL_supportedLocks
    UTIL_htmlFooter
}

#**********************************************************
#*                                                        *
# --------------- CONVERT Hex to Decimal ---------------- *
#*                                                        *
#**********************************************************
function UTIL_hex2Dec() {

    HEX_VAL_TO_CONVERT=$1
    hex2dec=$((16#$HEX_VAL_TO_CONVERT))
    echo $hex2dec
    return
}

#**********************************************************
#*                                                        *
# --------------- CONVERT Decimal to Hex ---------------- *
#*                                                        *
#**********************************************************
function UTIL_dec2Hex() {

    DEC_VAL_TO_CONVERT=$1
    dec2hex=`printf '0x%x\n' $DEC_VAL_TO_CONVERT`
    echo $dec2hex
    return
}

#****************************************************************************************
#*                                                                                      *
# --------------- Parse conf file and extract locks and files sections ---------------- *
#*                                                                                      *
#****************************************************************************************
function parseConfFile() {

    CONF_FILENAME=$1
    lock_tag=`cat $CONF_FILENAME | grep "locks"`
    file_tag=`cat $CONF_FILENAME | grep "files"`

    if [[ $lock_tag == "[locks]" && $file_tag == "[files]" ]]   #Conf file has both locks and files section
    then
        awk '/\[locks\]/{flag=1;next}/\[files\]/{flag=0}flag' $1 | awk 'NF' > locks-files_$CONF_FILENAME.txt
        awk '/\[files\]/{flag=1;next}/\[priority\]/{flag=0}flag' $1 | awk 'NF' >> locks-files_$CONF_FILENAME.txt
        QLOGI "File = $CONF_FILENAME has locks & files sections"
    elif [[ $lock_tag == "" && $file_tag == "[files]" ]] #Conf file has files section, but no locks
    then
        awk '/\[files\]/{flag=1;next}/\[priority\]/{flag=0}flag' $1 | awk 'NF' >> locks-files_$CONF_FILENAME.txt
        QLOGI "File = $CONF_FILENAME has ONLY files section"
    elif [[ $lock_tag == "[locks]" &&  $file_tag == "" ]] #Conf file has locks section, but no files
    then
        awk '/\[locks\]/{flag=1;next}/\[priority\]/{flag=0}flag' $1 | awk 'NF' >> locks-files_$CONF_FILENAME.txt
        QLOGI "File = $CONF_FILENAME has ONLY locks section"
    fi
}

#****************************************************************************************
#*                                                                                      *
# --------------- Extract info from conf files and form resource arrays --------------- *
#*                                                                                      *
#****************************************************************************************
# ---------------- USED FOR PROFILE VERIFICATION -- START ------------------
declare -a profile_map=()

let gLineCounter=0
let gProfileIdCounter=0
# ---------------- USED FOR PROFILE VERIFICATION -- END ------------------

function extractInfoFromConf() {

    CONF_FILE=$1
    parseConfFile $CONF_FILE

    while IFS= read -r line # Read a line
    do
        if [[ $line == *"0x"* ]]
        then
            #Mark Resource type as Lock
            conf_resource_type[gLineCounter]="L"

            # Extract lock info from line
            my_line=`(echo $line | cut -d' ' -f1)`

            len=$(UTIL_strlen $my_line)

            #Extract old opcode from line and convert to decimal
            if [[ $len == 5 ]]
            then
                conf_resource_old_lock_op_code=$(UTIL_hex2Dec ${my_line:2:1})
            elif [[ $len == 6 ]]
            then
                conf_resource_old_lock_op_code=$(UTIL_hex2Dec ${my_line:2:2})
            fi

            conf_resource_old_op_code_name=${old_resources_list[conf_resource_old_lock_op_code]}

            if [[ $conf_resource_old_op_code_name == "CPU0_MIN_FREQ" ]]
            then
                conf_resource_old_op_code_name="MIN_FREQ_CLUSTER_BIG_CORE_0"
            elif [[ $conf_resource_old_op_code_name == "CPU0_MAX_FREQ" ]]
            then
                conf_resource_old_op_code_name="MAX_FREQ_CLUSTER_BIG_CORE_0"
            elif [[ $conf_resource_old_op_code_name == "CPU4_MIN_FREQ" ]]
            then
                conf_resource_old_op_code_name="MIN_FREQ_CLUSTER_LITTLE_CORE_0"
            elif [[ $conf_resource_old_op_code_name == "CPU4_MAX_FREQ" ]]
            then
                conf_resource_old_op_code_name="MAX_FREQ_CLUSTER_LITTLE_CORE_0"
            fi

            conf_resource_new_lock_op_code[gLineCounter]=$(getNewOpCodeFromOldResName $conf_resource_old_op_code_name)
            conf_resource_sysfs_map[gLineCounter]=$(getSysNodeFromOpCode ${conf_resource_new_lock_op_code[gLineCounter]})

            #Extract value for opcode from line and convert to decimal
            conf_resource_val[gLineCounter]=$(UTIL_hex2Dec ${my_line:4:2})
            ((gLineCounter++))
        elif [[ $line == *"sys"* ]]
        then
            #Mark Resource type as Files
            conf_resource_type[gLineCounter]="F"

            #Files do not have opcode.. Hence mark it as 0XDEAD
            conf_resource_new_lock_op_code[gLineCounter]="0xDEAD"

            #Files have values. Extract the value and store it to array
            #Handle Seperately for target_loads
            if [[ $line == *"target_loads"* ]]
            then
                file_val=`(echo $line | cut -d'/' -f1)`
                echo $file_val > "target-load"_$CONF_FILE
                conf_resource_val[gLineCounter]="-1"
            elif [[ $line != *"target_loads"* ]]
            then
                file_val=`(echo $line | cut -d' ' -f1)`
                conf_resource_val[gLineCounter]=$file_val
            fi

            #Files have sysfs Node. Extract the value and store it to array
            file_node=`(echo $line | rev | cut -d' ' -f1 | rev)`
            conf_resource_sysfs_map[gLineCounter]=$file_node
            ((gLineCounter++))
        fi
    done < "locks-files_"$CONF_FILE.txt

    profile_map[gProfileIdCounter]=`echo $((gLineCounter-1))`
    ((gProfileIdCounter++))

    #Clean-up files pulled from device and generated files
    rm -rf $CONF_FILE
    rm -rf "locks-files_"$CONF_FILE.txt
}

#**********************************************************
#*                                                        *
# --------------- Print all items in a array ------------ *
#*                                                        *
#**********************************************************
function UTIL_printArray() {

    debugArray=$1

    array_index=0;
    echo "Array to Loop thorugh is " $debugArray
    loopArray="$debugArray[@]"

    for item in ${!loopArray};
    do
        echo "Values in" $debugArray" Array @ index " $array_index "= " $item
        ((array_index++))
    done
}

#**********************************************************
#*                                                        *
# --------------- Calculate String Length --------------- *
#*                                                        *
#**********************************************************
function UTIL_strlen() {

    LENGTH_OF=$1
    echo ${#LENGTH_OF}
}

#**********************************************************
#*                                                        *
# --------------- Report Generation date ---------------- *
#*                                                        *
#**********************************************************
function UTIL_reportDate() {

    echo "<p id=pDate>" >> $output_filename
    GENERATED_DATE=`date`
    echo "Generated : $GENERATED_DATE" >> $output_filename
    echo "</p id=pDate>" >> $output_filename
}

#**********************************************************
#*                                                        *
# -----------Print Device supported locks --------------- *
#*                                                        *
#**********************************************************
function UTIL_supportedLocks() {

    echo "<h3><a href=\"javascript:;\" onclick=toggle('perflock-superset')>Perflocks Supported / Unsupported List</a></h3>" >> $output_filename
    echo "<div id=\"perflock-superset\" style=\"display:none\">" >> $output_filename
    echo "<table>" >> $output_filename
        echo "<tr>" >> $output_filename
            echo "<td><b>Resource Name</b></td>" >> $output_filename
            echo "<td><b>Sysfs Node</b></td>" >> $output_filename
            echo "<td><b>Supported ?? </b></td>" >> $output_filename
        echo "</tr>" >> $output_filename

        for (( i = 0 ; i < ${#resource_name[@]} ; i=$i+1 ));
        do
            echo "<tr>" >> $output_filename
                echo "<td>" ${resource_name[i]} "</td>" >> $output_filename
                echo "<td>" ${resource_sysfs_node[i]} "</td>" >> $output_filename

                if [[ ${resource_supported[i]} == "Y" ]]
                then
                    echo "<td>"  >> $output_filename
                        echo "<p id=pG>" >> $output_filename
                        echo ${resource_supported[i]} >> $output_filename
                        echo "</p id=pG>" >> $output_filename
                    echo "</td>" >> $output_filename
                else
                    echo "<td>"  >> $output_filename
                        echo "<p id=pR>" >> $output_filename
                        echo ${resource_supported[i]} >> $output_filename
                        echo "</p id=pR>" >> $output_filename
                    echo "</td>" >> $output_filename
                fi
            echo "</tr>" >> $output_filename
        done
    echo "</table>" >> $output_filename
    echo "</div>" >> $output_filename
}

#************************************************************************************
#*                                                                                  *
# -----------Parse perfboostsconfig.xml and prepare resource arrays --------------- *
#*                                                                                  *
#************************************************************************************

# ---------------- Variables Used for PerfHint Parsing and Verification --->>>>>>>>>
declare -a file_array=()
declare -a hint_resource_list_array=()
declare -a hint_resource_array=()
declare -a hint_id_array=()
declare -a hint_resolution_array=()
declare -a hint_type_array=()
declare -a hint_id_resource_count_map_array=()
declare -a hint_resource_opcode_array=()
declare -a hint_resource_sysfs_node_array=()
declare -a hint_resource_val_array=()
declare -a hint_before_node_val_array=()
declare -a hint_during_node_val_array=()
declare -a hint_after_node_val_array=()
declare -a hint_result=()
let hintOpcodeIndex=0;
let perHintResourceMapCount=0;
#<<<<<------------- Variables Used for PerfHint Parsing and Verification -------------

function getHintResourcesFromConfigFile() {

    while IFS= read -r line # Read a line
    do
        hint_config_line_array+=("$line") # Append line to the array
    done < "$1"

    let my_count=0;
    let hint_counter=0;
    IFS=';';
    for item in ${hint_config_line_array[@]};
    do
        hint_resource_array[my_count]=$item
        ((my_count++))
    done
    hint_resource_array[my_count]="END"
    unset IFS

    index=0

    for item in ${hint_resource_array[@]};
    do
        if [[ $item != "END" ]]
        then
            if [[ $item == *"Id_"* ]]
            then
                if [[ hint_counter -gt 0 ]]
                then
                    hint_id_array[hint_counter]=`(echo $item | cut -d'_' -f2)`
                    ((hint_counter++))
                    hint_id_resource_count_map_array[index]=$perHintResourceMapCount
                    perHintResourceMapCount=0;
                    ((index++))
                elif [[ hint_counter -eq "0" ]]
                then
                    hint_id_array[hint_counter]=`(echo $item | cut -d'_' -f2)`
                    ((hint_counter++))
                fi
             elif [[ $item == *"Type_"* ]]
            then
                hint_type_array[hint_counter-1]=`(echo $item | cut -d'_' -f2)`
            elif [[ $item == *"Resolution_"* ]]
            then
                hint_resolution_array[hint_counter - 1]=`(echo $item | cut -d'_' -f2)`
            elif [[ $item -ge 0x40400000 ]]
            then
                hint_resource_opcode_array[hintOpcodeIndex]=$item
                ((hintOpcodeIndex++))
                ((perHintResourceMapCount++))
            else
                hint_resource_val_array[val_counter]=$item
                ((val_counter++))
            fi
        elif [[ $item == "END" ]]
        then
            echo "";
            hint_id_resource_count_map_array[index]=$perHintResourceMapCount
            break;
        fi
    done

    let sysfs_node_array_counter=0;
    for item in ${hint_resource_opcode_array[@]};
    do
        hint_resource_sysfs_node_array[sysfs_node_array_counter]=$(getSysNodeFromOpCode $item)
        ((sysfs_node_array_counter++))
    done

}

#*******************************************************************
#*                                                                 *
# -----------Pre-Setup params for perfLock Testing --------------- *
#*                                                                 *
#*******************************************************************
function initPerflockTest() {

    fileExists "$PERFLOCK_NATIVE_TEST_EXECUTABLE"
    setStayAwakeOn
    getDeviceNameAndStorageType
    split_resource_array
    check_clusters_and_master_cores
    compute_interactive_governor_nodes
    validate_target_supported_perflocks
}

#*******************************************************************
#*                                                                 *
# -----------Pre-Setup params for perfHint Testing --------------- *
#*                                                                 *
#*******************************************************************
function initPerfhintTest() {

    fileExists "$PERFLOCK_NATIVE_TEST_EXECUTABLE"
    setStayAwakeOn
    getDeviceNameAndStorageType
    split_resource_array
    check_clusters_and_master_cores
    compute_interactive_governor_nodes
    validate_target_supported_perflocks

    fileExists "$BOOST_CONFIG_XML_PARSER_EXECUTABLE"
    sleep 2
    fileExists "$PERFBOOST_CONFIG_XML"
    sleep 2

    #-----------------------------------------------------------------------
    #Use boostconfig-parser to parse perfboostsconfig file.
    #boostconfig-parser will generate perfHintConfig.txt file in the device
    #-----------------------------------------------------------------------
     adb shell "$BOOST_CONFIG_XML_PARSER_EXECUTABLE"

     QLOGI "Pulling perfHintConfig Parsed output file...."
     adb pull "$PERFBOOST_CONFIG_PARSED_OUTPUT_FILE"

    #Remove lines Not containing targetname
    sed -i "/Target_$targetName/!d" "perfHintConfig.txt"

    #To remove target names
    sed -i "s/\;Target_$targetName//g" "perfHintConfig.txt"

    #Remove lines Not matching Device Resolution (Resolution Independent configs must not be removed)
    sed -i "/.*Resolution_$resolution\|.*Resolution_None/!d" "perfHintConfig.txt"

    #---------------------------------------
    #Set Delimiters in perfHintConfig file
    #for further processing
    #---------------------------------------
    sed -i -- 's/\, /\;/g' "perfHintConfig.txt"

    getHintResourcesFromConfigFile "perfHintConfig.txt"
}

#*******************************************************************
#*                                                                 *
# -----------Pre-Setup params for Profile Testing --------------- *
#*                                                                 *
#*******************************************************************
function initProfileTest() {

    let profileIndex=0

    fileExists "$PERFLOCK_NATIVE_TEST_EXECUTABLE"
    fileExists "$PERFLOCK_PROFILE_CONF_FILE_PATH"
    setStayAwakeOn
    getDeviceNameAndStorageType
    split_resource_array
    check_clusters_and_master_cores
    compute_interactive_governor_nodes
    validate_target_supported_perflocks

    FILES=*.conf

    adb shell "ls $PERFLOCK_PROFILE_CONF_FILE_PATH" | tr -d '\r' | xargs -n1 adb pull
    sleep 2

    check_clusters_and_master_cores

    for f in $FILES
    do
        FULL_FILENAME=$f

        profile_id[profileIndex]=$(echo $FULL_FILENAME | tr -dc '0-9')
        profile_name[profileIndex]=$FULL_FILENAME

        extractInfoFromConf $FULL_FILENAME
        ((profileIndex++))
    done
}

#***********************************************
#*                                             *
# --------------- SCRIPT MAIN ---------------- *
#*                                             *
#***********************************************
PERFLOCK_TEST_TYPE=$1
output_filename=$2

rootAndremount

if [[ $PERFLOCK_TEST_TYPE == "--basic" ]]
then
    echo " "
    QLOGI "Preparing to Start PerfLock Basic Tests......"
    echo " "
    sleep 5
    initPerflockTest
    startPerflockBasicTest
    adb reboot
elif [[ $PERFLOCK_TEST_TYPE == "--concurrency" ]]
then
    echo " "
    QLOGI "Preparing to Start PerfLock Concurrency Tests......"
    echo " "
    sleep 5
    initPerflockTest
    startPerflockConcurrencyTest
    adb reboot
elif [[ $PERFLOCK_TEST_TYPE == "--crash" ]]
then
    echo " "
    QLOGI "Preparing to Start PerfLock Crash-Recovery Tests......"
    echo " "
    sleep 5
    initPerflockTest
    startPerflockCrashTest
    adb reboot
elif [[ $PERFLOCK_TEST_TYPE == "--hint" ]]
then
    echo " "
    QLOGI "Preparing to Start PerfHint Tests......"
    echo " "
    sleep 5
    initPerfhintTest
    startPerflockHintTest

    #Clean-up files pulled / generated
    rm -rf perfHintConfig.txt
    adb shell "rm -rf $PERFBOOST_CONFIG_PARSED_OUTPUT_FILE"
    adb reboot
elif [[ $PERFLOCK_TEST_TYPE == "--profile" ]]
then
    echo " "
    QLOGI "Preparing to Start Perf Profile Tests......"
    echo " "
    sleep 5
    initProfileTest
    startPerflockProfileTest

    #Clean-up files pulled / generated
    rm -rf target-load*
    adb reboot
else
    echo " "
    QLOGE "Invalid Test type..... "
    echo " "
    QLOGI "Usage : ./perflocktest <TestType> <Output Filename>"
    QLOGI "         --basic basic.html"
    QLOGI "         --concurrency concurrent.html"
    QLOGI "         --crash crash.html"
    QLOGI "         --hint hint.html"
    QLOGI "         --profile profile.html"
    echo " "
    exit 1
fi

#: <<'END'
