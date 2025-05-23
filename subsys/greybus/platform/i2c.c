/*
 * Copyright (c) 2020 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <zephyr/drivers/i2c.h>
#include <dt-bindings/greybus/greybus.h>
#include <greybus/greybus.h>
#include <greybus/platform.h>
#include <stdint.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#define DT_DRV_COMPAT zephyr_greybus_i2c_controller
#include <zephyr/device.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(greybus_platform_i2c_control, CONFIG_GREYBUS_LOG_LEVEL);

#include "../i2c-gb.h"
#include "transport.h"

struct greybus_i2c_control_config {
	const uint8_t id;
	const uint8_t bundle;
	const struct device *greybus_i2c_controller;
	const char *const bus_name;
};

struct greybus_i2c_control_data {
	const struct device *greybus_i2c_controller;
};

static int greybus_i2c_control_init(const struct device *dev)
{

	struct greybus_i2c_control_data *drv_data = (struct greybus_i2c_control_data *)dev->data;
	struct greybus_i2c_control_config *config =
		(struct greybus_i2c_control_config *)dev->config;
	int r;
	const struct device *bus;

	drv_data->greybus_i2c_controller = config->greybus_i2c_controller;
	if (NULL == drv_data->greybus_i2c_controller) {
		return -ENODEV;
	}

	bus = device_get_binding(config->bus_name);
	if (NULL == bus) {
		LOG_ERR("i2c control: failed to get binding for device '%s'", config->bus_name);
		return -ENODEV;
	}

	r = gb_add_cport_device_mapping(config->id, drv_data->greybus_i2c_controller);
	if (r < 0) {
		LOG_ERR("i2c control: failed to add mapping between %u and %s", config->id,
			dev->name);
		return r;
	}

	LOG_DBG("probed cport %u: bundle: %u protocol: %u", config->id, config->bundle,
		CPORT_PROTOCOL_I2C);

	return 0;
}

#define DEFINE_GREYBUS_I2C_CONTROL(_num)                                                           \
                                                                                                   \
	BUILD_ASSERT(DT_PROP(DT_PARENT(DT_DRV_INST(_num)), bundle_class) ==                        \
			     BUNDLE_CLASS_BRIDGED_PHY,                                             \
		     "BUNDLE_CLASS_BRIDGED_PHY required");                                         \
                                                                                                   \
	BUILD_ASSERT(DT_PROP(DT_DRV_INST(_num), cport_protocol) == CPORT_PROTOCOL_I2C,             \
		     "CPORT_PROTOCOL_I2C required");                                               \
                                                                                                   \
	static struct greybus_i2c_control_config greybus_i2c_control_config_##_num = {             \
		.id = (uint8_t)DT_INST_PROP(_num, id),                                             \
		.bundle = (uint8_t)DT_PROP(DT_PARENT(DT_DRV_INST(_num)), id),                      \
		.greybus_i2c_controller =                                                          \
			DEVICE_DT_GET(DT_PHANDLE(DT_DRV_INST(_num), greybus_i2c_controller)),      \
		.bus_name = DT_NODE_FULL_NAME(DT_PARENT(DT_PARENT(DT_DRV_INST(_num)))),            \
	};                                                                                         \
                                                                                                   \
	static struct greybus_i2c_control_data greybus_i2c_control_data_##_num;                    \
                                                                                                   \
	DEVICE_DT_INST_DEFINE(_num, greybus_i2c_control_init, NULL,                                \
			      &greybus_i2c_control_data_##_num,                                    \
			      &greybus_i2c_control_config_##_num, POST_KERNEL,                     \
			      CONFIG_GREYBUS_CPORT_INIT_PRIORITY, NULL);

DT_INST_FOREACH_STATUS_OKAY(DEFINE_GREYBUS_I2C_CONTROL);
