CC := $(ATH_CROSS_COMPILE_TYPE)gcc
TARGET_TYPE ?= AR9888
TARGET_VERS ?= v2

all:
	$(CC) -g3 -Wall  \
		cld-fwlog-netlink.c parser.c nan-parser.c cld-diag-parser.c -o cnss_diag

clean:
	rm -f cnss_diag
