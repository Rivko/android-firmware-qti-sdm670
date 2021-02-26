#!/system/bin/sh

# Copyright (c) 2013-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

export PATH=/system/bin:$PATH

strBakForReplace=".bakforspec"
strExcludeFiles="exclude.list"
strExcludeFolder="exclude"
strForLink=".link"
currentSpec=""
SourceFolder=""
DestFolder=""
BasePath=""
LocalFlag=""
mode=""
strPathDefHosts="/system/vendor/hosts"
strPathForHosts="/system/etc/hosts"
#auditTargetList=("sdm845" "msmnile" "sdm710" "sdm660" "msm8998")
auditTargetList=("sdm845") # [[BEGIN:"ro.platform.audited"="true":END]]
hostsRedirect="false"
targetAudited="false"
funcInstall="installFunc"
funcUnInstall="uninstallFunc"

auditList()
{
    target="$1"
    if [ "" != "${target}" ]; then
        for t in ${auditTargetList[*]}
        do
            if [ "" != "$t" ]; then
                if [ "${target}" == "$t" ]; then
                    return 1
                    break
                fi
            fi
        done
    fi
    return 0
}

auditTarget()
{
    auditedProp=`getprop ro.platform.audited`
    if [ "true" == "${auditedProp}" ]; then
        targetAudited="true"
    elif [ "false" != "${auditedProp}" ]; then
        if [ -f /sys/devices/soc0/machine ]; then
            target=`cat /sys/devices/soc0/machine | tr [:upper:] [:lower:]`
        else
            target=`getprop ro.board.platform`
        fi
        auditList $target
        if [ "1" == $? ]; then
            targetAudited="true"
        fi
    fi
}

auditProp()
{
    step=0;
    flag=0;

    for line in $(cat $1); do
        ((index=$step/2))
        ((flag=$step%2))

        if [ 0 -eq ${flag} ]; then
            echo "$2.$3${index}=${line}"
        else
            echo "$2.$4${index}=${line}"
        fi

        ((step=$step+1))
    done
}

auditHosts()
{
    auditEnabled="$1"
    if [ "-audit" == "${auditEnabled}" ] && [ "true" == "${targetAudited}" ]; then
        hostsRedirect="true"
    fi
}

redirectHosts()
{
    funcName="$1"
    srcPath="$2"
    dstPath="$3"
    dstDir="$4"
    sFlag="0"

    if [ "${strPathDefHosts}" == "${dstPath}" ]; then
        if [ "${funcInstall}" == "${funcName}" ]; then
            if [ "false" == "${hostsRedirect}" ]
            then
                sFlag="1"
            else
                sFlag="2"
            fi
        elif [ "${funcUnInstall}" == "${funcName}" ]; then
            if [ "${srcPath%$strForLink}" == "${srcPath}" ] || [ ! -f $srcPath ]
            then
                sFlag="1"
            else
                sFlag="2"
            fi
        else #UnUsed
            sFlag="0"
        fi
    fi

    case "${sFlag}" in
        "1")
            echo "redirectHosts return from ${funcName} for ${srcPath} ${dstPath} ${dstDir}"
            ;;
        "2")
            echo "redirectHosts redirect the ${dstPath} to ${strPathForHosts}"
            ;;
    esac

    return "${sFlag}"
}

createFolder()
{
  local dirPath=$1
  if [ -d "$dirPath" ]
  then
    echo "Exist $dirPath"
  else
    createFolder "${dirPath%/*}"
    echo "mkdir and chmod $dirPath"
    mkdir "$dirPath"
    chmod 755 "$dirPath"
  fi
}

