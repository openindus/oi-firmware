
.. _OIconsole:

OIConsole
=========

Description
-----------

OIConsole is a feature that allow you to execute commands on your module directly from a serial monitor via USB interface.
By default console is not set you have to set it in your code if you want to use it.

.. warning:: OIConsole is not compatible with Arduino Serial. Please make sure you are not using both at the same time.

Example
-------

If you want the Console in your monitor terminal, add the following line in your code. 

.. code-block:: cpp

    Console.start();

Result when you open monitor terminal :

.. code-block:: console

    Type 'help' to get the list of commands.
    Use UP/DOWN arrows to navigate through command history.
    Press TAB when typing command name to auto-complete.

    ==================================================
    |                                                |
    |       Welcome to the OpenIndus System          |
    |                                                |
    ==================================================

    OICore> 

OIConsole commands 
------------------

Restart module :

.. code-block:: console

    OICore> restart

Set ID :

.. code-block:: console

    OICore> set-id 4

Set ID 4 to your module.

Get ID :

.. code-block:: console

    OICore> get-id
    4

Set log level :

.. code-block:: console

    OICore> log-level 1

.. note:: LOG levels are from 0 to 5.

- 0 : LOG_NONE (No log output)
- 1 : LOG_ERROR (Critical errors, software module can not recover on its own)
- 2 : LOG_WARN (Error conditions from which recovery measures have been taken)
- 3 : LOG_INFO (Information messages which describe normal flow of events)
- 4 : LOG_DEBUG (Extra information which is not necessary for normal use (values, pointers, sizes, etc))
- 5 : LOG_VERBOSE (Bigger chunks of debugging information, or frequent messages which can potentially flood the output.)

.. warning :: LOGs significantly affect program performances, use it only in development.

Ping module with specific ID :

.. code-block:: console

    OICore> ping 4
    0
    I (8933) OIConsole: ping module id: 4 time: 874 us

Ping module with ID 4.

.. warning:: You need to set at least LOG_INFO to see ping information.

Functions
---------

.. doxygenfunction:: OIConsole::start


