.. _tools_index:

Tools
=====

In this section you will find useful information for using the different commands available in the OpenIndus extension.

.. _getid-index:

Get device ID
*************
This is useful if you want to know the device of a board. 

.. note:: If only one device is plug in your computer, you will get is ID. But if several devices are plugged, you will be asked to select the devices manually.


.. _setid-index:

Set device ID
*************
Use this tool to set the id a device. The ID must be between 0 and 254.

.. _updatedevicefirmware-index:

Update device firmware
**********************
Use this tools to upload or re-upload the standard code to a device. The last stable version of the firmware will be flashed.

.. note:: This is very useful if you want to update or reupload a slave device. For example if your system is composed of an OICore and an OIDiscrete, you put your custom code on the OICore with PlatformIO and you put the standard firmware with this tool.