installFunc()
{
  local srcPath=$1
  local dstPath=$2
  local dstDir="${dstPath%/*}"
  createFolder $dstDir
  echo "installFunc $srcPath $dstPath $dstDir"
  if [ "${dstPath%$strForLink}" != "$dstPath" ]
  then
    dstPath="${dstPath%$strForLink}"
  fi
  if [ "$dstPath" == "/system/vendor/default.prop" ] ; then
    echo "return installFunc $srcPath $dstPath $dstDir"
    return
  fi

  redirectHosts "${funcInstall}" "$srcPath" "$dstPath" "$dstDir"
  case "$?" in
    "1")
        return
        ;;
    "2")
        dstPath=$strPathForHosts
        ;;
  esac

  if [ "${srcPath%$strForLink}" != "$srcPath" ]
  then
    if [ "${dstPath#${BasePath}/system/}" != "${dstPath}" ]
    then
      if [ -f "${srcPath%$strForLink}" ] || [ -h "${srcPath%$strForLink}" ]
      then
        if [ -f "$dstPath" ]
        then
          rm "$dstPath"
        fi
        mv "${srcPath%$strForLink}" $dstPath
        chmod 644 "$dstPath"
      fi
    else
      cp -p "${srcPath%$strForLink}" $dstPath
      chmod 644 "$dstPath"
      chown system:system "$dstPath"
    fi
  elif [ -h "$srcPath$strForLink" ]
  then
    installFunc "$srcPath$strForLink" $dstPath
  else
    if [ -f "$dstPath" ]
    then
      if [ -f "$dstPath$strBakForReplace$currentSpec" ]
      then
        rm "$dstPath$strBakForReplace$currentSpec"
      fi
      mv $dstPath $dstPath$strBakForReplace$currentSpec
    fi

    if [ "$mode" == "compiling" ] || [ "$mode" == "running" ]
    then
      ln -s ${dstPath#$BasePath} "$srcPath$strForLink"
    fi
    installFunc "$srcPath$strForLink" $dstPath
  fi
}

uninstallFunc()
{
  local srcPath=$1
  local dstPath=$2
  echo "uninstallFunc $srcPath $dstPath"
  if [ "${dstPath%$strForLink}" != "$dstPath" ]
  then
    dstPath="${dstPath%$strForLink}"
  fi
  if [ "$dstPath" == "/system/vendor/default.prop" ] ; then
    echo "return uninstallFunc $srcPath $dstPath $dstDir"
    return
  fi

  redirectHosts "${funcUnInstall}" "$srcPath" "$dstPath" "$dstDir"
  case "$?" in
    "1")
        return
        ;;
    "2")
        dstPath=$strPathForHosts
        ;;
  esac

  if [ "${srcPath%$strForLink}" != "$srcPath" ]
  then
    if [ "${dstPath#${BasePath}/system/}" != "${dstPath}" ]
    then
      if [ -f "$dstPath" ] || [ -h "$dstPath" ]
      then
        if [ -f "${srcPath%$strForLink}" ] || [ -h "${srcPath%$strForLink}" ]
        then
          rm $dstPath
        else
          mv $dstPath "${srcPath%$strForLink}"
          chmod 644 "${srcPath%$strForLink}"
        fi
      fi
    else
      echo "remove $dstPath"
      rm $dstPath
    fi
    if [ -f "$dstPath$strBakForReplace$currentSpec" ]
    then
      if [ -f "$dstPath" ]
      then
        rm $dstPath
      fi
      mv $dstPath$strBakForReplace$currentSpec $dstPath
    fi
    if [ "$mode" == "compiling" ] || [ "$mode" == "running" ]
    then
      rm $srcPath
    fi
  elif [ -h "$srcPath$strForLink" ] || [ -f "$srcPath" ]
  then
    uninstallFunc "$srcPath$strForLink" $dstPath
  else
    echo "Finish install"
  fi
}

installFolderFunc()
{
  local srcPath=$1
  local dstPath=$2
  for item in `ls -a $srcPath`
  do
    echo "find item=$item"
    if [ "$item" = "." ]
    then
      echo "current folder"
    else
      if [ "$item" = ".." ]
      then
        echo "upfolder"
      elif [ "$item" = ".preloadspec" ] || [ "$item" = "$strExcludeFiles" ]
      then
        echo "specflag"
      else
        if [ -f "$srcPath/$item" ]
        then
          installFunc "$srcPath/${item}" "$dstPath/${item}"
        elif [ -h "$srcPath/$item" ]
        then
          installFunc "$srcPath/${item}" "$dstPath/${item}"
        else
          if [ -d "$srcPath/$item" ]
          then
            installFolderFunc "$srcPath/${item}" "$dstPath/${item}"
          fi
        fi
      fi
    fi
  done
}

