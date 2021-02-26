#=============================================================================
# Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#=============================================================================

#set USE_QMI=true when QMI service is up
USE_QMI=true
export USE_QMI

#========================================
# This "TOP_PID is important, here is why:
# If inside a function we have an error we want to die.
# Problem: Functions are 'pseudo-sub-processes'
# Thus, we need to kill the *PARENT* process
# We record that here in TOP_PID
export TOP_PID=$$
# Then "on-exit" we kill our parent process.
trap "exit 1" TERM
#----------------------------------------
# And this is our failure function that
# actually kills our parent process.
function failure() {
	msg="$0: $1"
	echo "Error: $msg"
	kill -s TERM $TOP_PID
	exit 1
}
#========================================

#Comment this for testing QMI
SENDDATA=/system/vendor/bin/testapp_diag_senddata
QMI_HELPER=/system/vendor/bin/qdss_qmi_helper

# these may have been "adb pushed" and not marked as exe
# so we mark these as executable
# (not security risk because this is a debug environment)

echo "USE_QMI=$USE_QMI"

case x"$USE_QMI" in
    x"true")
	echo "QMI based configuration"
	;;
    x"false")
	echo "DIAG based configuration"
	;;
    x"")
	failure "please set USE_QMI to true or false"
	;;
    *)
	failure "please set USE_QMI to true or false"
	;;
esac

function ash_tolower() {

    # I wish ash (or android) had a tolower or tr 
    # but it does not, so we do this by hand
    name=wrong
    case  x"$1" in
	x"MODEM")
	    name="modem"
	    ;;
	*)
	    # Assume it is ok
	    name="$1"
	    ;;
    esac
    echo $name
}


# Execute a command and test its error exit value
function execute() {
	msg="$1"
	cmd="$2"
	echo "$msg"
	echo "$cmd"
    $cmd
	if [ $? != 0 ]
	then
		echo ; echo ; echo ; echo
		echo "================"
		echo "FAIL: $cmd"
		echo "================"
		echo ; echo ; echo ; echo
		failure "Line: $LINENO See above"
	fi
}

function info () {
    # We ignore this command
    # We have means to verify anything
    # A real agent .. would dothe right thing here
}

function sink () {
    if [ x"$1" != x"set" ]
    then
	failure "Line: $LINENO sink $1 not supported"
	exit 1
    fi

target=`getprop ro.board.platform`
	case $target in
	"sdm845")
	    case x"$2" in
		x"ETB")
		    echo 1 > /sys/bus/coresight/reset_source_sink
		    echo 1   > /sys/bus/coresight/devices/coresight-tmc-etf/enable_sink
		    ;;
		x"DDR")
		    echo 1 > /sys/bus/coresight/reset_source_sink
		    echo mem > /sys/bus/coresight/devices/coresight-tmc-etr/out_mode
		    echo 1   > /sys/bus/coresight/devices/coresight-tmc-etr/enable_sink
		    ;;
		x"USB")
		    echo 1 > /sys/bus/coresight/reset_source_sink
		    echo 1   > /sys/bus/coresight/devices/coresight-tmc-etr/enable_sink
		    echo usb > /sys/bus/coresight/devices/coresight-tmc-etr/out_mode
		    ;;
	        *)
		    failure "Line: $LINENO sink $2 not supported"
		    exit 1
		    ;;
	esac ;;
	*)
            case x"$2" in
		x"ETB")
		    echo 1   > /sys/bus/coresight/devices/coresight-tmc-etf/curr_sink
		    ;;
		x"DDR")
		    echo 1   > /sys/bus/coresight/devices/coresight-tmc-etr/curr_sink
		    echo mem > /sys/bus/coresight/devices/coresight-tmc-etr/out_mode
		    ;;
		x"USB")
		    echo 1   > /sys/bus/coresight/devices/coresight-tmc-etr/curr_sink
		    echo usb > /sys/bus/coresight/devices/coresight-tmc-etr/out_mode
		    ;;
		*)
		    failure "Line: $LINENO sink $2 not supported"
		    exit 1
		;;

	esac ;;
    esac
}


