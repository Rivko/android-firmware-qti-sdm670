
bdfUtil is a utility program to convert a BDF file in text/bin format to a bin/text format file.

To convert text to bin
----------------------

Usage: bdfUtil <t2b|txt2bin> <bdf template> <input bdftxt> <output bdf.bin> <templateHeaderFile.h>

Example:
bdfUtil t2b bdf_qca61x0_1.tpl bdwlan.txt mybdwlan.bin mybdwlan.h

To convert bin to text
----------------------
Usage: bdfUtil <b2t|bin2txt> <bdf template> <input bdf.bin> <output bdf.txt> <templateHeaderFile.h>

Example:
bdfUtil b2t bdf_qca61x0_1.tpl bdwlan.bin mybdwlan.txt

