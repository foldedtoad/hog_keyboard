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
[00:00:02.060,485] <inf> fs_nvs: nvs_mount: 6 Sectors of 4096 bytes
[00:00:02.060,516] <inf> fs_nvs: nvs_mount: alloc wra: 0, ef0
[00:00:02.060,516] <inf> fs_nvs: nvs_mount: data wra: 0, 284
[00:00:02.062,438] <inf> bt_hci_core: hci_vs_init: HW Platform: Nordic Semiconductor (0x0002)
[00:00:02.062,469] <inf> bt_hci_core: hci_vs_init: HW Variant: nRF52x (0x0002)
[00:00:02.062,500] <inf> bt_hci_core: hci_vs_init: Firmware: Standard Bluetooth controller (0x00) Version 3.3 Build 99
[00:00:02.062,896] <inf> bt_hci_core: bt_init: No ID address. App must call settings_load()
[00:00:02.062,927] <inf> hog_kb: bt_ready: Bluetooth initialized
[00:00:02.064,819] <inf> bt_hci_core: bt_dev_show_info: Identity: FB:C4:9A:A9:B0:1B (random)
[00:00:02.064,849] <inf> bt_hci_core: bt_dev_show_info: HCI: version 5.4 (0x0d) revision 0x0000, manufacturer 0x05f1
[00:00:02.064,880] <inf> bt_hci_core: bt_dev_show_info: LMP: version 5.4 (0x0d) subver 0xffff
[00:00:02.068,328] <inf> hog_kb: bt_ready: Advertising started
[00:00:34.242,309] <inf> bas: blvl_ccc_cfg_changed: BAS Notifications enabled
[00:00:34.242,340] <inf> bt_smp: sec_level_reachable: BT_SECURITY_L3
[00:00:34.242,370] <inf> bt_smp: sec_level_reachable: io_capa(3) != BT_SMP_IO_NO_INPUT_OUTPUT(3)
[00:00:34.242,370] <err> bt_smp: smp_keys_check: 384: return false
[00:00:34.242,401] <inf> bt_smp: smp_send_security_req: 2798: EINVAL
[00:00:34.242,401] <wrn> bt_gatt: bt_gatt_connected: Failed to set security for bonded peer (-22)
[00:00:34.242,584] <inf> hog_kb: connected: Connected 00:1A:7D:DA:71:13 (public)
[00:00:34.242,614] <inf> bt_smp: sec_level_reachable: BT_SECURITY_L2
[00:00:34.955,535] <inf> hog_kb: security_changed: Security changed: 00:1A:7D:DA:71:13 (public), level 2
[00:00:49.001,373] <inf> hog: hog_button_loop: button pressed...
[00:00:49.001,403] <inf> hog: hog_send_string: send keycode 0x27 -- '0'
[00:00:49.001,586] <inf> hog: hog_send_string: send keycode 0x37 -- '.'
[00:00:49.001,739] <inf> hog: hog_send_string: send keycode 0x21 -- '4'
[00:00:49.001,922] <inf> hog: hog_send_string: send keycode 0x22 -- '5'
[00:00:49.038,513] <inf> hog: hog_send_string: send keycode 0x24 -- '7'
[00:00:49.039,123] <inf> hog: hog_send_string: send keycode 0x10 -- 'm'
[00:00:49.039,733] <inf> hog: hog_send_string: send keycode 0x10 -- 'm'
[00:00:49.040,344] <inf> hog: hog_send_string: send keycode 0x28 -- '.'
[00:00:49.040,954] <inf> hog: hog_send_string: Send key release
```
