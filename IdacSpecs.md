# General

The IDAC USB devices are initialized in two stages.
In the first stage, the device is located and firmware is uploaded to it.
The device then reboots using the new firmware.  This will take several seconds,
in which time the device is unavailable.  While waiting for the IDAC to come online again,
the GUI will need to poll for the device.
Assuming that the IDAC rebooted sucessfully, the GUI will now be able to interact with the fully functional IDAC.

# IDAC 2

