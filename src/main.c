/* main.c - HOG-based keyboard - main entry point */

/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/bas.h> 
#include <zephyr/bluetooth/services/dis.h> 

#include "hog.h"
#include "buttons.h"

#define LOG_LEVEL 3
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(hog_kb);

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define FIXED_PASSKEY  123456U

static const struct bt_data advert[] = {
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE,
                  (CONFIG_BT_DEVICE_APPEARANCE >> 0) & 0xff,
                  (CONFIG_BT_DEVICE_APPEARANCE >> 8) & 0xff),
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
              BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL),
              BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

static const struct bt_data scand[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static const struct bt_le_adv_param *advert_param = 
    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME,
                    BT_GAP_ADV_FAST_INT_MIN_2,
                    BT_GAP_ADV_FAST_INT_MAX_2,
                    NULL);

static const char * levels[] = {
    "L0",
    "L1",
    "L2",
    "L3",
    "L4",
};

static const char * errors[] = {
    "SUCCESS",
    "AUTH_FAIL",
    "PIN_OR_KEY_MISSING",
    "OOB_NOT_AVAILABLE",
    "AUTH_REQUIREMENT",
    "PAIR_NOT_SUPPORTED",
    "PAIR_NOT_ALLOWED",
    "INVALID_PARAM",
    "KEY_REJECTED",
    "UNSPECIFIED",
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void start_advertising(void)
{
    int err;

    err = bt_le_adv_start(advert_param, 
                          advert, ARRAY_SIZE(advert), 
                          scand, ARRAY_SIZE(scand));
    if (err) {
        if (err == -EALREADY) {
            LOG_INF("Advertising continued");
        } 
        else {
            LOG_ERR("Start advertising failed: %d", err);
        }
        return;
    }

    LOG_INF("Advertising started");
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void connected(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (err) {
        LOG_ERR("Failed to connect to %s (%u)", addr, err);
        return;
    }

    LOG_INF("Connected %s", addr);

    int ret = bt_conn_set_security(conn, BT_SECURITY_L2);
    if (ret) {
        LOG_ERR("Failed to set security: %d", ret);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Disconnected from %s, reason %d", addr, reason);

    start_advertising();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void security_changed(struct bt_conn *conn, bt_security_t level,
                 enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    if (!err) {
        LOG_INF("Security changed: %s, level %s", addr, levels[level]);
    }
    else {
        LOG_ERR("Security failed: %s, level %s, err %s", 
                 addr, levels[level], errors[err]);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected        = connected,
    .disconnected     = disconnected,
    .security_changed = security_changed,
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void bt_ready(int err)
{
    if (err) {
        LOG_ERR("Bluetooth init failed: %d", err);
        return;
    }

    LOG_INF("Bluetooth initialized");

    hog_init();

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    start_advertising();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Passkey for %s: %06u", addr, passkey);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_WRN("Pairing cancelled: %s", addr);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static struct bt_conn_auth_cb auth_cb_display = {
    .passkey_display = auth_passkey_display,
    .passkey_entry   = NULL,
    .cancel          = auth_cancel,
};

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
static void bas_notify(void)
{
    uint8_t battery_level = bt_bas_get_battery_level();

    battery_level--;

    if (battery_level == 50) {
        battery_level = 100U;
    }

    bt_bas_set_battery_level(battery_level);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void main(void)
{
    int err;

    err = bt_enable(bt_ready);
    if (err) {
        LOG_ERR("Bluetooth not ready: %d", err);
        return;
    }

    bt_conn_auth_cb_register(&auth_cb_display);
    LOG_INF("Bluetooth authentication callbacks registered.");

    err = bt_passkey_set(FIXED_PASSKEY);
    if (err) {
        LOG_INF("Fixed Passkey Set failed: %d", err);
    }
    else {
        LOG_INF("Fixed Passkey Set to %u", FIXED_PASSKEY);
    }


    buttons_init();

    //hog_init();

    while (1) {
        /* Battery level simulation */
        k_sleep(K_SECONDS(5));
        bas_notify();
    }    
}
