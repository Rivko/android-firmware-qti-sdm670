#! /usr/bin/env python
"""
===========================================================================

  Copyright (c) 2012 QUALCOMM Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

===========================================================================
"""

"""
Splits elf files into segments.

If the elf is signed, the elf headers and the hash segment are output to
the *.mdt file, and then the segments are output to *.b<n> files.

If the elf isn't signed each segment is output to a *.b<n> file and the
elf headers are output to the *.mdt file.
"""

import sys
import struct

def die(message):
	print message
	exit(1)

def usage():
	print "Usage: %s <elf> <prefix>" % sys.argv[0]
	exit(1)

def dump_data(input, output, start, size):
	"""Dump 'size' bytes from 'input' at 'start' into newfile 'output'"""

	if size == 0:
		return

	input.seek(start)
	outFile = open(output, 'wb')
	outFile.write(input.read(size))
	outFile.close()

	print 'BIN %s' % output

def append_data(input, output, start, size):
	"""Append 'size' bytes from 'input' at 'start' to 'output' file"""

	if size == 0:
		return

	input.seek(start)
	outFile = open(output, 'ab')
	outFile.write(input.read(size))
	outFile.close()

def gen_struct(format, image):
	"""Generates a dictionary from the format tuple by reading image"""

	str = "<%s" % "".join([x[1] for x in format])
	elems = struct.unpack(str, image.read(struct.calcsize(str)))
	keys = [x[0] for x in format]
	return dict(zip(keys, elems))

def parse_metadata(image, is_64_bit):
	"""Parses elf header metadata"""
	metadata = {}

	if is_64_bit:
		elf_hdr = [
				("ident", "16s"),
				("type", "H"),
				("machine", "H"),
				("version", "I"),
				("entry", "Q"),
				("phoff", "Q"),
				("shoff", "Q"),
				("flags", "I"),
				("ehsize", "H"),
				("phentsize", "H"),
				("phnum", "H"),
				("shentsize", "H"),
				("shnum", "H"),
				("shstrndx", "H"),
				]

		elf_phdr = [
				("type", "I"),
				("flags", "I"),
				("offset", "Q"),
				("vaddr", "Q"),
				("paddr", "Q"),
				("filesz", "Q"),
				("memsz", "Q"),
				("align", "Q"),
				]
	else:
		elf_hdr = [
				("ident", "16s"),
				("type", "H"),
				("machine", "H"),
				("version", "I"),
				("entry", "I"),
				("phoff", "I"),
				("shoff", "I"),
				("flags", "I"),
				("ehsize", "H"),
				("phentsize", "H"),
				("phnum", "H"),
				("shentsize", "H"),
				("shnum", "H"),
				("shstrndx", "H"),
				]

		elf_phdr = [
				("type", "I"),
				("offset", "I"),
				("vaddr", "I"),
				("paddr", "I"),
				("filesz", "I"),
				("memsz", "I"),
				("flags", "I"),
				("align", "I"),
				]
	
	elf_hdr = gen_struct(elf_hdr, image)
	metadata['num_segments'] = elf_hdr['phnum']
	metadata['pg_start'] = elf_hdr['phoff']
	metadata['ehdr_size'] = elf_hdr['ehsize']
	metadata['phdr_size'] = elf_hdr['phentsize']

	metadata['segments'] = []
	for i in xrange(metadata['num_segments']):
		image.seek(metadata['pg_start'] + (i * elf_hdr['phentsize']))
		phdr = gen_struct(elf_phdr, image)
		metadata['segments'].append(phdr)
		phdr['hash'] = (phdr['flags'] & (0x7 << 24)) == (0x2 << 24)

	return metadata

def dump_metadata(metadata, image, name):
	"""Creates <name>.mdt file from elf metadata"""

	name = "%s.mdt" % name
	# Dump out the elf header
	
	dump_data(image, name, 0, metadata['ehdr_size'])
	# Append the program headers
	append_data(image, name, metadata['pg_start'], metadata['phdr_size'] * metadata['num_segments'])

	for seg in metadata['segments']:
		if seg['hash']:
			append_data(image, name, seg['offset'], seg['filesz'])
			break

def dump_segments(metadata, image, name):
	"""Creates <name>.bXX files for each segment"""
	for i, seg in enumerate(metadata['segments']):
		start = seg['offset']
		size = seg['filesz']
		output = "%s.b%02d" % (name, i)
		dump_data(image, output, start, size)

def elf_check(file):
	"""Returns if a file is ELF file and if the file is a 64-bit ELF file"""
	elf_hdr = [
			("ident_magic", "4s"),
			("ident_class", "B"),
			("ident_data", "B"),
			("ident_version", "B"),
			("ident_os_abi", "B"),
			("ident_abi_ver", "B"),
			("ident_unused", "7s"),
			("type", "H"),
			("machine", "H"),
			("version", "I"),
			]
			
	elf_hdr = gen_struct(elf_hdr, image)
	file.seek(0)
	
	is_elf = (elf_hdr["ident_magic"] == "\x7fELF")
	is_64_bit = (elf_hdr["ident_class"] == 2)

	return is_elf, is_64_bit

if __name__ == "__main__":

	if len(sys.argv) != 3:
		usage()

	image = open(sys.argv[1], 'rb')
	is_elf, is_64_bit = elf_check(image)
	if not is_elf:
		usage()

	prefix = sys.argv[2]
	metadata = parse_metadata(image, is_64_bit)
	dump_metadata(metadata, image, prefix)
	dump_segments(metadata, image, prefix)