uninstallFolderFunc()
{
  local srcPath=$1
  local dstPath=$2
  for item in `ls -a $srcPath`
  do
    echo "uitem=$item"
    if [ "$item" = "." ]
    then
      echo "current folder"
    else
      if [ "$item" = ".." ]
      then
        echo "upfolder"
      elif [ "$item" = ".preloadspec" ] || [ "$item" = "$strExcludeFiles" ]
      then
        echo "specflag"
      else
        if [ -f "$srcPath/$item" ]
        then
          uninstallFunc "$srcPath/${item}" "$dstPath/${item}"
        elif [ -h "$srcPath/$item" ]
        then
          uninstallFunc "$srcPath/${item}" "$dstPath/${item}"
        else
          if [ -d "$srcPath/$item" ]
          then
            uninstallFolderFunc "$srcPath/${item}" "$dstPath/${item}"
          fi
        fi
      fi
    fi
  done
}

excludeFilesFunc()
{
  local srcPath=$1
  if [ -f "$srcPath" ]
  then
    echo "exclude the files in current spec"
    while read line
    do
      if [ -f "$DestFolder/$line" ]
      then
        local dstPath="$SourceFolder/$strExcludeFolder/$line"
        local dstDir="${dstPath%/*}"
        createFolder $dstDir
        if [ "${line#system/}" != "${line}" ]
        then
          mv $DestFolder/$line $dstPath
        else
          cp -p $DestFolder/$line $dstPath
        fi
      fi
    done < "$srcPath"
  fi
}

includeFilesFunc()
{
  local srcPath=$1
  if [ -f "$srcPath" ]
  then
    echo "restore the files excluded in previous spec"
    while read line
    do
      if [ -f "$SourceFolder/$strExcludeFolder/$line" ]
      then
        local dstPath="$DestFolder/$line"
        if [ "${line#system/}" != "${line}" ]
        then
          mv "$SourceFolder/$strExcludeFolder/$line" $dstPath
        else
          cp -p "$SourceFolder/$strExcludeFolder/$line" $dstPath
        fi
      fi
    done < "$srcPath"
  fi
}

