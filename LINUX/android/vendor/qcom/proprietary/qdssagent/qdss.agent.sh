#=============================================================================
# Copyright (c) 2014-2016,2018 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#=============================================================================

HERE=/mnt/vendor/persist/coresight
ENABLE_FILE=$HERE/enable

# check if qdss agent is enabled
if [ ! -f $ENABLE_FILE ]
then
    ENABLE=1
else
    ENABLE=`cat $ENABLE_FILE`
fi

if [ $ENABLE -ne 1 ]
then
    exit
fi

QDSS_AGENT_COMPLETED=`getprop ro.vendor.dbg.coresight.cfg_file`

# Choose config file based upon given event name
CONFIG_FILE=$HERE/coresight.config.on.boot.txt
LOG_FILE=$HERE/qdss.log

# Check if running during early-boot.
if [ $1 == "early-boot" ]
then
    if [ -f $CONFIG_FILE ]
    then
        # Delay execution of user config script till boot completed.
        exit
    fi
    CONFIG_FILE=$2
elif [ $1 == "post-boot" ]
then
    if [ ! -f $CONFIG_FILE ]
    then
        #use external param so we can exec different script post-boot if needed
        source $2
        exit
    fi
fi

# Prevent running qdss agent multiple times.
if [ ! -z $QDSS_AGENT_COMPLETED ]
then
    exit
fi

if [ -f $CONFIG_FILE ]
then
    if [ -f $LOG_FILE ]
    then
            mv $LOG_FILE  $LOG_FILE.old
    fi
    # If the file exists... execute the agent
    echo "START: $CONFIG_FILE" > $LOG_FILE
    chmod 0664 $LOG_FILE
    /system/bin/sh $HERE/qdss.config.sh $CONFIG_FILE  >> $LOG_FILE 2>&1
    echo "QDSS Is Configured" > /dev/coresight-stm
    setprop ro.vendor.dbg.coresight.cfg_file $CONFIG_FILE
fi
