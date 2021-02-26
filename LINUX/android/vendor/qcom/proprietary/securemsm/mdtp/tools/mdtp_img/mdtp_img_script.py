#===============================================================================
# Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#===============================================================================

#!/usr/bin/python

import os
import sys
import codecs
import math
import argparse
from PIL import Image
from struct import *


"""
Script Work Flow:
	1. Files existence check.
		1.1 Exit if one of the directories doesn't exist.
		1.2 Exit if mandatory files are missing.
	2. Create images Meta-Data header - 4KB.
		2.1 Initialize value of virtual eFuse.
		2.2 Initialize value of digit space.
		2.3 Initialize version.
		2.4 Initialize metadata type.
		2.5 If the metadata describes image sets, initialize the number of image sets.
		2.3 Offset, width, height for each image/image set (in case of an image set, the width and height are referring to the screen resolution).
		2.4 Each image meta data is stored in intended, predetermined location.
	3. Storing images in mdtp.img, based on a predetermined sequence.
		3.1 Removing BMP format header, keeping only raw-data.
		3.2 Padding raw-data to a multiple of 4K size.

General Instructions:
	-Python Imaging Library(PIL) is needed to run the script.
	-All files in FileList must be present in each image set directory.
	-At least one image set has to be provided as a parameter, along with the screen resolution it was designed for (optional: space between the displayed PIN digits).
	-Width, height and digit_space are defined in pixels.
	-Image size is limited by screen resolution.
	-Image color depth must be 24bit.
	-Image width must be a multiple of 4 pixels.
	-Python version 2.7

Supported command syntax:
	mdtp_img_script.py [-h] --dir DIR --width WIDTH --height HEIGHT [--digit_space DIGIT_SPACE] [--dir DIR --width WIDTH --height HEIGHT [--digit_space DIGIT_SPACE]]...

Mandatory Files List (Reading order):
    acceptedit_text.bmp         0
    alert_message.bmp           1
    btn_OK_off.bmp              2
    btn_OK_on.bmp               3
    maintext_5seconds.bmp       4
    maintext_enterpin.bmp       5
    maintext_incorrectpin.bmp   6
    pintext.bmp                 7
    pin_selected_0.bmp          8
    pin_selected_1.bmp          9
    pin_selected_2.bmp          10
    pin_selected_3.bmp          11
    pin_selected_4.bmp          12
    pin_selected_5.bmp          13
    pin_selected_6.bmp          14
    pin_selected_7.bmp          15
    pin_selected_8.bmp          16
    pin_selected_9.bmp          17
    pin_unselected_0.bmp        18
    pin_unselected_1.bmp        19
    pin_unselected_2.bmp        20
    pin_unselected_3.bmp        21
    pin_unselected_4.bmp        22
    pin_unselected_5.bmp        23
    pin_unselected_6.bmp        24
    pin_unselected_7.bmp        25
    pin_unselected_8.bmp        26
    pin_unselected_9.bmp        27
    btn_help_off.bmp            28
    btn_help_on.bmp             29
    help_text.bmp               30
    maintext_entermasterpin.bmp 31
    press_any_key.bmp           32
"""
#Constants
BLOCK_SIZE = 4096                           #LK reading block size
METADATA_SIZE = BLOCK_SIZE                  #Meta-data header size
IMAGES_METADATA_OFFSET = METADATA_SIZE/2    #Images meta-data offset in the mdtp_img header
BMP_HEADER_LEN = 14                         #BMP header in bytes
DIB_HEADER_LEN = 0                          #DIB header in bytes
SUPPORTED_BPP = 3                           #BPP supported by MDTP (RGB mode)
METADATA_PER_IMAGE_SIZE = 12                #Total size (bytes) of (offset,width,height)
MAX_IMAGE_SETS = 40
MAX_PARAMS_PER_IMAGE_SET = 4                #Image set params are: directory path, screen width, screen height, digit_space
MAX_FLAGS_PER_IMAGE_SET = 4                 #Command line flags for the above params
FILE_NAME = "mdtp.img"
MAX_COMMAND_LINE_ARGUMENTS = MAX_IMAGE_SETS*(MAX_PARAMS_PER_IMAGE_SET + MAX_FLAGS_PER_IMAGE_SET) + 1

