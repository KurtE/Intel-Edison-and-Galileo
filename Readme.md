Warning
=======

This is a Work In Progress!  There are no warrantees or Guarantees 
of any type that this code or documents are usable for anything.  But I hope they are…

Note: this project currently is being used to hold documents or files specific to the Intel Edison and Galileo boards.  

Most of the code that I use on these boards is seperated into two parts.

The first is Arduino sketches and libraries.  I will probably put up some of my different test sketches and the like up here.

The second is Linux projects.  I will put some Intel specific things here, but most of this work is in my other project Raspberry Pi. 


Note: a lot of the following information is extracts from the readme in the Raspberry Pi project.

Quick Start Guide
=================

WIP--- Many places here mention sudo apt-get... On Intel use opkg


After installing check for any updates that are not in the latest distro by running:

    sudo apt-get update && sudo apt-get upgrade

Next thing will install git... nearly everything needs it.  You can also push directly into your git hub online with it:

    sudo apt-get install git-core

For SSL certs:

`sudo apt-get install ca-certificates`


Putty and WinSCP
----------------

Also if setting up a PC to talk to a PI through the network, more information from Kevin:
So for file transfer I use WinSCP. It's a nice little GUI for simple FTP. I still code on my PC 
then when I want to test compile I just transfer over the directories. Here is the direct link. 
It should auto download. Your raspberry Pi should tell you it's IP towards the end of it's boot cycle.

http://winscp.net/download/winscp514setup.exe

I run my rpi headless so I simply ssh into its IP. I use the below program to get a terminal in.

http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html

As long as you are comfortable with a terminal this should work fine.

Archive files
------
 
As I always forget the right options to pass to tar for different types of archives.  Lots of places on web to find info like: http://www.thegeekstuff.com/2010/04/unix-tar-command-examples/
    
    tar -xvf archive_name.tar
    tar -zxvf  archive_name.tar.gz
    tar -jxvf archive_name.tar.bz2
    gunzip archive_name.gz
    bunzip2 archive_name.bz2


Configure BeagleBone Black (See what parts apply)
--------------------------

Setup for date and time update
    ntpdate us.pool.ntp.org
    nano /etc/default/ntpdate   (Set us.pool.ntp.org in NTPSERVERS)

    rm /etc/localtime
    ln -s /usr/share/zoneinfo/America/Los_Angeles /etc/localtime
    
    Better yet:
        dpkg-reconfigure tzdata

Nameserver may not be set properly
    echo nameserver 8.8.8.8 > /etc/resolv.conf
    

Configuring Intel Edison
------------------------
**Warning**: This is the start of my working notes as I Just started with Edison

Follow the steps to configure Edison: https://communities.intel.com/docs/DOC-23147
If new board update firmware to latest: https://communities.intel.com/docs/DOC-23192

Notes also found on forum: http://forums.trossenrobotics.com/showthread.php?7145-Experiment-with-Intel-Edison

wifi did not work after configure: Talked about in thread: https://communities.intel.com/thread/55527
edit: /etc/systemd/system/basic.target.wants/network-gadget-init.service
change: 192.168.2.15 to something like: 192.168.99.15

Configure to use other repos to get stuff: 

MRAA:

    echo "src mraa-upm http://iotdk.intel.com/repos/1.1/intelgalactic" > /etc/opkg/mraa-upm.conf
    opkg update
    opkg install libmraa0

Lots of good stuff: http://alextgalileo.altervista.org/edison-package-repo-configuration-instructions.html  

set the contents of: /etc/opkg/base-feeds.conf 

    src/gz all http://repo.opkg.net/edison/repo/all
    src/gz edison http://repo.opkg.net/edison/repo/edison
    src/gz core2-32 http://repo.opkg.net/edison/repo/core2-32

The Boot segment gets full when you do updates as the whole space allocated for the boot is not actually
used.  Instructions on how to fix up at: http://alextgalileo.altervista.org/blog/install-kernel-from-repo-onto-edison-official-image/


If playing around with my Adafruit code base, may be issue with weird Adafruit_GFX files includes instead of mine.  If so delete the library:   Robot_Control out of the Intel IDE

