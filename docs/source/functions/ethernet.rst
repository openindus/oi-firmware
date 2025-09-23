.. _ethernet_s:

Ethernet 10BaseT/100BaseTX
==========================

Description
-----------

Ethernet functionality is available on the :ref:`OI-Core<OI-Core>` module, featuring an embedded Wiznet W5500 chip. This allows for communication over any Ethernet network.

Communication between the micro-controller unit (MCU) and the Ethernet chip is facilitated through an SPI link. Example code for this setup is provided.

.. To enhance communication performance, consider utilizing the esp-idf framework.

.. Configuration:

.. * Ethernet SPI (W5500)
.. * SPI HOST Number: 1
.. * SPI MOSI GPIO: 15
.. * SPI MISO GPIO: 16
.. * SPI CLOCK GPIO: 17
.. * SPI CS GPIO: 18
.. * SPI INTERRUPT GPIO: 34
.. * SPI RESET GPIO: 35

.. For additional information and example code, refer to: `GitHub - esp-idf/examples/ethernet/basic <https://github.com/espressif/esp-idf/tree/master/examples/ethernet>`_

The W5500 chip serves as a Hardwired TCP/IP embedded Ethernet controller, simplifying Internet connectivity for embedded systems. It integrates a TCP/IP stack, 10/100 Ethernet MAC, and PHY.

Features of the W5500 chip include:

* Support for Hardwired TCP/IP Protocols: TCP, UDP, ICMP, IPv4, ARP, IGMP, PPPoE
* Simultaneous support for 8 independent sockets
* Power-down mode and Wake on LAN over UDP functionality
* Embedded 10BaseT/100BaseTX Ethernet PHY
* Auto Negotiation support for full/half duplex and 10/100-based operation

.. note:: 
    On OI-Core, ethernet does not support auto-MDIX feature. Thus, user should use straight-through cables to connect to other switches or routers and
    crossover cables to connect to devices such as servers, workstations or another W5500.
    However, user can use either type of cable to connect to other devices with auto-MDIX
    enabled, and the interface automatically corrects for any incorrect cabling.


Code examples
-------------

The example code below demonstrates how to initialize the Ethernet interface using the W5500 chip and establish a connection. 
It also includes a simple web server that responds to HTTP requests.

.. literalinclude:: ../../examples/Ethernet.cpp
    :language: cpp

For a complete list of examples, please check the `ethernet library on github <https://github.com/PaulStoffregen/Ethernet/tree/master/examples>`_.