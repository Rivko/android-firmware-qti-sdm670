# Disable this makefile, After verify all commands can work, please remove the ifeq condition
ifeq (1,1)
ifeq ($(call is-board-platform-in-list, msm8996 msm8916 sdm660 msm8998 apq8098_latv msm8953 msm8937 sdm845 msmpeafowl sdm710 qcs605 msm8952 msm8909 msmnile),true)
TARGET_NOT_SUPPORT :=true
endif

ifeq ($(call is-board-platform-in-list, msm8909 msm8916),true)
TARGET_PACKAGE_ISSUPPORT_QGP :=false
endif

#only for optimization QGP speccfg file under /persist/speccfg/
#All of those file will be restored during runtime by carrierconfig at first boot-up time.
ifeq ($(call is-board-platform-in-list, sdm845),true)
TARGET_ISREDUCED_SPECFG_FILE :=true
endif

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
###########################################################
GENERATED_PACKAGE_PATH := $(TARGET_OUT_VENDOR)/package
PRESET_PACKAGE_PATH := /$(TARGET_COPY_OUT_VENDOR)/package
##############radio property#####################
# Set the default value for CSVT and this prop needn't control by toos.
#ADDITIONAL_BUILD_PROPERTIES += persist.radio.calls.on.ims=0
#ADDITIONAL_BUILD_PROPERTIES += persist.radio.jbims=0
#ADDITIONAL_BUILD_PROPERTIES += persist.radio.csvt.enabled=false
# Enable the custom ecclist, need database file also
#ADDITIONAL_BUILD_PROPERTIES += persist.radio.custom_ecc=1
# Enable radio tech support when PLMN search and select network
#ADDITIONAL_BUILD_PROPERTIES += persist.radio.rat_on=combine
# Set the timeout of pending ACK for mt sms
#ADDITIONAL_BUILD_PROPERTIES += persist.radio.mt_sms_ack=20

#################################################
PRE_LOAD_SPEC := .preloadspec
$(shell mkdir -p $(GENERATED_PACKAGE_PATH)/Default)
$(shell cp -r $(LOCAL_PATH)/$(PRE_LOAD_SPEC) $(GENERATED_PACKAGE_PATH)/Default/$(PRE_LOAD_SPEC))

#################################################
SWITCH_SPEC_SH := switch_spec.sh
$(shell mkdir -p $(TARGET_OUT_VENDOR)/speccfg)
$(shell cp -r $(LOCAL_PATH)/$(SWITCH_SPEC_SH) $(TARGET_OUT_VENDOR)/speccfg/$(SWITCH_SPEC_SH))

#################################################
GMS_INSTALL_SH := GMSInstall.sh
$(shell mkdir -p $(TARGET_OUT_VENDOR)/speccfg)
$(shell cp -r $(LOCAL_PATH)/$(GMS_INSTALL_SH) $(TARGET_OUT_VENDOR)/speccfg/$(GMS_INSTALL_SH))

################################################
DEFAULT_PROP := default.prop
$(shell mkdir -p $(GENERATED_PACKAGE_PATH)/Default/system/vendor/)
$(shell cp $(LOCAL_PATH)/$(DEFAULT_PROP) $(GENERATED_PACKAGE_PATH)/Default/system/vendor/$(DEFAULT_PROP))

################################################
DEFAULT_ROW_MBN := mcfg_sw.mbn
$(shell mkdir -p $(GENERATED_PACKAGE_PATH)/Default/data/)
$(shell mkdir -p $(GENERATED_PACKAGE_PATH)/Default/data/modem_row_config/)
$(shell cp $(LOCAL_PATH)/$(DEFAULT_ROW_MBN) $(GENERATED_PACKAGE_PATH)/Default/data/modem_row_config/$(DEFAULT_ROW_MBN))

################################################
SEPOLICY_PATH := "device/qcom/sepolicy"
SEPOLICY_COMMON_PATH := "device/qcom/sepolicy/common"
SEPOLICY_PRIVATE_PATH := "device/qcom/sepolicy/private"
define is-regionalization-supported
$(shell product=$(1) ; \
  if [ -d "$(SEPOLICY_PATH)/$$product" ] ; then \
    result=`grep -r "regionalization_service" $(SEPOLICY_PATH)/$$product` ;\
    if [ "$$result" != "" ] ; then echo "true" ;\
    else echo "false" ;\
    fi ;\
  elif [ -d "$(SEPOLICY_PATH)/$${product%%_*}" ] ; then \
    result=`grep -r "regionalization_service" $(SEPOLICY_PATH)/$${product%%_*}` ;\
    if [ "$$result" != "" ] ; then echo "true" ;\
    else echo "false" ;\
    fi ;\
  fi)
endef

define is-regionalization-supported_under_common_and_private
$(shell if [ -d "$(SEPOLICY_COMMON_PATH)" ] ; then \
    result=`grep -r "regionalization_service" $(SEPOLICY_COMMON_PATH)` ;\
    if [ "$$result" != "" ] ; then echo "true" ;\
    elif [ -d "$(SEPOLICY_PRIVATE_PATH)" ] ; then \
        result=`grep -r "regionalization_service" $(SEPOLICY_PRIVATE_PATH)` ;\
        if [ "$$result" != "" ] ; then echo "true" ;\
          else echo "false" ;\
        fi ;\
     else echo "false" ;\
    fi ;\
    fi)
endef

# Switcher for regional environment
ifeq ($(call is-regionalization-supported,$(TARGET_PRODUCT)),true)
USES_REGIONALIZATION_ENVIRONMENT := true
else
ifeq ($(call is-regionalization-supported_under_common_and_private),true)
USES_REGIONALIZATION_ENVIRONMENT := true
else
USES_REGIONALIZATION_ENVIRONMENT := false
endif
endif

# Support to customize the partition in order to
# preset or install Carrier packages
ifeq ($(TARGET_NOT_SUPPORT),true)
USES_REGIONALIZATION_PARTITIONS := system
else
USES_REGIONALIZATION_PARTITIONS := system oem
endif

#Add PRODUCT_VENDOR_MOVE_ENABLED
#ifeq ($(PRODUCT_VENDOR_MOVE_ENABLED),true)
FSTAB_DIR := $(TARGET_OUT_VENDOR_ETC)
#else
#FSTAB_DIR := $(TARGET_ROOT_OUT)
#endif

