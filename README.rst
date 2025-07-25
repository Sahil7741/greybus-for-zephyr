.. _greybus_for_zephyr:

******************
Greybus for Zephyr
******************


Build Status
############
.. image:: https://github.com/Sahil7741/greybus-for-zephyr/actions/workflows/module.yml/badge.svg
   :target: https://github.com/Sahil7741/greybus-for-zephyr/actions/workflows/module.yml
   :alt: Greybus Module CI

Overview
########
This repository contains a `Greybus <https://lwn.net/Articles/715955/>`_
`module <https://docs.zephyrproject.org/latest/guides/modules.html>`_ for the
`Zephyr Real-Time Operating System <https://zephyrproject.org/>`_ and the assosciated changes for instantiating mikroBUS ports over Greybus.

Building and Running
####################

First, ensure that all required tools are installed by following Zephyr's
`Getting Started Guide <https://docs.zephyrproject.org/latest/getting_started/index.html>`_.

Next, clone and synchronize repositories.

.. code-block:: bash

    west init -m https://openbeagle.org/beagleconnect/greybus-for-zephyr.git zephyrproject
    cd zephyrproject
    west update

While that is updating, choose a board from Zephyr's list of 
`Supported Boards <https://docs.zephyrproject.org/latest/boards/index.html>`_.

Next, update git submodules. Currently, there is a git submodule for the
``manifesto`` utility.

.. code-block:: bash

    cd greybus
    git submodule init
    git submodule update
    cd ..

Lastly, flash firmware for the chosen ``$BOARD`` using the commands below.

.. code-block:: bash

    cd zephyr
    source zephyr-env.sh
    git apply ../modules/greybus/zephyr-gpio-get-direction.patch
    git apply ../modules/greybus/zephyr-sys-byteorder-posix.patch
    git apply ../modules/greybus/zephyr-dynamic-pthread-stacks.patch
    west build -b $BOARD -t flash ../modules/greybus/samples/subsys/greybus/net

A number of patches are required until respective functionality is added upstream in
the Zephyr Project.

Note, there may be an overlay required for the connectivity options that a particular
board supports.

IEEE 802.15.4 Overlay
*********************

In that case, say for IEEE 802.15.4 support, the ``west build`` command
above should be modified as shown below.

.. code-block:: bash

    west build -b $BOARD -t flash ../modules/greybus/samples/subsys/greybus/net \
        -- -DOVERLAY_CONFIG="overlay-802154.conf"

Bluetooth Low Energy Overlay
****************************

In that case, say for BLE support, the ``west build`` command
above should be modified as shown below.

.. code-block:: bash

    west build -b $BOARD -t flash ../modules/greybus/samples/subsys/greybus/net \
        -- -DOVERLAY_CONFIG="overlay-bt.conf"

Network Configuration
#####################

While Greybus is capable of operating over any reliable transport, the focus
of this project is TCP/IP (although quite an assortment of physical and
data-link layers are `supported in Zephyr <https://docs.zephyrproject.org/latest/samples/net/sockets/echo_server/README.html>`_).

Depending on the board that was chosen and the available connectivity
options, some additional steps may be required for network connectivity.

* `Network Setup for Bluetooth Low Energy <doc/ble-setup.rst>`_
* `Network Setup for IEEE 802.15.4 <doc/802154-setup.rst>`_

Once network connectivity is established, it should be possible to use an
`mDNS <https://en.wikipedia.org/wiki/Multicast_DNS>`_ browser such as
`Avahi <https://www.avahi.org/>`_ to scan for `service discovery <https://en.wikipedia.org/wiki/Zero-configuration_networking>`_.  

.. code-block:: bash

    avahi-browse -t -r _greybus._tcp
    + lowpan0 IPv6 zephyr              _greybus._tcp        local
    = lowpan0 IPv6 zephyr              _greybus._tcp        local
       hostname = [zephyr.local]
       address = [2001:db8::1]
       port = [4242]
       txt = []

Client-Side Software Requirements
#################################

A Linux machine is required in order to connect to and control our Zephyr
device via Greybus so be sure to follow
`Linux Software Requirements <doc/linux-setup.rst>`_.

Using Greybus for I/O
#####################

At this point, we should be ready to perform some I/O on our remote devices
using Greybus. Currently, this module supports the protocols below. 

* `GPIO <doc/gpio.rst>`_
* `I2C <doc/i2c.rst>`_
* `SPI <doc/spi.rst>`_

Trying out different add-on boards/devices over mikroBUS
####################

Currently the add-on board manifests are selected at compile time(temporarily until add-on board ID driver is implemented in Zephyr) and the add-on board can be choosen by updating the Kconfig options `here <https://github.com/vaishnav98/greybus-for-zephyr/blob/9f937760960a8303179bff6b9c6fefc9d9622d38/samples/subsys/greybus/net/boards/cc1352r1_launchxl.conf#L14>`_

.. code-block:: bash

    CONFIG_GREYBUS_CLICK_MANIFEST_BUILTIN=y
    CONFIG_GREYBUS_MIKROBUS_CLICK1_NAME="WEATHER-CLICK"
    CONFIG_GREYBUS_MIKROBUS_CLICK2_NAME="AIR-QUALITY-2-CLICK"

The names of the add-on boards should be specified same as that present in the `manifesto/manifests <https://github.com/vaishnav98/manifesto/tree/6b68006f6c62f3d680b947d4a91068be9ff22218/manifests>`_ repository.

Describing On-board devices through Zephyr DT
####################

For MCU Clients like the SensorTag,the on-board fixed devices(like the OPT3001) can be described over the `Zephyr Device Tree Overlay <https://github.com/vaishnav98/greybus-for-zephyr/blob/9f937760960a8303179bff6b9c6fefc9d9622d38/samples/subsys/greybus/net/boards/cc1352r_sensortag.overlay#L180>`_ in this manner :

.. code-block:: bash

    gbstring3 {
                label = "GBSTRING_3";
                status = "okay";
                compatible = "zephyr,greybus-string";
                id = <3>;
                greybus-string = "opt3001";
        };

    gbdevice0 {
            label = "GBDEVICE_0";
            status = "okay";
            compatible = "zephyr,greybus-device";
            id = <1>;
            driver-string-id = <&gbstring3>;
            protocol = <3>;
            addr = <0x44>;
        };

Contribute Back
***************

In case a board was chosen that does not yet support Greybus, consider
submitting a `DeviceTree <https://www.devicetree.org/>`_ overlay file similar
to `cc1352r_sensortag.overlay <samples/subsys/greybus/net/boards/cc1352r_sensortag.overlay>`_
and a board-specific configuration file similar to
`cc1352r_sensortag.conf <samples/subsys/greybus/net/boards/cc1352r_sensortag.conf>`_.

The contribution guidelines for this module follow the
`Zephyr Contribution Guidelines <https://docs.zephyrproject.org/latest/contribute/index.html>`_.

To summarize,

#. Create an `Issue <https://openbeagle.org/beagleconnect/zephyr/greybus-for-zephyr/issues>`_
#. Code up your contribution
#. Code up any `tests <tests>`_ and `samples <samples>`_ as required
#. Ensure that tests pass and samples run as expected
#. Make a `Pull Request <openbeagle.org/beagleconnect/zephyr/greybus-for-zephyr/-/merge_requests>`_

Additional Information
**********************

Additional Information about Greybus including videos, slide presentations,
and deprecated demo instructions can be found `here <doc/old.md>`_.

A compiled version of the `Greybus Specification <https://github.com/projectara/greybus-spec>`_
is available `here <doc/GreybusSpecification.pdf>`_.
