#! /usr/bin/python

import sys
import json
import os.path
from os.path import expanduser
import subprocess

import getopt

root_jsn = 'modemr.jsn'

def usage():
    print "\nUsage: ", os.path.basename(sys.argv[0]), "[OPTIONS]"
    print """
    Configure PD restart configuration in jsn file.

    OPTIONS:
    -e, --enable-pdr
        Enable PD restart on the device, default behavior

    -d, --disable-pdr
        Disable PD restart on the device

    -x, --disable-dumps
        Disables PD dump collection
    
    -p, --enable-dumps
        Enables PD dump collection, default behavior
        
    -m, --modem-root
        Enable PD restart for Root PD on Modem, default behavior

    -a, --adsp-root
        Enable PD restart for Root PD on ADSP

    -f, -j, --jsn-file <file.jsn>
        Enable PD restart for provided jsn file on the target

    -s, --specific-device <adb device>
        Specific ADB device to enable PD restart
    
    -n, --no-reboot
        Do not reboot, default is to reboot to take effect the changes

    -h, --help
        Display this help and exit
    """


def form_adb_cmd(device=None, wait=0, adb_cmd='shell', *args):
    cmd = "adb"
    if device:
        cmd += " -s " + device
    
    if wait:
        cmd += " " + 'wait-for-device'

    cmd += " " + adb_cmd

    for arg in args:
        cmd += " " + arg

    #print 'Run: %s' % cmd
    return cmd

def enable_pdr_in_jsn (data):
    for service in data['sr_service']:
        if service['service'] == 'pdr_enabled':
            return False
    
    data['sr_service'].append({})
    data['sr_service'][-1]['provider'] = "tms"
    data['sr_service'][-1]['service'] = "pdr_enabled"
    data['sr_service'][-1]['service_data_valid'] = 0
    data['sr_service'][-1]['service_data'] = 0

    return True

def disable_pdr_in_jsn (data):
    disabled = False
    for service in data['sr_service']:
        if service['service'] == 'pdr_enabled':
            data['sr_service'].remove(service)
            disabled = True
        if service['service'] == 'pddump_disabled':
            data['sr_service'].remove(service)

    return disabled 

def disable_pd_dumps_in_jsn (data):
    for service in data['sr_service']:
        if service['service'] == 'pddump_disabled':
            return False
    
    data['sr_service'].append({})
    data['sr_service'][-1]['provider'] = "tms"
    data['sr_service'][-1]['service'] = "pddump_disabled"
    data['sr_service'][-1]['service_data_valid'] = 0
    data['sr_service'][-1]['service_data'] = 0

    return True 
    
def enable_pd_dumps_in_jsn (data):
    enabled = False
    
    for service in data['sr_service']:
        if service['service'] == 'pddump_disabled':
            data['sr_service'].remove(service)
            enabled = True
            break;

    return enabled 
    
def change_jsn_file(jsn_file, enable_pdr=1, disable_dumps=0):

    updated_pdr = False
    updated_dumps = False
    
    f = open(jsn_file, 'r')
    data = json.load(f)
    f.close()

    if enable_pdr:
        updated_pdr = enable_pdr_in_jsn(data)
    else:
        updated_pdr = disable_pdr_in_jsn(data)

    if disable_dumps:
        updated_dumps = disable_pd_dumps_in_jsn(data)
    else:
        updated_dumps = enable_pd_dumps_in_jsn(data)
    
    if (updated_dumps == False) and (updated_pdr == False): 
        return None

    f = open(jsn_file, 'wb+')
    f.write(json.dumps(data, indent=4, separators=(',', ': ')))
    f.close()

    return jsn_file

def configure_pdr_in_device(device=None, enable_pdr=True, disable_dumps=False):
    home = expanduser('~')
    
    subprocess.check_output(form_adb_cmd(device, 1, 'root'), stderr=subprocess.STDOUT)
    subprocess.check_output(form_adb_cmd(device, 1, 'shell mount -o rw,remount /firmware'), stderr=subprocess.STDOUT)
    
    try:
       jsn_exists = subprocess.check_call(form_adb_cmd(device, 0, 'shell ls /firmware/image/' + root_jsn), stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as e:
       print "Specified JSON file not found"

    subprocess.check_output(form_adb_cmd(device, 1, 'pull', '/firmware/image/' + root_jsn, home), stderr=subprocess.STDOUT)

    pdr_jsn_file = change_jsn_file(os.path.join(home, root_jsn), enable_pdr, disable_dumps)
    if pdr_jsn_file == None:
        if enable_pdr == True:
            print "PDR already enabled!"
        else:
            print "PDR already disabled!"
        if disable_dumps == True:
            print "PD dumps already disabled!"
        else:
            print "PD dumps already enabled or PDR is disabled!"
        return False

    subprocess.check_output(form_adb_cmd(device, 0, 'push', pdr_jsn_file, '/firmware/image/' + root_jsn), stderr=subprocess.STDOUT)
    subprocess.check_output(form_adb_cmd(device, 1, 'shell setprop persist.sys.pd_enable 1'), stderr=subprocess.STDOUT)
    subprocess.check_output(form_adb_cmd(device, 0, 'shell setprop persist.pd_locater_debug true'), stderr=subprocess.STDOUT)
    subprocess.check_output(form_adb_cmd(device, 1, 'shell sync'), stderr=subprocess.STDOUT)

    return True;

def main(argv):
    enable = True
    device = None
    reboot = True
    dumps_disabled = False
    global root_jsn

    if len(argv) == 0:
        usage()
        sys.exit(2)

    try:
        opts, args = getopt.getopt(argv, "hs:edxpnmaj:f:",
                ["help",
                 'specific-device',
                 'enable-pdr',
                 'disable-pdr',
                 'disable-dumps',
                 'enable-dumps',
                 'no-reboot',
                 'modem-root',
                 'adsp-root',
                 'jsn-file',
                ])
    except getopt.GetoptError as err:
        print str(err)
        usage()
        sys.exit(2)

    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("-s", "--specific-device"):
            device = a
        elif o in ("-e", "--enable-pdr"):
            enable = True
        elif o in ("-d", "--disable-pdr"):
            enable = False
        elif o in ("-x", "--disable-dumps"):
            dumps_disabled = True
        elif o in ("-p", "--enable-dumps"):
            dumps_disabled = False
        elif o in ("-n", "--no-reboot"):
            reboot = False
        elif o in ("-m", "--modem-root"):
            root_jsn = 'modemr.jsn'
        elif o in ("-a", "--adsp-root"):
            root_jsn = 'adspr.jsn'
        elif o in ("-j", "-f", "--jsn-file"):
            root_jsn = a
        else:
            usage()
            sys.exit(1)

    updated = configure_pdr_in_device(device, enable, dumps_disabled)

    if updated == False:
        print "PD restart configuration was not updated!"
        return

    if enable == True:
        message = 'enabled'
    else:
        message = 'disabled'
        
    if dumps_disabled == True:
        dump_state = 'disabled'
    else:
        dump_state = 'enabled'
        
    if reboot == True:
        subprocess.check_output(form_adb_cmd(device, 0, 'shell reboot'), stderr=subprocess.STDOUT)
        print "PD restart %s, dumps are %s, please wait for device to reboot!" % (message, dump_state)
    else:
        print "PD restart %s, dumps are %s, please reboot the device to take effect!" % (message, dump_state)


if __name__ == "__main__":
    main(sys.argv[1:])