function swt_apps() {
	area=$1
	thing=$2
	
	echo 1 > /sys/kernel/debug/tracing/events/$area/$thing/enable
}

function swt_rpm() {
	group=$1
	value=$2

	if [ x"$USE_QMI" == x"true" ]
	then
	    execute "Configure RPM events (qmi)" \
		"$QMI_HELPER modem swe_rpm 1 $group $value"
	else 
	    execute "Configure RPM events (diag)" \
		"$SENDDATA 75 90 0x0c 0x02 1 $group -u64 $value"
	fi
}


    
function swt_modem_enable() 
{
    #     swt "modem" enable "EVENT" id MODEM_CLOCK_EVENT_CLOCK_FREQ
    # or  swt "modem" enable "EVENT" id 81
    #      0     1      2       3     4  5
    
    # Create our variable name
    VARNAME=${1}_${5}
    value=$(($VARNAME+0))
    if [ x"$value" == x"0" ]
    then
	value=$5
    fi

    if [ x"${value}" == x"" ]
    then
	# The variable exists, return the value
	failure "Line: $LINENO, $VARNAME does not exist"
    fi


    #     swt "modem" enable "EVENT" id 81 # CLOCK_EVENT_CLOCK_FREQ
    # OR  swt "modem" enable "EVENT" id MODEM_CLOCK_EVENT_CLOCK_FREQ
    if [ x"$USE_QMI" == x"true" ]
    then
	execute "Configure swevent (qmi)" \
	    "$QMI_HELPER $1 swe enable $value"
    else
	execute "Configure modem event $1" \
	    "$SENDDATA 75 90 0x0b 0x02 0x01 -u32 $value 0x00 0x00"
    fi
}

function swt_remember_event_id() {
    # 1 = subsystem name, ie:  "MODEM"
    # 2 = variable name
    # 3 = numeric value
    # All we do is create a global shell var 
    # based on 1 and 2
    # Normalize case:

    name=`ash_tolower $1`

    #echo "SET: ${1}_${2}=${3}"
    eval ${1}_${2}=${3}
}


function swt_modem() {

    #echo "swt_modem 1=$1, 2=$2, 3=$3, 4=$4,5=$5"
    
    # Parameter 3 may not be present.
    if [ x"$3" == x"" ]
    then
	x=_NOTHING_
    else
	x="$3"
    fi
    
    case x"$x" in
	x"_NOTHING_")
            # Global enable for all SW events, 0x06 = turn on, 0x02 is dest(modem)
	    if [ x"$USE_QMI" == x"true" ]
	    then
		execute "Enable all $1 SW events" \
		    "$QMI_HELPER $1 swe_all enable"
	    else 
		execute "Enable all MODEM SW Events"  \
		    "$SENDDATA 75 90 0x06 0x02 1"
	    fi
	    ;;
	x"EVENT")
	    # swt "modem" enable "EVENT" id 81 # CLOCK_EVENT_CLOCK_FREQ
	    #  0    1      2       3      4 5
	    if [ x"$2" == x"enable" ]
	    then
		swt_modem_enable $1 $2 $3 $4 $5
	    else
		failure "Line: $LINENO swt not supported: $1 $2 $3 $4 $5" 
	    fi
	    ;;
	x"event")
	    # swt "MODEM" info "event" id 1 ADSPPM_CLK_SET "clk set,clkId=%d reqfreq=%d grantedfreq=%d" 0
	    #  0    1       2     3     4 5   6                7                                        8
	    swt_remember_event_id $1  $6 $5
	    ;;
	x"TDS")
	    # Ignored
	    ;;
	x"ULOG")
	    # ignored
	    ;;
	x"DIAG")
	    # ignored
	    ;;
	*)
	    failure "Line: $LINENO modem unknown: $1"
    esac
	# Fix me - if event id is > 255 we need to break it up
}