If playing around with my Adafruit stuff with makefiles useing MRAA, issue with mraa with duplicate symbols, problem with their header files included in multiple source files.  I edited the file: /usr/include/mraa/common.hpp and made all of the functions inline, which appears to have solved this.

Warning: my testAdafruit_ILI9341 test program can cause the Edison to reboot!

Note: did not find any OSTYPE defined to be able to update makefiles to check to for now I am forcing it by creating the file  ~/.profile

    export OSTYPE=Edison
Note: still working on how to do udev rules on this machine may need to add lines like below to the .profile file until I do.

    ln -s /dev/ttyO1 /dev/ttyXBEE
    ln -s /dev/ttyO2 /dev/ttyRCLAW`

This works for the USB2AX in /etc/udev/rules.d/99-usb-serial.rules

	SUBSYSTEM=="tty", ATTRS{idVendor}=="16d0", ATTRS{idProduct}=="06a7", ATTRS{serial}=="74031303437351D02210", SYMLINK+="ttyUSB2AX"

Change Arduino IDE to allow wifi uploads of programs.  Currently I have created the file 
c:\arduino-1.5.3-Intel.1.0.4\hardware\arduino\edison\tools\izmir\clupload_win_hacked.sh that contains:
	#!/bin/sh
	
	echo "starting download script"
	echo "Args to shell:" $*
	
	edison_ip="192.168.2.115"
	edison_pw="*** YourPassword ***"
	SKETCH=/sketch/sketch.elf
	OLD_SKETCH=/sketch/sketch.elf.old
	
	
	$1/plink -pw $edison_pw root@$edison_ip  "mv -f $SKETCH $OLD_SKETCH"
	
	# Execute the target download command
	
	#Download the file.
	$1/pscp -scp -pw $edison_pw $2 root@$edison_ip:$SKETCH
	$1/plink  -pw $edison_pw root@$edison_ip "chmod +x $SKETCH"
	$1/plink  -pw $edison_pw root@$edison_ip "systemctl restart clloader"
	
You then change the file: 
c:\arduino-1.5.3-Intel.1.0.4\hardware\arduino\edison\platform.win.txt
and change the line: 

    tools.izmirdl.cmd.path={runtime.ide.path}/hardware/arduino/edison/tools/izmir/clupload_win.sh

to:

	tools.izmirdl.cmd.path={runtime.ide.path}/hardware/arduino/edison/tools/izmir/clupload_win_hacked.sh



Setup tty Device on RPI
-----------------------

Also My code is setup to use an XBee and an SSC-32, both of which connect up as USB devices.  I did not want my
code to have to depend on the order the devices are added so I wished to setup rules to create an alias for these devices
I found a lot of good information up at: http://hintshop.ludvig.co.nz/show/persistent-names-usb-serial-devices/
Note: I had to experiment as some of the devices would hang, so I had to find the right command sequence to work.
Example: udevadm info --query=property --name=ttyUSB0

From this I created a file named 99-usb-serial.rules in the /etc/udev/rules.d directory.  A copy of mine from the first
Raspberry Pi is contained in the Phoenix directory.  It looks like:
```
    SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="A800fclo", SYMLINK+="ttyXBEE"
    SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", ATTRS{serial}=="A4014UWE", SYMLINK+="ttySSC-32"
```
Note: You will have to change the {Serial} value (and maybe some others if not FTDI) to the actual device on your machine.

Aflter I installed this file, if I type something like:

    ls -l /dev/ttyXBEE

I see something like:

    lrwxrwxrwx 1 root root 7 Dec 31  1969 /dev/ttyXBEE -> ttyUSB0


Setup TTy Device on BBBk
------------------------

I can also use the same type of setup on the BBBk, but in addition to this, the BBBk has multiple 
Usarts available on expansion connectors P8/P9.  However to use these, you need to update the device
tree.  I will put more information in here on how to do that, including files I use to update.
More information up on: http://blog.pignology.net/2013/05/getting-uart2-devttyo1-working-on.html

Note: It is now a lot easier than what it was earlier to do this.  From the current pignology... You see:
just add you BB-UART2 to /media/BEAGLEBONE/uEnv.txt, with the key capemgr.enable_partno.
```
    This is my current uEnv.txt:
        root@beaglebone:/lib/firmware# cat /media/BEAGLEBONE/uEnv.txt
        optargs=quiet video=HDMI-A-1:1280x1024@60e capemgr.enable_partno=BB-SPI0DEV,BB-UART4,BB-CANBUS1
    As you can see, I have the BB-UART4 enabled. This gives me /dev/ttyO4. (And I have SPIO and CAN enabled, but that is a different story).
