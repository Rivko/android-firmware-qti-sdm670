
Required Parameters : 

-G <gdb port> -dump <dump_file_location> <dump_start_addr> <dump_size> -dpage <dummy_page_virtual_address> <dummy_page_physical_address> <dummy_page_size> -asid <process_id> -elf <elf_file_location>

Example:

-G 9009 -dump ../data/1799690/DDRCS0.BIN 0x0 0x1FFFFFFF -dpage 0xf0000000 0x05a00000 0x1400000 -asid 0x0 -elf ../data/1799690/M8974AAAAAAAAQ1234.elf

Steps to run:
	
	1) open a cmd shell and run crash_sim.exe <params>
	2) open another shell and run hexagon-gdb <elf filename>
		a) in gdb prompt -> say "target remote localhost:<port_no>"
		b) symbol-file <elf filename>
		c) say "info threads" for displaying all threads
		d) say "thread <num>" for choosing a thread; num is the number of the thread and not thread id.
		d) say "info registers" for diaplaying register for the selected thread
