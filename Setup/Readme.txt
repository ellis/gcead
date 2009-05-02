Date: 2009-05-02

I'm currently using BitRock InstallBuilder to create the installers from
the linux command line.

For the Linux installer:
- build GcEad in release mode
- run ./copylibs.sh
- ${path_to_builder} build GcEad-linux.xml linux

For the Windows installer:
- build GcEad in release mode on Windows
- copy GcEad.exe to the linux machine in the gcead/Installables/Windows directory
- ${path_to_builder} build GcEad-windows.xml windows