#Initialize global variables
version = 1                                 #Metadata version
root_dir = os.getcwd()                      #The folder in which all image set directories (which contain the required BMP images) are located
default_digit_space = 8                     #The default spacing in pixels between the digits to be displayed in the recovery UI
imageSetOffset = METADATA_SIZE              #Image sets data is located right after the meta-data header
imageSetList = []                           #A list to hold tuples of the image set params

#Mandatory file list
#All files must be in each image set directory
FileList = ["acceptedit_text.bmp", "alert_message.bmp",
			"btn_OK_off.bmp", "btn_OK_on.bmp",
			"maintext_5seconds.bmp", "maintext_enterpin.bmp",
			"maintext_incorrectpin.bmp", "pintext.bmp",
			"pin_selected_0.bmp", "pin_selected_1.bmp",
			"pin_selected_2.bmp", "pin_selected_3.bmp",
			"pin_selected_4.bmp", "pin_selected_5.bmp",
			"pin_selected_6.bmp", "pin_selected_7.bmp",
			"pin_selected_8.bmp", "pin_selected_9.bmp",
			"pin_unselected_0.bmp", "pin_unselected_1.bmp",
			"pin_unselected_2.bmp", "pin_unselected_3.bmp",
			"pin_unselected_4.bmp", "pin_unselected_5.bmp",
			"pin_unselected_6.bmp",	"pin_unselected_7.bmp",
			"pin_unselected_8.bmp",	"pin_unselected_9.bmp",
			"btn_help_off.bmp",	"btn_help_on.bmp",
			"help_text.bmp", "maintext_entermasterpin.bmp",
			"press_any_key.bmp"]

#MetadataType:
#		IMAGES	   - the metadata is used for managing images
# 		IMAGE_SETS - the metadata is used for managing one or more image sets
class MetadataType:
	IMAGES     = 1
	IMAGE_SETS = 2

#Converts string to binary
def to_binary(s):
    result = []
    for c in s:
        bits = bin(ord(c))[2:]
        bits = '00000000'[len(bits):] + bits
        result.extend([int(b) for b in bits])
    return result

#Converts string's binary to int
def to_int(bits):
	integer = 0
	for b in range(len(bits) / 8):
		byte = bits[b*8:(b+1)*8]
		iter = len(byte) - 1
		for bit in byte:
			integer = integer + pow(2,iter)*bit
			iter = iter - 1
	return integer

#Calculating DIB length from BMP header
DIB_LEN_OFFSET_IN_BMP = 10					#BMP+DIB headers length location int BMP header
def get_dib_len(bmp_header):
	FULL_HEADER_LEN = bmp_header[DIB_LEN_OFFSET_IN_BMP:BMP_HEADER_LEN]
	FULL_HEADER_LEN = to_binary(FULL_HEADER_LEN)
	FULL_HEADER_LEN = to_int(FULL_HEADER_LEN)
	dib_len = FULL_HEADER_LEN - BMP_HEADER_LEN
	return dib_len

#Checks if a given directory is valid:
#	Directory exists
#	Directory contains all the mandatory files
def is_dir_valid(dir):
	if os.path.exists(os.path.join(root_dir,dir)) != True:
		print (" Error: directory `" + dir + "` doesn't exist.")
		return False

	missing_files = False
	for file in FileList:
		if os.path.exists(os.path.join(dir,file)) != True:
			print (" Error: file `" + file + "` in `" + dir + "` is missing.")
			missing_files = True
	if missing_files == True:
		return False

	return True

#Parse command line arguments for a single image set and append the parsed params to imageSetList
def parse_image_set_params(imageSetParams):
	parser = argparse.ArgumentParser()
	parser.add_argument("--dir", required=True)
	parser.add_argument("--width", required=True)
	parser.add_argument("--height", required=True)
	parser.add_argument("--digit_space", default=str(default_digit_space))

	args = parser.parse_args(imageSetParams)

	if is_dir_valid(args.dir) != True:
		sys.exit()

	imageSetTuple = (args.dir, args.width, args.height, args.digit_space)
	imageSetList.append(imageSetTuple)



