import sys
import re
import os

if (len(sys.argv) < 3):
    exit()

target=sys.argv[1]
writefile = open(sys.argv[2],"a")
writefile1 = open(sys.argv[3],"a")
name_conv = sys.argv[4]
ADSP_BUILDROOT = sys.argv[5]
openDSPpath = sys.argv[6]
cdsp_flag = sys.argv[7]

if name_conv == "qurt_model":
    cond = 0
    for r,d,f in os.walk(ADSP_BUILDROOT+"\\core\\kernel\\qurt\\build"):
        for file in f:
            win_dump  = os.path.join(r,file)
            print win_dump
            if "qurt_model.t32" in win_dump:
                print win_dump
                print win_dump.split('qurt_model.t32')[0]
                writefile.write(win_dump.split('\\qurt_model.t32')[0]+"\n")
                writefile.close()
                writefile1.close()
                cond = 1
                break
        if cond == 1:
            break
elif name_conv == "GETELFNAME":
    elfname = False
    sensorelfname = False
    audioelfname = False
    dummyelfname = "M" + target + "AAAAAAAAQ"
    dummyelfname_1 = "ROOT_" + target + ".adsp.prodQ"
    if cdsp_flag == "True":
        dummyelfname_1 = "ROOT_" + target + ".cdsp.prodQ"
    print dummyelfname,dummyelfname_1
    if openDSPpath == "False":
        cond = 0
        for r,d,f in os.walk(ADSP_BUILDROOT+"\\build\\ms"):
            for file in f:
                win_dump  = os.path.join(r,file)
                # print win_dump
                if (dummyelfname in win_dump or dummyelfname_1 in win_dump) and "_SENSOR.elf" in win_dump:
                    sensorelfname = win_dump
                    elfname = win_dump.replace('_SENSOR.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_SENSOR_reloc.elf" in win_dump:
                    sensorelfname = win_dump.replace('_SENSOR_reloc.elf','_SENSOR.elf')
                    elfname = win_dump.replace('_SENSOR_reloc.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_AUDIO.elf" in win_dump:
                    audioelfname = win_dump
                    elfname = win_dump.replace('_AUDIO.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_AUDIO_reloc.elf" in win_dump:
                    audioelfname = win_dump.replace('_AUDIO_reloc.elf','_AUDIO.elf')
                    elfname = win_dump.replace('_AUDIO_reloc.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_reloc.elf" in win_dump and not "_AUDIO_reloc.elf" in win_dump and not "_SENSOR_reloc.elf" in win_dump:
                    elfname = win_dump.replace('_reloc.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and ".elf" in win_dump and not "_AUDIO" in win_dump and not "_SENSOR" in win_dump and not "_AUDIO_INTERNAL" in win_dump and not "_SENSOR_INTERNAL" in win_dump:
                    elfname = win_dump
                    cond = 1
                    break
            if cond == 1:
                break
    else:
        cond = 0
        for r,d,f in os.walk(openDSPpath):
            for file in f:
                win_dump  = os.path.join(r,file)
                #print win_dump
                if (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_SENSOR.elf" in win_dump:
                    sensorelfname = win_dump
                    elfname = win_dump.replace('_SENSOR.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_SENSOR_reloc.elf" in win_dump:
                    sensorelfname = win_dump.replace('_SENSOR_reloc.elf','_SENSOR.elf')
                    elfname = win_dump.replace('_SENSOR_reloc.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_AUDIO.elf" in win_dump:
                    audioelfname = win_dump
                    elfname = win_dump.replace('_AUDIO.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_AUDIO_reloc.elf" in win_dump:
                    audioelfname = win_dump.replace('_AUDIO_reloc.elf','_AUDIO.elf')
                    elfname = win_dump.replace('_AUDIO_reloc.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and "_reloc.elf" in win_dump and not "_AUDIO_reloc.elf" in win_dump and not "_SENSOR_reloc.elf" in win_dump:
                    elfname = win_dump.replace('_reloc.elf','.elf')
                    cond = 1
                    break
                elif (dummyelfname in win_dump or dummyelfname_1 in win_dump ) and ".elf" in win_dump and not "_AUDIO" in win_dump and not "_SENSOR" in win_dump and not "_AUDIO_INTERNAL" in win_dump and not "_SENSOR_INTERNAL" in win_dump:
                    elfname = win_dump
                    cond = 1
                    break
            if cond == 1:
                break
    
    print elfname
    if os.path.exists(elfname):
        writefile.write(elfname+"\n")
        writefile.write(elfname.split('\\')[-1].split('.')[0]+"\n")
        if "ROOT_" in elfname:
            if os.path.exists(elfname.replace('ROOT_','SENSOR_')):
                sensorelfname = elfname.replace('ROOT_','SENSOR_')
                writefile_sen = open(sys.argv[2].replace('.txt','_sensor.txt'),"a")
                writefile_sen.write(sensorelfname+"\n")
                writefile_sen.write(sensorelfname.split('\\')[-1].split('.')[0]+"\n")
                writefile_sen.close()
            if os.path.exists(elfname.replace('ROOT_','AUDIO_')):
                audioelfname = elfname.replace('ROOT_','AUDIO_')
                writefile_sen = open(sys.argv[2].replace('.txt','_audio.txt'),"a")
                writefile_sen.write(audioelfname+"\n")
                writefile_sen.write(audioelfname.split('\\')[-1].split('.')[0]+"\n")
                writefile_sen.close()
        else:
            if os.path.exists(elfname.replace('.elf','_SENSOR.elf')):
                sensorelfname = elfname.replace('.elf','_SENSOR.elf')
                writefile_sen = open(sys.argv[2].replace('.txt','_sensor.txt'),"a")
                writefile_sen.write(sensorelfname+"\n")
                writefile_sen.write(sensorelfname.split('\\')[-1].split('.')[0]+"\n")
                writefile_sen.close()
            if os.path.exists(elfname.replace('.elf','_AUDIO.elf')):
                audioelfname = elfname.replace('.elf','_AUDIO.elf')
                writefile_sen = open(sys.argv[2].replace('.txt','_audio.txt'),"a")
                writefile_sen.write(audioelfname+"\n")
                writefile_sen.write(audioelfname.split('\\')[-1].split('.')[0]+"\n")
                writefile_sen.close()
    print elfname
    print sensorelfname
    print audioelfname
    writefile.close()
    writefile1.close()
    
else:
    elf_name_list = {'8x10':'M8x10AAAAAAAAQ1234', 
                     '8x26':'M8x26AAAAAAAAQ1234', 
                     '8626':'M8x26AAAAAAAAQ1234', 
                     '9x25':'M9x25AAAAAAAAAQ1234', 
                     '8994':'M8994AAAAAAAAQ1234', 
                     '8996':'M8996AAAAAAAAQ1234', 
                     '8998':'M8998AAAAAAAAQ1234', 
                     '8974':'M8974AAAAAAAAQ1234', 
                     '9x35':'M9x35AAAAAAAAQ1234', 
                     '8084':'M8084AAAAAAAAQ1234', 
                     '8992':'M8992AAAAAAAAQ1234', 
                     '8952':'M8952AAAAAAAAQ1234', 
                     '8953':'M8953AAAAAAAAQ1234', 
                     '8937':'M8937AAAAAAAAQ1234' 
                     }

       
    sensor_elf_name_list = {'8x10':'M8x10AAAAAAAAQ1234_SENSOR',
                            '8910':'M8x10AAAAAAAAQ1234_SENSOR',
                            '8x26':'M8x26AAAAAAAAQ1234_SENSOR',
                            '8626':'M8x26AAAAAAAAQ1234_SENSOR',
                            '8926':'M8x26AAAAAAAAQ1234_SENSOR',
                            '8994':'M8994AAAAAAAAQ1234_SENSOR',
                            '8996':'M8996AAAAAAAAQ1234_SENSOR',
                            '8998':'M8998AAAAAAAAQ1234_SENSOR',
                            '9x35':'M9x35AAAAAAAAQ1234_SENSOR',
                            '9x25':'M9x25AAAAAAAAAQ1234_SENSOR',
                            '8974':'M8974AAAAAAAAQ1234_SENSOR',
                            '8084':'M8084AAAAAAAAQ1234_SENSOR',
                            '8992':'M8992AAAAAAAAQ1234_SENSOR',
                            '8952':'M8952AAAAAAAAQ1234_SENSOR',
                            '8953':'M8953AAAAAAAAQ1234_SENSOR',
                            '8937':'M8937AAAAAAAAQ1234_SENSOR'
                            }

    for key in elf_name_list:
        if target.rstrip('\n') == key.rstrip('\n'):
            writefile.write(elf_name_list[key]+"\n")
            break
    for key in sensor_elf_name_list:
        if target.rstrip('\n') == key.rstrip('\n'):
            writefile1.write(sensor_elf_name_list[key]+"\n")
            break
    writefile.close()
    writefile1.close()