getCurrentSpec()
{
  local specPath=$1
  currentSpec=""
  if [ -f "$specPath" ]
  then
    . $specPath
    while read line
    do
      currentSpec=${line#*=}
    done < $specPath
  fi
}

makeFlagFolder()
{
  if [ -d "$DestFolder/data/switch_spec" ]
  then
    echo "no need to create flag"
  else
    mkdir "$DestFolder/data/switch_spec"
    chmod 770 "$DestFolder/data/switch_spec"
    if [ "$mode" != "compiling" ]
    then
      chown system:system "$DestFolder/data/switch_spec"
    fi
  fi
}

changeDirMode()
{
  local strCurPath=$1
  chmod 755 $strCurPath
  for item in `ls -a $strCurPath/`
  do
    if [ "$item" = "." ] || [ "$item" = ".." ]
    then
      echo ".."
    elif [ -f "$strCurPath/$item" ]
    then
      chmod 644 "$strCurPath/$item"
    elif [ -d "$strCurPath/$item" ]
    then
      changeDirMode "$strCurPath/$item"
    else
      echo "who is $strCurPath/$item"
    fi
  done
}

recoveryDataPartition()
{
  local specPath=$1
  installFolderFunc "$SourceFolder/system/vendor/$currentSpec/data" "$DestFolder/data"
  currentSpec="Default"

  if [ -f "$specPath" ]
  then
    # Recovery the data partition for each spec
    local x=0
    while read line
    do
      if [ "$x" -ge "1" ]
      then
        if [ "${line#*=}" != "" ]
        then
          installFolderFunc "$SourceFolder/system/vendor/${line#*=}/data" "$DestFolder/data"
          currentSpec="${line#*=}"
        fi
      fi
      let "x+=1"
    done < $specPath
  fi
}

prepareActionData()
{
  # Mark if user has operated switching by CarrierConfigure app
  local iSSwitchByAction="false"

  # Copy spec folder from $SwitchData/cache/system/vendor to /cache/temp
  if [ -f "$SwitchApp/cache/action" ]
  then
    mkdir -p "$DestFolder/cache/temp"
    local x=0
    while read line
    do
      if [ "$x" -ge "1" ]
      then
        local specItem="${line#*=}"
        echo "specItem="$specItem
        echo "SwitchCacheData="$SwitchCacheData
        if [ -d "$SwitchData/cache/system/vendor/$specItem" ]
        then
          cp -rvf "$SwitchData/cache/system/vendor/$specItem" "$DestFolder/cache/temp/"
        elif [ -d "$SwitchCacheData/cache/system/vendor/$specItem" ]
        then
          cp -rvf "$SwitchCacheData/cache/system/vendor/$specItem" "$DestFolder/cache/temp/"
        fi
      fi
      let "x+=1"
    done < "$SwitchApp/cache/action"
    # Copy action spec list to $SwitchActionFlag
    cp -rf "$SwitchApp/cache/action" "$SwitchActionFlag"
    iSSwitchByAction="true"
  elif [ -f "$DestFolder/cache/action" ]
  then
    iSSwitchByAction="true"
  fi

  echo $iSSwitchByAction
}

getNewSpecList()
{
  local SwitchFlag=$1
  local specList

  if [ -f "$SwitchFlag" ]
  then
    local strNewSpec=""
    local newPackCount=0
    . "$SwitchFlag"
    if [ "$newPackCount" -ge "1" ]
    then
      local x=0
      while read line
      do
        if [ "$x" -ge "1" ]
        then
          local specItem="${line#*=}"
          specList[$x-1]=$specItem
        fi
        let "x+=1"
      done < $SwitchFlag
    else
      specList[0]=$strNewSpec
    fi
  fi
  echo ${specList[*]}
}

# Remove all files except folders
removeFilesUnderFolder()
{
  local folder=$1
  local array

  if [ -d "$folder" ]
  then
    array=(`ls $folder`)
    local x=0
    while [ "$x" -lt "${#array[@]}" ]
    do
      if [ -d "$folder/${array[$x]}" ]
      then
        removeFilesUnderFolder "$folder/${array[$x]}"
      elif [ -f "$folder/${array[$x]}" ]
      then
        echo "Remove file $folder/${array[$x]} ..."
        rm -f $folder/${array[$x]}
      fi
      let "x+=1"
    done
  fi
}

uninstallOldSpecList()
{
  local specPath=$1
  local specList
  if [ -f "$specPath" ]
  then
    local x=0
    while read line
    do
      if [ "$x" -ge "1" ]
      then
        specList[$x-1]="${line#*=}"
      fi
      let "x+=1"
    done < "$specPath"
  fi

  local x="${#specList[@]}"
  while [ "$x" -gt "0" ]
  do
    let "x-=1"
    if [ "$x" -ge "1" ]
    then
      currentSpec=${specList[$x-1]}
    else
      currentSpec="Default"
    fi
    if [ "${specList[$x]}" != "Default" ]
    then
      uninstallFolderFunc "$SourceFolder/${specList[$x]}" "$DestFolder"
      includeFilesFunc "$SourceFolder/${specList[$x]}/$strExcludeFiles"
    fi
  done
  rm -rf $SourceFolder/$strExcludeFolder/*

  # Reinstall Default pack
  mv -f $DestFolder/system/build.prop.bakforspecDefault $DestFolder/system/build.prop
  if [ -f $DestFolder/etc/hosts.bakforspecDefault ]
  then
    mv -f $DestFolder/etc/hosts.bakforspecDefault $DestFolder/etc/hosts
  fi
  uninstallFolderFunc "$SourceFolder/Default" "$DestFolder"
  if [ "$mode" == "running" ]
  then
    removeFilesUnderFolder "$DestFolder/data"
  fi
  installFolderFunc "$SourceFolder/Default" "$DestFolder"
  echo "packCount=1" > $specPath
  echo "strSpec1=Default" >> $specPath
}

overrideRoProperty()
{
  local srcprop=$1
  local dstprop=$2
  local tempfile=${dstprop%/*}"/temp.prop"

  echo "Override ro.* property from $srcprop to $dstprop ..."

  while IFS=$'\n' read -r srcline
  do
    if [ "${srcline:0:1}" != "#" ] && [ "${srcline#*=}" != "${srcline}" ]
    then
      local flag=0
      while IFS=$'\n' read -r dstline
      do
        if [ "${srcline%%.*}" = "ro" ] && [ "${srcline%%[ =]*}" = "${dstline%%[ =]*}" ]
        then
          echo "Override $srcline ..."
          echo -E $srcline >> $tempfile
        else
          echo -E $dstline >> $tempfile
        fi
      done < $dstprop
      mv -f $tempfile $dstprop
    fi
  done < $srcprop

  chmod 644 $dstprop
}

installNewSpecList()
{
  local specList
  specList=(`echo "$@"`)

  if [ "${#specList[@]}" -eq "1" ] && [ "${specList[0]}" = "Default" ]
  then
    echo "Default spec already have been installed, do nothing here!"
  else
    # Check if the list is ready
    local x=0
    local y=0
    local newList
    if [ "${#specList[@]}" -ge "1" ]
    then
      while [ "$x" -lt "${#specList[@]}" ]
      do
        if [ "${specList[$x]}" != "" ]
        then
          # Copy spec folder from /cache/temp to /system/vendor
          if [ -d "$DestFolder/cache/temp/${specList[$x]}" ]
          then
            if [ -d "$SourceFolder/${specList[$x]}" ]
            then
              rm -rf "$SourceFolder/${specList[$x]}"
            fi
            cp -rf "$DestFolder/cache/temp/${specList[$x]}" "$SourceFolder/${specList[$x]}"
          fi
          if [ -d "$SourceFolder/${specList[$x]}" ]
          then
            newList[$y]=${specList[$x]}
            let "y+=1"
          fi
        fi
        let "x+=1"
      done
    fi

    # remove $DestFolder/cache/temp
    if [ -d "$DestFolder/cache/temp" ]
    then
      rm -rf "$DestFolder/cache/temp"
    fi

    # Install spec as list
    if [ "${#newList[@]}" -ge "1" ]
    then
      # Backup build.prop for Default
      cp -f $DestFolder/system/build.prop $DestFolder/system/build.prop.bakforspecDefault
      if [ "true" == "${targetAudited}" ]
      then
          cp -f $DestFolder/etc/hosts $DestFolder/etc/hosts.bakforspecDefault
      fi
      local x=0
      echo "packCount=${#newList[@]}" > $LocalFlag
      while [ "$x" -lt "${#newList[@]}" ]
      do
        excludeFilesFunc "$SourceFolder/${newList[$x]}/$strExcludeFiles"
        changeDirMode "$SourceFolder/${newList[$x]}"
        installFolderFunc "$SourceFolder/${newList[$x]}" "$DestFolder"
        overrideRoProperty "$DestFolder/system/vendor/vendor.prop" "$DestFolder/system/build.prop"
        let "x+=1"
        currentSpec="${newList[$x-1]}"
        echo "strSpec$x=$currentSpec" >> $LocalFlag
      done
    fi
  fi
}

cleanOldSpecs()
{
  local specList
  specList=(`echo "$@"`)

  # When in step 1 of switching mode,
  # should ensure that action specs are not cleared.
  if [ "$mode" = "switching" ]
  then
     local actionSpecList
     actionSpecList=(`getNewSpecList "$SwitchActionFlag"`)
     specList+=("${actionSpecList[@]}")
  fi

  for item in `ls -a $SourceFolder`
  do
    if [ "$item" = "Default" ]
    then
      echo "Default spec, no need remove"
    elif [ "$item" = ".." ] || [ "$item" = "." ]
    then
      echo "Current path"
    elif [ -f "$SourceFolder/$item/.preloadspec" ]
    then
      echo "find $item"
      local x=0
      local flag=0
      while [ "$x" -lt "${#specList[@]}" ]
      do
        if [ "$item" = "${specList[$x]}" ]
        then
          flag=1
          break
        fi
        let "x+=1"
      done

      if [ "$flag" -eq "0" ]
      then
        rm -rf "$SourceFolder/$item"
      fi
    fi
  done
}

initSwitchingMode()
{
  if [ -f "$SwitchModeFlag" ]
  then
    . "$SwitchModeFlag"
    echo "Before mode = $mode"
  fi

  if [ "$mode" = "" ]
  then
    if [ "$DestFolder" != "" ]
    then
      # compiling mode means that switch spec when compiling the source code
      # in Android.mk.
      mode="compiling"
    else
      if [ "$(prepareActionData)" = "true" ]
      then
        # switching mode means that switch spec through CarrierConfigure App or
        # SIM Trigger, which includes two steps:
        # 1. switch to Default and clean the old specs
        # 2. switch to the new spec list
        mode="switching"
        echo "mode=$mode" > "$SwitchModeFlag"
      else
        # running mode means that run switch_spec.sh to switch spec
        # manully through cmd on DUT.
        mode="running"
      fi
    fi
  fi
}

findDependency()
{
  local storagePos=$1
  local spec=$2
  local depend=""
  if [ -e $storagePos/$spec/.preloadspec ] ; then
    depend=`grep "\(^[^#].*\)Dependency=" $storagePos/$spec/.preloadspec`
    depend="${depend##*"Dependency=\""}"
    depend="${depend%%\"*}"
  fi
  echo "${depend##*/}"
}

