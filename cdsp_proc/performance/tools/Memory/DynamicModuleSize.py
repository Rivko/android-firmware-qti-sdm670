import subprocess
import os
import sys
import csv
import math
#the location of shared object files as a command line argument
so_file_directory=sys.argv[1]
csv_file=open("DynModulesSize.csv", "wb")
c = csv.writer(csv_file,delimiter=',')
c.writerow(["Name of the Module","No. of Load Sections","RW Section Size in KB","RE,RO Section Size in KB","Total Size in KB"])
extensions = ['.so.1','.so']
#Executing the cmd hexagon-readelf on dynamic module, size of the module
#is calculated. This script provided No of loadable sections, Size of RE Sec
#aligned to 4K,Size of RO Sec aligned to 4K and Total size of the dynamic 
#module is shown which is aligned to 4K.
for file_name in os.listdir(so_file_directory):
	if file_name.endswith(tuple(extensions)):
	    file_contents=subprocess.check_output('hexagon-readelf -e "'+so_file_directory + file_name+'"', shell=True)
	    load_split_list=file_contents.split("\n  LOAD")

	    no_load_sec=len(load_split_list)-1

	    RE_size=0
	    RW_size=0
	    for index in range(len(load_split_list)):
		if(index!=0):
		    l=load_split_list[index].split("\n")
		    if(l[0].find("RW")!=-1):
			RW_memsiz=l[0].split()[4]
			RW_size+=int(math.ceil(int(RW_memsiz,16) / 4096.0)) * 4096
		    elif(l[0].find("R E")!=-1):
			    RE_memsiz=l[0].split()[4]
			    RE_size+=int(math.ceil(int(RE_memsiz,16) / 4096.0)) * 4096
		    else:
			if(l[0].find("R")!=-1):
			    RE_memsiz=l[0].split()[4]
			    RE_size+=int(math.ceil(int(RE_memsiz,16) / 4096.0)) * 4096			    
	    RW_size/=1024
	    RE_size/=1024

	    last_load_split=load_split_list[no_load_sec]
	    addr_split=last_load_split.split()
	    virtaddr=addr_split[1].strip()
	    memsiz=addr_split[4].strip()
	    int_virtaddr=int(virtaddr,16)
	    int_memsiz=int(math.ceil(int(memsiz,16) / 4096.0)) * 4096
	    total_size=(int_virtaddr+int_memsiz)/1024

	    c.writerow([file_name,no_load_sec,RW_size,RE_size,total_size])
    
csv_file.close()
    
