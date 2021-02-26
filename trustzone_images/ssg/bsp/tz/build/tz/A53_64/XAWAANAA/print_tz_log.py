import collections
import os.path
from sys import argv
import argparse
import sys


try:
	class readable_dir(argparse.Action):
		def __call__(self,parser, namespace, values, option_string=None):
			prospective_dir=values
			if not os.path.isdir(prospective_dir):
				raise argparse.ArgumentTypeError("readable_dir:{0} is not a valid path".format(prospective_dir))
			if os.access(prospective_dir, os.R_OK):
				setattr(namespace,self.dest,prospective_dir)
			else:
				raise argparse.ArgumentTypeError("readable_dir:{0} is not a readable dir".format(prospective_dir))


	parser = argparse.ArgumentParser(description='Process the input log file details.')

	#check the arguments passed by user.
	parser.add_argument('-t','--tz',
					   help='Location of tz build to pick dictionary',action=readable_dir)
	parser.add_argument('-l','--log', 
					   help='Absolute Location of log file', type=argparse.FileType('r'))
	parser.add_argument('-o','--output', 
					   help='Absolute Location of output file', type=argparse.FileType('w'), default=os.path.join(os.environ['TEMP'] ,'TZ_LOG_STR.txt') )
	parser.add_argument('-s','--stdout', 
					   help='Option to disable printing on standard out.This is true by default.Pass False to disable', default=True)

	args = parser.parse_args()

	if args.tz is None or args.log is None :
		parser.print_help()
		exit()

	dict_txt=os.path.join(args.tz,'trustzone_images', 'ssg','bsp','qsee','errorCodesDict.txt') 
		 
	 
	with args.log :
		with args.output :
			with open(dict_txt, 'r') as errCodesDictFile :

				'''create a collection from dictionary the first element is error code second element is the list containing value of error code and string.'''
				errCodeDict = collections.defaultdict(list)
				errorCodeDesc={}
				for line in errCodesDictFile:
					if ':' in line :
						(errorCodeNumber,errorCodeString,errorCodeDesc)= line.strip().split(':',2)
						errCodeDict[errorCodeNumber].append(errorCodeString)
						if (errorCodeDesc) : 
							errCodeDict[errorCodeNumber].append(errorCodeDesc)
				'''end creating a collection from dictionary--> errCodeDict
				write to the new file, converted strings
				each line is --> (600003a 10e 76048 0 100 0 0 20 ffffffff00000020 0)'''
				for line in args.log:
					line =line.strip()
					line = line.rstrip("\r\n")
					if '](' in line:
						timestamp=line.split('](')[0]
						line='('+  line.split('](')[1]
					'''if line has ( and ) at start and end we process it else print as it is'''
					if line[:1]=='(' and line[-1]==')' :
						errorCodeInLog=line.strip().split('(')[1]
						'''split each line with space to get list of error codes and args'''
						if ' ' in errorCodeInLog :
							arguments = line.replace(')', '').split(' ')
							errorCodeInLog = arguments[0]
							errorCodeInLog = errorCodeInLog.replace('(', '').strip()
							arguments.pop(0)
						else :
							arguments=')'
						
						'''remove any ) or space if present in errorCode'''
						errorCodeInLog = errorCodeInLog.replace(')', '').strip()
						'''list of format specifiers in C'''
						formatSpecifiers=['%X', '%x', '%c', '%d','%e', '%f', '%g', '%o', '%p',  '%u']
						'''map all formart specifiers to %s, since we replace all format specifiers with %s'''
						mapping = { '%X':'%s', '%x':'%s', '%c':'%s', '%d':'%s','%e':'%s', '%f':'%s', '%g':'%s', '%o':'%s', '%p':'%s',  '%u':'%s'}
						''' get the error code value from dictionary -->errCodeDict[errorCodeInLog])'''
						if errorCodeInLog in errCodeDict :
							if (len(errCodeDict[errorCodeInLog]) >1) :
								stringValue=errCodeDict[errorCodeInLog][1].replace('\n', '')
							else:
								stringValue=' '
								stringValueOrig=''
								
							''' if any format specifier is present convert it to %s and replace with arguments'''	
							if any(fmtSpecifier in stringValue for fmtSpecifier in formatSpecifiers):
								stringValueOrig=stringValue
								for k, v in mapping.iteritems():
									stringValue = stringValue.replace(k, v)
								''' if count of format specifiers is not equal then don't error out hence check is added.'''
								if stringValue.count('%s')==len(arguments) :
									stringValue=stringValue % tuple(arguments)
									stringValue=" "+ stringValue
								else :
									argsVal= " ".join((str(p) for p in arguments) )
									argsVal=argsVal.replace('\n', '')	
									stringValue=" "+ argsVal +'  '+stringValueOrig
									
							if  stringValue == ' ' and    len(arguments)>0 :
								argsVal= " ".join((str(p) for p in arguments) )
								argsVal=argsVal.replace('\n', '')	
								stringValue=" "+ argsVal +'  '+stringValueOrig              	
				   
							args.output.write(timestamp +']('+errCodeDict[errorCodeInLog][0]+stringValue +')'+'\n')
							if args.stdout==True :
								print(timestamp +']('+errCodeDict[errorCodeInLog][0]+stringValue +')'+'\n')			
						else :
							args.output.write(timestamp +']'+line+ '\n')
							if args.stdout==True :
								print(timestamp +']'+line+ '\n')		
					else:
						args.output.write(line+ '\n')
						if args.stdout==True :
							print(line+ '\n')

				print "The output file is at :"+args.output.name
				
				exit(0)

except Exception as e:
        #taceback.print_exc(file = sys.stderr)
		print e
		exit(1)


		