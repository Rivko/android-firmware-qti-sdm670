"""
Copyright (c) 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
"""
from subprocess import Popen, PIPE

# name of the dd executable. On Mac it does not have status=noxfer
# however it is possible to use https://pigiuz.wordpress.com/2013/09/12/resolving-symbols-conflict-between-libraries-on-osx-with-objcopy/
DD = 'dd'

def dsystemf(fmt, *args) :
	cmd = fmt % args
	p = Popen(cmd , shell = True, stdout = PIPE, stderr = PIPE)
	out, err = p.communicate()
	# should we raise an exception if error code is not 0?
	if (p.returncode): raise Exception("command failed (%i): `%s`: %s" % (p.returncode, cmd, err))
	return out

def dd(skip, count, input_file, output_file):
	return dsystemf('%s skip=%d count=%d if=%s of=%s bs=1 status=noxfer > /dev/null 2>&1',
					DD, skip, count, input_file, output_file)

def rm(filename):
	dsystemf('rm %s', filename)

