# Copyright (c) 2016 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

#!/bin/bash

REGIONAL_SOURCE_PATH="vendor/qcom/proprietary/qrdplus/Global/Regional"
REGIONALSP_SOURCE_PATH="vendor/qcom/proprietary/qrdplus/Global"

# Job only for oem partition
# Can modify below lists to add more jobs for other partitions
MIN_OEMIMAGE_PARTITION_SIZE=268435456  # The minimum oem partition size is 256M
PresetPackList1=( TMO/oem MPCS/oem )
PresetPackList2=( LatamAMX/oem LatamTelcelMexico/oem LanixTelcelMexico/oem )
PresetPackList3=( LatamAMX/oem LatamClaroBrazil/oem LatamClaroPeru/oem LanixClaroColombia/oem LatamClaroChile/oem LatamClaroColombia/oem )
PresetPackList4=( RJIL/oem IndiaCommon/oem EUCommon/oem OrangeCommon/oem OrangeFrance/oem OrangePoland/oem OrangeRomania/oem OrangeSlovakia/oem OrangeSpain/oem OrangeMoldavia/oem OrangeBelgium/oem TelecomItaliaMobile/oem VietnamOpenMarket/oem CherryCommon/oem CherryCambodia/oem CherryLaos/oem CherryMyanmar/oem CherryPhilippines/oem CherryThailand/oem)

CList1=( TMO/oem MPCS/oem )
CList2=( LatamAMX/oem LatamTelcelMexico/oem LanixTelcelMexico/oem )
CList3=( LatamAMX/oem LatamClaroBrazil/oem LatamClaroPeru/oem LanixClaroColombia/oem LatamClaroChile/oem LatamClaroColombia/oem )
Clist4=( VietnamOpenMarket/oem CherryCommon/oem RJIL/oem LatamOpenAMX/oem IndiaCommon/oem CherryCambodia/oem CherryThailand/oem CherryLaos/oem CherryMyanmar/oem CherryPhilippines/oem EUCommon/oem OrangeCommon/oem OrangeFrance/oem OrangePoland/oem OrangeRomania/oem OrangeSlovakia/oem OrangeSpain/oem )
CPList=( ${CList1[*]} ${CList2[*]} ${CList3[*]} ${Clist4[*]} )

JOBS_NUM=4
CurrentJob=1


addpackToCleanlist()
{
  echo "######## Add pack info to clean list ......"
  TARGET_FILES=$ProductOut/OTA_Target_Files
  CListFile=$TARGET_FILES/temp/cleanlist
  echo "show all packlist from oem: ${CPList[*]}"

  if [ -f $CListFile  ]
  then
      rm -f $CListFile
  fi

  for pack in ${CPList[@]}
  do
     item="${pack//\// }"
     item=($item)
     cpackname="${item[0]}"
     echo "$cpackname" >> "$CListFile"
  done
}


presetPackages()
{
  local PackList
  PackList=$1

  echo "###### 1. Preset non-pure data package list ${PackList[@]} to partition ${UsesPartitions[@]} ..."

  if [ -f "$RegionalizationEnvPathSpec/partition" ]
  then
    rm $RegionalizationEnvPathSpec/partition
  fi
  touch $RegionalizationEnvPathSpec/partition

  for partition in ${UsesPartitions[@]}
  do
    rm -rf $ProductOut/$partition
    mkdir -p $ProductOut/$partition
    echo "/$partition" >> $RegionalizationEnvPathSpec/partition
  done

  for pack in ${PackList[@]}
  do
    if [ "${pack//\//}" != "$pack" ]
    then
      item="${pack//\// }"
      item=($item)
      packname="${item[0]}"
      partition="${item[1]}"
      echo "packname = $packname"
      if [ -d "$ProductOut/$partition" ]
      then
        if [ -d "$REGIONAL_SOURCE_PATH/$packname" ]
        then
          mmm $REGIONAL_SOURCE_PATH/$packname
        else
          if [ "$packname" == "TMO" ]
          then
            fullName="TMobile"
          elif [ "$packname" == "MPCS" ]
          then
            fullName="MetroPCS"
          fi
          if [ -d "$REGIONALSP_SOURCE_PATH/$fullName" ]
          then
            mmm $REGIONALSP_SOURCE_PATH/$fullName
          fi
        fi

        if [ -d "$ProductOut/system/vendor/$packname" ]
        then
          cp -rf $ProductOut/system/vendor/$packname $ProductOut/$partition/
          echo "true" > $ProductOut/$partition/flag
        else
          echo "Warning: Preset pack $packname to partition $partition failed !"
          echo "Warning: There is no pack $packname or partition $partition !"
          return 1
        fi
      fi
    fi
  done
}