function swt_all(){
	
	case x"$2" in
	x"reset")
       case x"$1" in
         x"modem")
		    if [ x"$USE_QMI" == x"true" ]
		    then
		       #execute "Reset all sw entites" "$QMI_HELPER $1 entity_reset" 
		       execute "Reset all modem sw events" \
			   "$QMI_HELPER $1 swe_all disable" 
		    else
		       execute "Reset all modem entities" \
			   "$SENDDATA 75 90 0x08 0x02 0 0x0ff"
		        execute "Reset all modem events" \
			   "$SENDDATA 75 90 0x0b 0x02 0 -u32 0xffffffff 0x00 0x00" 
		    fi
		    ;;
        x"apps")
            echo "Disable Linux ftrace"
            echo "echo 0 > /sys/kernel/debug/tracing/events/enable"
            echo 0 > /sys/kernel/debug/tracing/events/enable
		# FIXME:
		#   Future: add other subsystems, adsp, wcns, etc            
            ;;
	    *)
		    failure "Line: $LINENO swt_all() unknown 1=$1 2=$2 3=$3"
       esac
         ;;
       *)
         failure "Line: $LINENO swt_all() unknown 1=$1 2=$2 3=$3"
	esac
}

function swt_info() {
    # This is a version check
    # We are a simple parser so we do simple checks
    if [ x"$2" != x"format" ]
    then
	failure "Line: $LINENO not supported: swt $1 $2 $3 $4"
    fi
    if [ x"$3" != x"1" ]
    then
	failure "Line: $LINENO Unsupported event id definition file"
    fi
}


function swt () {
    # handles all swt commands.
    
    #echo "SWT: 1=$1, 2=$2, 3=$3 4=$4, 5=$5"

    name=`ash_tolower $1`

    case x"$name" in
	x"info")
	    swt_info $name $2 $3
	    ;;
	x"all")
	    echo "FIXME: This is hard coded for modem only today"
		swt_all apps $2 $3
        swt_all modem $2 $3
	    # Ignored
	    ;;
	x"apps")
	    swt_apps $5 $7
	    ;;
	x"wcn")
	    if [ x"$USE_QMI" == x"false" ]
	    then
		failure "WCN does not support DIAG configuration!"
	    else
		# reuse the modem code
		swt_modem $name $2 $3 $4 $5 $6
	    fi
	    ;;
	x"adsp")
	    if [ x"$USE_QMI" == x"false" ]
	    then
		failure "ADSP does not support DIAG configuration!"
	    else
		# reuse the modem code
		swt_modem $name $2 $3 $4 $5 $6
	    fi
	    ;;
	x"sensor")
	    if [ x"$USE_QMI" == x"false" ]
	    then
		failure "SENSOR does not support DIAG configuration!"
	    else
		swt_modem $name $2 $3 $4 $5 $6
	    fi
	    ;;
	x"modem")
	    # For SW EVENTS it is this
	    # swt_modem $5
	    swt_modem $name $2 $3 $4 $5 $6
	    ;;
	x"rpm")
	    swt_rpm $7 $9 
	    ;;
	x"info")
	    swt_info $name $2
	    ;;
	*)
	    failure "Line: $LINENO, not supported: swt $name $2 $3 $4 $5 $6 "
	    exit 1
	    ;;
    esac
   sleep 0.2
}

function hwe_modem() {
    # we cannot access modem registers
    # we must proxy the modem registers via the modem
    if [ x"$USE_QMI" == x"true" ]
    then
	execute "Configure $1 HW EVENT $2 $3" \
	    "$QMI_HELPER $1 hwe $2 $3"
    else
	execute "Configure $1 HW Event $2 $3" \
	    "$SENDDATA 75 90 0x31 0x02 -u32 $2 -u32 $3"
    fi
}

