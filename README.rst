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

``` 
*** Booting Zephyr OS build zephyr-v3.3.0-1787-g286f10323ce1 ***
[00:00:12.304,290] <inf> fs_nvs: nvs_mount: 6 Sectors of 4096 bytes
[00:00:12.304,321] <inf> fs_nvs: nvs_mount: alloc wra: 0, f70
[00:00:12.304,321] <inf> fs_nvs: nvs_mount: data wra: 0, e8
[00:00:12.306,213] <inf> bt_hci_core: hci_vs_init: HW Platform: Nordic Semiconductor (0x0002)
[00:00:12.306,243] <inf> bt_hci_core: hci_vs_init: HW Variant: nRF52x (0x0002)
[00:00:12.306,274] <inf> bt_hci_core: hci_vs_init: Firmware: Standard Bluetooth controller (0x00) Version 3.3 Build 99
[00:00:12.306,701] <inf> bt_hci_core: bt_init: No ID address. App must call settings_load()
[00:00:12.306,732] <inf> hog_kb: bt_ready: Bluetooth initialized
[00:00:12.306,732] <inf> hog: hog_init: 
[00:00:12.308,013] <inf> bt_hci_core: bt_dev_show_info: Identity: FB:C4:9A:A9:B0:1B (random)
[00:00:12.308,044] <inf> bt_hci_core: bt_dev_show_info: HCI: version 5.4 (0x0d) revision 0x0000, manufacturer 0x05f1
[00:00:12.308,074] <inf> bt_hci_core: bt_dev_show_info: LMP: version 5.4 (0x0d) subver 0xffff
[00:00:12.311,553] <inf> hog_kb: bt_ready: Advertising started
[00:00:12.311,584] <inf> buttons: buttons_init: 
[00:00:12.311,645] <inf> hog: hog_init: 
[00:00:31.011,352] <inf> bas: blvl_ccc_cfg_changed: BAS Notifications enabled
[00:00:31.011,413] <wrn> bt_gatt: bt_gatt_connected: Failed to set security for bonded peer (-22)
[00:00:31.011,596] <inf> hog_kb: connected: Connected 00:1A:7D:DA:71:13 (public)
[00:00:31.703,308] <inf> hog_kb: security_changed: Security changed: 00:1A:7D:DA:71:13 (public), level 2
[00:03:55.160,858] <inf> hog: hog_button_event: Button 1
``` 

The result of pressing button 1 and viewed from shell terminal is shown below

```  
user@ubuntu:~$ echo 0.123in
0.123in
```


