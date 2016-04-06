# General

The IDAC USB devices are initialized in two stages.
In the first stage, the device is located and firmware is uploaded to it.
The device then reboots using the new firmware.  This will take several seconds,
in which time the device is unavailable.  While waiting for the IDAC to come online again,
the GUI will need to poll for the device.
Assuming that the IDAC rebooted sucessfully, the GUI will now be able to interact with the fully functional IDAC.

# IDAC 2

The maximum input voltage is +-5V (the range is therefore 10V).

The IDAC2 accepts six range settings, 0 - 5:

* range 0: +-5V
* range 1: +-5V / 4
* range 2: +-5V / 16
* range 3: +-5V / 64
* range 4: +-5V / 256
* range 5: +-5V / 1024