RegionalizationEnvPathSpec := $(PRODUCT_OUT)/persist/speccfg
#INCLUDELIST for particular target 
INSTALLCARRIER_INCLUDELIST := Default ChinaTelecom ChinaMobile ChinaUnicom CTA RJIL

# Add for proper install for carrier
InstallCarrier_MainPackList := Default
#InstallCarrier_PresetPackList := Default ChinaTelecom ChinaMobile ChinaUnicom EUCommon TelefonicaGermany CTA VodafoneGermany VodafoneUK H3GUK H3GItalia IndonesiaOpenmarket ThailandOpenMarket NorthAmerica VodafoneTurkey VodafoneCzech VodafoneHungary VodafoneIreland VodafoneNetherlands VodafoneSouthAfrica VodafoneIT VodafoneES VodafonePT VodafoneGreece VodafoneGroup RJIL LatamTelefonica LatamTelefonicaArgentina LatamTelefonicaBrazil LatamTelefonicaChile LatamTelefonicaColombia LatamTelefonicaCostaRica LatamTelefonicaEcuador LatamTelefonicaElSalvador LatamTelefonicaGuatemala LatamTelefonicaMexico LatamTelefonicaNicaragua LatamTelefonicaPanama LatamTelefonicaPeru LatamTelefonicaUruguay LatamTelefonicaVenezuela TelefonicaSpain Cambodia DTGermany Laos LatamBrazil MalaysiaOpenMarket PhilippinesOpenMarket EEEU
InstallCarrier_PresetPackList := Default ChinaMobile ChinaTelecom ChinaUnicom CTA EUCommon VodafoneGroup VodafoneUK TelefonicaGermany
InstallCarrier_OTASourcePackList :=
InstallCarrier_OTABinaryPackList :=

#None-pure package to pure package when without oem support
ifeq ($(call is-board-platform-in-list, sdm660),true)
#InstallCarrier_PresetPackList := $(InstallCarrier_PresetPackList) LatamAMX LatamClaroChile LatamTelcelMexico LatamClaroColombia LatamClaroPeru LatamClaroBrazil TelecomItaliaMobile OrangeCommon OrangeFrance OrangeSpain OrangeSlovakia OrangePoland OrangeRomania OrangeMoldavia OrangeBelgium
endif

# Local Parameter
InstallCarrierPathCarrier := $(LOCAL_PATH)
InstallCarrierPathSpec := $(TARGET_OUT_VENDOR)/speccfg
#ifeq ($(TARGET_ARCH),arm64)
#  InstallCarrierUpdateBinary := $(call intermediates-dir-for,EXECUTABLES,updater,,,32)/updater
#  InstallCarrierWrapperUpdateBinary := $(call intermediates-dir-for,EXECUTABLES,wrapper-updater,,,32)/wrapper-updater
#else
  InstallCarrierUpdateBinary := $(call intermediates-dir-for,EXECUTABLES,updater)/updater
  InstallCarrierWrapperUpdateBinary := $(call intermediates-dir-for,EXECUTABLES,wrapper-updater)/wrapper-updater
#endif
InstallCarrierSignTool := $(HOST_OUT_JAVA_LIBRARIES)/signapk.jar
InstallCarrierSignPerm := $(DEFAULT_SYSTEM_DEV_CERTIFICATE).x509.pem
InstallCarrierSignKey := $(DEFAULT_SYSTEM_DEV_CERTIFICATE).pk8
InstallCarrierSysImage := $(call intermediates-dir-for,PACKAGING,systemimage)/system.img
InstallCarrierFileList := $(PRODUCT_OUT)/installed-files.txt
InstallCarrierUpdateScript := $(InstallCarrierPathCarrier)/updater-script

# Local Parameter for refactor build.porp
BuildPropFileName := system/build.prop
OriginalBuildPropPath := $(PRODUCT_OUT)/$(BuildPropFileName)

# For OTA package in block mode
TARGET_FILES := $(PRODUCT_OUT)/OTA_Target_Files
BINARY_PACKS := $(PRODUCT_OUT)/OTA_Binary_Packs