function hwe () {
    
    # Handle hardware event requests.

    case x"$1" in
	x"set")
	    # Ignored
	    ;;
	x"enable")
        echo "Enable STM HW events"
        echo "echo 1 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable"
        echo 1 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable

	    echo "Enable HW events"
        echo "echo 1 > /sys/bus/coresight/devices/coresight-hwevent/enable"
        echo 1 > /sys/bus/coresight/devices/coresight-hwevent/enable
	    ;;
	x"disable")
	    echo "Disable HW events" 
        echo "echo 0 > /sys/bus/coresight/devices/coresight-hwevent/enable"
        echo 0 > /sys/bus/coresight/devices/coresight-hwevent/enable
	    
        echo "Disable STM HW events" 
        echo "echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable"	    
        echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable
	    ;;
	x"apps")
	    # hwe "apps" setreg addr 0x0086cfb0 value 0xc5acce55
	    #  0   1       2     3     4           5  6
	    echo "echo $4 $6 > /sys/bus/coresight/devices/coresight-hwevent/setreg"
        echo $4 $6 > /sys/bus/coresight/devices/coresight-hwevent/setreg
	    ;;
	x"modem")
	    # hwe "modem" setreg addr 0x0408200c value 0x1
	    #  0     1      2     3     4         5    6
	    hwe_modem $1 $4 $6
	    ;;
	x"wcn")
	    # hwe "modem" setreg addr 0x0408200c value 0x1
	    #  0     1      2     3     4         5    6
	    if [ x"$USE_QMI" == x"false" ]
	    then
		failure "WCN does not support DIAG configuration!"
	    else
		hwe_modem $1 $4 $6
	    fi
	    ;;
	x"adsp")
	    # hwe "modem" setreg addr 0x0408200c value 0x1
	    #  0     1      2     3     4         5    6
	    if [ x"$USE_QMI" == x"false" ]
	    then
		failure "WCN does not support DIAG configuration!"
	    else
		hwe_modem $1 $4 $6
	    fi
	    ;;
	*)
	    failure "Line: $LINENO HWE for $1 is not supported"
	    exit 1
    esac
}

function stm () {

target=`getprop ro.board.platform`
	case $target in
	"sdm845")
	    case x"$1" in
		x"enable")
		    echo 1 > /sys/bus/coresight/devices/coresight-stm/enable_source
		    ;;
		x"disable")
		    echo 0 > /sys/bus/coresight/devices/coresight-stm/enable_source
		    ;;
		*)
		    failure "Line: $LINENO Unknown stm command $1"
		    ;;
	    esac
	    ;;

	*)
	    case x"$1" in
		x"enable")
		    echo 1 > /sys/bus/coresight/devices/coresight-stm/enable
		    ;;
		x"disable")
		    echo 0 > /sys/bus/coresight/devices/coresight-stm/enable
		    ;;
		*)
		    failure "Line: $LINENO Unknown stm command $1"
		    ;;
	    esac
	    ;;
    esac
}

function etm () {

    if [ x"$2" != x"enable" ]
    then
	failure "Line: $LINENO Only enable is supported"
    fi

    # Fix me: How will these be documented?
    case x"$1" in
	x"adsp")
	    echo 1 > /sys/bus/coresight/devices/coresight-audio-etm0/enable
            execute "QMI command ADSP ETM" "$QMI_HELPER adsp etm enable"
	    ;;
	x"modem")
	    echo 1 > /sys/bus/coresight/devices/coresight-modem-etm0/enable
	    ;;
        x"rpm")
            echo 1 > /sys/bus/coresight/devices/coresight-rpm-etm0/enable
            execute "QMI cmd rpm etm on via modem" "$QMI_HELPER modem etm_rpm enable"
            #FIX me: temporary work around. Need QMI API to send data
            #execute "Diag command to enable RPM" "$SENDDATA 0x4B 0x5A 0x40 0x05 0x01"
            ;;
        x"apps")
        #Enables ETM tracing from all CPU cores of APPS
        #Fix me: Need definition of debug agent commands for granular control of APPS ETM with attributes
            stop mpdecision
            echo 1 > /sys/devices/system/cpu/cpu1/online
            echo 1 > /sys/devices/system/cpu/cpu2/online
            echo 1 > /sys/devices/system/cpu/cpu3/online
            echo 1 > /sys/bus/coresight/devices/coresight-etm0/enable
            echo 1 > /sys/bus/coresight/devices/coresight-etm1/enable
            echo 1 > /sys/bus/coresight/devices/coresight-etm2/enable
            echo 1 > /sys/bus/coresight/devices/coresight-etm3/enable
            start mpdecision
            ;;
	*)
	    failure "Line: $LINENO $1 not supported"
	    ;;
    esac
}


