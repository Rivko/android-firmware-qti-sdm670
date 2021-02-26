CC := $(ATH_CROSS_COMPILE_TYPE)gcc

all:
	$(CC) -g3 -Wall  \
		-I /usr/include/libnl3 \
		-I /usr/include/libxml2 \
		-D __linux__ \
		nl_cmd_wrapper.c ven_cmd_tool.c -o vendor_cmd_tool -lnl-3 -lnl-genl-3 -lxml2 -lpthread

clean:
	rm -f vendor_cmd_tool