```
Note: for debian install, the uEnv.txt file is in /boot/uboot directory.  I now have a line that looks like:

    optargs=capemgr.enable_partno=BB-UART1,BB-UART2

Now trying to figure out how to set up the symbolic links to these devices.  Can get more information about these devices with command:

    sudo udevadm info --name=ttyO1 --query=all --attribute-walk

Have two ways that work:

    SUBSYSTEM=="tty", ATTRS{port}=="0x0", ATTRS{line}=="1", SYMLINK+="ttyXBEE"
    SUBSYSTEM=="tty", ATTRS{port}=="0x0", ATTRS{line}=="2", SYMLINK+="ttyRCLAW"

    SUBSYSTEM=="tty", KERNEL=="ttyO1", SYMLINK+="ttyXBEE"
    SUBSYSTEM=="tty", KERNEL=="ttyO2", SYMLINK+="ttySSC-32"


Download and Build this code.
=============================

Then you can copy or git the sources that I have up on github. The commands I did to do this is:

    cd ~
    mkdir git
    cd git
     git clone git://github.com/KurtE/Raspberry_Pi 
    cd Raspberry_Pi/Phoenix 
    make

PCM Sound and ESpeak (EDIT)
====================

I want some form of sound capability on my robots, to help with different things like simply 
acknowledgments and also fun to play with speech.  There is now code in the library directory for
doing this.


I have been playing around with installing everything necessary to be able to install my Phoenix code 
on this Pi and be able to compile it. I installed it by using the following commands:

First currently code is dependent on using espeak code. Will probably make this optional later... 
To install the necessary stuff you need to do something like:

    sudo apt-get install espeak
    sudo apt-get install libespeak-dev

Also the Phoenix code base now has the capability of outputting tones to the speaker using PCM.  To build
using this capability you will need the appropriate header files and the like:  

    sudo apt-get libasound2-dev
Note: this maybe should be:

    sudo apt-get install libasound2 

Note: you should then try to configure espeak and see if you can get it to work. More information
earlier in this thread, but look at the data up at the link:  http://elinux.org/RPi_Text_to_Speech_%28Speech_Synthesis%29


Recently someone else solved this by uninstalling and re-installing alsa-dev package:

    opkg install alsa-dev

Note: on some systems, the install of libasound did not create the library file:
/usr/lib/libasound.so ,but instead created version 2 specific files.  That is in that directory, there were two files: libasound.so.2 which then points to libasound.so.2.0.0.  In these cases our links with just -lasound will fail.  two ways to fix:

    1) Make our own link: ln -s /dev/lib/libasound.so.2.0.0 /dev/lib/libasound.so
    2) change our link to: -l:libasound.so.2


This did not set the USB device to be default sound device, I used the command: aplay -L
to list the nodes.  From which I created the configuration file: /etc/asound.conf

    pcm.!default sysdefault:Device
    
If linking with -lasound  fails, I have seen on the net two answers.  

The first is to create a link, such that your code will use which ever version of the library you have made your link to (in the case of multiple versions.)

The other is to change the link to be more explicit and and do something like -l:libasound.so.2

As for espeak.  I am just missing the file /usr/include/espeak/speak_lib.h

Next up, install ESpeak, which also relies on a library portaudio. I downloaded the most recent package for
portaudio from www.portaudio.com.  I extracted it and then:

    ./configure
    make install
    
I downloaded espeak-1.47.11-source.zip from sourceforge.net.  After I extracted the stuff from the zip file and
copied down the sources, I then did:

    ./configure
    make clean
    make
    make install

Tried running and found that the portaudio library was in /usr/local/lib
Could maybe move it, but instead defined:

    LD_LIBRARY_PATH=/usr/local/lib
    export LD_LIBRARY_PATH

in /etc/profile

...  



Other notes
=============


Warning
=======

This is a work in progress