function fix_cti_name() {
    
    # AGH... some things come as caps
    # we need them lower case! GRR...
    # We need a 'tolower' command in ash
    #===
    # Sigh ... we must do this by hand.
    #===
    case x"$2" in
	x"CTI0")
	    name=cti0
	    ;;
	x"CTI1")
	    name=cti1
	    ;;
	x"CTI2")
	    name=cti2
	    ;;
	x"CTI3")
	    name=cti3
	    ;;
	x"CTI4")
	    name=cti4
	    ;;
	x"CTI5")
	    name=cti5
	    ;;
	x"CTI6")
	    name=cti6
	    ;;
	x"CTI7")
	    name=cti7
	    ;;
	x"CTI8")
	    name=cti8
	    ;;
	x"CPU0")
	    name=cti-cpu0
	    ;;
	x"CPU1")
	    name=cti-cpu1
	    ;;
	x"CPU2")
	    name=cti-cpu2
	    ;;
	x"CPU3")
	    name=cti-cpu3
	    ;;
        x"CPU4")
	    name=cti-cpu4
	    ;;
        x"CPU5")
	    name=cti-cpu5
	    ;;
        x"CPU6")
	    name=cti-cpu6
	    ;;
        x"CPU7")
	    name=cti-cpu7
	    ;;
	x"MODEM-CPU0")
	    name=cti-modem-cpu0
	    ;;
        x"AUDIO-CPU0")
            name=cti-audio-cpu0
            ;;
	x"RPM-CPU0")
	    name=cti-rpm-cpu0
	    ;;
	x"VIDEO-CPU0")
	    name=cti-video-cpu0
	    ;;
	x"WCN-CPU0")
	    name=cti-wcn-cpu0
	    ;;
	*)
	    failure "Line: $LINENO Unsupported cti name: $2"
	    ;;
    esac
    # convert this to the coresight device name
    result=/sys/bus/coresight/devices/coresight-$name
    # Make sure it exists!
    if [ ! -d $result ]
    then
	failure "Line: $LINENO Does not exist: $result"
    fi
    echo $result
}
	    
function cti_reset() {
    
    # cti reset block "VIDEO-CPU0"
    #  0   1     2      3
    sleep 0.2
    if [ x"$2" != x"block" ]
    then
	failure "Line: $LINENO Unknown cti reset request"
    fi

    name=`fix_cti_name cti $3`
    #FIX ME:execute function containing arguments with spaces does not work
    #cmd_string="echo 1 > $name/reset"
    #execute "Perform CTI reset" "${cmd_string}"
    
    echo "Perform CTI reset"  
    echo "echo 1 > $name/reset"
    echo 1 > $name/reset
}

function cti_map() {
    # cti map block "CTI0" type "trigin" trigger 0 channel 0
    #  0   1   2     3      4     5        6     7   8     9
    sleep 0.2
    if [ x"$2" != x"block" ]
    then
	failure "Line: $LINENO cti map not supported"
    fi
    name=`fix_cti_name cti $3`
    #FIX ME:execute function containing arguments with spaces does not work
    echo "Configure CTI $3, $5  ..." 
    echo "echo $7 $9 > $name/map_$5"
    echo $7 $9 > $name/map_$5
}


