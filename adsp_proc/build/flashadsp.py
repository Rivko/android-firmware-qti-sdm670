#!/usr/local/bin/python
#===============================================================================
#
# Push ADSP image to Linux Android target
#
# GENERAL DESCRIPTION
#  Give ADSP image ID and will find build location and push adsp image to target
# 
# Usage:
#  flashadsp.py <BUILD_LOCATION>
# Example: 
#  flashadsp.py <Build_location>
#
#  $Header: //components/rel/dspbuild.adsp/5.0/flashadsp.py#6 $
#  
#  $Author: pwbldsvc $
#  $Change: 16325929 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when        who     what, where, why
# ----------  ---     ---------------------------------------------------------
# 05/02/2017 c_bpanwa  created the script and tested on 845 target.
# 03/12/2018 c_rcham   Chipset is default set to real_location + "\\obj\\qdsp6v5_ReleaseG\\" and Added some features
# 05/29/2018 c_rcham   Added Check Partition for dsp and firmware.
#===============================================================================
import os, re
import sys
import subprocess
import argparse

def runadbcommand(command, shell_val=True):
	try:
		info = subprocess.Popen(command, stdout= subprocess.PIPE, stderr=subprocess.STDOUT, shell=shell_val)
		print info.stdout.read()
	except subprocess.CalledProcessError:
		print "Subprocess called error" # handle errors in the called executable
	except OSError:
		print "os error" # executable not found
		
		
def flash_adsp_build(real_location, chipset):
	# adb commands 
	adb_wait = "adb wait-for-device"
	adb_root = "adb wait-for-device root"
	disable_verity = "adb wait-for-device disable-verity"
	reboot = "adb wait-for-device reboot"
	remount = "adb wait-for-device remount"
	runadbcommand(adb_root)
	runadbcommand(adb_wait)
	if not check_verity():
		runadbcommand(reboot)
	dsp_partition = "/vendor/dsp"
	firmware_partition = "/vendor/firmware_mnt"
	print "Waiting for adb device to come up"
	runadbcommand(adb_root)
	runadbcommand(remount)
	#checking dsp partiton
	if not check_partition(dsp_partition):
		dsp_partition="/dsp"
	#checking firmware partition
	if not check_partition(firmware_partition):
		firmware_partition="/firmware"
	print "dsp_partition = ",dsp_partition
	print "firmware_partition=",firmware_partition
	'''umount_firmware= "adb wait-for-device shell umount -lf %s"%firmware_partition
	firmware_remove = "adb wait-for-device shell rm -rf %s"%firmware_partition
	set_enforce_0 = "adb wait-for-device shell setenforce 0"
	mkdir_firmware = "adb wait-for-device shell mkdir %s"%firmware_partition
	chmod_firmware = "adb wait-for-device shell chmod 777 %s"%firmware_partition
	mount_modem = "adb wait-for-device shell mount -t vfat -o rw /dev/block/bootdevice/by-name/modem_a %s"%firmware_partition
	set_enforce_1 = "adb wait-for-device shell setenforce 1" '''
	remount_firmware = "adb wait-for-device shell mount -o rw,remount %s"%firmware_partition
	remount_dsp = "adb wait-for-device shell mount -o rw,remount %s"%dsp_partition
	rm_adsp_FI = "adb wait-for-device shell rm -rf %s/image/adsp.*"%firmware_partition
	#m_adsp_EF = "adb wait-for-device shell rm /etc/firmware/adsp*"
	rm_shared = "adb wait-for-device shell rm -rf %s/adsp/.*"%dsp_partition
	push_adsp_FI = "adb wait-for-device push " + real_location + "\\obj\\qdsp6v5_ReleaseG\\" + chipset + "\\signed\\LA\\system\\etc\\firmware\\. %s/image"%firmware_partition
	push_shared  = "adb wait-for-device push " + real_location + "\\build\\dynamic_modules\\" + chipset + "\\. %s/adsp"%dsp_partition
	push_json  = "adb wait-for-device push " + real_location + "\\build\\ms\\servreg\\" + chipset + "Q\\. %s/image"%firmware_partition
	sync_cmd = "adb wait-for-device shell sync"
	meta = "adb wait-for-device shell cat %s/verinfo/ver_info.txt"%firmware_partition
	adsp = 'adb wait-for-device shell "cd /sys/devices/soc0;echo 12 > select_image && echo "`cat image_version`"'
	cdsp = 'adb wait-for-device shell "cd /sys/devices/soc0;echo 16 > select_image && echo "`cat image_version`"'
	#runadbcommand(umount_firmware)
	#runadbcommand(firmware_remove)
	#runadbcommand(set_enforce_0)
	#runadbcommand(mkdir_firmware)
	#runadbcommand(chmod_firmware)
	#runadbcommand(mount_modem)
	#runadbcommand(set_enforce_1)
	runadbcommand(remount_firmware)
	runadbcommand(remount_dsp)
	runadbcommand(rm_adsp_FI)
	runadbcommand(rm_shared)
	runadbcommand(push_adsp_FI) 
	runadbcommand(push_shared)
	runadbcommand(push_json)
	runadbcommand(sync_cmd)
	runadbcommand(reboot)
	print "Device rebooting..., Please wait...\n"
	runadbcommand(adb_wait)
	runadbcommand(adb_root)
	runadbcommand(remount)
	runadbcommand(meta)
	runadbcommand(adsp)
	runadbcommand(cdsp)
	runadbcommand(adb_wait)
	print "Done !!!"

	
def check_verity():
    cmnd="adb disable-verity"
    wcmdout = []
    process = subprocess.Popen(cmnd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    for line in process.stdout:
        wcmdout.append(line)
    vcheck=''.join(str(e) for e in wcmdout)
    vcheck=vcheck.replace('\r\n', ' ')
    verity_disabled=re.search('verity is already disabled', vcheck)
    if verity_disabled:
        return True
    else:
        return False
				

def check_partition(partition):
    cmnd='adb wait-for-device shell "cd %s" '%partition
    wcmdout = []
    process = subprocess.Popen(cmnd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    for line in process.stdout:
        wcmdout.append(line)
    dspcheck=''.join(str(e) for e in wcmdout)
    dspcheck=dspcheck.replace('\r\n', ' ')
    no_partition=re.search('No such file or directory', dspcheck)
    if no_partition:
        return False
    else:
        cmnd="adb wait-for-device shell mount -o rw,remount %s"%partition
        wcmdout = []
        process = subprocess.Popen(cmnd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        for line in process.stdout:
            wcmdout.append(line)
        dspcheck=''.join(str(e) for e in wcmdout)
        dspcheck=dspcheck.replace('\r\n', ' ')
        no_partition=re.search('not in /proc/mounts',dspcheck)
        if no_partition:
            return False
        return True
def main():
	if (args < 2 or  args > 2):
		parser = argparse.ArgumentParser()
		parser.add_argument( '--buildlocation', '-b',  dest='buildlocation', default = os.path.dirname(os.path.dirname(os.path.abspath(__file__))), help='adsp_proc build location')
		options = parser.parse_args()
		print options.buildlocation
		real_location = str(options.buildlocation)
	elif(args == 2):
		real_location = str(sys.argv[args-1])
	a = real_location + "\\obj\\qdsp6v5_ReleaseG\\"
	chipset = os.listdir(a)
	print "Build location: ", real_location
	chipset = chipset[0]
	print "Chipset is: ", chipset	
	Flag = flash_adsp_build(real_location, chipset)
	

if __name__ == '__main__':
	args=len(sys.argv)
	main()
