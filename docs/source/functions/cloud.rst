.. _cloud_s:

Cloud
=====

Description
-----------

The Cloud library connects an OpenIndus module to the OpenIndus cloud platform. It exposes
your application data as **cloud variables**, each backed by an MQTT topic, and takes care of
device provisioning, secure transport and reconnection automatically.

The communication uses **MQTT over WebSocket (TLS)**. A dedicated background task handles the
whole lifecycle, so your application code stays minimal: declare the variables, call
:cpp:func:`Cloud::begin`, and read/write the values in your ``loop()``. Nothing blocks.

.. note::
   The cloud requires an active internet connection. On a :ref:`OI-Core<OI-Core>` module this is
   typically provided by the cellular modem or by :ref:`Ethernet<ethernet_s>`. Make sure the
   network is up before (or shortly after) calling ``begin()``.

Variables
---------

A cloud variable has:

* a **name**, which is also the leaf of its MQTT topic,
* a **type**: ``bool``, ``int``, ``float`` or ``std::string`` (``BoolVariable``, ``IntVariable``,
  ``FloatVariable``, ``StringVariable``),
* an **update method** (:cpp:enum:`UpdateMethod`):

  * ``SYNCHRONOUS`` — published once every ``refreshInterval`` milliseconds,
  * ``ASYNCHRONOUS`` — published when the value changes, rate-limited to at most once every
    ``refreshInterval`` ms and at least once every ``maxRefreshInterval`` ms,

* an **update type / direction** (:cpp:enum:`UpdateType`):

  * ``PUBLISH`` — the module publishes the topic,
  * ``SUBSCRIBE`` — the module subscribes to the topic (use ``onReceive()`` to get a callback),
  * ``BOTH`` — the module both publishes and subscribes.

Default variables
-----------------

The following variables are created and managed automatically to provide base functionality:

.. list-table::
   :widths: 15 12 18 55
   :header-rows: 1
   :align: center

   * - Name
     - Type
     - Direction
     - Function
   * - ``ota``
     - string
     - subscribe / publish
     - Firmware update channel. Carries a JSON command envelope
       ``{"cmd": <int>, "args": <string|int>}`` in both directions:
       ``cmd = 0`` (UPDATE, cloud to module) with ``args`` the firmware download
       URL; ``cmd = 1`` (PROGRESS, module to cloud) with ``args`` the number of
       bytes written so far; ``cmd = 2`` (END, module to cloud) with ``args`` the
       error code, ``0`` meaning success. On UPDATE the module streams the image
       into its inactive OTA partition and reboots on it once the image is valid.
   * - ``restart``
     - bool
     - subscribe
     - Reboots the module when set
   * - ``log``
     - string
     - publish
     - Journalises info/errors from the module (see :cpp:func:`Cloud::log`)
   * - ``version``
     - string
     - publish
     - Publishes the firmware version on connection
   * - ``status``
     - int
     - publish
     - Publishes the module state (see :cpp:func:`Cloud::setStatus`): 0 ok, 1 warning, 2 error

Provisioning
------------

The preferred usage only specifies the **platform** credentials; the library obtains a device
``uuid`` and ``token`` on its own and stores them in NVS. On the first boot (or when the stored
platform UUID no longer matches), the module:

#. registers itself against the platform (``POST /api/v1/plateform/{platform_uuid}/device``),
#. waits for the device to be **accepted** by a user in the platform interface
   (``GET .../device/{device_uuid}/status``),
#. saves the returned credentials and connects to the MQTT broker.

On subsequent boots the credentials are read back from NVS and the module connects directly.

Alternatively, if you already have a device ``uuid``/``token``, call
:cpp:func:`Cloud::useDeviceCredentials` before ``begin()`` to skip provisioning entirely.

MQTT topics
-----------

Variable topics use the form ``device/{device_uuid}/{type}/{variable_name}`` where ``{type}`` is
``def`` (default variable), ``b`` (bool), ``i`` (int), ``f`` (float) or ``s`` (string). Values are
transmitted as plain text (``"0"``/``"1"`` for booleans, the decimal representation for numbers).

Code examples
-------------

The example below provisions a module, publishes two variables and reacts to a value pushed from
the cloud:

.. literalinclude:: ../../examples/Cloud.cpp
    :language: cpp

Software API
------------

.. doxygenclass:: Cloud
    :members:

.. doxygenclass:: CloudVariable
    :members:

.. doxygenenum:: UpdateMethod

.. doxygenenum:: UpdateType
