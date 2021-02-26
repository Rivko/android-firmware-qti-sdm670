#
# Copyright (c) 2016, Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#

if [[ -z ${1} ]] ; then
    echo "usage: VASim_test.sh [tag]"
#    echo "hi"
    exit
fi

printError()
{
    echo "[${printf_prefix}] error: ${1}" | tee -a ${outputLog}
}

printThis()
{
    echo "[${printf_prefix}] ${1}" | tee -a ${outputLog}
}

printThisWithDate()
{
    #if [[ -n ${1} ]] ; then
    echo "[${printf_prefix}] [`date +"%Y/%m/%d %H:%M:%S"`] ${1}" | tee -a ${outputLog}
}

###################################################
# read the settings - START

VASim_Dir="/usr/data/VAM"
if [[ ! -e "${VASim_Dir}/VASim_test.config" ]] ; then
    printError "VASim_test.config not exists"
    exit
fi
source "${VASim_Dir}/VASim_test.config"

targetNameTag=$1
printf_prefix="VASim_test"

if [[ ! ${Enable_DryRun_Only} = "enable" ]] ; then
    mkdir -p -- "${VASim_Output_Dir}/log/${targetNameTag}" && touch -- "${VASim_Output_Dir}/log/${targetNameTag}/VASim_test.log"
    outputLog="${VASim_Output_Dir}/log/${targetNameTag}/VASim_test.log"
fi

if [[ ! -e ${VASim_Dir} ]] ; then
    printError "VASim dir is not set."
    exit
fi

if [[ -z ${VASim_Config} || ! -e ${VASim_Config} ]] ; then
    printError "VASim config file \"${VASim_Config}\" not exist!"
    exit
fi

if [[ -z ${VASim_File_Input} || ! -e ${VASim_File_Input} ]] ; then
    printError  "input file list \"${VASim_File_Input}\" not exist!"
    exit
fi

if [[ -z ${VASim_Output_Dir} ]] ; then
    printError "VASim output dir is not set"
    exit
fi

# copy input file list & VASim config
printThis "cp ${VASim_File_Input} ${VASim_Output_Dir}/log/${targetNameTag}/"
printThis "cp ${VASim_Config} ${VASim_Output_Dir}/log/${targetNameTag}/"
if [[ ! ${Enable_DryRun_Only} = "enable" ]] ; then
    cp ${VASim_File_Input} ${VASim_Output_Dir}/log/${targetNameTag}/
    cp ${VASim_Config} ${VASim_Output_Dir}/log/${targetNameTag}/
fi

# copy enrollment file if needed
if [[ ${Enable_FR} == "enable" ]] ; then
    printThis "cp ${VASim_FR_Enrollment_File} ${VASim_Output_Dir}/log/${targetNameTag}/"
   if [[ ! ${Enable_DryRun_Only} = "enable" ]] ; then
        cp ${VASim_FR_Enrollment_File} ${VASim_Output_Dir}/log/${targetNameTag}/
    fi
fi

printThisWithDate "VASim_test.sh begins."
printThis "==============================="
printThis "  VASim           : ${Enable_VASim}"
printThis "  VAMReport       : ${Enable_VAMReport}"
printThis "  Dry Run         : ${Enable_DryRun_Only}"
printThis "  name tag        : ${targetNameTag}"
printThis "  VASim dir       : ${VASim_Dir}"
printThis "  VASim config    : ${VASim_Config}"
printThis "  file input list : ${VASim_File_Input}"
printThis "  output dir      : ${VASim_Output_Dir}"
printThis "==============================="

# read the settings - END
###################################################

