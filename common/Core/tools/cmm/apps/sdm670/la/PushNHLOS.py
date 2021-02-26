#===============================================================================
#
# PushNHLOS.py
#
# GENERAL DESCRIPTION
#    CBSP Products Script to 'push' images to the filesystem on Linux Android
#
# Copyright (c) 2011-2012 by QUALCOMM Technologies Incorporated.
# All Rights Reserved.
# 
# QUALCOMM Proprietary
#
#-------------------------------------------------------------------------------
#

#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 10/10/2012 AJCheriyan	 Created v1.0 for Aragorn (MSM8974)
#==============================================================================


import glob, itertools, os, string, sys, subprocess, re, platform, shutil, fnmatch, time
from optparse import OptionParser

# Function to terminate program with a message
def kill(msg):
	if msg:
		print msg
	sys.exit(1)

class TargetInfo:
	def __init__(self):
		self.__init__(self, msm8974)

	def validate(self, target):
		valid_target = ['msm8974', 'mdm9x25', 'msm8x26', 'msm8x10']
		# Provided in the following format : imagename:prefix to be used with pilsplitter
		valid_image = {
				'msm8974' : {'mpss':'modem', 'adsp':'adsp', 'mba':'mba','wcnss':'wcnss'},
				'msm8x26' : {'mpss':'modem', 'adsp':'adsp', 'mba':'mba','wcnss':'wcnss'},
				'msm8x10' : {'mpss':'modem', 'adsp':'adsp', 'mba':'mba','wcnss':'wcnss'},
		}

		if target in valid_target:
			return (target, valid_image[str(target)])
		else:
			print ("Invalid target specified. Valid Targets : %s " % (valid_target))

	def __init__(self, target, meta):
		print "Target is : %s " % (target)
		print "Metabuild is : %s " % (meta)
		try:
			(self.target, self.valid_images) = self.validate(target)
		except:
			print ("Invalid target specified : %s " % (target))
			kill("")

		# If no, meta assume this script is running from the required meta
		self.metabuild = meta


	def GetPrefix(self, image):
		try:
			return self.valid_images[image]
		except:
			print "Invalid Image : %s " % image
			kill("")



	
class ComponentImage:
	def cleanup(self):
		if os.path.exists(self.opdir):
			shutil.rmtree(self.opdir)
		print "Cleaned up temp files"

	def __init__(self, name, ipfile, prefix):
		self.name = name
		self.prefix = prefix
		if os.path.isfile(ipfile):
			self.ipfile = ipfile
		else:
			print "Input binary %s does not exist" % ipfile
			kill("")

		baseopdir = os.environ.get('TEMP')
		if os.path.exists(baseopdir):
			print "Using temp directory to create split binaries: %s" % baseopdir
		else:
			opdir = raw_input('Enter a location to save temp files:')
		# Now create a new folder
		opdir = os.sep.join([baseopdir, 'pil_output', self.name])
		
		
		try:
			if os.path.exists(opdir):
				shutil.rmtree(opdir)
			os.makedirs(opdir)
			print "Created directory to save split binaries: %s" % opdir
		except:
			kill('Attempt to create new directory failed')
		# Now assign given it was successful
		self.opdir = opdir
		self.opfile = opdir + os.sep + os.path.basename(self.ipfile)
		
		







# Function to parse input options
def parseoptions():
	global options
	argparser = OptionParser(version='%prog 1.0')

	argparser.add_option('-i', '--image', dest='imagename', help='Name of the image', metavar='FILE')
	argparser.add_option('-t', '--target', dest='targetname', help='Name of the target', metavar='NAME', default='msm8974')
	argparser.add_option('-m', '--meta', dest='metabuild', help='Base Meta Build to use', metavar='PATH')
	argparser.add_option('-b','--binary', dest='binary', help='Input binary to push',	metavar='FILE')
	argparser.add_option('-d', '--diag', action='store_true', dest='diagmsg', help='Diagnostic Messages', metavar='OPTION')
	
	(options, args) = argparser.parse_args()
	
	# Ask for input if not provided
	if not options.imagename:
		options.imagename = raw_input("Enter Image Name:")
	if not options.binary:
		options.binary = raw_input("Enter path to binary:")
		

	global target, image
	target = TargetInfo(options.targetname, options.metabuild)
	image = ComponentImage(options.imagename, options.binary, target.GetPrefix(options.imagename))
		

