GC-EAD v1.1
===========

If you want to access the IDAC hardware from Linux, please read this file.

In order to use IDAC hardware with GC-EAD on Linux, the user will need to have
system permission to access the device via USB.  If you installed this program
as the root user, you were given the option of registering the IDAC.  Hopefully
that worked.  Otherwise you'll need to do so manually.

USB permissions are handled differently on various systems.  Here are tips if
you're having problems accessing the IDAC on a system that uses "udev".  To
check whether your system uses "udev", look whether the directory "/etc/udev"
exists.  If not, I'm afraid I won't be able to help, except to note that the
program should work if you run it will root permissions.  Please search the
web for information on granting USB permissions on your system type.

So if you are having problems accessing the IDAC and your system uses udev,
you can try one of the following approaches:

1) as the root user, create a file named /etc/udev/60-syntech.rules and place
the following line in it:
ACTION=="add", SUBSYSTEM=="usb", SYSFS{idVendor}=="088d", MODE="0660", GROUP="plugdev"

2) run the program with root permissions.


GC-EAD is Free and Open Source software licensed under the GPL v3.
Website: http://gcead.sourceforge.net
