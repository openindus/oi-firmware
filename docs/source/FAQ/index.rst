.. _faq-index:

FAQ
===

In this section you will find answers to frequently asked question about our product.
If you cannot find the solution to your problem, please contact us and we will update the FAQ.

Software
--------

My device is not detected by my computer
****************************************

This is a common issue, in most case it is because Windows fails to automatically install the driver. Download it from `Silicon Labs website <https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers>`_. Go to the *"Download"* section and choose the driver adapted to your computer configuration.

If the problem is not solved after a successful driver installation, check if your device is detected when you use an other USB cable.


I cannot set or get the ID of my device
***************************************

The tool for setting or getting the ID use the serial port of the device. If you have a monitor opened you will get an error if your are trying to set or get the ID. Please close the serial monitor and/or unplug and replug the device.

If the problem persist, try to put back the default firmware with the :ref:`update firmware tool<updatedevicefirmware-index>` and try again.


I have a space in my ".platformio" directory or in my project directory
***********************************************************************

Please see `this link <https://docs.platformio.org/en/latest/frameworks/espidf.html#limitations>`_ on the PlaftormIO documentation for available solutions.

Hardware
--------

We will update this section soon !