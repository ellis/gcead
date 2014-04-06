# GcEAD

GcEad/2012 is software for GC-EAD research. It is free and open source by [Syntech](http://www.syntech.nl/).

## What is GC-EAD?

GC-EAD is coupled gas chromatography - electroantennographic detection. It is an analytical procedure that permits the rapid identification of compounds in complex mixtures that stimulate the olfactory sensilla of an insect. In other words, it can tell you what specific chemicals an insect can smell (and, to some degree, ones it can't), and it can use odors derived directly from natural sources. This information can be used to discover potentially useful compounds (such as sex pheromones) that alter the behavior of insects.  
(Excerpt from [USDA](http://www.srs.fs.usda.gov/idip/spb_ii/gcead_what.html))

## News

* 2012-06-24: Version 1.2.4 released. In the pre-recording dialog, give FID display a fixed height.
* 2011-03-15: Version 1.2.3 released. Compatibility with Windows 7 and IDAC4 bugfix.
* 2010-11-07: Version 1.2.2 released. Update for Windows to let GcEad/2010 use the IDAC drivers from older Syntech software.
* 2010-10-16: Version 1.2.1 released. Significant improvements to marker functionality.
* 2010-09-24: Version 1.2.0 released. Fixed a recording crash and added EAD amplitude measurement.
* 2010-02-07: Version 1.1.2 released. Improved recording view and added time markers for EAD waves.
* 2009-05-25: Version 1.1 released. Now available for Mac and Linux, and now supports IDAC2 acquisition devices.
* 2008-09-24: Project made available on sourceforge.net

## Developer information

The following information is for developers working on the source code -- it is not intended for users.

### Directories

- *Core*: A few semi-generic files that may be used by all sub-projects
- *Idac*:
  Classes to interface between the hardware drivers and the main application code.
  IdacFactory and IdacProxy are the only two IDAC classes that should be used by
  the main application (other than the settings and capabilities structures).
  IdacProxy runs in the main application thread and shields the main application
  programmer from all driver threads and details.
- *IdacDriver*:
  The general driver code.  This is not really a "driver", since it runs in
  user-space via libusb.
- *IdacDriver2*:
  USB protocol implementation for the IDAC2
- *IdacDriver4*:
  USB protocol implementation for the IDAC4
- *Model*:
  The basic data classes for GC-EAD
- *Scope*:
  Classes to implement non-UI code to support the UI.  Basically, this is a
  representation of the UI in a form that can potentially be unit-tested without
  user-interaction.  This approach wasn't taken until near the end of the project,
  so there's not much in here.
- *View*:
  UI code and main()


### USB drivers

The IDAC drivers were hacked together very quickly.  Part of the source-code
for the old proprietary IDAC4 driver was available to me, so that helped.  I
didn't have any source code for IDAC2 though, so I had to make guesses based
on analyzing the USB transfer stream of the proprietary drivers and use what
I had gleaned from the old IDAC4 sourcecode.  There are still a couple loose
ends in the IDAC2 code.

Recording objects:
- Idac: interfaces with hardware
- RecordHandler: converts idac data to raw and display data
- RecordDialog: shows preview of the incoming EAD data and lets user adjust settings
- RecordSettingsDialog: for setting the recording options
- MainScope:
  - adds data to the EadFile
  - tells ChartWidget to update itself when in Recording view
  - auto-stops Idac recording when duration is reached

### Install programs

#### Installer for windows

The installer for Windows is build with NSI.  The following instructions are no longer valid...

* Install Qt SDK.  I installed it in C:\Qt with Qt 5.2.  Adapt Setup/GcEad-windows.nsi for newer versions or different directories.
* build the project (e.g. by opening GcEad.pro with Qt Creator)
* Install NSIS (I have v2.46 installed)
* Open Setup/GcEad-windows.nsi and compile it.
* Upload Setup/GcEad-x.x.x-windows-installer.exe to sourceforge.net.

#### Installer for linux

I have not created a linux installer for a long time.  Back when I did it,
I used BitRock InstallBuilder to create the installer for Linux.

* build the project
* cd Setup
* run: ./copylibs
* run: $path_to_builder build GcEad-linux.xml linux

#### Installer for mac

* make sure the Qt SDK is installed
* install homebrew
* run `brew install automake` from the appropriate user account
* run `make -f libusb.mak`
* optionally, run `cd extern/libusb-compat-0.1 && git clean -fd`
* build the project, for example with QtCreator
* cd release (e.g. ``~/src/build-GcEad-Desktop_Qt_5_2_0_clang_64bit-Release/View``)
* run ``macdeployqt GcEad.app -dmg``;
  since I installed Qt to ~/local/Qt, on my machine the above command line is
  ``~/local/Qt/5.2.0/clang_64/bin/macdeployqt GcEad.app -dmg``
* GcEad.dmg is now ready for deployment
