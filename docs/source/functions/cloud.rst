.. _cloud_s:

Cloud
=====

Description
-----------

The Cloud library connects an OpenIndus module to the OpenIndus cloud platform. It exposes
your application data as **cloud variables**, each backed by an MQTT topic, and takes care of
device provisioning, secure transport, over-the-air firmware updates and reconnection
automatically.

The communication uses **MQTT over WebSocket (TLS)**. A dedicated background task handles the
whole lifecycle, so your application code stays minimal: declare the variables, call
:cpp:func:`Cloud::begin`, and read/write the values in your ``loop()``. Nothing blocks.

.. note::
   The cloud requires an active internet connection. On a :ref:`OI-Core<OI-Core>` module this is
   typically provided by the cellular modem or by :ref:`Ethernet<ethernet_s>`. Make sure the
   network is up before (or shortly after) calling ``begin()``.

``begin()`` can be called without argument, in which case the default host
``oicloud.openindus.com`` is used; pass a host explicitly to target another instance.

Variables
---------

A cloud variable has:

* a **name**, which is also the leaf of its MQTT topic,
* a **type**: ``bool``, ``int``, ``float`` or ``std::string`` (``BoolVariable``, ``IntVariable``,
  ``FloatVariable``, ``StringVariable``),
* an **update method** (:cpp:enum:`UpdateMethod`):

  * ``SYNCHRONOUS`` — published once every ``refreshInterval`` milliseconds,
  * ``ASYNCHRONOUS`` — published when the value changes, rate-limited to at most once every
    ``refreshInterval`` ms and at least once every ``maxRefreshInterval`` ms. A
    ``maxRefreshInterval`` of ``0`` disables the periodic heartbeat, so the variable is
    published only when its value actually changes,

* an **update type / direction** (:cpp:enum:`UpdateType`):

  * ``PUBLISH`` — the module publishes the topic,
  * ``SUBSCRIBE`` — the module subscribes to the topic (use ``onReceive()`` to get a callback),
  * ``BOTH`` — the module both publishes and subscribes.

Every publisher variable is re-announced on each new MQTT session, so the cloud always sees a
fresh value after a reconnection.

Default variables
-----------------

The following variables are created and managed automatically to provide base functionality.
They live under the ``def`` topic type (see `MQTT topics`_) and, when they publish, they are all
``ASYNCHRONOUS`` with no heartbeat: they are emitted only when their value changes.

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
     - subscribe (+ progress published on the same topic)
     - Firmware update channel, see `Firmware update (OTA)`_ below
   * - ``log``
     - string
     - publish
     - Journalises info/errors from the module (see :cpp:func:`Cloud::log`)
   * - ``version``
     - string
     - publish
     - Published once on connection: the ``projectVersion`` passed to the constructor, or the
       firmware software version when none was given
   * - ``modules``
     - string
     - publish
     - Published once on connection: a JSON array describing the local board and, on a master,
       every module discovered on the rail. Each entry holds ``serial_number``, ``position``,
       ``version``, ``board_type``, ``variant`` and ``timestamp``. The local board is reported
       at position ``1023`` so it always comes first

Provisioning
------------

The preferred usage only specifies the **platform** credentials; the library obtains a device
``uuid`` and ``token`` on its own and stores them in NVS. On the first boot (or when the stored
platform UUID no longer matches), the module:

#. registers itself against the platform (``POST /api/v1/platform/{platform_uuid}/device``),
#. waits for the device to be **accepted** by a user in the platform interface
   (``GET /api/v1/plateform/{platform_uuid}/device/{device_uuid}/status``),
#. saves the returned credentials and connects to the MQTT broker.

On subsequent boots the credentials are read back from NVS and the module connects directly.

Alternatively, if you already have a device ``uuid``/``token``, call
:cpp:func:`Cloud::useDeviceCredentials` before ``begin()`` to skip provisioning entirely.

The progress of this sequence is reported by :cpp:func:`Cloud::getState`
(:cpp:enum:`CloudState`); :cpp:func:`Cloud::isConnected` is the simple check for "the link is
up". A mid-session MQTT drop is first left to the transport's own auto-reconnect and only
escalates to a full re-provisioning pass if it does not recover within 30 s.

MQTT topics
-----------

Variable topics use the form ``device/{device_uuid}/{type}/{variable_name}`` where ``{type}`` is
``def`` (default variable), ``b`` (bool), ``i`` (int), ``f`` (float) or ``s`` (string). Values are
transmitted as plain text (``"0"``/``"1"`` for booleans, the decimal representation for numbers).

Firmware update (OTA)
---------------------

The whole update is driven from the OpenIndus cloud web application at
`oicloud.openindus.com <https://oicloud.openindus.com>`_: you upload the firmware binary there,
trigger the update on the device, and the image is downloaded straight from that host — no
external storage or build server is involved, and the progress is reported back in the web
application.

The ``ota`` default variable carries a JSON command envelope
``{"cmd": <int>, "args": <string|int>}`` in both directions (see :cpp:enum:`CloudOtaCmd`):

.. list-table::
   :widths: 12 10 18 60
   :header-rows: 1
   :align: center

   * - ``cmd``
     - Name
     - Direction
     - ``args``
   * - ``0``
     - UPDATE
     - cloud to module
     - Firmware download URL, as sent by the web application. A path-only value
       (e.g. ``/firmware/xxx.bin``) is resolved against the cloud host, so the image is fetched
       from ``https://oicloud.openindus.com`` by default; a full URL must use ``https://``
   * - ``1``
     - PROGRESS
     - module to cloud
     - Number of bytes written so far, emitted every 64 KB and once when the image is complete
   * - ``2``
     - END
     - module to cloud
     - Error code, ``0`` meaning success

On UPDATE the module spawns a dedicated task that streams the image straight into its inactive
OTA partition (nothing is staged in RAM), authenticating with its device token — dropped if the
download is redirected off the cloud host, so it never leaks to a third-party storage bucket.
Once the image is validated the module sets the new boot partition, publishes END and reboots on
it. On failure the download is aborted, the error code is published as END and the module keeps
running the current firmware; a new UPDATE command can start a fresh attempt. A second UPDATE is
ignored while one is already in progress.

Generating the application code
-------------------------------

You do not have to write the variable declarations by hand. Once the variables are described in
the web application at `oicloud.openindus.com <https://oicloud.openindus.com>`_, its **code
generation** feature scaffolds a ready-to-build ``main.cpp`` for you: the platform credentials,
the ``OICloud`` instance, one declaration per cloud variable with its type, direction and refresh
policy, and the matching ``addVariable()`` calls in ``setup()``. Download it as your starting
point and fill in the application logic in ``loop()``.

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

.. doxygenenum:: CloudState

.. doxygenenum:: CloudOtaCmd