adjustPresetPackages()
{
  echo "##### 2. Link libs for app Preset 3rd apks to data/app-regional/ ..."

  for partition in ${UsesPartitions[@]}
  do
    for path in `find $ProductOut/$partition -name ".preloadspec"`
    do
      echo "$path"
      tmp="${path%\/.preloadspec}"
      pack="${tmp##*\/}"
      echo "$pack"

      if [ -d "$ProductOut/$partition/$pack/system/app" ]
      then
        for app in `ls $ProductOut/$partition/$pack/system/app`
        do
          if [ -d "$ProductOut/$partition/$pack/system/app/$app" ]
          then
            if [ -d "$ProductOut/$partition/$pack/system/lib" ]
            then
              ln -s /$partition/$pack/system/lib $ProductOut/$partition/$pack/system/app/$app/lib
            fi
            if [ -d "$ProductOut/$partition/$pack/system/lib64" ]
            then
              ln -s /$partition/$pack/system/lib64 $ProductOut/$partition/$pack/system/app/$app/lib64
            fi
          elif [ -f "$ProductOut/$partition/$pack/system/app/$app" ]
          then
            appname="${app%.apk}"
            if [ -d "$ProductOut/$partition/$pack/system/lib" ]
            then
              mkdir -p $ProductOut/$partition/lib
              ln -s /$partition/$pack/system/lib $ProductOut/$partition/lib/$appname
            fi
            if [ -d "$ProductOut/$partition/$pack/system/lib64" ]
            then
              mkdir -p $ProductOut/$partition/lib64
              ln -s /$partition/$pack/system/lib64 $ProductOut/$partition/lib64/$appname
            fi
          fi
        done
      fi

      if [ -d "$ProductOut/$partition/$pack/system/priv-app" ]
      then
        for app in `ls $ProductOut/$partition/$pack/system/priv-app`
        do
          if [ -d "$ProductOut/$partition/$pack/system/priv-app/$app" ]
          then
            if [ -d "$ProductOut/$partition/$pack/system/lib" ]
            then
              ln -s /$partition/$pack/system/lib $ProductOut/$partition/$pack/system/priv-app/$app/lib
            fi
            if [ -d "$ProductOut/$partition/$pack/system/lib64" ]
            then
              ln -s /$partition/$pack/system/lib64 $ProductOut/$partition/$pack/system/priv-app/$app/lib64
            fi
          elif [ -f "$ProductOut/$partition/$pack/system/priv-app/$app" ]
          then
            appname="${app%.apk}"
            if [ -d "$ProductOut/$partition/$pack/system/lib" ]
            then
              mkdir -p $ProductOut/$partition/lib
              ln -s /$partition/$pack/system/lib $ProductOut/$partition/lib/$appname
            fi
            if [ -d "$ProductOut/$partition/$pack/system/lib64" ]
            then
              mkdir -p $ProductOut/$partition/lib64
              ln -s /$partition/$pack/system/lib64 $ProductOut/$partition/lib64/$appname
            fi
          fi
        done
      fi

      if [ -d "$ProductOut/$partition/$pack/data/app" ]
      then
        for app in `ls $ProductOut/$partition/$pack/data/app`
        do
          if [ -f "$ProductOut/$partition/$pack/data/app/$app" ]
          then
            appname="${app%.apk}"
            if [ -d "$ProductOut/$partition/$pack/system/lib" ]
            then
              mkdir -p $ProductOut/data/app-lib
              ln -s /$partition/$pack/system/lib $ProductOut/data/app-lib/$appname
            fi
          fi
        done
      fi

      partitionSize=0
      packsSize=0
      if [ "$partition" == "oem" ]
      then
        partitionSize=`awk 'BEGIN { printf ("%.0f\n",'$OEMIMAGE_PARTITION_SIZE'-('$OEMIMAGE_PARTITION_SIZE'/32)) }'`
      else
        # Add code here to handle other partitions
        echo "Warning: Not support to resize the preset packages size for $partition!"
        return 1
      fi

      # Try to remove 3rd apks (preset to data partiton) to save partiton size
      if [ -d "$ProductOut/$partition/$pack/data/app" ]
      then
        if [ "`ls -A $ProductOut/$partition/$pack/data/app`" != "" ]
        then
          mkdir -p $ProductOut/data/app-regional/$pack
          cp -rfv $ProductOut/$partition/$pack/data/app/* $ProductOut/data/app-regional/$pack
          packsSize=`du -sb $ProductOut/$partition | cut -f1`
          if [ $partitionSize -gt 0 ] && [ $packsSize -gt $partitionSize ]
          then
            echo "Remove the 3rd apks in package to save partition size ..."
            rm -rfv $ProductOut/$partition/$pack/data/app
          fi
        fi
      fi

      # Try to remove odex file to save partition size
      packsSize=`du -sb $ProductOut/$partition | cut -f1`
      if [ $partitionSize -gt 0 ] && [ $packsSize -gt $partitionSize ]
      then
        echo "Remove the oat in package to save partition size ..."
        rm -rfv `find $ProductOut/$partition/$pack -name "oat" -type d`
      fi

      # Finally judge if packages size is ok for partition
      packsSize=`du -sb $ProductOut/$partition | cut -f1`
      if [ $packsSize -gt $partitionSize ]
      then
        echo "Partition size is too small to preset packages ..."
        return 1
      fi
    done
  done
}

generatePartitionImage()
{
  echo "##### 3. Generate the regionalization partition images ..."

  for partition in ${UsesPartitions[@]}
  do
    echo -e "===> Generate ${partition}.img ..."
    make showcommands $partition"_image"
    if [ $? -eq 0 ]
    then
      mv $ProductOut/$partition".img" $ProductOut/$partition$CurrentJob".img"
    else
      echo -e "Warning: Can't generate ${partition}.img using \"make showcommands ${partition}_image\" !"
      echo -e "Warning: Please check if support $partition partition !"
      return 1
    fi
  done
}

########################Main func############################

###### Load running parameters and judge if can run the job
if [ "$#" -ge "4" ]
then
  ProductOut=$1
  RegionalizationEnvPathSpec=$2
  UsesPartitions=$3
  OEMIMAGE_PARTITION_SIZE=$4
  # ODMIMAGE_PARTITION_SIZE=$5

  if [ "$UsesPartitions" == "system" ]
  then
    echo "Warning: Only preset packs to system partition, no need this job!!!"
    exit 0
  else
    # Remove system partition from partition list
    addpackToCleanlist

    x=0
    for partition in ${UsesPartitions[@]}
    do
      if [ "$partition" != "system" ]
      then
        if [ "$partition" == "oem" ]
        then
          if [ "$OEMIMAGE_PARTITION_SIZE" -ge "$MIN_OEMIMAGE_PARTITION_SIZE" ]
          then
            partitions[$x]=$partition
            let "x+=1"
          else
            echo "Warning: oem partition size is too small!!!"
            exit 0
          fi
        else
          # Add code here to handle other partitions
          echo "Warning: Faild for $partition partition, only support oem partition now!!!"
          exit 0
        fi
      fi
    done
    unset UsesPartitions
    UsesPartitions=${partitions[*]}
  fi
else
  echo "Warning: Need more parameters to run this job!!!"
  exit 0
fi
######




. build/envsetup.sh

while [ $CurrentJob -le $JOBS_NUM ]
do
  echo "###### Running job $CurrentJob for non-pure data packages ..."

  case "$CurrentJob" in
  "1" )
    presetPackages "${PresetPackList1[*]}"
  ;;
  "2" )
    presetPackages "${PresetPackList2[*]}"
  ;;
  "3" )
    presetPackages "${PresetPackList3[*]}"
  ;;
  "4" )
    presetPackages "${PresetPackList4[*]}"
  ;;
  esac
  # Check for func "presetPackages"
  if [ $? -ne 0 ]
  then
    # Only skip this job
    echo "###### Skipping job $CurrentJob ..."
    let "CurrentJob+=1"
    continue
  fi

  adjustPresetPackages
  # Check for func "adjustPresetPackages"
  if [ $? -ne 0 ]
  then
    # Only skip this job
    echo "###### Skipping job $CurrentJob ..."
    let "CurrentJob+=1"
    continue
  fi

  generatePartitionImage
  # Check for func "generatePartitionImage"
  if [ $? -ne 0 ]
  then
    # Only skip this job
    echo "###### Skipping job $CurrentJob ..."
    let "CurrentJob+=1"
    continue
  fi

  echo "###### Running job $CurrentJob for non-pure data packages end !"
  let "CurrentJob+=1"
done

#############################################################
