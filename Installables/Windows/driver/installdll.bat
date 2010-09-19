@echo on

if "%PROCESSOR_ARCHITECTURE%" == AMD64 GOTO AMD64
if "%PROCESSOR_ARCHITEW6432%" == AMD64 GOTO AMD64
if "%PROCESSOR_ARCHITECTURE%" == IA64 GOTO IA64
if "%PROCESSOR_ARCHITEW6432%" == IA64 GOTO IA64

copy x86\libusb0_x86.dll %SystemRoot%\system32\libusb0.dll
goto end

:AMD64
copy amd64\libusb0.dll %SystemRoot%\system32\libusb0.dll
goto end

:IA64
copy ia64\libusb0.dll %SystemRoot%\system32\libusb0.dll
goto end

:end