#Input validation
if len(sys.argv) > MAX_COMMAND_LINE_ARGUMENTS:
	sys.exit(" Error: too many arguments.")

#Group a single image set params and parse
imageSetParams = []
for arg in sys.argv[1:]:
	if (arg == "--dir"):
		if imageSetParams:
			parse_image_set_params(imageSetParams)
			imageSetParams = []
	imageSetParams.append(arg)
parse_image_set_params(imageSetParams)

imageSetList.sort(key=lambda x: (int(x[1]),int(x[2])))          #Sort image set list by (width,height)

#Create file
#Write main structure parameters
targetFile = open(FILE_NAME,"wb")                               #Create the target file
targetFile.seek(0,0)
targetFile.write(pack('I',0))                                   #Set virtual eFuse to 0
targetFile.write(pack('I',0))                                   #Set digit-space to 0
targetFile.write(pack('I',int(version)))                        #Set version
targetFile.write(pack('I',int(MetadataType.IMAGE_SETS)))        #Set metadata type
targetFile.write(pack('I',int(len(imageSetList))))              #Set number of image sets

imageSetIndex = 0

for imageSet in imageSetList:

	#Initialize single image set parameters
	imageSetDir = os.path.join(root_dir,imageSet[0])
	imageSetWidth = int(imageSet[1])
	imageSetHeight = int(imageSet[2])
	imageSetDigitSpace = int(imageSet[3])

	#Write single image set parameters
	targetFile.seek(imageSetOffset,0)
	targetFile.write(pack('I',0))                               #Set eFuse to 0
	targetFile.write(pack('I',int(imageSetDigitSpace)))         #Set digit-space
	targetFile.write(pack('I',int(version)))                    #Set version
	targetFile.write(pack('I',int(MetadataType.IMAGES)))        #Set metadata type

	index = width = height = size = 0
	offset = imageSetOffset + METADATA_SIZE

	#Calculating & writing images + meta-data into target file
	for file in os.listdir(imageSetDir):
		if file not in FileList:                #Skips unnecessary files
			continue
		index = FileList.index(file)            #Finds intended location of meta data

		image = Image.open(os.path.join(imageSetDir,file))
		(width, height) = image.size            #(width,height) tuple
		if(image.mode != "RGB"):                #Checks image's mode/bpp
			sys.exit(" Error: file mode of `" + file + "` isn't RGB(3BPP).")

		file_name = open(os.path.join(imageSetDir,file),"rb+")
		img = file_name.read(BMP_HEADER_LEN)    #Cutting BMP header(const 14Bytes)
		DIB_HEADER_LEN = get_dib_len(img)
		img = file_name.read(DIB_HEADER_LEN)    #Cutting DIB header(length is version dependent)
		size = width*height*SUPPORTED_BPP       #Image size in bytes (BMP/DIB headers not included)

		if size%BLOCK_SIZE != 0:				#Padding image to BLOCK_SIZE
			size = (math.floor(size/BLOCK_SIZE)+1)*BLOCK_SIZE

		targetFile.seek(imageSetOffset + IMAGES_METADATA_OFFSET + METADATA_PER_IMAGE_SIZE*index,0)	#Writes the image meta-data to a constant location
		struct = pack('III',offset,width,height)
		targetFile.write(struct)

		img = file_name.read()                  #Reading raw-data
		targetFile.seek(offset,0)               #Moving file ptr to offset point
		targetFile.write(img)                   #Copy data to targetFile
		file_name.close()
		offset += size                          #Update offset
		index = width = height = size = 0

	targetFile.seek(IMAGES_METADATA_OFFSET + METADATA_PER_IMAGE_SIZE*imageSetIndex,0)	#Writes a single set meta-data to a constant location
	struct = pack('III',imageSetOffset,imageSetWidth, imageSetHeight)
	targetFile.write(struct)

	imageSetOffset = offset                     #Update imageSetOffset to be right after the previous image set
	imageSetIndex += 1                          #Update index

targetFile.close()
sys.exit(" " + FILE_NAME + " created successfully!")

