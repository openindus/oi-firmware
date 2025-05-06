.. _usbhost_s:

Usb Host
========

Description
-----------

With :ref:`OI-Core<OI-Core>`, you can utilize a USB Host port to connect a variety of equipment, including USB sticks for mass storage, cameras, mice, joysticks, and more.

The connectivity is based on a MAX3421E chip. You can use the `USB Host shield library <https://github.com/felis/USB_Host_Shield_2.0>`_ to develop applications on top of USB Host.


Code examples
-------------

The example code above demonstrates how to use the usb host library to connect an HID mouse. 

.. literalinclude:: ../../examples/USBHostHIDMouse.cpp
    :language: cpp