getParamFromTheLine()
{
    targetVideoID=""
    targetVideoFile=""
    targetJSONRuleConfig=""
    grounTruthFilePrefix=""

#    let num=$(echo "${1}"|tr -dc ','|wc -c)+1

    for idx in `seq 1 4` ; do
        #echo "idx: ${idx}"
        arg=$(echo ${1}|cut -d',' -f${idx}|sed 's/^[[:blank:]]//'|sed 's/[[:blank:]]$//')

        if [[ ${idx} == "1" ]] ; then
            targetVideoID="${arg}";    fi
        if [[ ${idx} == "2" ]] ; then
            targetVideoFile="${VASim_Video_Input_Folder}/${arg}"; fi
        if [[ ${idx} == "3" ]] ; then
	    ${dos2UnixBin} ${VASim_VAM_Rule_Input_Folder}/${arg}
            targetJSONRuleConfig="${VASim_VAM_Rule_Input_Folder}/${arg}";  fi
        if [[ ${idx} == "4" ]] ; then
            grounTruthFilePrefix="${VASim_Ground_Truth_Input_Folder}/${arg}";  fi
    done
}

doVASim()
{
    if [ ! -e ${targetVideoFile} ] ; then
        printError "video file ${targetVideoFile} not found!"
        return
    fi

    if [ ! -e ${targetJSONRuleConfig} ] ; then
        printError "rule config file ${targetJSONRuleConfig} not found!"
        return
    fi

    ############################################
    # create folders
    ############################################
    outputFolder=${VASim_Output_Dir}/${targetVideoID}/${targetNameTag}
    if [[ ! ${Enable_DryRun_Only} = "enable" ]] ; then
        mkdir -p ${outputFolder}/metadata/
        mkdir -p ${outputFolder}/event/
        mkdir -p ${outputFolder}/log/
        mkdir -p ${outputFolder}/input/
    fi

    ############################################
    # do VASim
    ############################################
    printThisWithDate "VASim begins. "
    printThis "=============================="
    printThis "  video renderer  : ${Enable_Renderer}"
    printThis "  snapshot        : ${Enable_Snapshot}"
    printThis "  target video    : ${targetVideoFile}"
    printThis "  rule config     : ${targetJSONRuleConfig}"
    printThis "  output metadata : ${outputFolder}/metadata/"
    printThis "  output event    : ${outputFolder}/event/"
    printThis "  engine data     : /usr/lib64/engine_libs"
    printThis "  log             : ${outputFolder}/log/VASim.log"
    printThis "=============================="

    printThis "cp ${targetJSONRuleConfig} ${outputFolder}/input/"
    if [[ ! ${Enable_DryRun_Only} = "enable" ]] ; then
        cp ${targetJSONRuleConfig} ${outputFolder}/input/
    fi

    # export the library path
    export LD_LIBRARY_PATH=/usr/lib64
    if [[ ${Enable_DryRun_Only} = "enable" ]] ; then
        printThis " ${VASim_Dir}/bin/VASim -c ${VASim_Config} -i ${targetVideoFile} -r ${targetJSONRuleConfig} -mo ${outputFolder}/metadata/ -eo ${outputFolder}/event/ -dl /usr/lib/vam_engines -dd ${VASim_Dir}/engine_data --display=\"${Enable_Renderer}\" --snapshot=\"${Enable_Snapshot}\" --enableFR=\"${Enable_FR}\" --filmStripNum ${FilmStripNum} --FREnrollmentFileName \"${VASim_FR_Enrollment_File}\" --FREnrollmentFolder \"${VASim_FR_Enrollment_Folder}\" \
        2>&1 | tee ${outputFolder}/log/VASim.log"
    else
        echo "[${printf_prefix}] [`date +"%Y/%m/%d %H:%M:%S"`] VASim Begin." >> ${outputFolder}/log/VASim.log
        /usr/bin/VASim -c ${VASim_Config}\
	 -i ${targetVideoFile}\
	 -r ${targetJSONRuleConfig}\
	 -mo ${outputFolder}/metadata/\
	 -eo ${outputFolder}/event/\
	 -dl /usr/lib/vam_engines/\
	 -dd ${VASim_Dir}/engine_data/\
	 --display="${Enable_Renderer}"\
	 --snapshot="${Enable_Snapshot}"\
	 --enableFR="${Enable_FR}"\
	 --filmStripNum ${FilmStripNum}\
	 --FREnrollmentFileName "${VASim_FR_Enrollment_File}"\
	 --FREnrollmentFolder "${VASim_FR_Enrollment_Folder}" \
         2>&1 | tee ${outputFolder}/log/VASim.log
        echo printThis "[${printf_prefix}] [`date +"%Y/%m/%d %H:%M:%S"`] VASim finished." >> ${outputFolder}/log/VASim.log
    fi
    printThisWithDate "VASim finished."
}