findAllDependencys()
{
  local storagePos=$1
  local spec=$2
  local depend=""
  depend="$(findDependency "$storagePos" "$spec")"
  local depends="$depend"
  while [ "$depend" != "" ]
  do
    depend="$(findDependency "$storagePos" "$depend")"
    depends=" $depends $depend"
  done
  echo "$spec $depends"
}

# Used to switch spec in regionalization way
# When "ro.regionalization.support" is true.
regionalizationSwitch()
{
  if [ "$(getprop ro.regionalization.support)" != "true" ]
  then
    return
  fi

  local storagePos=""
  local spec=""
  if [ "$#" -eq "2" ]
  then
    storagePos=$1
    spec=$2
  else
    storagePos="$DestFolder/system/vendor"
  fi

  # Create spec and prop pointer file if not exist
  if [ ! -f "$RegionalizationEnvSpecPath/spec" ]
  then
    echo "packStorage=$storagePos" > "$RegionalizationEnvSpecPath/spec"
    echo "packCount=1" >> "$RegionalizationEnvSpecPath/spec"
    echo "strSpec1=Default" >> "$RegionalizationEnvSpecPath/spec"
  fi
  chmod 666 "$RegionalizationEnvSpecPath/spec"
  chown system:system "$RegionalizationEnvSpecPath/spec"

  chmod 666 "$RegionalizationEnvSpecPath/devicetype"
  chown system:system "$RegionalizationEnvSpecPath/devicetype"

  chmod 666 "$RegionalizationEnvSpecPath/mbnversion"
  chown system:system "$RegionalizationEnvSpecPath/mbnversion"

  chmod 666 "$RegionalizationEnvSpecPath/.not_triggered"
  chown system:system "$RegionalizationEnvSpecPath/.not_triggered"
  

  if [ ! -f "$RegionalizationEnvSpecPath/vendor_ro.prop" ]
  then
    echo "import $storagePos/Default/system/vendor/vendor.prop" > "$RegionalizationEnvSpecPath/vendor_ro.prop"
  fi
  chmod 644 "$RegionalizationEnvSpecPath/vendor_ro.prop"
  chown system:system "$RegionalizationEnvSpecPath/vendor_ro.prop"

  if [ ! -f "$RegionalizationEnvSpecPath/vendor_persist.prop" ]
  then
    echo "import $storagePos/Default/system/vendor/vendor.prop" > "$RegionalizationEnvSpecPath/vendor_persist.prop"
  fi
  chmod 644 "$RegionalizationEnvSpecPath/vendor_persist.prop"
  chown system:system "$RegionalizationEnvSpecPath/vendor_persist.prop"

  if [ ! -f "$RegionalizationEnvSpecPath/submask" ]
  then
    touch "$RegionalizationEnvSpecPath/submask"
  fi
  chmod 644 "$RegionalizationEnvSpecPath/submask"
  chown system:system "$RegionalizationEnvSpecPath/submask"

  if [ ! -f "$RegionalizationEnvSpecPath/partition" ]
  then
    touch "$RegionalizationEnvSpecPath/partition"
  fi
  chmod 644 "$RegionalizationEnvSpecPath/partition"
  chown system:system "$RegionalizationEnvSpecPath/partition"

  # Find dependency specs
  local specList
  if [ "$spec" != "" ]
  then
    specList=($(findAllDependencys "$storagePos" "$spec"))
    echo "Installing ${specList[@]} ... "
  fi

  # Modify spec files under /persist/speccfg
  if [ "${#specList[@]}" -gt 0 ]
  then
    cat /dev/null > "$RegionalizationEnvSpecPath/spec"
    cat /dev/null > "$RegionalizationEnvSpecPath/vendor_ro.prop"
    cat /dev/null > "$RegionalizationEnvSpecPath/vendor_persist.prop"
    echo "packStorage=$storagePos" >> "$RegionalizationEnvSpecPath/spec"
    echo "packCount=${#specList[@]}" >> "$RegionalizationEnvSpecPath/spec"
    local x=0
    while [ "$x" -lt "${#specList[@]}" ]
    do
      let "x+=1"
      echo "import $storagePos/${specList[$x-1]}/system/vendor/vendor.prop" >> "$RegionalizationEnvSpecPath/vendor_ro.prop"
    done
    local y=1
    while [ "$x" -gt "0" ]
    do
      echo "strSpec$y=${specList[$x-1]}" >> "$RegionalizationEnvSpecPath/spec"
      echo "import $storagePos/${specList[$x-1]}/system/vendor/vendor.prop" >> "$RegionalizationEnvSpecPath/vendor_persist.prop"
      let "x-=1"
      let "y+=1"
    done
  fi
}

