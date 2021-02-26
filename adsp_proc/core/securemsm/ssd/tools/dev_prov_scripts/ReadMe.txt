
Overview:
This tool is used to provision SSD RSA Keys to the Device at factory stage
--------------------------------------------------------------------------------------------
Required Directory structure:
<tool_root>				: Root direcotry where all the '.PL' files are stored
<tool_root>\keys		: Directory where all the keys are stored
--------------------------------------------------------------------------------------------

--------------------------------------------------------------------------------------------
There are currently 2 different tools to support Key provisioning using 2 different methods

1. Provision keys using fastboot for LA targets & UEFI for Windows targets
	-->	generate_keystore.py
2. Provision keys using QXDM (this is a set of perl scripts that have to be run in a particular order)
	--> parse_keys.pl
	--> send_to_diag.pl
this method will be deprecated going forward
============================================================================================

--------------------------------------------------------------------------------------------
1. Provision keys using fastboot for LA targets & UEFI for Windows targets
--------------------------------------------------------------------------------------------
1.a. create keystore.dat file
	
	\>python generate_keystore.py
	
	--	output of this script is 'keystore.dat'
	--	this scrip parses the file <tool_root>\keys\keys.conf & writes the RSA keys in a structured 
		format that will be used by the SSD keystore module
	
1.b	provision the keystore.dat to the device

1.b.1 LA targets:
	\>fastboot flash ssd keystore.dat

1.b.2 Windows targets:
	
	-- TBD
============================================================================================


--------------------------------------------------------------------------------------------
2. Provision keys using QXDM
--------------------------------------------------------------------------------------------
2.a Description of Perl scripts:

2.a.1	parse_keys.pl			: This script will parse the <tool_root>\keys\keys.conf file 
								  & creates a set of QXDM commands & writes it to an 
								  input file keys.inp.

2.a.2	send_to_diag.pl			: This scripts expects an Input file (.inp) & sends 
								  the command(s) listed in the inp file to QXDM


2.b Command files for send_to_diag.pl

2.b.1	<tool_root>\erase_data.inp:
	-- This file is used as an input to the 'send_to_diag.pl" script.
	-- It is used to erase all previously provisioned data

2.b.2	<tool_root>\validate_data.inp
	-- This file is used as an input to the 'send_to_diag.pl" script.
	-- It is used to validate the previsouly provisioned data

2.b.3	<tool_root>\keys.inp
	-- This file is used as an input to the 'send_to_diag.pl" script.
	-- It is used to provision all the keys listed in the "keys.conf" file
============================================================================================

--------------------------------------------------------------------------------------------
3. Description of other files:
--------------------------------------------------------------------------------------------
<tool_root>\keys\keys.conf:

This is a configuration file that lists all the keys & their types, that need to be 
provisioned to the device. It is constructed in a Loose markup language.

Tags in the file:
DEV_PRIV_KEY	: Device private key PKCS8 DER format
				  Can appear more than once.

DEV_PUB_KEY_ID	: SHA-256 Hash of the Device Public key (in PKCS1 DER format)
				  Can appear more than once

OEM_PUB_KEY		: OEM's public key in PKCS1 DER format
				  Can appear only once

OEM_PUB_KEY_ID	: SHA-256 Hash of the OEM Public key (in PKCS1 DER format)
				  Can appear more than once
============================================================================================