doVAMReport()
{
    ############################################
    # create folders
    ############################################
    outputFolder=${VASim_Output_Dir}/${targetVideoID}/${targetNameTag}
    if [[ ! ${Enable_DryRun_Only} = "enable" ]] ; then
        mkdir -p ${outputFolder}/report/
        mkdir -p ${outputFolder}/log/
    fi

    ############################################
    # do VAMReport
    ############################################

    export LD_LIBRARY_PATH=/usr/lib64

    if [[ -n ${grounTruthFilePrefix} ]] ; then
        printThisWithDate "VAMReport begins."
        printThis "=============================="
        printThis "  ground truth  : ${grounTruthFilePrefix}_"
        printThis "  output report : ${outputFolder}/report/report*"
        printThis "  log           : ${outputFolder}/log/VASimReport.log"
        printThis "=============================="
        ${dos2UnixBin} ${grounTruthFilePrefix}*.idx
        ${dos2UnixBin} ${grounTruthFilePrefix}*.json*

        if [[ ${Enable_DryRun_Only} = "enable" ]] ; then
            printThis "${VASim_Dir}/bin/VAMReport ${grounTruthFilePrefix} ${outputFolder}/metadata/ 0 0 1 ${outputFolder}/report/report 200 0 2>&1 | tee ${outputFolder}/log/VASimReport.log"
        else
            echo "[${printf_prefix}] [`date +"%Y/%m/%d %H:%M:%S"`] VAMReport begin." >> ${outputFolder}/log/VAMReport.log
            /usr/bin/VAMReport ${grounTruthFilePrefix} ${outputFolder}/metadata/ 0 0 1 ${outputFolder}/report/report 200 0 2>&1 | tee -a ${outputFolder}/log/VAMReport.log
            echo "[${printf_prefix}] [`date +"%Y/%m/%d %H:%M:%S"`] VAMReport finished." >> ${outputFolder}/log/VAMReport.log
        fi
        printThisWithDate "VAMReport finished."
    else
        printError "ground truth file ${grounTruthFilePrefix} not exist"
    fi
}

# use dos2unix or fromdos
dos2UnixBin="dos2unix"
#if [[ -n `which fromdos` ]]; then
#    dos2UnixBin="fromdos"
#fi
${dos2UnixBin} ${VASim_File_Input}

printThis ""

maxLoops=1
if [[ ${Enable_Loop} = "enable" ]] ; then
    maxLoops=1000
    originalNameTag=${targetNameTag}
fi

for loopIDX in `seq 1 ${maxLoops}` ; do

    if [[ ${Enable_Loop} = "enable" ]] ; then
        printThisWithDate "[${loopIDX}th looping begins]"
        targetNameTag="${originalNameTag}-${loopIDX}"
    fi

    # looping through file list input

    while read -r line
    do
        if [[ -z ${line} ]]; then
            continue
        fi

        if [[ ${isHeaderLine} == "true" ]] ; then
            # first line is header
            isHeaderLine="false"
            continue
        fi

        getParamFromTheLine "${line}"

        if [[ ${Enable_VASim} = "enable" ]] ; then
            doVASim
            printThis ""
        fi

        if [[ ${Enable_VAMReport} = "enable" ]] ; then
            doVAMReport
            printThis ""
        fi

    done < "${VASim_File_Input}"

    if [[ ${Enable_Loop} = "enable" ]] ; then
        printThisWithDate "[${loopIDX}th looping ended]"
    fi

done

printThisWithDate "VASim_test.sh finished."

#for D in $(find $dir -maxdepth 1 -type d -exec basename '{}' \;) ; do
#for D in $(less fileInput.txt) ; do
#    let idx=$(($idx + 1));
#    echo "${idx}: $D..."
#done
# ls -d -1 $PWD/../VASim/*