# Back up all carrier packages
#BackupAllCarriers: $(InstallCarrierFileList) $(FULL_SYSTEMIMAGE_DEPS)
BackupAllCarriers: $(FULL_SYSTEMIMAGE_DEPS)
	@if [ -d "$(TARGET_FILES)" ] ; then \
           cd $(TARGET_FILES) ;\
           rm -rf `ls | grep -v "Default.zip"` ;\
           cd - ;\
         fi
	@if [ -d "$(BINARY_PACKS)" ] ; then \
           rm -rf $(BINARY_PACKS)/* ;\
         fi
	@mkdir -p $(TARGET_FILES)
	@mkdir -p $(BINARY_PACKS)
	@mkdir -p $(TARGET_FILES)/temp
	@for path in `find $(GENERATED_PACKAGE_PATH) -name ".preloadspec"` ; do \
           tmp="$${path%\/.preloadspec}" ;\
           pack="$${tmp##*\/}" ;\
           if [ "$$pack" != "" ] ; then \
             echo "Back up $$pack to $(TARGET_FILES)/temp/$$pack ..." ;\
             echo "$$pack" >> "$(TARGET_FILES)/temp/packlist" ;\
             cp -rf $(GENERATED_PACKAGE_PATH)/$$pack $(TARGET_FILES)/temp ;\
           fi ;\
         done
	@mkdir -p $(RegionalizationEnvPathSpec)
	@echo "omt" > $(RegionalizationEnvPathSpec)/devicetype
	@touch $(RegionalizationEnvPathSpec)/mbnversion
	@touch $(RegionalizationEnvPathSpec)/.not_triggered

build_oem_carrier_image: $(InstallCarrierFileList) $(FULL_SYSTEMIMAGE_DEPS) InstallCarrier
	@if [ "$(TARGET_NOT_SUPPORT)" != "true" ] ; then \
	  /bin/bash $(InstallCarrierPathCarrier)/preset_packs_to_partitions.sh \
                    "$(PRODUCT_OUT)" \
                    "$(RegionalizationEnvPathSpec)" \
                    "$(USES_REGIONALIZATION_PARTITIONS)" \
                    "$(BOARD_OEMIMAGE_PARTITION_SIZE)" ;\
    fi


InstallCarrier: $(OriginalBuildPropPath) $(FULL_SYSTEMIMAGE_DEPS) $(InstallCarrierUpdateBinary) $(InstallCarrierSignTool) $(InstallCarrierSignPerm) $(InstallCarrierSignKey) BackupAllCarriers
#InstallCarrier: $(InstallCarrierFileList) $(FULL_SYSTEMIMAGE_DEPS) $(InstallCarrierSignTool) $(InstallCarrierSignPerm) $(InstallCarrierSignKey) $(FSTAB_DIR)/fstab.qcom BackupAllCarriers

	@if [ -f "$(OriginalBuildPropPath)" ]; then \
	  cp "$(OriginalBuildPropPath)" "$(GENERATED_PACKAGE_PATH)"/Default/"$(BuildPropFileName)";\
        else \
	  echo "The file "$(OriginalBuildPropPath)" is not found! ";\
        fi
	@if [ -f "$(OriginalBuildPropPath)".bakforspec ]; then \
	    cp "$(OriginalBuildPropPath)".bakforspec "$(GENERATED_PACKAGE_PATH)"/Default/"$(BuildPropFileName)" ;\
	 fi
	@echo -e "\n" >> "$(GENERATED_PACKAGE_PATH)"/Default/"$(BuildPropFileName)"
	@echo -e "import $(PRESET_PACKAGE_PATH)/vendor.prop\n" >> "$(GENERATED_PACKAGE_PATH)"/Default/"$(BuildPropFileName)"
	@echo -e "import /system/vendor/vendor.prop\n" >> "$(GENERATED_PACKAGE_PATH)"/Default/"$(BuildPropFileName)"
	@if [ "$(USES_REGIONALIZATION_ENVIRONMENT)" == "true" ] ; then \
           temp="$(GENERATED_PACKAGE_PATH)/Default/temp.prop" ;\
           echo -e "#IMPORT REGIONALIZATION VENDOR PROP PATH FIRST IN ORDER TO CUSTOMIZE ro.* PROPERTIES#" > "$$temp" ;\
           echo -e "import /persist/speccfg/vendor_ro.prop\n" >> "$$temp" ;\
           cat "$(GENERATED_PACKAGE_PATH)/Default/$(BuildPropFileName)" >> "$$temp" ;\
           echo -e "\n" >> "$$temp" ;\
           echo -e "#IMPORT REGIONALIZATION VENDOR PROP PATH LAST IN ORDER TO OVERRIDE PROPERTIES#" >> "$$temp" ;\
           echo -e "import /persist/speccfg/vendor_persist.prop\n" >> "$$temp" ;\
           mv "$$temp" "$(GENERATED_PACKAGE_PATH)/Default/$(BuildPropFileName)" ;\
         fi
	@echo -e "import $(PRESET_PACKAGE_PATH)/default.prop\n" >> "$(GENERATED_PACKAGE_PATH)"/Default/"$(BuildPropFileName)"
	@echo -e "import $(PRESET_PACKAGE_PATH)/power.prop\n" >> "$(GENERATED_PACKAGE_PATH)"/Default/"$(BuildPropFileName)"
	@cp  $(GENERATED_PACKAGE_PATH)/Default/$(BuildPropFileName) $(OriginalBuildPropPath)
	@echo "###################### Carrier Installation ######################"
#	@echo "###  The current package list is $(InstallCarrier_MainPackList)"
#	@echo "###  Preset $(InstallCarrier_PresetPackList) into build"
#	@echo "###  Make ota package for $(InstallCarrier_OTASourcePackList)"
#	@echo "##################################################################"
ifeq (0,1)
	@echo "### 1. Make the default switch ota package"
	@mkdir -p $(PRODUCT_OUT)/META-INF/com/google/android/
	@mkdir -p $(PRODUCT_OUT)/META-INF/com/android/
	@echo "InstallCarrierUpdateBinary = $(InstallCarrierUpdateBinary)"
	@cp "$(InstallCarrierUpdateBinary)" "$(PRODUCT_OUT)/META-INF/com/google/android/"
	@mv "$(PRODUCT_OUT)/META-INF/com/google/android/updater" "$(PRODUCT_OUT)/META-INF/com/google/android/update-binary"
	@rm -rf $(PRODUCT_OUT)/META-INF/com/android/metadata
	@touch $(PRODUCT_OUT)/META-INF/com/android/metadata
	@while read line ; do \
          if [ "$${line/#mount\(/}" != "$$line" ] ; then \
            line="$${line/#mount\(\"/}" ;\
            line="$${line//\",\"/ }" ;\
            line="$${line//\"\)\;/}" ;\
            arrCfg=($$line) ;\
            while read item ; do \
              if [ "$${item/#\//}" != "$$item" ] ; then\
                itemCfg=($$item) ; \
                if [ "$${arrCfg[3]}" == "$${itemCfg[1]}" ] ; then \
                  echo "mount(\"$${itemCfg[2]}\",\"EMMC\",\"$${itemCfg[0]}\",\"$${itemCfg[1]}\");" >> $(PRODUCT_OUT)/META-INF/com/google/android/updater-script ;\
                fi ;\
              fi ;\
            done < $(FSTAB_DIR)/fstab.qcom ;\
          else \
            echo $$line >> $(PRODUCT_OUT)/META-INF/com/google/android/updater-script ;\
          fi \
        done < $(InstallCarrierPathCarrier)/updater-script
	@rm -rf $(PRODUCT_OUT)/ota.zip
	@cd $(PRODUCT_OUT) ;zip -r ota.zip META-INF/com/google/android/* ;cd -
	@rm -rf $(PRODUCT_OUT)/system/vendor/speccfg/ota.zip
	@java -Xmx2048m -Djava.library.path=$(SIGNAPK_JNI_LIBRARY_PATH) -jar $(InstallCarrierSignTool) -w $(InstallCarrierSignPerm) $(InstallCarrierSignKey) $(PRODUCT_OUT)/ota.zip $(PRODUCT_OUT)/system/vendor/speccfg/ota.zip
	@echo "### 2. Make OTA Package list for $(InstallCarrier_OTASourcePackList)"
	@rm -rf $(PRODUCT_OUT)/META-INF/com/google/android/updater-script
	@if [ "$(USES_REGIONALIZATION_ENVIRONMENT)" == "true" ] ; then \
           if [ "$(USES_REGIONALIZATION_PARTITIONS)" != "" ] && [ "$(USES_REGIONALIZATION_PARTITIONS)" != "system" ] ; then \
             for partition in $(USES_REGIONALIZATION_PARTITIONS) ; do \
               if [ "$$partition" == "oem" ] ; then \
                 break ;\
               fi ;\
             done ;\
             awk '{gsub("<partition>","'$$partition'"); print $$0 > "temp"}' $(InstallCarrierPathCarrier)/partition-updater-script ;\
             mv temp $(PRODUCT_OUT)/META-INF/com/google/android/updater-script ;\
           fi ;\
         fi
	@if [ ! -f "$(PRODUCT_OUT)/META-INF/com/google/android/updater-script" ] ; then \
           while read line ; do \
             if [ "$${line/#mount\(/}" != "$$line" ] ; then \
               line="$${line/#mount\(\"/}" ;\
               line="$${line//\",\"/ }" ;\
               line="$${line//\"\)\;/}" ;\
               arrCfg=($$line) ;\
               while read item ; do \
                 if [ "$${item/#\//}" != "$$item" ] ; then\
                   itemCfg=($$item) ; \
                   if [ "$${arrCfg[3]}" == "$${itemCfg[1]}" ] ; then \
                     echo "mount(\"$${itemCfg[2]}\",\"EMMC\",\"$${itemCfg[0]}\",\"$${itemCfg[1]}\");" >> $(PRODUCT_OUT)/META-INF/com/google/android/updater-script ;\
                   fi ;\
                 fi ;\
               done < $(FSTAB_DIR)/fstab.qcom ;\
             else \
               echo $$line >> $(PRODUCT_OUT)/META-INF/com/google/android/updater-script ;\
             fi \
           done < $(InstallCarrierPathCarrier)/regional-updater-script ;\
         fi
	@mkdir -p $(InstallCarrierPathSpec)
	@for item in $(InstallCarrier_OTASourcePackList) ; do \
           if [[ "$$item" =~ "_" ]] ; then \
             continue ;\
           fi ;\
           cd $(PRODUCT_OUT);\
           echo "strNewSpec=$$item" > system/vendor/speccfg/spec.new ;\
           rm -rf $$item.zip ;\
           finger=`cat system/build.prop | grep ro.build.fingerprint`;\
           device=`cat system/build.prop | grep ro.product.device=`;\
           timestamp=`cat system/build.prop | grep ro.build.date.utc`;\
           echo "post-build="${finger#*=} > $(PRODUCT_OUT)/META-INF/com/android/metadata;\
           echo "pre-device="${device#*=} >> $(PRODUCT_OUT)/META-INF/com/android/metadata;\
           echo "post-timestamp="${timestamp#*=} >> $(PRODUCT_OUT)/META-INF/com/android/metadata;\
           echo "create file metadata done!";\
           zip -r $$item.zip system/vendor/$$item/* system/vendor/$$item/.preloadspec META-INF/* system/vendor/speccfg/spec.new ;\
           cd - ;\
           java -Xmx2048m -Djava.library.path=$(SIGNAPK_JNI_LIBRARY_PATH) -jar $(InstallCarrierSignTool) -w $(InstallCarrierSignPerm) $(InstallCarrierSignKey) $(PRODUCT_OUT)/$$item.zip $(PRODUCT_OUT)/$$item.ota.zip ;\
           rm $(PRODUCT_OUT)/$$item.zip ;\
        done
	@rm -rf $(PRODUCT_OUT)/META-INF
endif
#	@echo "### 3. Install the packages $(InstallCarrier_MainPackList)"
	@rm -rf "$(TARGET_OUT_VENDOR)/app/google"
	@if [ "$(USES_REGIONALIZATION_ENVIRONMENT)" == "true" ] ; then \
           if [ -f "$(RegionalizationEnvPathSpec)/partition" ] ; then \
             rm $(RegionalizationEnvPathSpec)/partition ;\
           fi ;\
           touch $(RegionalizationEnvPathSpec)/partition ;\
           for partition in $(USES_REGIONALIZATION_PARTITIONS) ; do \
             echo "" > $(TARGET_FILES)/temp/$$partition"list" ;\
             if [ "$$partition" != "system" ] ; then \
               rm -rf $(PRODUCT_OUT)/$$partition ;\
               rm -rf $(PRODUCT_OUT)/$$partition".img" ;\
               mkdir -p $(PRODUCT_OUT)/$$partition ;\
               echo "/$$partition" >> $(RegionalizationEnvPathSpec)/partition ;\
             fi ;\
           done ;\
           count=0; for pack in $(InstallCarrier_MainPackList) ; do \
             if [ "$${pack//\//}" != "$$pack" ] ; then \
               item="$${pack//\// }" ;\
               item=($$item) ;\
               packname="$${item[0]}" ;\
               partition="$${item[1]}" ;\
               if [ -d "$(GENERATED_PACKAGE_PATH)/$$packname" ] && [ -d "$(PRODUCT_OUT)/$$partition" ] ; then \
                 cp -rf $(GENERATED_PACKAGE_PATH)/$$packname $(PRODUCT_OUT)/$$partition/ ;\
                 echo "true" > $(PRODUCT_OUT)/$$partition/flag ;\
                 storagePos="/$$partition" ;\
                 echo "$$packname" >> $(TARGET_FILES)/temp/$$partition"list" ;\
               else \
                 echo "Error: Install main pack $$packname to partition $$partition failed !" ;\
                 echo "Error: There is no pack $$packname or partition $$partition !" ;\
                 exit 1 ;\
               fi ;\
             else \
               if [ -d "$(GENERATED_PACKAGE_PATH)/$$pack" ] ; then \
                 storagePos="$(PRESET_PACKAGE_PATH)" ;\
                 echo "$$pack" >> $(TARGET_FILES)/temp/systemlist ;\
               else \
                 echo "Error: Install main pack $$pack to /system/vendor failed !" ;\
                 echo "Error: There is no pack $$pack !" ;\
                 exit 1 ;\
               fi ;\
             fi ;\
             let "count+=1" ;\
           done ;\
           echo "packStorage=$$storagePos" > $(RegionalizationEnvPathSpec)/spec ;\
           echo "packCount=$$count" >> $(RegionalizationEnvPathSpec)/spec ;\
           echo -e "#POINT TO REGIONALIZATION PACK'S VENDOR PROP PATHS#\n" > $(RegionalizationEnvPathSpec)/vendor_ro.prop ;\
           echo -e "" > $(RegionalizationEnvPathSpec)/submask ;\
           echo -e "#POINT TO REGIONALIZATION PACK'S VENDOR PROP PATHS#" > $(RegionalizationEnvPathSpec)/vendor_persist.prop ;\
           x=1; for pack in $(InstallCarrier_MainPackList) ; do \
             if [ "$${pack//\//}" != "$$pack" ] ; then \
               item="$${pack//\// }" ;\
               item=($$item) ;\
               packname="$${item[0]}" ;\
               echo -e "import $$storagePos/$$packname/system/vendor/vendor.prop" >> $(RegionalizationEnvPathSpec)/vendor_persist.prop ;\
               echo "strSpec$$x=$$packname" >> $(RegionalizationEnvPathSpec)/spec ;\
             else \
               echo -e "import $$storagePos/$$pack/system/vendor/vendor.prop" >> $(RegionalizationEnvPathSpec)/vendor_persist.prop ;\
               echo "strSpec$$x=$$pack" >> $(RegionalizationEnvPathSpec)/spec ;\
             fi ;\
             let "x+=1" ;\
           done ;\
           while read line; do \
             if [ "$${line:0:1}" != "#" ] ; then \
               sed -i "2i$$line" $(RegionalizationEnvPathSpec)/vendor_ro.prop ;\
             fi ;\
           done < $(RegionalizationEnvPathSpec)/vendor_persist.prop ;\
           echo "ro.regionalization.support=$(USES_REGIONALIZATION_ENVIRONMENT)" >> $(GENERATED_PACKAGE_PATH)/Default/system/vendor/$(DEFAULT_PROP) ;\
           cp -rf $(GENERATED_PACKAGE_PATH)/Default/system/vendor/$(DEFAULT_PROP) $(GENERATED_PACKAGE_PATH)/ ;\
           rm -rf $(PRODUCT_OUT)/data/switch_spec ;\
         else \
           echo "Default" > $(TARGET_FILES)/temp/systemlist ;\
           count=0; for item in $(InstallCarrier_MainPackList) ; do \
             let "count+=1" ;\
           done ;\
           echo "newPackCount=$$count" > $(TARGET_OUT_VENDOR)/speccfg/spec.new ;\
           x=1; for item in $(InstallCarrier_MainPackList) ; do \
             echo "$(GENERATED_PACKAGE_PATH)/$$item" ;\
             if [ ! -d "$(GENERATED_PACKAGE_PATH)/$$item" ] ; then \
               echo "Error: Install main pack $$item to /system/vendor failed !" ;\
               echo "Error: There is no pack $$item !" ;\
               exit 1 ;\
             fi ;\
             echo "strNewSpec$$x=$$item" >> $(TARGET_OUT_VENDOR)/speccfg/spec.new ;\
             echo "$$item" >> $(TARGET_FILES)/temp/systemlist ;\
             let "x+=1" ;\
           done ;\
           if [ "vendorisdisabled" == "true" ] ; then \
               /bin/bash $(InstallCarrierPathCarrier)/switch_spec.sh $(GENERATED_PACKAGE_PATH) $(PRODUCT_OUT) $(PRODUCT_OUT) $(InstallCarrierPathSpec)/spec ;\
           fi;\
           cp -rf $(GENERATED_PACKAGE_PATH)/Default/system/vendor/$(DEFAULT_PROP) $(GENERATED_PACKAGE_PATH)/ ;\
         fi
	@mkdir -p $(InstallCarrierPathSpec)
#	@echo "### 4. Get all preset package list ..."
	@for pack in $(InstallCarrier_PresetPackList) ; do \
           if [ "$${pack//\//}" != "$$pack" ] ; then \
             item="$${pack//\// }" ;\
             item=($$item) ;\
             packname="$${item[0]}" ;\
             partition="$${item[1]}" ;\
             if [ -d "$(GENERATED_PACKAGE_PATH)/$$packname" ] && [ -d "$(PRODUCT_OUT)/$$partition" ] ; then \
               cp -rf $(GENERATED_PACKAGE_PATH)/$$packname $(PRODUCT_OUT)/$$partition/ ;\
               echo "true" > $(PRODUCT_OUT)/$$partition/flag ;\
               echo "$$packname" >> $(TARGET_FILES)/temp/$$partition"list" ;\
             else \
               echo "Error: Preset pack $$packname to partition $$partition failed !" ;\
               echo "Error: There is no pack $$packname or partition $$partition !" ;\
               exit 1 ;\
             fi ;\
           else \
             echo "$$pack" >> $(TARGET_FILES)/temp/systemlist ;\
           fi ;\
         done
	@if [ "$(TARGET_PACKAGE_ISSUPPORT_QGP)" == "false" ] ; then \
        rm -rf $(TARGET_FILES)/temp/systemlist ;\
        for pack in $(INSTALLCARRIER_INCLUDELIST) ; do \
           if [ "$${pack//\//}" != "$$pack" ] ; then \
             item="$${pack//\// }" ;\
             item=($$item) ;\
             packname="$${item[0]}" ;\
             partition="$${item[1]}" ;\
             if [ -d "$(GENERATED_PACKAGE_PATH)/$$packname" ] && [ -d "$(PRODUCT_OUT)/$$partition" ] ; then \
               cp -rf $(GENERATED_PACKAGE_PATH)/$$packname $(PRODUCT_OUT)/$$partition/ ;\
               echo "true" > $(PRODUCT_OUT)/$$partition/flag ;\
               echo "$$packname" >> $(TARGET_FILES)/temp/$$partition"list" ;\
             else \
               echo "Error: Preset pack $$packname to partition $$partition failed !" ;\
               echo "Error: There is no pack $$packname or partition $$partition !" ;\
               exit 1 ;\
             fi ;\
           else \
             echo "$$pack" >> $(TARGET_FILES)/temp/systemlist ;\
           fi ;\
         done;\
	fi
	@if [ "$(USES_REGIONALIZATION_ENVIRONMENT)" == "true" ] ; then \
		   if [ "true" == "false" ] ; then \
            echo "### 5. Link libs for app and Preset 3rd apks to data/app-regional/ ..." ;\
		   fi;\
           for partition in $(USES_REGIONALIZATION_PARTITIONS) ; do \
             if [ "$$partition" == "system" ] ; then \
               storagePos="$(PRESET_PACKAGE_PATH)" ;\
             else \
               storagePos="$$partition" ;\
             fi ;\
             while read line; do \
               if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/app" ] ; then \
                 for app in `ls $(PRODUCT_OUT)/$$storagePos/$$line/system/app` ; do \
                   if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/app/$$app" ] ; then \
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib" ] ; then \
                       ln -s /$$storagePos/$$line/system/lib $(PRODUCT_OUT)/$$storagePos/$$line/system/app/$$app/lib ;\
                     fi ;\
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib64" ] ; then \
                       ln -s /$$storagePos/$$line/system/lib64 $(PRODUCT_OUT)/$$storagePos/$$line/system/app/$$app/lib64 ;\
                     fi ;\
                   elif [ -f "$(PRODUCT_OUT)/$$storagePos/$$line/system/app/$$app" ] ; then \
                     appname="$${app%.apk}" ;\
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib" ] ; then \
                       mkdir -p $(PRODUCT_OUT)/$$partition/lib ;\
                       ln -s /$$storagePos/$$line/system/lib $(PRODUCT_OUT)/$$partition/lib/$$appname ;\
                     fi ;\
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib64" ] ; then \
                       mkdir -p $(PRODUCT_OUT)/$$partition/lib64 ;\
                       ln -s /$$storagePos/$$line/system/lib64 $(PRODUCT_OUT)/$$partition/lib64/$$appname ;\
                     fi ;\
                   fi ;\
                 done ;\
               fi ;\
               if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/priv-app" ] ; then \
                 for app in `ls $(PRODUCT_OUT)/$$storagePos/$$line/system/priv-app` ; do \
                   if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/priv-app/$$app" ] ; then \
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib" ] ; then \
                       ln -s /$$storagePos/$$line/system/lib $(PRODUCT_OUT)/$$storagePos/$$line/system/priv-app/$$app/lib ;\
                     fi ;\
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib64" ] ; then \
                       ln -s /$$storagePos/$$line/system/lib64 $(PRODUCT_OUT)/$$storagePos/$$line/system/priv-app/$$app/lib64 ;\
                     fi ;\
                   elif [ -f "$(PRODUCT_OUT)/$$storagePos/$$line/system/priv-app/$$app" ] ; then \
                     appname="$${app%.apk}" ;\
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib" ] ; then \
                       mkdir -p $(PRODUCT_OUT)/$$partition/lib ;\
                       ln -s /$$storagePos/$$line/system/lib $(PRODUCT_OUT)/$$partition/lib/$$appname ;\
                     fi ;\
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib64" ] ; then \
                       mkdir -p $(PRODUCT_OUT)/$$partition/lib64 ;\
                       ln -s /$$storagePos/$$line/system/lib64 $(PRODUCT_OUT)/$$partition/lib64/$$appname ;\
                     fi ;\
                   fi ;\
                 done ;\
               fi ;\
               if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/data/app" ] ; then \
                 for app in `ls $(PRODUCT_OUT)/$$storagePos/$$line/data/app` ; do \
                   if [ -f "$(PRODUCT_OUT)/$$storagePos/$$line/data/app/$$app" ] ; then \
                     appname="$${app%.apk}" ;\
                     if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/system/lib" ] ; then \
                       mkdir -p $(PRODUCT_OUT)/data/app-lib ;\
                       ln -s /$$storagePos/$$line/system/lib $(PRODUCT_OUT)/data/app-lib/$$appname ;\
                     fi ;\
                   fi ;\
                 done ;\
               fi ;\
               partitionSize=0 ;\
               packsSize=0 ;\
               if [ "$$partition" == "oem" ] ; then \
                 partitionSize=`awk 'BEGIN { printf ("%.0f\n",'$(BOARD_OEMIMAGE_PARTITION_SIZE)'-('$(BOARD_OEMIMAGE_PARTITION_SIZE)'/32)) }'` ;\
               fi ;\
               if [ -d "$(PRODUCT_OUT)/$$storagePos/$$line/data/app" ] ; then \
                 if [ "`ls -A $(PRODUCT_OUT)/$$storagePos/$$line/data/app`" != "" ] ; then \
                   mkdir -p $(PRODUCT_OUT)/data/app-regional/$$line ;\
                   cp -rfv $(PRODUCT_OUT)/$$storagePos/$$line/data/app/* $(PRODUCT_OUT)/data/app-regional/$$line ;\
                   packsSize=`du -sb $(PRODUCT_OUT)/$$partition | cut -f1` ;\
                   if [ $$partitionSize -gt 0 ] && [ $$packsSize -gt $$partitionSize ] ; then \
                     echo "Remove the 3rd apks in package to save partition size ..." ;\
                     rm -rfv $(PRODUCT_OUT)/$$storagePos/$$line/data/app ;\
                   fi ;\
                 fi ;\
               fi ;\
               packsSize=`du -sb $(PRODUCT_OUT)/$$partition | cut -f1` ;\
               if [ $$partitionSize -gt 0 ] && [ $$packsSize -gt $$partitionSize ] ; then \
                 echo "Remove the oat in package to save partition size ..." ;\
                 rm -rfv `find $(PRODUCT_OUT)/$$storagePos/$$line -name "oat" -type d` ;\
               fi ;\
             done < $(TARGET_FILES)/temp/$$partition"list" ;\
           done ;\
		   if [ "true" == "false" ] ; then \
		      echo "### 6. Generate the regionalization partition images ..." ;\
		   fi;\
           for partition in $(USES_REGIONALIZATION_PARTITIONS) ; do \
             if [ "$$partition" != "system" ] ; then \
               content=`cat $(TARGET_FILES)/temp/$$partition"list"` ;\
               if [ "$$content" == "" ] ; then \
			     if [ "true" == "false" ] ; then \
				   echo -e "Preset pack list is null, no need to generate $${partition}.img ..." ;\
				 fi;\
               else \
                 echo -e "===> Generate $${partition}.img ..." ;\
                 make showcommands $$partition"_image" ;\
                 if [ $$? -eq 0 ] ; then \
                   echo "Generate $${partition}.img success !" ;\
                 else \
                   echo -e "Error: Can't generate $${partition}.img using \"make showcommands $${partition}_image\" !" ;\
                   echo -e "Error: Please check if support $$partition partition !" ;\
                   exit 1 ;\
                 fi ;\
               fi ;\
             fi ;\
           done ;\
         fi
#@echo "checking optimization speccfg file under persist patition"
	@if [ $(TARGET_ISREDUCED_SPECFG_FILE) == true ];then \
	  rm $(RegionalizationEnvPathSpec)/devicetype ;\
	  rm $(RegionalizationEnvPathSpec)/mbnversion ;\
	  rm $(RegionalizationEnvPathSpec)/.not_triggered ;\
      rm $(RegionalizationEnvPathSpec)/submask ;\
	  rm $(RegionalizationEnvPathSpec)/partition ;\
      if [ "$(InstallCarrier_MainPackList)" == "Default" ] ; then \
	     rm $(RegionalizationEnvPathSpec)/vendor_persist.prop ;\
	     rm $(RegionalizationEnvPathSpec)/vendor_ro.prop ;\
         x=1; for pack in $(InstallCarrier_MainPackList) ; do \
             if [ "$${pack//\//}" != "$$pack" ] ; then \
               item="$${pack//\// }" ;\
               item=($$item) ;\
               packname="$${item[0]}" ;\
               echo "vendor_persist=import $(PRESET_PACKAGE_PATH)/$$packname/system/vendor/vendor.prop" >> $(RegionalizationEnvPathSpec)/spec ;\
               echo "vendor_ro=import $(PRESET_PACKAGE_PATH)/$$packname/system/vendor/vendor.prop" >> $(RegionalizationEnvPathSpec)/spec ;\
             else \
               echo "vendor_persist=import $(PRESET_PACKAGE_PATH)/$$pack/system/vendor/vendor.prop" >> $(RegionalizationEnvPathSpec)/spec ;\
               echo "vendor_ro=import $(PRESET_PACKAGE_PATH)/$$pack/system/vendor/vendor.prop" >> $(RegionalizationEnvPathSpec)/spec ;\
             fi ;\
             let "x+=1" ;\
           done ;\
           echo "devicetype=omt" >> $(RegionalizationEnvPathSpec)/spec ;\
    fi;\
	fi

#	@echo "### 7. Try to remove outside package list ..."
clean_outside_packlist:build_oem_carrier_image
	@if [ -f "$(TARGET_FILES)/temp/packlist" ] ;then \
	    rm -rf $(TARGET_FILES)/temp/packlist ;\
	fi
	@for path in `find $(GENERATED_PACKAGE_PATH) -name ".preloadspec"` ; do \
           tmp="$${path%\/.preloadspec}" ;\
           pack="$${tmp##*\/}" ;\
           if [ "$$pack" != "" ] ; then \
             echo "Back up $$pack to $(TARGET_FILES)/temp/$$pack ..." ;\
             echo "$$pack" >> "$(TARGET_FILES)/temp/packlist" ;\
             cp -rf $(GENERATED_PACKAGE_PATH)/$$pack $(TARGET_FILES)/temp ;\
           fi ;\
         done
	@while read line; do \
           flag="" ;\
           while read pack; do \
             if [ "$$pack" == "$$line" ] ; then \
                 flag="keep" ;\
                 break ;\
             fi ;\
           done < $(TARGET_FILES)/temp/systemlist ;\
           if [ "$$flag" != "keep" ] ; then \
		     if [ "true" == "false" ] ;  then \
			  echo "packlist:Remove $(GENERATED_PACKAGE_PATH)/$$line ..." ;\
			 fi;\
             rm -rf $(GENERATED_PACKAGE_PATH)/$$line ;\
           fi ;\
           done < $(TARGET_FILES)/temp/packlist
	@if [ -f "$(TARGET_FILES)/temp/cleanlist" ] ; then \
	   while read line; do \
           flag="" ;\
           while read pack; do \
             if [ "$$pack" == "$$line" ] ; then \
                 flag="keep" ;\
                 break ;\
             fi ;\
           done < $(TARGET_FILES)/temp/systemlist ;\
           if [ "$$flag" != "keep" ] ; then \
		    if [ "true" == "false" ]; then\
			 echo "cleanlist:Remove $(GENERATED_PACKAGE_PATH)/$$line ..." ;\
			fi;\
            rm -rf $(GENERATED_PACKAGE_PATH)/$$line ;\
           fi ;\
           done < $(TARGET_FILES)/temp/cleanlist ;\
	fi
	@echo "################### End Carrier Installation ######################"


ifeq ($(call is-vendor-board-platform,QCOM),true)
$(InstallCarrierSysImage): clean_outside_packlist

ifeq ($(USES_REGIONALIZATION_ENVIRONMENT),true)
$(INSTALLED_PERSISTIMAGE_TARGET):InstallCarrier
endif

endif

packageFiles: systemimage notice_files
InstallPackage: packageFiles target-files-package
	@if [ "$(InstallCarrier_MainPackList)" == "Default" ] ; then \
           cp -f $(BUILT_TARGET_FILES_PACKAGE) $(TARGET_FILES)/Default.zip ;\
         fi
	@echo "######Generate OTA Target Files start...######"
	@if [ -f "$(TARGET_FILES)/Default.zip" ] ; then \
           cp -f $(InstallCarrierPathCarrier)/switch_spec.sh $(TARGET_FILES)/switch_spec.sh ;\
           cp -f $(HOST_OUT_EXECUTABLES)/fs_config $(TARGET_FILES)/fs_config ;\
           mkdir -p $(TARGET_FILES)/DEFAULT ;\
           for item in $(InstallCarrier_OTABinaryPackList) ; do \
             echo "###Generate $$item target files...###" ;\
             cd $(TARGET_FILES)/DEFAULT ;\
             rm -rf ./* ;\
             unzip -o ../Default.zip -d ./ ;\
             rm -rf IMAGES ;\
             mv SYSTEM system ;\
             mv DATA data ;\
             echo -e "persist.radio.blockbased=true\n" >> $(BuildPropFileName) ;\
             x=0 ;\
             list="$${item//_/ }" ;\
             for pack in $$list ; do \
               if [ -d "../temp/$$pack" ] ; then \
                 if [ -d "system/vendor/$$pack" ] ; then \
                   rm -rf system/vendor/$$pack ;\
                 fi ;\
                 cp -rf ../temp/$$pack system/vendor/ ;\
               else \
                 echo "Generate $$item target files failed: There is no pack for $$pack !!!" ;\
                 exit 1 ;\
               fi ;\
               let "x+=1" ;\
               echo "strNewSpec$$x=$$pack" >> system/vendor/speccfg/temp ;\
             done ;\
             echo "newPackCount=$$x" > system/vendor/speccfg/spec.new ;\
             cat system/vendor/speccfg/temp >> system/vendor/speccfg/spec.new ;\
             rm -f system/vendor/speccfg/temp ;\
             /bin/bash ../switch_spec.sh system/vendor ./ ./ system/vendor/speccfg/spec ;\
             mv system SYSTEM ;\
             mv data DATA ;\
             zip -qry ../$$item.zip ./* ;\
             cd - ;\
             zipinfo -1 $(TARGET_FILES)/$$item.zip | awk 'BEGIN { FS="SYSTEM/" } /^SYSTEM\// {print "system/" $$2}' | $(HOST_OUT_EXECUTABLES)/fs_config -C -D $(TARGET_OUT) -S $(TARGET_FILES)/DEFAULT/BOOT/RAMDISK/file_contexts > $(TARGET_FILES)/DEFAULT/META/filesystem_config.txt ;\
             cd $(TARGET_FILES)/DEFAULT ;\
             zip ../$$item.zip -r META/filesystem_config.txt ;\
             cd - ;\
             ./build/tools/releasetools/add_img_to_target_files -v $(TARGET_FILES)/$$item.zip ;\
           done ;\
           rm -f $(TARGET_FILES)/switch_spec.sh ;\
           rm -f $(TARGET_FILES)/fs_config ;\
           rm -rf $(TARGET_FILES)/DEFAULT ;\
         fi
	@echo "######Generate OTA Target Files end...######"
	@echo "######Generate OTA Binary Packs start...######"
	@if [ -f "$(TARGET_FILES)/Default.zip" ] ; then \
           mkdir -p $(BINARY_PACKS)/META-INF/com/google/android ;\
           cp -f $(InstallCarrierUpdateBinary) $(BINARY_PACKS)/META-INF/com/google/android/ ;\
           cp -f $(InstallCarrierWrapperUpdateBinary) $(BINARY_PACKS)/META-INF/com/google/android/ ;\
           mv $(BINARY_PACKS)/META-INF/com/google/android/updater $(BINARY_PACKS)/META-INF/com/google/android/ori-update-binary ;\
           mv $(BINARY_PACKS)/META-INF/com/google/android/wrapper-updater $(BINARY_PACKS)/META-INF/com/google/android/update-binary ;\
           for item in $(InstallCarrier_OTABinaryPackList) ; do \
             ./build/tools/releasetools/ota_from_target_files --block -k $(DEFAULT_SYSTEM_DEV_CERTIFICATE) -vw -i $(TARGET_FILES)/Default.zip $(TARGET_FILES)/$$item.zip $(BINARY_PACKS)/$$item.zip ;\
             ./build/tools/releasetools/ota_from_target_files --block -k $(DEFAULT_SYSTEM_DEV_CERTIFICATE) -vw -i $(TARGET_FILES)/$$item.zip $(TARGET_FILES)/Default.zip $(BINARY_PACKS)/$$item"2Default.zip" ;\
             \
             mkdir -p $(BINARY_PACKS)/system/vendor/speccfg ;\
             mkdir -p $(BINARY_PACKS)/data/modem_config ;\
             mkdir -p $(BINARY_PACKS)/data/modem_row_config ;\
             x=0 ;\
             list="$${item//_/ }" ;\
             for pack in $$list ; do \
               let "x+=1" ;\
               echo "strNewSpec$$x=$$pack" >> $(BINARY_PACKS)/system/vendor/speccfg/temp ;\
               if [ -f "$(TARGET_FILES)/temp/$$pack/.preloadspec" ] ; then \
                 cp -f $(TARGET_FILES)/temp/$$pack/.preloadspec $(BINARY_PACKS)/.preloadspec ;\
               fi ;\
               if [ -d "$(TARGET_FILES)/temp/$$pack/data/modem_config" ] ; then \
                 cp -rf $(TARGET_FILES)/temp/$$pack/data/modem_config/* $(BINARY_PACKS)/data/modem_config ;\
               fi ;\
               if [ -d "$(TARGET_FILES)/temp/$$pack/data/modem_row_config" ] ; then \
                 cp -rf $(TARGET_FILES)/temp/$$pack/data/modem_row_config/* $(BINARY_PACKS)/data/modem_row_config ;\
               fi ;\
             done ;\
             cd $(BINARY_PACKS) ;\
             echo "newPackCount=$$x" > system/vendor/speccfg/spec.new ;\
             cat system/vendor/speccfg/temp >> system/vendor/speccfg/spec.new ;\
             rm -f system/vendor/speccfg/temp ;\
             mkdir -p system/vendor/$$item ;\
             mv .preloadspec system/vendor/$$item/ ;\
             zip $$item.zip -ry system/ data/ META-INF/ ;\
             echo "newPackCount=1" > system/vendor/speccfg/spec.new ;\
             echo "strNewSpec1=Default" >> system/vendor/speccfg/spec.new ;\
             mv system/vendor/$$item system/vendor/$$item"2Default" ;\
             if [ -f "../system/vendor/Default/.preloadspec" ] ; then \
               cp -f ../system/vendor/Default/.preloadspec system/vendor/$$item"2Default"/ ;\
             fi ;\
             zip $$item"2Default.zip" -ry system/ META-INF/ data/modem_row_config/ ;\
             rm -rf system data ;\
             cd - ;\
             \
             java -Xmx2048m -Djava.library.path=$(SIGNAPK_JNI_LIBRARY_PATH) -jar $(InstallCarrierSignTool) -w $(InstallCarrierSignPerm) $(InstallCarrierSignKey) $(BINARY_PACKS)/$$item.zip $(BINARY_PACKS)/$$item.ota.zip ;\
             rm -f $(BINARY_PACKS)/$$item.zip ;\
             java -Xmx2048m -Djava.library.path=$(SIGNAPK_JNI_LIBRARY_PATH) -jar $(InstallCarrierSignTool) -w $(InstallCarrierSignPerm) $(InstallCarrierSignKey) $(BINARY_PACKS)/$$item"2Default.zip" $(BINARY_PACKS)/$$item"2Default.ota.zip" ;\
             rm -f $(BINARY_PACKS)/$$item"2Default.zip" ;\
           done ;\
           rm -rf $(BINARY_PACKS)/META-INF ;\
         fi
	@rm -rf $(TARGET_FILES)/temp
	@echo "######Generate OTA Binary Packs end!######"
	@cp $(BUILT_TARGET_FILES_PACKAGE) $(PRODUCT_OUT)/package_backup.zip
endif
#ifeq ($(call is-vendor-board-platform,QCOM),true)
#droidcore: InstallPackage
#endif
