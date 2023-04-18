.. _peripheral_hids:

Bluetooth: HID Over GATT -- HOG Keyboard
##########################

Overview
********

Similar to the :ref:`Peripheral <ble_peripheral>` sample, except that this
application specifically exposes the HID GATT Service. The report map used is
for a generic keyboard.

In the default configuration the sample uses passkey authentication (displays a
code on the peripheral and requires that to be entered on the host during
pairing) and requires an authenticated link to access the GATT characteristics.
To disable authentication and just use encrypted channels instead, build the
sample with `CONFIG_SAMPLE_BT_USE_AUTHENTICATION=n`.

Requirements
************

* BlueZ running on the host, or
* A board with BLE support

Building and Running
********************

Currently this is not fully functional.  
There are security-related issues.
It does work on Ubuntu 20.04.6 LTS, but fails with Android, IoS, and MacOS.
