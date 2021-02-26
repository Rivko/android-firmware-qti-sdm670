##  Steps:
## 1)Copy Starting 0x3000(12288) bytes of unstripped signed so file to starting of unsigned unstripped so file.
## 2)Update the value at offset 0x20 to new value (old value at 0x20 offset in unsigned unstriped so + 0x3000) Ex: New value 0x00421AA0(0x0041EAA0+0x3000)
## 3)Update the values at offset 0x2c,0x2d,0x30,0x31,0x32,0x33 to same values as in unsigned unstripped so  
## 4)Change the offsets and addresses of all the sections 

from subprocess import check_output as qx
import os
import re

in_file1="C:\\ASHA_work\\tools\\signed_so\\unstripped_signed_with_issue\\libfastcvadsp.so"            # unstripped signed so file
in_file2="C:\\ASHA_work\\tools\\signed_so\\unstripped_unsigned\\libfastcvadsp.so"					  # unsigned unstripped so file
out_file="C:\\ASHA_work\\tools\\signed_so\\unstripped_signed_with_issue\\libfastcvadsp_new.so"		  # New so file

#Step-1: 
with open(in_file1, "rb") as in_file:
	piece = in_file.read(12288)
in_file.close()
						
with open(in_file2, "rb") as in_file:
	data=in_file.read()
in_file.close()

with open(out_file, "wb") as out_file1:
	out_file1.write(piece)
	out_file1.write(data)
out_file1.close()

#Step-2:
cmd = '''\\\\rover\\hyd_dspfw\\ADSP_Tools\\Hexagon_Tools\\5.1.05\\gnu\\bin\\hexagon-readelf.exe -S %s'''%(in_file2)	
output = qx(cmd)
lines = output.splitlines()
print output

print "\nMain offset " + lines[0].split("offset")[-1].split(":")[0]
t1=lines[0].split("offset")[-1].split(":")[0]
hex_int = int(t1, 16)
#print hex_int
hex_int = hex_int + 12288
hex1=format(hex_int, '08x')
print "New_offset   "+ str(hex1)
r = re.compile('(..)').findall(hex1)

#step-3:
with open(in_file2, "rb") as in_file:
	in_file.seek(44)
	data_2c=in_file.read(1)
	data_2d=in_file.read(1)
	in_file.seek(48)
	data_30=in_file.read(1)
	data_31=in_file.read(1)
	data_32=in_file.read(1)
	data_33=in_file.read(1)
in_file.close()

with open(out_file, "rb+") as out_file2:
	out_file2.seek(32)
	out_file2.write(chr(int(r[3],16)))
	out_file2.write(chr(int(r[2],16)))
	out_file2.write(chr(int(r[1],16)))
	out_file2.write(chr(int(r[0],16)))
	out_file2.seek(44)
	out_file2.write(data_2c)
	out_file2.write(data_2d)
	out_file2.seek(48)
	out_file2.write(data_30)
	out_file2.write(data_31)
	out_file2.write(data_32)
	out_file2.write(data_33)
out_file2.close()

#step-4
offset_new=[]
offset_old=[]
addrs_new=[]
addrs_old=[]

for i in range(5,14):
	t1=lines[i].split()[5]
	offset_old.append(t1)
	t1 = int(t1, 16)
	t1_int = t1 + 12288
	hex1=format(t1_int, '06x')
	offset_new.append(hex1)
	
	t2=lines[i].split()[4]
	addrs_old.append(t2)
	t2 = int(t2, 16)
	if t2!=0:
		t2_int = t2 + 12288
		hex1=format(t2_int, '08x')
	else:
		hex1=format(t2, '08x')
	addrs_new.append(hex1)


for i in range(14,(len(lines)-4)):
	t2=lines[i].split()[4]
	offset_old.append(t2)
	t2 = int(t2, 16)
	t2_int = t2 + 12288
	hex1=format(t2_int, '06x')
	offset_new.append(hex1)
	
	t2=lines[i].split()[3]
	offset_old.append(t2)
	t2 = int(t2, 16)
	if t2!=0:
		t2_int = t2 + 12288
		hex1=format(t2_int, '08x')
	else:
		hex1=format(t2, '08x')
	addrs_new.append(hex1)
		
#print "offset_new " 
# print offset_new
# print "Address_new "
# print addrs_new		
		
for i in range(0,len(offset_new)):
	with open(out_file, "rb+") as out_file2:
		r1 = re.compile('(..)').findall(addrs_new[i])
		out_file2.seek(hex_int+52+(i*40))
		out_file2.write(chr(int(r1[3],16)))
		out_file2.write(chr(int(r1[2],16)))
		out_file2.write(chr(int(r1[1],16)))
		out_file2.write(chr(int(r1[0],16)))

		r1 = re.compile('(..)').findall(offset_new[i])
		out_file2.seek(hex_int+56+(i*40))
		out_file2.write(chr(int(r1[2],16)))
		out_file2.write(chr(int(r1[1],16)))
		out_file2.write(chr(int(r1[0],16)))
		
out_file2.close()	

cmd = '''\\\\rover\\hyd_dspfw\\ADSP_Tools\\Hexagon_Tools\\5.1.05\\gnu\\bin\\hexagon-readelf.exe -S %s'''%(out_file)
output = qx(cmd)
print output
