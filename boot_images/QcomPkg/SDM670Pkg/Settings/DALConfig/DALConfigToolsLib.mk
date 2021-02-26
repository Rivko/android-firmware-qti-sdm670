# @file DALConfigToolsLib.mk
#
#  DAL configuration Tools library Custom Makefile
#
#  Copyright (c) 2015-2017, Qualcomm Technologies, Inc.
#  All rights reserved.
#  Qualcomm Confidential and Proprietary
#
#

#==============================================================================
#                              EDIT HISTORY
#
#  $Header: $
#  $DateTime: $
#  $Author:  $
#
# when       who     what, where, why
# --------   ---     ----------------------------------------------------------
# 04/26/17   vk      Use qti extension for include  
# 10/09/16   kpa     remove usb xml till tech team ports the driver
# 02/23/16   ap      Added dependency fix for C Config files
# 12/15/15   bh      Settings relocation
# 03/12/15   ap      Added DALConfigToolsLib.mk for DALConfigToolsLib.inf
#
#==============================================================================



#=======================================README===========================================
# Device Config user adding new xml or updating existing xml need to update
# modify the following variables: 
# 1]  INC_DEPS
# 2]  XML_DEPS
#
# XML_DEPS need to be modified if a user adds a config xml in DALConfigLib 
# folder of any of its subdirectories. The proper path of the xml file needs 
# to be appended to the variable XML_DEPS
#
# INC_DEPS need to be modified if the added config xml file includes (#include directive)
# any header file(s). Proper paths of the header files need to be appended to
# the variable INC_DEPS
# Nothing else should be modified in the file. 
#
# Any xxxx.dec Package entries meant to be added in DALConfigToolsLib.inf have to be
# now added to the DALGenLibTools.inf
#======================================================================================== 



MACRO_NAME              =  tools

#             
# Following macro i.e. INC_DEPS lists all the header files that
# are included in the xml files. Any new header file added in the 
# config xml files need to be appended to the following list     
#
INC_DEPS = $(WORKSPACE)/QcomPkg/Include/api/dal/*.h \
           $(WORKSPACE)/QcomPkg/SDM670Pkg/Settings/DALConfig/*.h \
 
#             
# Following macro i.e. XML_DEPS lists all the xml config files that
# It also enlists any xml files which are #included by other xml files.
# Any new xml file added will need to be appended to the following list.    
#                           
# __FIX__SDM845:  uncomment usb xml when fixed
XML_DEPS = $(WORKSPACE)/QcomPkg/SDM670Pkg/Settings/DALSystem/$(MACRO_NAME)/dalsystem_$(MACRO_NAME).xml \
           

#=====================USERS NEED NOT EDIT ANYTHING BEYOND THIS LINE======================== 



 
#
# Build Macro
#
OBJECT_FILES =  \
    $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj

    
         
INC = @$(MODULE_BUILD_DIR)/../DALGenLibTools/OUTPUT/inc_list.qti
    
OBJECT_FILES_LIST = $(OUTPUT_DIR)/object_files.lst


DALCONFIG_C        =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).c       
DALCONFIG_d        =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).d     
DALCONFIG_XML      =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).xml        
DALCONFIG_XML_PRE  =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME)_pre.xml
DALCONFIG_XML_OUT  =  $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME)_out.xml

#=======================================================================
#Include the dependency file for DALConfig_<MACRO>.c file
#If any files #included by the DALConfig C file are changed,
#Then DALConfig_<MACRO>.obj/lib and the dependency file are rebuilt.

#The first 3 lines below are targeted for NMAKE. GNUMake ignores them 
#thinking that they are comments. We exploit the following fact for this:
#"NMAKE doesn't recognize line continuation character in comments whereas Make does".
#In essence, to enforce GNU Make to bypass a line, use a trailing "# \" on the very
#preceeding line. 

#The next 3 lines are intended for GNUMake. We enforce NMAKE to bypass it by 
#using the "!IF 0" preprocessor directive. Here again we again use "# \" to bypass
#syntax meant for NMAKE i.e "!IF 0" and "!ENDIF".

# \
!IF EXIST($(DALCONFIG_d)) # \
!include $(DALCONFIG_d)   # \
!ENDIF # \

# \
!IF 0
-include $(DALCONFIG_d) # \
!ENDIF
      
#======================================================================      
#
# Individual Object Build Targets
#
$(OUTPUT_DIR)/DALConfigToolsLib.lib : $(OBJECT_FILES)
	"$(SLINK)" -cr -D $(OUTPUT_DIR)/DALConfigToolsLib.lib $(SLINK_FLAGS) $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj

$(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj : $(MAKE_FILE)
$(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj : $(COMMON_DEPS)
$(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj : $(DALCONFIG_C) $(DALCONFIG_d)
	"$(CC)" $(CC_FLAGS) -o $(OUTPUT_DIR)/DALConfig_$(MACRO_NAME).obj $(INC) $(DALCONFIG_C)

$(DALCONFIG_d) : $(DALCONFIG_C)
	( "$(CC)" $(CC_FLAGS) $(INC) -MM -MT "$(OBJECT_FILES) $(DALCONFIG_d)" $(DALCONFIG_C) -o $(DALCONFIG_d) )

$(DALCONFIG_C) : $(DALCONFIG_XML_OUT)
	( python $(WORKSPACE)/QcomPkg/Tools/propgen.py $(DALCONFIG_XML_OUT) $(OUTPUT_DIR) DALConfig_$(MACRO_NAME).c )
         
$(DALCONFIG_XML_OUT) :  $(DALCONFIG_XML_PRE)
	( python $(WORKSPACE)/QcomPkg/Tools/cleanup_xml.py -p $(DALCONFIG_XML_PRE) -o $(DALCONFIG_XML_OUT) -c $(DALCONFIG_C) )

$(DALCONFIG_XML_PRE) :  $(DALCONFIG_XML)
	( "$(CC)" -x assembler-with-cpp -E -o $(DALCONFIG_XML_PRE) -I$(WORKSPACE)/QcomPkg/Include/api/dal -I$(MODULE_DIR) $(INC)  $(DALCONFIG_XML) )

$(DALCONFIG_XML) :  $(INC_DEPS) $(XML_DEPS)
	( python $(WORKSPACE)/QcomPkg/Tools/cleanup_xml.py -d $(MODULE_DIR)/../DALSystem/$(MACRO_NAME) -i $(DALCONFIG_XML) -s dalsystem_$(MACRO_NAME).xml -m $(MACRO_NAME) $(XML_DEPS))


all: $(OUTPUT_DIR)/DALConfigToolsLib.lib    

#
# Build Flash Device Image
#
gen_fds:
	@"$(MAKE)" $(MAKE_FLAGS) -f $(BUILD_DIR)\Makefile fds
	@cd $(MODULE_BUILD_DIR)

#
# Target to update the FD
#

fds: mbuild gen_fds


#
# clean all intermediate files
#
clean:
	if exist $(OUTPUT_DIR) $(RD) $(OUTPUT_DIR)
	

#
# clean all generated files
#
cleanall:
	if exist $(DEBUG_DIR) $(RD) $(DEBUG_DIR)
	if exist $(OUTPUT_DIR) $(RD) $(OUTPUT_DIR)
	$(RM) *.pdb *.idb > NUL 2>&1
	$(RM) $(BIN_DIR)\$(MODULE_NAME).efi

#
# clean all dependent libraries built
#
cleanlib:
	@cd $(MODULE_BUILD_DIR)
