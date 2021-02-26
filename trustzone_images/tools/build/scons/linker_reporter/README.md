linker_reporter
===============

Tool to parse GNU LD map files and generate section size reports similar to the ARM linker output

Usage:
  python report_generator.py mapfile.map
  python report_generator.py metabuild_dir/
  
  In the first command, just supplying a mapfile will generate the report in the terminal. If desired,
  the report can be stored in a text file by redirecting output:
    > python report_generator.py mapfile.map > output.txt

  In the second command, the entire metabuild directory will be searched for *.map files.  When a *.map
  file is located, a report will be created and saved in the same directory in the metabuild as the *.map
  file.  The report will be saved as *.map.linkreport.

  One final note:
  The tool uses unix utility "ar" to extract archived .o files in .lib files. By default the script
  uses the LLVMBIN environment variable from the build environment to locate the llvm-ar executable.
  If the script cannot locate this executable (cannot read environment variable), the ar_tool_path
  in the report_generator.cfg file is used instead.
