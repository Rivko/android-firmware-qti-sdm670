import sys
import os

if os.path.exists('./build/ms/RA_FunctionStackSizes.csv'):
		if os.path.exists('./build/ms/RA_FunctionStackSizes_audio.csv'):
				os.remove('./build/ms/RA_FunctionStackSizes_audio.csv')
		os.renames('./build/ms/RA_FunctionStackSizes.csv','./build/ms/RA_FunctionStackSizes_audio.csv')
		
	
if os.path.exists('./build/ms/NoStackOverflow.txt'):
	os.remove('./build/ms/NoStackOverflow.txt')

input_csv=open("./qdsp6/tools/input/stack_input.csv", "r")

array_lines=[]
for line in input_csv:
		array_lines.append(line)
		
input_threads=[]
input_functions=[]
input_stacks=[]
stack_used=[]
Func_output=[]

flag =0
		
for i in array_lines:
		
		if flag > 0:
				words=i.split(',')
				input_threads.append(words[0])
				input_functions.append(words[1])
				words[2]=words[2].rstrip()
				input_stacks.append(words[2])
		flag= flag + 1
		
		
input_csv.close()


for func in input_functions:
		flag=0
		hexagon_csv=open("./build/ms/RA_FunctionStackSizes_normal.csv","r")
		for line in hexagon_csv:
				words=line.split(',')
				only_name=words[0].split('(')
				only_name[0]=only_name[0].replace('"','').strip()
				if func == only_name[0]:
						flag=1
						Func_output.append(func)
						var = float(words[1])
						var1 = var/1024.0
						var1=round(var1,3)
						stack_used.append(var1)
						
		hexagon_csv.close()				
		
		if flag == 0:
				if sys.argv[1] == 'mpd':
						if os.path.exists('%s'%sys.argv[2]):
								hexagon_sensor_csv=open("./build/ms/RA_FunctionStackSizes_audio.csv","r")
								for line1 in hexagon_sensor_csv:
										words1=line1.split(',')
										only_name1=words1[0].split('(')
										only_name1[0]=only_name1[0].replace('"','').strip()
										if func == only_name1[0]:
												flag=1
												Func_output.append(func)
												var = float(words1[1])
												var1 = var/1024.0
												var1=round(var1,3)
												stack_used.append(var1)
								hexagon_sensor_csv.close()
				if flag == 0:				
						Func_output.append(func)
						stack_used.append('xxxxx')
				
		
		
	
		

output_csv=open("./build/ms/stack_output.csv","w")

output_csv.write('Thread Name,')
output_csv.write('Function Name,')
output_csv.write('Stack Allocated(KB),')
output_csv.write('Maximum Stack Required(KB),')
output_csv.write('Error Info\n')

overflow_count=0

for a,b,c,d in zip(input_threads,Func_output,input_stacks,stack_used):
					output_csv.write(a)
					output_csv.write(',')
					output_csv.write(b)
					output_csv.write(',')
					output_csv.write(c)
					output_csv.write(',')
					output_csv.write(str(d))
					if d == 'xxxxx':
							output_csv.write(',')
							output_csv.write('Function Missing')
					elif float(c) < float(d):
							output_csv.write(',')
							output_csv.write('Stack Overflow')
							overflow_count=overflow_count+1
							print 'Warning : Stack Overflow in thread ',a,'\n'		
		
					output_csv.write('\n')

output_csv.close()					

if overflow_count == 0:
		overflow_check=open("./build/ms/NoStackOverflow.txt","w")
		overflow_check.close()
		
elif overflow_count > 0:		
		print 'Warning : Stack overflow detected, for more details refer /adsp_proc/build/ms/stack_output.csv\n'
	
