/** @file
 *  @brief HoG Service sample
 */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <zephyr/drivers/gpio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "hid.h"         // clone of USB hid.h
#include "ascii2hid.h"

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(hog);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
enum {
	HIDS_REMOTE_WAKE = BIT(0),
	HIDS_NORMALLY_CONNECTABLE = BIT(1),
};

struct hids_info {
	uint16_t version; /* version number of base USB HID Specification */
	uint8_t code;     /* country HID Device hardware is localized for.*/
	uint8_t flags;
} __packed;

struct hids_report {
	uint8_t id;   /* report id   */
	uint8_t type; /* report type */
} __packed;

static struct hids_info info = {
	.version = 0x0000,
	.code = 0x00,
	.flags = HIDS_NORMALLY_CONNECTABLE,
};

enum {
	HIDS_INPUT = 0x01,
	HIDS_OUTPUT = 0x02,
	HIDS_FEATURE = 0x03,
};

static struct hids_report input = {
	.id = 0x01,
	.type = HIDS_INPUT,
};

static uint8_t simulate_input;
static uint8_t ctrl_point;

/*
 *   User predefined keyboard mapping -- TBD trim it later.
 */
static uint8_t report_map[] = HID_KEYBOARD_REPORT_DESC();



/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static ssize_t read_info(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr, void *buf,
			  uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
				 sizeof(struct hids_info));
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static ssize_t read_report_map(struct bt_conn *conn,
			       const struct bt_gatt_attr *attr, void *buf,
			       uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, report_map,
				 sizeof(report_map));
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static ssize_t read_report(struct bt_conn *conn,
			   const struct bt_gatt_attr *attr, void *buf,
			   uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
				 sizeof(struct hids_report));
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void input_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	simulate_input = (value == BT_GATT_CCC_NOTIFY) ? 1 : 0;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static ssize_t read_input_report(struct bt_conn *conn,
				 const struct bt_gatt_attr *attr, void *buf,
				 uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset, NULL, 0);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static ssize_t write_ctrl_point(struct bt_conn *conn,
				const struct bt_gatt_attr *attr,
				const void *buf, uint16_t len, uint16_t offset,
				uint8_t flags)
{
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(ctrl_point)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);

	return len;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
#if CONFIG_SAMPLE_BT_USE_AUTHENTICATION
/* Require encryption using authenticated link-key. */
#define SAMPLE_BT_PERM_READ BT_GATT_PERM_READ_AUTHEN
#define SAMPLE_BT_PERM_WRITE BT_GATT_PERM_WRITE_AUTHEN
#else
/* Require encryption. */
#define SAMPLE_BT_PERM_READ BT_GATT_PERM_READ_ENCRYPT
#define SAMPLE_BT_PERM_WRITE BT_GATT_PERM_WRITE_ENCRYPT
#endif

/* HID Service Declaration */
BT_GATT_SERVICE_DEFINE(hog_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_HIDS),
	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_INFO, BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, read_info, NULL, &info),
	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT_MAP, BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, read_report_map, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       SAMPLE_BT_PERM_READ,
			       read_input_report, NULL, NULL),
	BT_GATT_CCC(input_ccc_changed,
		    SAMPLE_BT_PERM_READ | SAMPLE_BT_PERM_WRITE),
	BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
			   read_report, NULL, &input),
	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_CTRL_POINT,
			       BT_GATT_CHRC_WRITE_WITHOUT_RESP,
			       BT_GATT_PERM_WRITE,
			       NULL, write_ctrl_point, &ctrl_point),
);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void hog_init(void)
{
	// TBD
}

#define SW0_NODE   DT_ALIAS(sw0)

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void hog_send_string(char * string) 
{
	int keycode;

	static uint8_t report[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	for (int i=0; ; i++) {

		memset(&report, 0, sizeof(report));
		
		if (string[i] == 0) {

			LOG_INF("Send key release");

			bt_gatt_notify(NULL, &hog_svc.attrs[5],
					       report, sizeof(report));
			break;
		}
		else {

			keycode = ascii_to_hid(string[i]);

			if (keycode == -1) {
				LOG_WRN("bad char in string: 0x%02X", string[i]);
				continue;
			}
			LOG_INF("send keycode 0x%02X -- '%c'", keycode, 
				(string[i] >= 32) ? string[i] : '.');

			if (needs_shift(string[i])) {
				report[0] |= HID_KBD_MODIFIER_RIGHT_SHIFT;
			}
			report[7] = keycode;

			bt_gatt_notify(NULL, &hog_svc.attrs[5],
					       report, sizeof(report));
		}			
	}
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void hog_button_loop(void)
{
	static char * string = "0.457mm\n";

	const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

	gpio_pin_configure_dt(&sw0, GPIO_INPUT);

	while (true) {

		if (gpio_pin_get_dt(&sw0)) {   // change to use real button support

			LOG_INF("button pressed...");

			hog_send_string( string );
		}

		k_sleep(K_MSEC(100));
	}
}