# Function to check for adb installation
def checkadb():
	if platform.system() == "Linux":
		cmd = 'which adb'
		error = r'(?P<error>no adb in)'
	else:
		cmd = 'where adb'
		error = r'(?P<error>INFO: Could not)'
	# Now check
	adb = subprocess.Popen(cmd, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
	
	(stdout, stderr) = adb.communicate()
	# Windows is stupid at times. So check if the file actually exists as well instead of relying on the return message
	output = re.search(error, stdout, flags=re.IGNORECASE)
	if (output or not (os.path.exists(stdout.rstrip()))):
		kill("ADB is not installed or not in your path.")
	

	




# Function to actually split the binaries
# Note: Target Information should have been populated
def splitbins(SingleImage, metabuild):
	# Full path to the script
	pilsplitter = ''
	scriptname = 'pil-splitter.py'
	error = r'(?P<error>usage.*)'

	# If no metabuild is provided, assume we are running from current
	if metabuild:
		print "Locating PIL Splitter in the build: %s " % metabuild
		location = metabuild
	else:
		print "Looking for PIL splitter in current build"
		location = os.sep.join([os.path.dirname(__file__),'..','..','..',])

	for root, dirs, files in os.walk(location):
		for file in fnmatch.filter(files, scriptname):
			print "Found pil-splitter.py : %s " % (root)
			pilsplitter = os.sep.join([root, file])
	
	# Check if we found a pilsplitter
	if pilsplitter:
		# Now call after transitioning over to images temp directory
		cwd = os.getcwd()
		
		os.chdir(SingleImage.opdir)
		# Create the command
		cmd = ' '.join(['python', pilsplitter, SingleImage.ipfile,SingleImage.prefix])

		pilsplittercall = subprocess.Popen(cmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
		(stdout, stderr) = pilsplittercall.communicate()
		output = re.search(error, stdout, flags=re.IGNORECASE)
		if output:
			print output.group('error')
			kill("Encountered error while calling pil-splitter.py")
		else:
			print "Created Split Binaries for : %s" % SingleImage.prefix
			os.chdir(cwd)
	else:
		kill("Failed to locate PIL Splitter Script")


# Pushes a particular image on to the target's filesystem
def adbpush(ImageToPush):
	connectcmd = 'adb wait-for-device'
	
	prepushcmd = ['adb root', 'adb remount', 'adb shell rm /etc/firmware/' + ImageToPush.prefix + '.*']
	prepusherr = [r'(?P<error>device not found)',r'(?P<error>remount failed)', r'(?P<error>rm failed for)']

	pushcmd = [' '.join(['adb push', ImageToPush.opdir,'/etc/firmware/'])]
	pusherr = [r'(?P<error>No such file or directory)']

	postpushcmd = ['adb shell sync']
	postpusherr = [r'(?P<error>not found)']

	# Make sure connection can be established.
	# Block until target is connected
	print "Trying to connect to target. Will not exit until connection is established"
	adbconnect = subprocess.Popen(connectcmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
	(stdout, stderr) = adbconnect.communicate()
	print "Connected to target."

	# Target is alive. So do your work
	for count in range(len(prepushcmd)):
		if (options.diagmsg):
			print "Running command : %s" % (prepushcmd[count])
		prepush = subprocess.Popen(prepushcmd[count], stderr=subprocess.PIPE, stdout=subprocess.PIPE)
		(stdout, stderr) = prepush.communicate()

		rmsg = re.search(prepusherr[count], stdout, flags=re.IGNORECASE)
		if rmsg:
			print "Error: %s" % rmsg.group('error')
			kill("Encountered error trying to push images")


	for count in range(len(pushcmd)):
		if (options.diagmsg):
			print "Running command : %s" % (pushcmd[count])
		push = subprocess.Popen(pushcmd[count], stderr=subprocess.PIPE, stdout=subprocess.PIPE)
		(stdout, stderr) = push.communicate()
		rmsg = re.search(pusherr[count], stdout, flags=re.IGNORECASE)
		if rmsg:
			print rmsg.group('error')

	for count in range(len(postpushcmd)):
		if (options.diagmsg == 1):
			print "Running command : %s" % (postpushcmd[count])
		postpush = subprocess.Popen(postpushcmd[count], stderr=subprocess.PIPE, stdout=subprocess.PIPE)
		(stdout, stderr) = postpush.communicate()
		rmsg = re.search(postpusherr[count], stdout, flags=re.IGNORECASE)
		if rmsg:
			print rmsg.group('error')

	
	# Successfully pushed images
	print "Successfully pushed images"
	




	
	
		

# Actual start of the script a.k.a the meat
if __name__ == '__main__':

	# First parse the input
	parseoptions()

	# Now check if the necessary tools are installed
	checkadb()

	# Now actually split the binaries
	splitbins(image, target.metabuild)

	# Connect to the target and push
	adbpush(image)

	# Clean up the temp files
	image.cleanup()


	
	

	
