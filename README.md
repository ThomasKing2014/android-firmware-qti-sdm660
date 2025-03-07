What's this?
==================
This repository contains the firmware source code for the Qualcomm Snapdragon 660, also known as SDM660.

Build
==================
| Product               | Distribution             | Context | ECCN      | Serial Number       |
| --------------------- | :------------------------|:--------|:----------|:--------------------|
| SDM660.LA.3.0.2       | CS 0.0.008.1	         | OEM     | 5D002.c.1 | 236082              |


Build Components
==================

| Image                   | Build/Label                                  | Path                  | Format           |
| ----------------------- | :--------------------------------------------|:----------------------|:-----------------|
| LA.UM.8.2.1.R1          | LA.UM.8.2.1.r1-04200-sdm660.0-1              | *Unavailable*         | Source           |
| ADSP.VT.4.1.C1          | ADSP.VT.4.1.c1-00009-SDM660-1                | adsp_proc             | Source           |
| BOOT.XF.1.4             | BOOT.XF.1.4-00280-S660LZB-1                  | boot_images           | Source           |
| BTFM.CHE.2.1.5.C5       | BTFM.CHE.2.1.5.c5-00007-QCACHROMZ-1          | btfm_proc             | Pre-compiled     |
| CDSP.VT.1.1.C1          | CDSP.VT.1.1.c1-00005-SDM660-1                | cdsp_proc             | Source           |
| SDM660.LA.3.0.2	        | SDM660.LA.3.0.2-00008-STD.PROD-1	       | common, contents.xml  | Source           |
| CPE.TSF.2.0             | CPE.TSF.2.0-00006-W9335AAAAAAAZQ-1           | cpe_proc	             | Pre-compiled     |
| MPSS.AT.3.1.C27         | MPSS.AT.3.1.c27-00007-SDM660_GEN_PACK-1      | modem_proc            | Pre-compiled APQ |
| RPM.BF.1.8              | RPM.BF.1.8-00067-SDM660AAAAANAZR-1           | rpm_proc              | Source           |
| TZ.BF.4.0.7.1           | TZ.BF.4.0.7.1-00006-S660AAAAANAZT-2          | trustzone_images      | Pre-compiled     |
| VIDEO.VE.4.4            | VIDEO.VE.4.4-00066-PROD-1                    | venus_proc            | Pre-compiled     |
| WDSP.9340.1.0.C1        | WDSP.9340.1.0.c1-00008-W9340AAAAAAAZQ-1      | wdsp_proc	       | Pre-compiled     |
| WLAN.HL.3.3.5           | WLAN.HL.3.3.5-00149-QCAHLSWMTPLZ-1           | wlan_proc	       | Pre-compiled     |


How do I build the bootloader?
==================
* Well first off you need the dependencies used for building Android, you can install them by using the command below.

      sudo apt install python2.7 git-core gnupg flex bison build-essential zip curl zlib1g-dev gcc-multilib g++-multilib libc6-dev-i386 libncurses5 lib32ncurses5-dev x11proto-core-dev libx11-dev lib32z1-dev libgl1-mesa-dev libxml2-utils xsltproc unzip fontconfig uuid-dev sqlite3 iasl gcc g++ nasm libtinfo5
* If you had installed them before, you may skip this command.
* After installing the normal dependencies, now you need the Qualcomm Snapdragon LLVM OEM Toolchain which is used for building the firmware.
* In order to get the toolchain you need a Qualcomm account that has a license to it and you also need the Qualcomm Package Manager (QPM), which you can download from Qualcomm's developer site anyway or you can get it [here](https://github.com/David112x/QCOM_LLVM_10.0).
* Also, the reason why Python 2.7 was installed in the previous command is because the build scripts used for the firmware were programmed in Python 2.7 so you must use that version to execute them.
* Make sure that if you enter "python" in the terminal, it calls Python 2.7 instead of Python 3.10.
* Now after installing the Snapdragon LLVM OEM Toolchain, you have to copy it to the following folder.

      /pkg/qct/software/llvm/release/arm/3.9.2/
      
* Since the path doesn't exist, you may make all the folders and set the correct permissions as they'll be created with the root user since the folders are supposed to go in the root filesystem of your Linux installation.
* Now after you've got everything you need installed and where it is supposed to be in, let's get to building the bootloader itself.
* In order to build it, you must execute the commands below.

      # Setup build environment variables for building the XBL (eXtensible Bootloader)
      bash boot_images/edksetup.sh
      # Build the bootloader
      python2.7 boot_images/QcomPkg/buildit.py -t Sdm660Pkg --variant LA -r DEBUG -c -Wno-error
**Congratulations**, you have successfully built the bootloader for the Qualcomm Snapdragon 660 *(that is, if you built it successfully)*, if you weren't able to build it successfully or you found an issue you can't fix, you may report them in the issues tab of this repository and I'll see if we can fix it, I should also warn you to **NOT** install this because of the OEM's TrustZone in your SDM660 device might not boot your custom build of XBL so you must build your own TrustZone images as well which means that you have to build the entire source.

How do I build everything?
==================
Well, for now you may only build bootloader, I'll add a build script that can build everything soon.