function cti () {
    # Handle all cross trigger requests
    case x"$1" in
	x"reset")
	    cti_reset $1 $2 $3
	    ;;
	x"map")
	    # cti map block "A53-0" type "trigin" trigger 0 channel 0
	    cti_map $1 $2 $3 $4 $5 $6 $7 $8 $9
	    ;;
	*)
	    failure "Line: $LINENO cti unknown: $1 $2 $3 $4 ..."
    esac
}
	    

function ping_modem() {
	execute "Ping the modem with Hi Mom!" \
		"$SENDDATA 75 18 41 0x48 0x69 0x20 0x4d 0x6f 0x6d 0x21"
	# 48 = H
	# 69 = i
	# 20 = space
	# 4d = M
	# 6f = o
	# 6d = m
	# 21 = !
}

function modem_error_fatal() {
    execute "Modem ERROR Fatal"        "$SENDDATA 75 37 03 00"
}

function modem_wdog_fatal() {
    execute "Modem Watchdog Fatal"     "$SENDDATA 75 37 03 00 01"
}

function modem_nullpointer_fatal() {
    execute "Modem Null Pointer Fatal" "$SENDDATA 75 37 03 00 02"
}

function apps_kernel_panic () {
    echo c > /proc/sysrq-trigger
}


    

# Not possible from diag consumer interfae.
#function wcnss_error_fatal() {
#    execute "Wcnss ERROR Fatal"        "$SENDDATA 75 37 03 32"
#}

# Not possible from diag consumer interfae.
# function wcnss_wdog_fatal() {
#    execute "Wcnss Watchdog Fatal"     "$SENDDATA 75 37 03 32 01"
# }

# Not possible from diag consumer interfae.
# function wcnss_nullpointer_fatal() {
#    execute "Wcnss Null Pointer Fatal" "$SENDDATA 75 37 03 32 02"
# }

# Not possible from diag consumer interfae.
# function adsps_error_fatal() {
#     execute "Adsps ERROR Fatal"        "$SENDDATA 75 37 03 48"
# }

# Not possible from diag consumer interfae.
# function adsps_wdog_fatal() {
#     execute "Adsps Watchdog Fatal"     "$SENDDATA 75 37 03 48 01"
# }

# Not possible from diag consumer interfae.
# function adsps_nullpointer_fatal() {
#     execute "Adsps Null Pointer Fatal" "$SENDDATA 75 37 03 48 02"
# }

function get_qdss_capture_device() {
    IS_DDR=`cat /sys/bus/coresight/devices/coresight-tmc-etr/curr_sink`
    DDR_MODE=`cat /sys/bus/coresight/devices/coresight-tmc-etr/out_mode`
    IS_ETB=`cat /sys/bus/coresight/devices/coresight-tmc-etf/curr_sink`

    QDSS_CAPTURE_HUMAN=other
    if [ x"$IS_DDR.$DDR_MODE" == x"1.mem" ]
    then
	QDSS_CAPTURE_HUMAN="DDR"
	QDSS_CAPTURE_DEVICE=/dev/coresight-tmc-etr
    fi

    if [ x"$IS_DDR.$DDR_MODE" == x"1.usb" ]
    then
	QDSS_CAPTURE_HUMAN="USB"
	QDSS_CAPTURE_DEVICE=/dev/coresight-tmc-etr
    fi

    if [ x"$IS_ETB" == x"1" ]
    then
	QDSS_CAPTURE_HUMAN="ETB"
	QDSS_CAPTURE_DEVICE=/dev/coresight-tmc-etf
    fi

}

function qdss_capture_device_name() {

    # Always reget, it may have changed
    get_qdss_capture_device
    echo $QDSS_CAPTURE_DEVICE
}

function qdss_capture_human_name() {
    
    # Always reget, it may have changed
    get_qdss_capture_device
    echo $QDSS_CAPTURE_HUMAN
}
