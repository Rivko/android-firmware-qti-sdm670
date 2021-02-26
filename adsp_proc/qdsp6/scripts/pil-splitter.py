#! /usr/bin/env python
"""
Splits elf files into segments.

If the elf is signed, the elf headers and the hash segment are output to
the *.mdt file, and then the segments are output to *.b<n> files.

If the elf isn't signed each segment is output to a *.b<n> file and the
elf headers are output to the *.mdt file.
"""

import sys
import subprocess
import re
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

def segment_is_hash(offset, image):
	"""Checks the program header for the HASH bit

	Returns True if this segment is a hash, and False otherwise.
	"""

	image.seek(offset + 24)
	flags = struct.unpack('<i', image.read(4))[0]
	return (flags & (0x7 << 24)) == (0x2 << 24)

def parse_metadata(image):
	"""Parses elf header metadata"""
	metadata = {}
	command = ['hexagon-readelf', '-l', image.name]
	r = subprocess.Popen(command, stdout=subprocess.PIPE)
	r.poll()
	sections = r.communicate()[0]
	if r.returncode != 0:
		print sections
		exit(r.returncode)

	hex = "0[xX][\dA-Fa-f]+"

	match = re.search(r'Entry point (%s)' % hex, sections)
	if match is None:
		die("Cannot find elf entry point")
	metadata['entry'] = match.group(1)

	match = re.search(r'There are (\d+) program headers, starting at offset (\d+)', sections)
	if match is None:
		die("Cannot find number of program headers")

	metadata['num_segments'] = int(match.group(1))
	metadata['pg_start'] = int(match.group(2))

	str = r'\s+\S+\s+(?P<offset>%s)\s+%s\s+(?P<phys>%s)\s+(?P<f_size>%s)\s+(?P<m_size>%s)' % (hex, hex, hex, hex, hex)
	metadata['segments'] = []
	for match in re.finditer(str, sections):
		metadata['segments'].append(match.groupdict())

	for i, seg in enumerate(metadata['segments']):
		# Elf program headers are 32 bytes large
		seg['hash'] = segment_is_hash(metadata['pg_start'] + (i * 32), image)

	return metadata

def dump_metadata(metadata, image, name):
	"""Creates <name>.mdt file from elf metadata"""

	name = "%s.mdt" % name
	# Dump out the elf header
	dump_data(image, name, 0, 52)
	# Append the program headers
	append_data(image, name, metadata['pg_start'], 32 * metadata['num_segments'])

	for seg in metadata['segments']:
		if seg['hash']:
			append_data(image, name, int(seg['offset'], 16), int(seg['f_size'], 16))
			break

def dump_segments(metadata, image, name):
	"""Creates <name>.bXX files for each segment"""
	for i, seg in enumerate(metadata['segments']):
		start = int(seg['offset'], 16)
		size = int(seg['f_size'], 16)
		output = "%s.b%02d" % (name, i)
		dump_data(image, output, start, size)

def is_elf(file):
	"""Verifies a file as being an ELF file"""
	file.seek(0)
	magic = file.read(4)
	return magic == '\x7fELF'

if __name__ == "__main__":

	if len(sys.argv) != 3:
		usage()

	image = open(sys.argv[1], 'rb')
	if not is_elf(image):
		usage()

	prefix = sys.argv[2]
	metadata = parse_metadata(image)
	dump_metadata(metadata, image, prefix)
	dump_segments(metadata, image, prefix)
