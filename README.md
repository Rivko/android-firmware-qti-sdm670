What's this?
==================
This repository contains the firmware source code for the Qualcomm Snapdragon 670 (or so it seems), also known as SDM670.

Build
==================
| Product               | Distribution             | Context | ECCN      | Serial Number       |
| --------------------- | :------------------------|:--------|:----------|:--------------------|
| QCS605.LA.2.0         | Post-CS[5.1] 0.0.036.1   | OEM     | 5D002.c.1 | 	282951              |


Build Components
==================

| Image                   | Build/Label                                  | Path                  | Format           |
| ----------------------- | :--------------------------------------------|:----------------------|:-----------------|
| LA.UM.6.8.1.R3          | LA.UM.6.8.1.r3-03500-QCS605.0-1              | *Unavailable*         | Source           |
| ADSP.VT.5.0             | ADSP.VT.5.0-00526-SDM710-1                   | adsp_proc             | Source           |
| AOP.HO.1.1.C1           | AOP.HO.1.1.C1-00025-SDM710AAAAANAZO-1        | aop_proc              | Source           |
| BOOT.XF.2.1             | BOOT.XF.2.1-00132-SDM710LZB-4                | boot_images           | Source           |
| BTFM.CHE.2.1.3          | BTFM.CHE.2.1.3-00353-QCACHROMZ-1             | btfm_proc             | Pre-compiled     |
| CDSP.VT.2.0             | CDSP.VT.2.0-00249-SDM710-2                   | cdsp_proc             | Source           |
| QCS605.LA.2.0		        | QCS605.LA.2.0-00036-STD.PROD-1    	         | common, contents.xml  | Source           |
| CPE.TSF.3.0             | CPE.TSF.2.0-00006-W9335AAAAAAAZQ-1           | cpe_proc	             | Pre-compiled     |
| GNSS.AT.4.0.2.C10       | GNSS.AT.4.0.2.c10-00025-SDM710_GPS_PACK-1    | modem_proc            | Pre-compiled APQ |
| TZ.XF.5.0.2             | TZ.XF.5.0.2-00232-S710AAAAANAZT-1            | trustzone_images      | Pre-compiled     |
| VIDEO.VE.5.2            | VIDEO.VE.5.2-00067-PROD-1                    | venus_proc            | Pre-compiled     |
| WDSP.9340.1.0           | WDSP.9340.1.0-00320-W9340AAAAAAAZQ-1         | wdsp_proc	           | Pre-compiled     |
| WLAN.HL.2.0.1.C10	      | WLAN.HL.2.0.1.c10-00333-QCAHLSWMTPLZ-1       | wlan_proc	           | Pre-compiled     |


How do I build the bootloader?
==================
* Well first off you need the dependencies used for building Android, you can install them by using the command below.

      sudo apt install python2.7 git-core gnupg flex bison build-essential zip curl zlib1g-dev gcc-multilib g++-multilib libc6-dev-i386 libncurses5 lib32ncurses5-dev x11proto-core-dev libx11-dev lib32z1-dev libgl1-mesa-dev libxml2-utils xsltproc unzip fontconfig uuid-dev sqlite3 iasl gcc g++ nasm libtinfo5
* If you had installed them before, you may skip this command.
* After installing the normal dependencies, now you need the Qualcomm Snapdragon LLVM OEM Toolchain which is used for building the firmware.
* In order to get the toolchain you need a Qualcomm account that has a license to it and you also need the Qualcomm Package Manager (QPM), which you can download from Qualcomm's developer site anyway or you can get it [here](https://gitlab.com/David112x/clang) or [here](https://gitlab.com/David112x/clang2).
* Also, the reason why Python 2.7 was installed in the previous command is because the build scripts used for the firmware were made with Python 2.7 so you must use that version to execute them.
* Make sure that if you enter "python" in the terminal, it calls Python 2.7 instead of Python 3.
* Now after installing the Snapdragon LLVM OEM Toolchain, you have to copy it to the following folder.

      /pkg/qct/software/llvm/release/arm/3.9.1
      
* Since the path doesn't exist, you may make all the folders and set the correct permissions as they'll be created with the root user since the folders are supposed to go in the root filesystem of your Linux installation.
* Now after you've got everything you need installed and where it is supposed to be in, let's get to building the bootloader itself.
* In order to build it, you must execute the commands below.

      # Setup build environment variables for building the XBL (eXtensible Bootloader)
      bash boot_images/edksetup.sh
      # Build the bootloader
      python2.7 boot_images/QcomPkg/buildex.py -t Sdm670Pkg --variant LA -r DEBUG -c -Wno-error