######Main function start######

if [ "$#" -eq "0" ]
then
  RegionalizationEnvSpecPath="$DestFolder/persist/speccfg"

  # Just init here
  regionalizationSwitch

  if [ ! -d "$DestFolder/data/switch_spec" ]
  then
    if [ "$(getprop ro.regionalization.support)" == "true" ]
    then
      recoveryDataPartition "$RegionalizationEnvSpecPath/spec"
    else
      recoveryDataPartition "$DestFolder/system/vendor/speccfg/spec"
    fi
    makeFlagFolder
  fi
elif [ "$#" -eq "1" ]
then
  RmFlag="$1"
  SpecFile="$DestFolder/cache/action"

  # Get the current specs
  x=0
  newSpecList=""
  while read line
  do
    if [ "$x" -eq "0" ]
    then
      storagePos="${line#*=}"
      echo "storagePos = $storagePos"
    fi
    if [ "$x" -ge "2" ]
    then
      specItem="${line#*=}"
      newSpecList[$x-2]=$specItem
    fi
    let "x+=1"
  done < "$SpecFile"

  # Clean old specs to free $storagePos
  if [ "$RmFlag" -eq "1" ]
  then
    echo "Clean $storagePos ..."
    SourceFolder="$DestFolder$storagePos"
    cleanOldSpecs "${newSpecList[*]}"
  fi

  x=0
  while [ "$x" -lt "${#newSpecList[@]}" ]
  do
    if [ "${newSpecList[$x]}" == "" ]
    then
      let "x+=1"
      continue
    fi
    echo "new spec = ${newSpecList[$x]}"
    if [ -d "$DestFolder/cache/temp/system/vendor/${newSpecList[$x]}" ]
    then
      if [ -d "$DestFolder$storagePos/${newSpecList[$x]}" ]
      then
        rm -rf "$DestFolder$storagePos/${newSpecList[$x]}"
      fi
      cp -rf "$DestFolder/cache/temp/system/vendor/${newSpecList[$x]}" "$DestFolder$storagePos/${newSpecList[$x]}"
    fi
    let "x+=1"
  done

  if [ -d "$DestFolder/cache/temp" ]
  then
    rm -rf "$DestFolder/cache/temp"
  fi
  if [ -f "$SpecFile" ]
  then
    rm -f "$SpecFile"
  fi
