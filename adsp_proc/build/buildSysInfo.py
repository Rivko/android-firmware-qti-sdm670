#===============================================================================
#
# ADSP build system Info, flags and variables to control the build process 
#
# GENERAL DESCRIPTION
#    rules build script
#
# Copyright (c) 2014-2016 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
#
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/dspbuild.adsp/5.0/buildSysInfo.py#1 $
#  $DateTime: 2017/09/03 21:39:31 $
#  $Author: pwbldsvc $
#  $Change: 14261189 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who         what, where, why
# --------   -------     ---------------------------------------------------------
# 05/07/14   corinc      Initial creation
# 05/08/14   corinc      fixed the issue for alias
#===============================================================================
import os
import binascii
import struct




class defaultSysInfo:

#    global default_buildid; default_buildid = 0
    
    def __init__(self):
        self.default_buildid = 0
        self.default_chipset = 0
        self.default_pid = 0
        self.default_others = 0
        self.default_flags = 'GEN_SHARED_LIBS'
        self.default_verbose = 0
        self.default_numcore = 0
        self.default_module = 0
        self.default_alias = 0
        self.default_sconsargs = ''
        self.default_userargs = '' 
         
    def printDefaultInfo(self):
        print "Print printDefaultInfo\n" 
        print "default_buildid = %s\n"   % self.default_buildid
        print "default_chipset = %s\n" % self.default_chipset
        print "default_pid = %s\n"   % self.default_pid




class buildSysInfo:

#    global default_buildid; default_buildid = 0
    
    def __init__(self):
        self.all_flag = 0
        self.spd_flag = 0
        self.mpd_flag = 0
        self.clean_flag = 0
        self.sim_flag = 0
        self.pkg_all_flag = 0
        self.pkg_hy22_flag = 0
        self.pkg_hk11_flag = 0
        self.pkg_hk22_flag = 0
        self.pkg_oem_flag = 0
        self.pkg_hd11_flag = 0
        self.pkg_isv_flag = 0
        self.pkg_hcbsp_flag = 0
        self.pkg_hd22_flag = 0
        self.check_dsp_flag = 0
        self.other_option_flag = 0
        self.build_flags = 0
        self.build_filter_flag = 0
        self.build_verbose_flag = 0
        self.build_numcore_flag = 0
        self.image_alias_flag = 0
        self.build_sconsargs_flag = 0
        self.build_userargs_flag = 0

        self.buildid_flag = 0
        self.chipset_flag = 0
        self.chipset_file = 0
        self.protection_domain_flag = 0
         
        self.chipset_param = ''
        self.buildid_param = ''
        self.flags_param = ''
        self.opts_module = ''
        self.opts_verbose = ''
        self.opts_numcore = ''
        self.opts_sconsargs = ''
        self.opts_userargs = ''
        self.opts_alias = ''
        self.local_path = ''
        
        
    def printBuildSysInfo(self):
        print "Print from buildSysInfo.py\n" 
        print "mpd_flag = %s\n"   % self.mpd_flag
        print "buildid_flag = %s\n" % self.buildid_flag
        print "build_filter_flag = %s\n"   % self.build_filter_flag

 
 
 
 
 
	
	
