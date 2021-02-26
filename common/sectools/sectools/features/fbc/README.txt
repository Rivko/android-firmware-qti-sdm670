Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

FuseBlower can be used to generate and validate sec.dat file.

<sectools>/
| sectools.py (main tool launcher command interface)
|
| -- config/  			     (chipset-specific config template directory)
| -- config/<chipset>/ 		 (pre-configured templates directory)
| -- config/xsd 			 (xsd for config xml)
|
| -- sectools/features/fbc/fuseblower.py  	   (main FuseBlower python script)
| -- sectools/features/fbc/ 			       (main FuseBlower core code)
|
| -- sectools/features/fuse_common/datamodel/  (data model code)
| -- sectools/features/fuse_common/parsegen/   (data parsers/generators)
| -- sectools/features/fuse_common/validator/  (built-in validators)
| --
| -- sectools/common/core                      (infrastructure)
| -- sectools/common/crypto                    (cryptographic services)
| -- sectools/common/data_provisioning         (data provisioning)
| -- sectools/common/utils                     (core utilities)
| -- sectools/common/parsegen                  (data parser)
|
| -- resources/				                   (resources directory)
| -- resources/data_prov_assets		           (data provisioning assets)

------------------------------------------------------------------------------
Example Usage of the tool
------------------------------------------------------------------------------
Quick Help:

python sectools.py fuseblower -h
Options:
  --version             show program's version number and exit
  -h, --help            show this help message
  -v, --verbose         enable more logging.

  Generating sec.dat with or without secdat as input:
    -u <file>, --user_config_path=<file>
                        path to the user config file.
    -e <file>, --oem_config_path=<file>
                        path to the oem config file.
    -q <file>, --qti_config_path=<file>
                        path to the qti config file.
    -s <file>, --secdat=<file>
                        path to the sec dat file as input for common_output or
                        for validation.
    -p <platform>, --platform=<platform>
                        specify the platform, to get the config files as input
                        related to platform

  Specifying output location:
    -o <dir>, --output_dir=<dir>
                        directory to store output files. DEFAULT: "./"

  Specifying the mode of operation:
    -m <mode>, --mode=<mode>
                        fuseblower mode. Available modes: ['secboot', 'rot'].
                        DEFAULT: secboot

  Operations:
    -g, --generate      1. generates the sec.dat file using config files and
                        the cmd is  "-u, -e, -q, -g" 2. generates the sec.dat
                        file using config files & input secdat and the cmd is
                        "-u, -e, -q, -g, -s" 3. generates the sec.dat file
                        using platform and the cmd is "-p <platform>" 4.
                        generates the sec.dat file using platform & input
                        secdat and the cmd is "-p <platform> -s
    -a, --validate      validate the sec.dat file and the cmd is "-u, -e, -q,
                        -a, -s
    -l, --verify_inputs
                        verify the command line options.