elif [ "$#" -eq "2" ]
then
  # For adb cmds swtiching when regionalzation env is supported
  RegionalizationEnvSpecPath="$DestFolder/persist/speccfg"
  storagePos=$1
  spec=$2
  regionalizationSwitch "$storagePos" "$spec"
else
  SourceFolder="$1"
  DestFolder="$2"
  BasePath="$3"
  LocalFlag="$4"
  echo "SourceFolder=$SourceFolder DestFolder=$DestFolder BasePath=$BasePath LocalFlag=$LocalFlag"
  SwitchApp="$DestFolder/data/data/com.qualcomm.qti.carrierconfigure"
  SwitchData="$DestFolder/data/data/com.qualcomm.qti.loadcarrier"
  SwitchCacheData="$DestFolder/data/data/com.qualcomm.qti.accesscache"
  SwitchModeFlag="$DestFolder/system/vendor/speccfg/mode"
  SwitchFlag="$DestFolder/system/vendor/speccfg/spec.new"
  SwitchActionFlag="$DestFolder/cache/action"
  RmFlag="0"

  initSwitchingMode

  echo "Current mode = $mode"

  # Set the RmFlag for cleaning preset specs
  if [ -f "$SwitchApp/cache/rmflag" ]
  then
    RmFlag="1"
  fi

  if [ -d "$SourceFolder/$strExcludeFolder" ]
  then
    echo "no need to create excludefolder"
  else
    mkdir "$SourceFolder/$strExcludeFolder"
    chmod 770 "$SourceFolder/$strExcludeFolder"
  fi

  if [ "$#" -gt "4" ]
  then
    newSpecList="$5"
    echo "switchToSpec=${newSpecList[0]}"
    if [ "$#" -gt "5" ]
    then
      RmFlag="$6"
    fi
  else
    if [ "$mode" == "compiling" ] && [ -f "$SwitchFlag" ]
    then
      mkdir -p "$DestFolder/cache"
      mv -f $SwitchFlag $SwitchActionFlag
    fi

    newSpecList=(`getNewSpecList "$SwitchActionFlag"`)

    if [ -f "$SwitchModeFlag" ]
    then
      rm -rf "$SwitchModeFlag"
    fi
    if [ -f "$SwitchActionFlag" ]
    then
      rm -rf "$SwitchActionFlag"
    fi
  fi

  auditTarget
  if [ "$#" -ge "7" ]; then
      auditHosts "$7"
  fi

  getCurrentSpec "$LocalFlag"

  if [ "${#currentSpec}" -eq "0" ]
  then
    echo "No find spec, but need to install Default"
    installFolderFunc "$SourceFolder/Default" "$DestFolder"
    currentSpec="Default"
  fi

  uninstallOldSpecList "$LocalFlag"

  if [ "$RmFlag" -eq "1" ]
  then
    cleanOldSpecs "${newSpecList[*]}"
  fi

  if [ "${#newSpecList[@]}" -ge "1" ]
  then
    installNewSpecList "${newSpecList[*]}"
  fi

 # if [ "$mode" == "compiling" ]
 # then
   # rm -rf "$DestFolder/cache"
 # fi

  chmod 644 "$LocalFlag"
  chmod 755 "$DestFolder/system/vendor/speccfg"

  makeFlagFolder

  if [ "$newSpecList" == "ChinaMobile" ]
  then
    rm -rf /system/media/boot.wav
    rm -rf /system/media/shutdown.wav
  fi

fi

######Main function end######
